typedef int s32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct OSThread OSThread;
typedef struct OSThreadQueue { OSThread *head; OSThread *tail; } OSThreadQueue;
typedef struct OSThreadLink { OSThread *next; OSThread *prev; } OSThreadLink;
typedef struct OSMutex OSMutex;
typedef struct OSMutexQueue { OSMutex *head; OSMutex *tail; } OSMutexQueue;
typedef struct OSMutexLink { OSMutex *next; OSMutex *prev; } OSMutexLink;
struct OSMutex {
    OSThreadQueue queue;
    OSThread *thread;
    s32 count;
    OSMutexLink link;
};
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
    OSMutex *mutex;
    OSMutexQueue queueMutex;
};

void OSWakeupThread(OSThreadQueue *queue);

void __OSUnlockAllMutex(OSThread *thread) {
    OSMutex *mutex;

    while (thread->queueMutex.head) {
        OSMutex *next;
        mutex = thread->queueMutex.head;
        next = mutex->link.next;
        if (next == 0) {
            thread->queueMutex.tail = 0;
        } else {
            next->link.prev = 0;
        }
        thread->queueMutex.head = next;
        mutex->count = 0;
        mutex->thread = 0;
        OSWakeupThread(&mutex->queue);
    }
}
