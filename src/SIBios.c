#include "common.h"

typedef struct OSAlarm {
    void (*handler)(struct OSAlarm *, void *);
    u32 tag;
    s64 fire;
    struct OSAlarm *prev;
    struct OSAlarm *next;
    s64 period;
    s64 start;
} OSAlarm;

volatile u32 __SIRegs[64] : 0xCC006400;
u32 __OSBusClock : 0x800000F8;

static SIControl Si = {-1, 0, 0, NULL, NULL};
static SIPacket Packet[4];
static OSAlarm Alarm[4];
static u32 Type[4] = {8, 8, 8, 8};
static s64 TypeTime[4];
static s64 XferTime[4];
static void *TypeCallback[4][4];
static void *RDSTHandler[4];
static BOOL InputBufferValid[4];
static u32 InputBuffer[4][2];
static volatile u32 InputBufferVcount[4];

u32 SIGetType(s32 chan);
s64 __OSGetSystemTime(void);
BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback);

static inline void CallTypeAndStatusCallback(s32 chan, u32 type) {
    void (*callback)(s32 chan, u32 type);
    int i;

    for (i = 0; i < 4; ++i) {
        callback = (void (*)(s32, u32))TypeCallback[chan][i];
        if (callback) {
            TypeCallback[chan][i] = 0;
            callback(chan, type);
        }
    }
}

BOOL SIBusy(void) {
    return Si.chan != -1 ? TRUE : FALSE;
}

BOOL SIIsChanBusy(s32 chan) {
    return Packet[chan].chan != -1 || Si.chan == chan;
}

static u32 CompleteTransfer(void) {
    u32 sr;
    u32 i;
    u32 rLen;
    u8 *input;

    sr = __SIRegs[14];
    __SIRegs[13] = (__SIRegs[13] | 0x80000000) & ~1;

    if (Si.chan != -1) {
        XferTime[Si.chan] = __OSGetSystemTime();

        input = Si.input;

        rLen = Si.inputBytes / 4;
        for (i = 0; i < rLen; i++) {
            *(u32 *)input = __SIRegs[32 + i];
            input += 4;
        }

        rLen = Si.inputBytes & 3;
        if (rLen) {
            u32 temp = __SIRegs[32 + i];
            for (i = 0; i < rLen; i++) {
                *input++ = (u8)((temp >> ((3 - i) * 8)) & 0xff);
            }
        }

        if (__SIRegs[13] & 0x20000000) {
            sr >>= 8 * (3 - Si.chan);
            sr &= 0xf;

            if ((sr & 8) && !(Type[Si.chan] & 0x80)) {
                Type[Si.chan] = 8;
            }
            if (sr == 0) {
                sr = 4;
            }
        } else {
            TypeTime[Si.chan] = __OSGetSystemTime();
            sr = 0;
        }

        Si.chan = -1;
    }
    return sr;
}

static void GetTypeCallback(s32 chan, u32 error, void *context);
BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback, s64 delay);
BOOL OSCancelAlarm(OSAlarm *alarm);
u32 VIGetCurrentLine(void);
BOOL SIGetResponseRaw(s32 chan);
void SIInterruptHandler(s16 interrupt, void *context)
{
    u32 reg;

    reg = __SIRegs[13];
    if ((reg & 0xC0000000) == 0xC0000000) {
        s32 chan;
        u32 sr;
        void (*callback)(s32 chan, u32 sr, void *context);
        int i;
        SIPacket *packet;

        chan = Si.chan;
        sr = CompleteTransfer();
        callback = (void (*)(s32, u32, void *))Si.callback;
        Si.callback = NULL;
        {
            s32 next = chan;
            for (i = 0; i < 4; ++i) {
                ++next;
                next %= 4;
                packet = &Packet[next];
                if (packet->chan != -1 && packet->fire <= __OSGetSystemTime()) {
                    if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input, packet->inputBytes, packet->callback)) {
                        OSCancelAlarm(&Alarm[next]);
                        packet->chan = -1;
                    }
                    break;
                }
            }
        }
        if (callback) {
            callback(chan, sr, context);
        }
        sr = __SIRegs[14];
        sr &= 0x0F000000 >> (8 * chan);
        __SIRegs[14] = sr;
        if (Type[chan] == 0x80 && !SIIsChanBusy(chan)) {
            static u32 cmdTypeAndStatus = 0;
            SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback,
                       ((__OSBusClock / 4) / 125000) * 65 / 8);
        }
    }

    if ((reg & 0x18000000) == 0x18000000) {
        int i;
        u32 vcount;
        u32 x;

        vcount = VIGetCurrentLine() + 1;
        x = (Si.poll & 0x03FF0000) >> 16;
        for (i = 0; i < 4; ++i) {
            if (SIGetResponseRaw(i)) {
                InputBufferVcount[i] = vcount;
            }
        }
        for (i = 0; i < 4; ++i) {
            if (!(Si.poll & (0x80000000 >> (24 + i)))) {
                continue;
            }
            if (InputBufferVcount[i] == 0 || InputBufferVcount[i] + (x >> 1) < vcount) {
                return;
            }
        }
        for (i = 0; i < 4; ++i) {
            InputBufferVcount[i] = 0;
        }
        for (i = 0; i < 4; ++i) {
            if (RDSTHandler[i]) {
                ((void (*)(s16, void *))RDSTHandler[i])(interrupt, context);
            }
        }
    }
}

BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    BOOL rc;
    u32 reg;
    int i;

    enabled = OSDisableInterrupts();
    reg = __SIRegs[13];
    rc = (reg & 0x08000000) ? TRUE : FALSE;
    if (enable) {
        reg |= 0x08000000;
        for (i = 0; i < 4; i++) {
            InputBufferVcount[i] = 0;
        }
    } else {
        reg &= ~0x08000000;
    }
    reg &= ~0x80000001;
    __SIRegs[13] = reg;
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL SIRegisterPollingHandler(void *handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == handler) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == 0) {
            RDSTHandler[i] = handler;
            SIEnablePollingInterrupt(TRUE);
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL SIUnregisterPollingHandler(void *handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == handler) {
            RDSTHandler[i] = 0;
            for (i = 0; i < 4; ++i) {
                if (RDSTHandler[i]) {
                    break;
                }
            }
            if (i == 4) {
                SIEnablePollingInterrupt(FALSE);
            }
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SISetSamplingRate(u32 msec);
void *__OSSetInterruptHandler(s16 interrupt, void *handler);
u32 __OSUnmaskInterrupts(u32 mask);
void SIInterruptHandler(s16 interrupt, void *context);
void SIInit(void) {
    Packet[0].chan = Packet[1].chan = Packet[2].chan = Packet[3].chan = -1;
    Si.poll = 0;
    SISetSamplingRate(0);
    while (__SIRegs[13] & 1)
        ;
    __SIRegs[13] = 0x80000000;
    __OSSetInterruptHandler(0x14, SIInterruptHandler);
    __OSUnmaskInterrupts(0x800);
    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);
}

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback) {
    BOOL enabled;
    u32 rLen;
    u32 i;
    u32 sr;
    union {
        u32 val;
        struct {
            u32 tcint : 1;
            u32 tcintmsk : 1;
            u32 comerr : 1;
            u32 rdstint : 1;
            u32 rdstintmsk : 1;
            u32 pad0 : 4;
            u32 outlngth : 7;
            u32 pad1 : 1;
            u32 inlngth : 7;
            u32 pad2 : 5;
            u32 channel : 2;
            u32 tstart : 1;
        } f;
    } comcsr;

    enabled = OSDisableInterrupts();
    if (Si.chan != -1) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    sr = __SIRegs[14];
    sr &= (0x0F000000) >> (8 * chan);
    __SIRegs[14] = sr;

    Si.chan = chan;
    Si.callback = callback;
    Si.inputBytes = inputBytes;
    Si.input = input;

    rLen = (outputBytes + 3) / 4;
    for (i = 0; i < rLen; i++) {
        __SIRegs[32 + i] = ((u32 *)output)[i];
    }

    comcsr.val = __SIRegs[13];
    comcsr.f.tcint = 1;
    comcsr.f.tcintmsk = callback ? 1 : 0;
    comcsr.f.outlngth = (outputBytes == 128) ? 0 : outputBytes;
    comcsr.f.inlngth = (inputBytes == 128) ? 0 : inputBytes;
    comcsr.f.channel = chan;
    comcsr.f.tstart = 1;
    __SIRegs[13] = comcsr.val;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sr;
    int chanShift;

    enabled = OSDisableInterrupts();
    sr = __SIRegs[14];
    chanShift = 8 * (3 - chan);
    sr >>= chanShift;
    if (sr & 8) {
        if (!(Type[chan] & 0x80)) {
            Type[chan] = 8;
        }
    }
    OSRestoreInterrupts(enabled);
    return sr;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[3 * chan] = command;
}

