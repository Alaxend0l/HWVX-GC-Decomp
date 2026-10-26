#include "common.h"

void OSInitThreadQueue(void * arg0)
{
    ((s32 *)arg0)[1] = 0;
    ((s32 *)arg0)[0] = 0;
}

s32 OSGetCurrentThread(void)
{
    return *(s32 *)0x800000e4;
}
