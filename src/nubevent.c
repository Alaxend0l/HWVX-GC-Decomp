#include "common.h"

s32 TRKInitializeMutex(void *mutex);
s32 TRKAcquireMutex(void *mutex);
s32 TRKReleaseMutex(void *mutex);
void *TRK_memcpy(void *dst, const void *src, u32 n);
s32 TRKReleaseBuffer(s32 idx);

TRKEventQueue gTRKEventQueue;

void TRKDestructEvent(TRKEvent *event) {
    TRKReleaseBuffer(event->msgBufID);
}

void TRKConstructEvent(TRKEvent *event, s32 eventType) {
    event->eventType = eventType;
    event->eventID = 0;
    event->msgBufID = -1;
}

s32 TRKGetNextEvent(TRKEvent *event) {
    s32 result = 0;

    TRKAcquireMutex(&gTRKEventQueue);
    if (gTRKEventQueue.count > 0) {
        TRK_memcpy(event, &gTRKEventQueue.events[gTRKEventQueue.next], sizeof(TRKEvent));
        gTRKEventQueue.count--;
        if (++gTRKEventQueue.next == 2) {
            gTRKEventQueue.next = 0;
        }
        result = 1;
    }
    TRKReleaseMutex(&gTRKEventQueue);
    return result;
}

s32 TRKInitializeEventQueue(void) {
    TRKInitializeMutex(&gTRKEventQueue);
    TRKAcquireMutex(&gTRKEventQueue);
    gTRKEventQueue.count = 0;
    gTRKEventQueue.next = 0;
    gTRKEventQueue.eventID = 0x100;
    TRKReleaseMutex(&gTRKEventQueue);
    return 0;
}

