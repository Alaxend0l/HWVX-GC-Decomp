#include "common.h"

typedef struct OSThread OSThread;

typedef struct OSThreadQueue {
    OSThread *head;
    OSThread *tail;
} OSThreadQueue;

typedef struct OSThreadLink {
    OSThread *next;
    OSThread *prev;
} OSThreadLink;

typedef struct OSMutexQueue {
    struct OSMutex *head;
    struct OSMutex *tail;
} OSMutexQueue;

struct OSThread {
    u8 context[0x2C8];
    u16 state;
    u16 attr;
    s32 suspend;
    s32 priority;
    s32 base;
    void *val;
    OSThreadQueue *queue;
    OSThreadLink link;
    OSThreadQueue queueJoin;
    struct OSMutex *mutex;
    OSMutexQueue queueMutex;
    OSThreadLink linkActive;
    u8 *stackBase;
    u32 *stackEnd;
};

#define OS_THREAD_ATTR_DETACH 1
#define OS_THREAD_STATE_MORIBUND 8

OSThread *__OSCurrentThread : 0x800000E4;
OSThreadQueue __OSActiveThreadQueue : 0x800000DC;

void OSInitThreadQueue(void * arg0)
{
    ((s32 *)arg0)[1] = 0;
    ((s32 *)arg0)[0] = 0;
}

s32 OSGetCurrentThread(void)
{
    return *(s32 *)0x800000e4;
}

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
extern s32 Reschedule;
s32 OSDisableScheduler(void) {
    s32 enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule++;
    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSEnableScheduler(void) {
    s32 enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule--;
    OSRestoreInterrupts(enabled);
    return count;
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
extern volatile s32 RunQueueHint;
void __OSReschedule(void) {
    if (RunQueueHint) {
        SelectThread(0);
    }
}

void OSClearContext(void *context);
void __OSUnlockAllMutex(OSThread *thread);
void OSWakeupThread(OSThreadQueue *queue);
void OSExitThread(void *val) {
    OSThread *currentThread;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    currentThread = __OSCurrentThread;
    OSClearContext(currentThread);
    if (currentThread->attr & OS_THREAD_ATTR_DETACH) {
        OSThread *next = currentThread->linkActive.next;
        OSThread *prev = currentThread->linkActive.prev;
        if (next == NULL) {
            __OSActiveThreadQueue.tail = prev;
        } else {
            next->linkActive.prev = prev;
        }
        if (prev == NULL) {
            __OSActiveThreadQueue.head = next;
        } else {
            prev->linkActive.next = next;
        }
        currentThread->state = 0;
    } else {
        currentThread->state = OS_THREAD_STATE_MORIBUND;
        currentThread->val = val;
    }
    __OSUnlockAllMutex(currentThread);
    OSWakeupThread(&currentThread->queueJoin);
    RunQueueHint = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

