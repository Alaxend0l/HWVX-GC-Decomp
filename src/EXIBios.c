#include "common.h"

typedef void (*EXICallback)(s32 chan, void *context);

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
            __EXIRegs[chan * 5] = (__EXIRegs[chan * 5] & 0x7F5) | 0x800;
            exi->idTime = 0;
            ((s32 *)0x800030C0)[chan] = 0;
        }
        if (cpr & 0x1000) {
            t = (s32)(OSGetTime() / (*(u32 *)0x800000F8 / 4 / 1000) / 100) + 1;
            if (((s32 *)0x800030C0)[chan] == 0) {
                ((s32 *)0x800030C0)[chan] = t;
            }
            if (t - ((s32 *)0x800030C0)[chan] < 300 / 100) {
                rc = FALSE;
            }
        } else {
            exi->idTime = 0;
            ((s32 *)0x800030C0)[chan] = 0;
            rc = FALSE;
        }
    } else if (!(cpr & 0x1000) || (cpr & 0x800)) {
        exi->idTime = 0;
        ((s32 *)0x800030C0)[chan] = 0;
        rc = FALSE;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

u32 EXIGetState(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    return exi->state;
}
