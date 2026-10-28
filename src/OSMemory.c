#include "common.h"


u32 __OSMaskInterrupts(u32 mask);
s32 OnReset(s32 arg0)
{
    if (arg0 != 0) {
        ((volatile u16 *)0xCC004000)[0x8] = 0xFF;
        __OSMaskInterrupts(0xF0000000);
    }
    return 1;
}
