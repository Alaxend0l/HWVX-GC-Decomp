#include "common.h"

typedef void (*EXICallback)(s32 chan, void *context);

typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;

typedef struct EXIControl {
    EXICallback exiCallback;
    EXICallback tcCallback;
    EXICallback extCallback;
    volatile u32 state;
    s32 immLen;
    u8 *immBuf;
    u32 dev;
    u32 id;
    s32 idTime;
    s32 items;
    struct {
        u32 dev;
        EXICallback callback;
    } queue[3];
} EXIControl;

static EXIControl Ecb[3];

volatile u32 __EXIRegs[16] : 0xCC006800;
s32 __EXIProbeStartTime[2] : 0x800030C0;

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
s64 OSGetTime(void);
u32 __OSMaskInterrupts(u32 mask);
u32 __OSUnmaskInterrupts(u32 mask);
void *__OSGetInterruptHandler(s16 interrupt);

static inline u32 __EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext) {
    u32 cpr;
    u32 prev;

    prev = cpr = __EXIRegs[chan * 5];
    cpr &= 0x7F5;
    if (exi) {
        cpr |= 2;
    }
    if (tc) {
        cpr |= 8;
    }
    if (ext) {
        cpr |= 0x800;
    }
    __EXIRegs[chan * 5] = cpr;
    return prev;
}

u32 __OSGetDIConfig(void);

static inline void CompleteTransfer(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    u8 *buf;
    u32 data;
    int i;
    int len;

    if (exi->state & 3) {
        if ((exi->state & 2) && (len = exi->immLen)) {
            buf = exi->immBuf;
            data = __EXIRegs[chan * 5 + 4];
            for (i = 0; i < len; i++) {
                *buf++ = (u8)((data >> ((3 - i) * 8)) & 0xFF);
            }
        }
        exi->state &= ~3;
    }
}

#pragma use_lmw_stmw on
#pragma use_lmw_stmw on
#pragma use_lmw_stmw on
u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext);
u32 __OSUnmaskInterrupts(u32 mask);
#pragma use_lmw_stmw on
BOOL SetExiInterruptMask(s32 chan, EXIControl *exi) {
    EXIControl *exi2;

    exi2 = &Ecb[2];
    switch (chan) {
    case 0:
        if ((exi->exiCallback == 0 && exi2->exiCallback == 0) || (exi->state & 0x10)) {
            __OSMaskInterrupts(0x410000);
        } else {
            __OSUnmaskInterrupts(0x410000);
        }
        break;
    case 1:
        if (exi->exiCallback == 0 || (exi->state & 0x10)) {
            __OSMaskInterrupts(0x80000);
        } else {
            __OSUnmaskInterrupts(0x80000);
        }
        break;
    case 2:
        if (__OSGetInterruptHandler(25) == 0 || (exi->state & 0x10)) {
            __OSMaskInterrupts(0x40);
        } else {
            __OSUnmaskInterrupts(0x40);
        }
        break;
    }
}

BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, EXICallback callback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & 3) || !(exi->state & 4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if (exi->tcCallback) {
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(0x200000u >> (3 * chan));
    }

    exi->state |= 2;

    if (type != 0) {
        u32 data;
        int i;

        data = 0;
        for (i = 0; i < len; i++) {
            data |= ((u8 *)buf)[i] << ((3 - i) * 8);
        }
        __EXIRegs[chan * 5 + 4] = data;
    }

    exi->immBuf = buf;
    exi->immLen = (type != 1) ? len : 0;
    __EXIRegs[chan * 5 + 3] = (type << 2) | 1 | ((len - 1) << 4);

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIImmEx(s32 chan, void *buf, s32 len, u32 mode) {
    s32 xLen;

    while (len) {
        xLen = (len < 4) ? len : 4;
        if (!EXIImm(chan, buf, xLen, mode, NULL)) {
            return FALSE;
        }
        if (!EXISync(chan)) {
            return FALSE;
        }
        (u8 *)buf += xLen;
        len -= xLen;
    }
    return TRUE;
}

BOOL EXIDma(s32 chan, void *buf, s32 len, u32 type, EXICallback callback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & 3) || !(exi->state & 4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if (exi->tcCallback) {
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(0x200000u >> (3 * chan));
    }

    exi->state |= 1;
    __EXIRegs[chan * 5 + 1] = (u32)buf & 0x3FFFFE0;
    __EXIRegs[chan * 5 + 2] = (u32)len;
    __EXIRegs[chan * 5 + 3] = (type << 2) | 3;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXISync(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL rc = FALSE;
    BOOL enabled;

    while (exi->state & 4) {
        if (!(__EXIRegs[chan * 5 + 3] & 1)) {
            enabled = OSDisableInterrupts();
            if (exi->state & 4) {
                CompleteTransfer(chan);
                if (__OSGetDIConfig() != 0xFF || exi->immLen != 4 || (__EXIRegs[chan * 5] & 0x70) ||
                    (__EXIRegs[chan * 5 + 4] != 0x01010000 && __EXIRegs[chan * 5 + 4] != 0x05070000 &&
                     __EXIRegs[chan * 5 + 4] != 0x04220001) ||
                    *(u16 *)0x800030E6 == 0x8200) {
                    rc = TRUE;
                }
            }
            OSRestoreInterrupts(enabled);
            break;
        }
    }
    return rc;
}

u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext) {
    u32 cpr;
    u32 prev;

    prev = cpr = __EXIRegs[chan * 5];
    cpr &= 0x7F5;
    if (exi) {
        cpr |= 2;
    }
    if (tc) {
        cpr |= 8;
    }
    if (ext) {
        cpr |= 0x800;
    }
    __EXIRegs[chan * 5] = cpr;
    return prev;
}

#pragma use_lmw_stmw on
BOOL SetExiInterruptMask(s32 chan, EXIControl *exi);
EXICallback EXISetExiCallback(s32 chan, EXICallback exiCallback) {
    EXIControl *exi = &Ecb[chan];
    EXICallback prev;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    prev = exi->exiCallback;
    exi->exiCallback = exiCallback;

    if (chan != 2) {
        SetExiInterruptMask(chan, exi);
    } else {
        SetExiInterruptMask(0, &Ecb[0]);
    }

    OSRestoreInterrupts(enabled);
    return prev;
}

static BOOL __EXIProbe(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    BOOL rc;
    u32 cpr;
    s32 t;

    if (chan == 2) {
        return TRUE;
    }

    rc = TRUE;
    enabled = OSDisableInterrupts();
    cpr = __EXIRegs[chan * 5];
    if (!(exi->state & 8)) {
        if (cpr & 0x800) {
            __EXIClearInterrupts(chan, FALSE, FALSE, TRUE);
            exi->idTime = 0;
            __EXIProbeStartTime[chan] = 0;
        }
        if (cpr & 0x1000) {
            t = (s32)(OSGetTime() / (*(u32 *)0x800000F8 / 4 / 1000) / 100) + 1;
            if (__EXIProbeStartTime[chan] == 0) {
                __EXIProbeStartTime[chan] = t;
            }
            if (t - __EXIProbeStartTime[chan] < 300 / 100) {
                rc = FALSE;
            }
        } else {
            exi->idTime = 0;
            __EXIProbeStartTime[chan] = 0;
            rc = FALSE;
        }
    } else if (!(cpr & 0x1000) || (cpr & 0x800)) {
        exi->idTime = 0;
        __EXIProbeStartTime[chan] = 0;
        rc = FALSE;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 EXIGetID(s32 chan, u32 dev, u32 *id);
BOOL EXIProbe(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL rc;
    u32 id;

    rc = __EXIProbe(chan);
    if (rc && exi->idTime == 0) {
        rc = EXIGetID(chan, 0, &id) ? TRUE : FALSE;
    }
    return rc;
}

static inline BOOL __EXIAttach(s32 chan, EXICallback extCallback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & 8) || __EXIProbe(chan) == FALSE) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    exi->extCallback = extCallback;
    __OSUnmaskInterrupts(0x100000u >> (3 * chan));
    exi->state |= 8;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

s32 EXIProbeEx(s32 chan) {
    if (EXIProbe(chan)) {
        return 1;
    } else if (__EXIProbeStartTime[chan] != 0) {
        return 0;
    } else {
        return -1;
    }
}

BOOL EXIAttach(s32 chan, EXICallback extCallback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    BOOL rc;

    EXIProbe(chan);
    enabled = OSDisableInterrupts();
    if (exi->idTime == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    rc = __EXIAttach(chan, extCallback);
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL EXIDetach(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(exi->state & 8)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    if ((exi->state & 0x10) && exi->dev == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~8;
    __OSMaskInterrupts(0x500000u >> (3 * chan));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 __OSMaskInterrupts(u32 mask);
BOOL EXISelect(s32 chan, u32 dev, u32 freq) {
    EXIControl *exi = &Ecb[chan];
    u32 cpr;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & 4) ||
        chan != 2 && (dev == 0 && !(exi->state & 8) && !__EXIProbe(chan) ||
                      !(exi->state & 0x10) || exi->dev != dev)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state |= 4;
    cpr = __EXIRegs[chan * 5];
    cpr &= 0x405;
    cpr |= ((1 << dev) << 7) | (freq << 4);
    __EXIRegs[chan * 5] = cpr;

    if (exi->state & 8) {
        switch (chan) {
        case 0:
            __OSMaskInterrupts(0x100000);
            break;
        case 1:
            __OSMaskInterrupts(0x20000);
            break;
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

#pragma use_lmw_stmw off
#pragma peephole on
BOOL EXIDeselect(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    u32 cpr;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(exi->state & 4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~4;
    cpr = __EXIRegs[chan * 5];
    __EXIRegs[chan * 5] = cpr & 0x405;

    if (exi->state & 8) {
        switch (chan) {
        case 0:
            __OSUnmaskInterrupts(0x100000);
            break;
        case 1:
            __OSUnmaskInterrupts(0x20000);
            break;
        }
    }

    OSRestoreInterrupts(enabled);

    if (chan != 2 && (cpr & 0x80)) {
        return __EXIProbe(chan) ? TRUE : FALSE;
    }
    return TRUE;
}

void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
void EXIIntrruptHandler(s16 interrupt, OSContext *context) {
    s32 chan;
    EXIControl *exi;
    EXICallback callback;

    chan = (interrupt - 9) / 3;
    exi = &Ecb[chan];
    __EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    callback = exi->exiCallback;
    if (callback) {
        OSContext exceptionContext;

        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

void TCIntrruptHandler(s16 interrupt, OSContext *context) {
    s32 chan;
    EXIControl *exi;
    EXICallback callback;

    chan = (interrupt - 10) / 3;
    exi = &Ecb[chan];
    __OSMaskInterrupts(0x80000000U >> interrupt);
    __EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
    callback = exi->tcCallback;
    if (callback) {
        OSContext exceptionContext;

        exi->tcCallback = 0;
        CompleteTransfer(chan);

        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

void EXTIntrruptHandler(s16 interrupt, OSContext *context) {
    s32 chan;
    EXIControl *exi;
    EXICallback callback;

    chan = (interrupt - 11) / 3;
    __OSMaskInterrupts(0x500000U >> (3 * chan));
    exi = &Ecb[chan];
    callback = exi->extCallback;
    exi->state &= ~8;
    if (callback) {
        OSContext exceptionContext;

        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        exi->extCallback = 0;
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

void *__OSSetInterruptHandler(s16 interrupt, void *handler);
s32 OSGetConsoleType(void);
void TCIntrruptHandler(s16 interrupt, OSContext *context);
void EXIInit(void) {
    __OSMaskInterrupts(0x7F8000);
    __EXIRegs[0] = 0;
    __EXIRegs[5] = 0;
    __EXIRegs[10] = 0;
    __EXIRegs[0] = 0x2000;
    __OSSetInterruptHandler(9, EXIIntrruptHandler);
    __OSSetInterruptHandler(10, TCIntrruptHandler);
    __OSSetInterruptHandler(11, EXTIntrruptHandler);
    __OSSetInterruptHandler(12, EXIIntrruptHandler);
    __OSSetInterruptHandler(13, TCIntrruptHandler);
    __OSSetInterruptHandler(14, EXTIntrruptHandler);
    __OSSetInterruptHandler(15, EXIIntrruptHandler);
    __OSSetInterruptHandler(16, TCIntrruptHandler);
    if (OSGetConsoleType() & 0x10000000) {
        __EXIProbeStartTime[0] = __EXIProbeStartTime[1] = 0;
        Ecb[0].idTime = Ecb[1].idTime = 0;
        __EXIProbe(0);
        __EXIProbe(1);
    }
}

#pragma push
#pragma use_lmw_stmw on
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    if (exi->state & 0x10) {
        if (unlockedCallback) {
            for (i = 0; i < exi->items; i++) {
                if (exi->queue[i].dev == dev) {
                    OSRestoreInterrupts(enabled);
                    return FALSE;
                }
            }
            exi->queue[exi->items].callback = unlockedCallback;
            exi->queue[exi->items].dev = dev;
            exi->items++;
        }
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state |= 0x10;
    exi->dev = dev;
    SetExiInterruptMask(chan, exi);

    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma pop
void *memmove(void *dst, const void *src, u32 n);
BOOL EXIUnlock(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    EXICallback unlockedCallback;

    enabled = OSDisableInterrupts();
    if (!(exi->state & 0x10)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~0x10;
    SetExiInterruptMask(chan, exi);
    if (0 < exi->items) {
        unlockedCallback = exi->queue[0].callback;
        if (0 < --exi->items) {
            memmove(&exi->queue[0], &exi->queue[1], sizeof(exi->queue[0]) * exi->items);
        }
        unlockedCallback(chan, 0);
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 EXIGetState(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    return exi->state;
}

#pragma peephole on
static void UnlockedHandler(s32 chan, void *context) {
    u32 id;
    EXIGetID(chan, 0, &id);
}

