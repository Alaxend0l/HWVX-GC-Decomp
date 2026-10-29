typedef int s32;
typedef int BOOL;
typedef void *OSMessage;

typedef struct OSThread OSThread;
typedef struct OSThreadQueue { OSThread *head; OSThread *tail; } OSThreadQueue;

typedef struct OSMessageQueue {
    OSThreadQueue queueSend;
    OSThreadQueue queueReceive;
    OSMessage *msgArray;
    s32 msgCount;
    s32 firstIndex;
    s32 usedCount;
} OSMessageQueue;

#define OS_MESSAGE_NOBLOCK 0
#define OS_MESSAGE_BLOCK 1

void OSInitThreadQueue(OSThreadQueue *queue);
void OSSleepThread(OSThreadQueue *queue);
void OSWakeupThread(OSThreadQueue *queue);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
