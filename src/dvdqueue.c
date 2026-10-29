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