#pragma peephole on
void SITransferCommands(void) {
    __SIRegs[14] = 0x80000000;
}

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;
    enabled = OSDisableInterrupts();
    Si.poll &= ~(0x03FF0000 | 0x0000FF00);
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[12] = poll;
    OSRestoreInterrupts(enabled);
    return (u32)poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si.poll;
    }
    enabled = OSDisableInterrupts();
    poll >>= 24;
    en = poll & 0xF0;
    en >>= 4;
    Si.poll &= ~en;
    poll &= 0x03FFFFF0 | en;
    poll &= ~0x03FFFF00;
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[14] = 0x80000000;
    __SIRegs[12] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;

    if (poll == 0) {
        return Si.poll;
    }
    enabled = OSDisableInterrupts();
    poll >>= 24;
    poll &= 0xF0;
    poll = Si.poll & ~poll;
    __SIRegs[12] = poll;
    Si.poll = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

BOOL SIGetResponseRaw(s32 chan) {
    u32 sr;

    sr = SIGetStatus(chan);
    if (sr & 0x20) {
        InputBuffer[chan][0] = __SIRegs[3 * chan + 1];
        InputBuffer[chan][1] = __SIRegs[3 * chan + 2];
        InputBufferValid[chan] = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL SIGetResponse(s32 chan, void *data) {
    BOOL rc;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SIGetResponseRaw(chan);
    rc = InputBufferValid[chan];
    InputBufferValid[chan] = FALSE;
    if (rc) {
        ((u32 *)data)[0] = InputBuffer[chan][0];
        ((u32 *)data)[1] = InputBuffer[chan][1];
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

static void AlarmHandler(OSAlarm *alarm, void *context) {
    s32 chan;
    SIPacket *packet;

    chan = alarm - Alarm;
    packet = &Packet[chan];
    if (packet->chan != -1) {
        if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input, packet->inputBytes, packet->callback)) {
            packet->chan = -1;
        }
    }
}

void OSSetAlarm(OSAlarm *alarm, s64 tick, void (*handler)(OSAlarm *, void *));
s64 __OSGetSystemTime(void);
BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback, s64 delay) {
    BOOL enabled;
    SIPacket *packet = &Packet[chan];
    s64 now;
    s64 fire;

    enabled = OSDisableInterrupts();
    if (packet->chan != -1 || Si.chan == chan) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    now = __OSGetSystemTime();
    if (delay == 0) {
        fire = now;
    } else {
        fire = XferTime[chan] + delay;
    }
    if (now < fire) {
        delay = fire - now;
        OSSetAlarm(&Alarm[chan], delay, AlarmHandler);
    } else if (__SITransfer(chan, output, outputBytes, input, inputBytes, callback)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    packet->chan = chan;
    packet->output = output;
    packet->outputBytes = outputBytes;
    packet->input = input;
    packet->inputBytes = inputBytes;
    packet->callback = callback;
    packet->fire = fire;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

#pragma push
#pragma use_lmw_stmw on
extern u32 __PADFixBits;
u16 OSGetWirelessID(s32 chan);
void OSSetWirelessID(s32 chan, u16 id);
#pragma push
#pragma use_lmw_stmw on
static void GetTypeCallback(s32 chan, u32 error, void *context) {
    static u32 cmdFixDevice[4];
    u32 type;
    u32 chanBit;
    BOOL fix;
    u32 id;

    Type[chan] &= ~0x80;
    Type[chan] |= error;
    TypeTime[chan] = __OSGetSystemTime();

    type = Type[chan];

    chanBit = 0x80000000 >> chan;
    fix = (BOOL)(__PADFixBits & chanBit);
    __PADFixBits &= ~chanBit;

    if ((error & 0xF) || (type & 0x18000000) != 0x08000000 || !(type & 0x80000000) || (type & 0x04000000)) {
        OSSetWirelessID(chan, 0);
        CallTypeAndStatusCallback(chan, Type[chan]);
        return;
    }

    id = (u32)(OSGetWirelessID(chan) << 8);

    if (fix && (id & 0x100000)) {
        cmdFixDevice[chan] = 0x4Eu << 24 | (id & 0xCFFF00) | 0x100000;
        Type[chan] = 0x80;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    }

    if (type & 0x100000) {
        if ((id & 0xCFFF00) != (type & 0xCFFF00)) {
            if (!(id & 0x100000)) {
                id = type & 0xCFFF00;
                id |= 0x100000;
                OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));
            }
            cmdFixDevice[chan] = 0x4E << 24 | id;
            Type[chan] = 0x80;
            SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
            return;
        }
    } else if (type & 0x40000000) {
        id = type & 0xCFFF00;
        id |= 0x100000;
        OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));
        cmdFixDevice[chan] = 0x4E << 24 | id;
        Type[chan] = 0x80;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    } else {
        OSSetWirelessID(chan, 0);
    }

    CallTypeAndStatusCallback(chan, Type[chan]);
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
u32 SIGetType(s32 chan) {
    void GetTypeCallback(s32 chan, u32 error, void *context);
    static u32 cmdTypeAndStatus;
    BOOL enabled;
    u32 type;
    s64 diff;

    enabled = OSDisableInterrupts();
    type = Type[chan];
    diff = __OSGetSystemTime() - TypeTime[chan];
    if (Si.poll & (0x80 >> chan)) {
        if (type != 8) {
            TypeTime[chan] = __OSGetSystemTime();
            OSRestoreInterrupts(enabled);
            return type;
        } else {
            type = Type[chan] = 0x80;
        }
    } else if (diff <= (s64)(50 * ((__OSBusClock / 4) / 1000)) && type != 8) {
        OSRestoreInterrupts(enabled);
        return type;
    } else if (diff <= (s64)(75 * ((__OSBusClock / 4) / 1000))) {
        Type[chan] = 0x80;
    } else {
        type = Type[chan] = 0x80;
    }
    TypeTime[chan] = __OSGetSystemTime();
    SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback,
               (65 * ((__OSBusClock / 4) / 125000)) / 8);
    OSRestoreInterrupts(enabled);
    return type;
}
#pragma pop

