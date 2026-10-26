#include "common.h"

s32 OSGetCurrentThread(void)
{
    return *(s32 *)0x800000e4;
}
