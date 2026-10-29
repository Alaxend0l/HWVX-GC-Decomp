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
    u32 timeout;

    HoldDown = __OSGetSystemTime();
    timeout = (100 * ((*(u32 *)0x800000F8 / 4) / 125000)) / 8;
    while (__OSGetSystemTime() - HoldDown < timeout &&
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
