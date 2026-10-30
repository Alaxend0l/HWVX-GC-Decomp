#include "common.h"

typedef void (*OSResetCallback)(void);

s64 __OSGetSystemTime(void);
u32 __OSMaskInterrupts(u32 mask);

volatile u32 __PIRegs[12] : 0xCC003000;
u32 __OSBusClock : 0x800000F8;

static OSResetCallback ResetCallback;
static BOOL Down;
static BOOL LastState;
static s64 HoldUp;
static s64 HoldDown;

void __OSResetSWInterruptHandler(s16 exception, void *context)
{
    OSResetCallback callback;

    HoldDown = __OSGetSystemTime();
    while (__OSGetSystemTime() - HoldDown < ((100 * ((__OSBusClock / 4) / 125000)) / 8) &&
           !(__PIRegs[0] & 0x00010000)) {
        ;
    }
    if (!(__PIRegs[0] & 0x00010000)) {
        LastState = Down = TRUE;
        __OSMaskInterrupts(0x200);
        if (ResetCallback) {
            callback = ResetCallback;
            ResetCallback = NULL;
            callback();
        }
    }
    __PIRegs[0] = 2;
}

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
extern s64 __OSStartTime;
u8 GameChoice : 0x800030E3;
BOOL OSGetResetButtonState(void) {
    BOOL enabled;
    BOOL state;
    u32 reg;
    s64 now;

    enabled = OSDisableInterrupts();
    now = __OSGetSystemTime();
    reg = __PIRegs[0];
    if (!(reg & 0x00010000)) {
        if (!Down) {
            Down = TRUE;
            state = HoldUp ? TRUE : FALSE;
            HoldDown = now;
        } else {
            state = (HoldUp || ((100 * ((__OSBusClock / 4) / 125000)) / 8) < now - HoldDown) ? TRUE : FALSE;
        }
    } else if (Down) {
        Down = FALSE;
        state = LastState;
        if (state) {
            HoldUp = now;
        } else {
            HoldUp = 0;
        }
    } else if (HoldUp && (now - HoldUp < 40 * ((__OSBusClock / 4) / 1000))) {
        state = TRUE;
    } else {
        state = FALSE;
        HoldUp = 0;
    }
    LastState = state;

    if (GameChoice & 0x3F) {
        s64 fire = (GameChoice & 0x3F) * 60;
        fire = __OSStartTime + fire * (__OSBusClock / 4);
        if (fire < now) {
            now -= fire;
            now = (now / (__OSBusClock / 4)) / 2;
            if ((now & 1) == 0) {
                state = TRUE;
            } else {
                state = FALSE;
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return state;
}

