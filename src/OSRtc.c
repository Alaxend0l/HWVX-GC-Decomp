#include "common.h"

extern u32 Scb[21];

s32 UnlockSram(s32 commit, u32 offset);
#pragma peephole on
s32 __OSUnlockSram(s32 commit) {
    return UnlockSram(commit, 0);
}

s32 __OSSyncSram(void)
{
    return Scb[0x13];
}
