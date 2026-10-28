#include "common.h"

s32 OSGetCurrentContext(void)
{
    return *(s32 *)0x800000d4;
}
