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

void UnsetRun(void ** arg0)
{
    extern s32 RunQueueBits;
    void ** next;
    void ** prev;
    void ** next2;

    next = arg0[0xb8];
    prev = arg0[0xb7];
    next2 = arg0[0xb9];
    if (next == NULL)
    {
        prev[1] = next2;
    }
    else
    {
        next[0xb9] = next2;
    }
    if (next2 == NULL)
    {
        prev[0] = next;
    }
    else
    {
        next2[0xb8] = next;
    }
    if (prev[0] == NULL)
    {
        RunQueueBits &= ~(1 << (0x1f - (s32)arg0[0xb4]));
    }
    arg0[0xb7] = NULL;
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

void SelectThread(s32 yield);
extern s32 RunQueueHint;
void __OSReschedule(void) {
    if (RunQueueHint) {
        SelectThread(0);
    }
}

