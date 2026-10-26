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

s32 __OSGetEffectivePriority(void * thread)
{
    s32 priority;
    void * blocked;
    void * mutex;

    priority = ((s32 *)thread)[0xb5];
    for (mutex = (void *)((void **)thread)[0xbd]; mutex != NULL; mutex = (void *)((void **)mutex)[4])
    {
        blocked = (void *)((void **)mutex)[0];
        if (blocked != NULL && ((s32 *)blocked)[0xb4] < priority)
        {
            priority = ((s32 *)blocked)[0xb4];
        }
    }
    return priority;
}

