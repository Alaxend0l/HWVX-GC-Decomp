#include "common.h"


u32 __OSMaskInterrupts(u32 mask);
volatile u16 __MEMRegs[64] : 0xCC004000;
static s32 OnReset(s32 final)
{
    if (final != 0) {
        __MEMRegs[8] = 0xFF;
        __OSMaskInterrupts(0xF0000000);
    }
    return 1;
}
