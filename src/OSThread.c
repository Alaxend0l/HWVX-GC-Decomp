#include "common.h"

typedef struct OSThread OSThread;

typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u8 unk88[0x2C8 - 0x88];
} OSContext;

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
    OSContext context;
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
    s32 error;
};

#define OS_THREAD_ATTR_DETACH 1
#define OS_THREAD_STATE_MORIBUND 8

OSThread *__OSCurrentThread : 0x800000E4;
OSThreadQueue __OSActiveThreadQueue : 0x800000DC;
OSContext *__OSFPUContext : 0x800000D8;

void OSInitThreadQueue(void *queue);
void OSClearContext(void *context);
void OSSetCurrentContext(OSContext *context);
extern u8 _stack_addr[];
extern u8 _stack_end[];

static OSThreadQueue RunQueue[32];
static OSThread IdleThread;
static OSThread DefaultThread;
static OSContext IdleContext;

void __OSThreadInit(void) {
    extern volatile u32 RunQueueBits;
    extern volatile s32 RunQueueHint;
    extern s32 Reschedule;
    OSThread *thread = &DefaultThread;
    int prio;

    thread->state = 2;
    thread->attr = OS_THREAD_ATTR_DETACH;
    thread->priority = thread->base = 16;
    thread->suspend = 0;
    thread->val = (void *)-1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    __OSFPUContext = &thread->context;

    OSClearContext(&thread->context);
    OSSetCurrentContext(&thread->context);
    thread->stackBase = (u8 *)_stack_addr;
    thread->stackEnd = (u32 *)_stack_end;
    *(thread->stackEnd) = 0xDEADBABE;

    __OSCurrentThread = thread;
    RunQueueBits = 0;
    RunQueueHint = FALSE;
    for (prio = 0; prio <= 31; ++prio) {
        OSInitThreadQueue(&RunQueue[prio]);
    }

    OSInitThreadQueue(&__OSActiveThreadQueue);
    {
        OSThread *prev = __OSActiveThreadQueue.tail;
        if (prev == NULL) {
            __OSActiveThreadQueue.head = thread;
        } else {
            prev->linkActive.next = thread;
        }
        thread->linkActive.prev = prev;
        thread->linkActive.next = NULL;
        __OSActiveThreadQueue.tail = thread;
    }

    OSClearContext(&IdleContext);
    Reschedule = 0;
}

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

void OSInitContext(OSContext *context, u32 pc, u32 sp);
void OSExitThread(void *val);
BOOL OSCreateThread(OSThread *thread, void *(*func)(void *), void *param, void *stack, u32 stackSize, s32 priority, u16 attr) {
    BOOL enabled;
    u32 sp;

    if (priority < 0 || 31 < priority) {
        return FALSE;
    }

    thread->state = 1;
    thread->attr = (u16)(attr & OS_THREAD_ATTR_DETACH);
    thread->base = priority;
    thread->priority = priority;
    thread->suspend = 1;
    thread->val = (void *)-1;
    thread->mutex = NULL;
    thread->queueJoin.head = thread->queueJoin.tail = NULL;
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    sp = (u32)stack;
    sp &= ~7;
    sp -= 8;
    ((u32 *)sp)[0] = 0;
    ((u32 *)sp)[1] = 0;
    OSInitContext((OSContext *)thread, (u32)func, sp);
    thread->context.lr = (u32)OSExitThread;
    thread->context.gpr[3] = (u32)param;
    thread->stackBase = stack;
    thread->stackEnd = (u32 *)((u32)stack - stackSize);
    *thread->stackEnd = 0xDEADBABE;

    enabled = OSDisableInterrupts();
    {
        OSThread *prev = __OSActiveThreadQueue.tail;
        if (prev == NULL) {
            __OSActiveThreadQueue.head = thread;
        } else {
            prev->linkActive.next = thread;
        }
        thread->linkActive.prev = prev;
        thread->linkActive.next = NULL;
        __OSActiveThreadQueue.tail = thread;
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
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

void OSSleepThread(OSThreadQueue *queue) {
    BOOL enabled;
    OSThread *currentThread;

    enabled = OSDisableInterrupts();
    currentThread = __OSCurrentThread;
    currentThread->state = 4;
    currentThread->queue = queue;
    {
        OSThread *next;
        OSThread *prev;

        for (next = queue->head; next && next->priority <= currentThread->priority; next = next->link.next)
            ;
        if (next == NULL) {
            prev = queue->tail;
            if (prev == NULL) {
                queue->head = currentThread;
            } else {
                prev->link.next = currentThread;
            }
            currentThread->link.prev = prev;
            currentThread->link.next = NULL;
            queue->tail = currentThread;
        } else {
            currentThread->link.next = next;
            prev = next->link.prev;
            next->link.prev = currentThread;
            currentThread->link.prev = prev;
            if (prev == NULL) {
                queue->head = currentThread;
            } else {
                prev->link.next = currentThread;
            }
        }
    }
    RunQueueHint = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

extern OSThreadQueue RunQueue[32];
extern volatile u32 RunQueueBits;
void OSWakeupThread(OSThreadQueue *queue) {
    BOOL enabled;
    OSThread *thread;

    enabled = OSDisableInterrupts();
    while (queue->head) {
        OSThread *next;
        OSThread *prev;

        thread = queue->head;
        next = thread->link.next;
        if (next == NULL) {
            queue->tail = NULL;
        } else {
            next->link.prev = NULL;
        }
        queue->head = next;
        thread->state = 1;
        if (!(thread->suspend > 0)) {
            thread->queue = &RunQueue[thread->priority];
            prev = thread->queue->tail;
            if (prev == NULL) {
                thread->queue->head = thread;
            } else {
                prev->link.next = thread;
            }
            thread->link.prev = prev;
            thread->link.next = NULL;
            thread->queue->tail = thread;
            RunQueueBits |= 1u << (31 - thread->priority);
            RunQueueHint = TRUE;
        }
    }
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