u32 SIGetTypeAsync(s32 chan, void (*callback)(s32, u32)) {
    BOOL enabled;
    u32 type;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);
    if (Type[chan] & 0x80) {
        int i;
        for (i = 0; i < 4; ++i) {
            if (TypeCallback[chan][i] == callback) {
                break;
            }
            if (TypeCallback[chan][i] == 0) {
                TypeCallback[chan][i] = callback;
                break;
            }
        }
    } else {
        callback(chan, type);
    }
    OSRestoreInterrupts(enabled);
    return type;
}

s32 SIDecodeType(s32 type) {
    u32 error;
    s32 kind;

    error = type & 0xFF;
    kind = type & ~0xFF;
    if (error & 0x08) {
        return 0x08;
    }
    if (error & 0x47) {
        return 0x40;
    }
    if (error) {
        return 0x80;
    }
    if ((kind & 0x18000000) == 0) {
        switch (kind & 0xFFFF0000) {
        case 0x00040000:
        case 0x00020000:
        case 0x00010000:
        case 0x05000000:
        case 0x02000000:
            return kind & 0xFFFF0000;
        }
        return 0x40;
    }
    if ((kind & 0x18000000) != 0x08000000) {
        return 0x40;
    }
    switch (kind & 0xFFFF0000) {
    case 0x09000000:
    case 0x08000000:
        return kind & 0xFFFF0000;
    }
    if ((kind & 0xFFE00000) == 0x08200000) {
        return 0x08200000;
    }
    if ((kind & 0x80000000) && !(kind & 0x04000000)) {
        if ((kind & 0x8B100000) == 0x8B100000) {
            return 0x8B100000;
        }
        if (!(kind & 0x02000000)) {
            return 0x88000000;
        }
    }
    if ((kind & 0x09000000) == 0x09000000) {
        kind = 0x09000000;
        return kind;
    }
    return 0x40;
}

u32 SIGetType(s32 chan);
u32 SIProbe(s32 chan) {
    return SIDecodeType(SIGetType(chan));
}

