#include "common.h"

static DVDQueue WaitingQueue[4];

void __DVDClearWaitingQueue(void)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        DVDQueue *q = &WaitingQueue[i];
        q->next = q;
        q->prev = q;
    }
}

s32 __DVDCheckWaitingQueue(void)
{
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    u32 i;
    s32 enabled;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; i++) {
        if (WaitingQueue[i].next != &WaitingQueue[i]) {
            OSRestoreInterrupts(enabled);
            return 1;
        }
    }
    OSRestoreInterrupts(enabled);
    return 0;
}

s32 __DVDDequeueWaitingQueue(DVDQueue *block)
{
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    s32 enabled;
    DVDQueue *prev;
    DVDQueue *next;

    enabled = OSDisableInterrupts();
    prev = block->prev;
    next = block->next;
    if (prev == NULL || next == NULL) {
        OSRestoreInterrupts(enabled);
        return 0;
    }
    prev->next = next;
    next->prev = prev;
    OSRestoreInterrupts(enabled);
    return 1;
}

