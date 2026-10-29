#include "common.h"

typedef struct OSAlarm {
	void *(*handler)(void *, void *);
	u32 tag;
	s64 fire;
	struct OSAlarm *prev;
	struct OSAlarm *next;
	s64 fire2;
	s64 last;
} OSAlarm;

typedef struct OSAlarmQueue {
	OSAlarm *head;
	OSAlarm *tail;
} OSAlarmQueue;

OSAlarmQueue AlarmQueue;

void PPCMtdec(u32 newDec);
s64 __OSGetSystemTime(void);

static inline void SetTimer(OSAlarm *alarm) {
    s64 delta;

    delta = alarm->fire - __OSGetSystemTime();
    if (delta < 0) {
        PPCMtdec(0);
    } else if (delta < 0x80000000) {
        PPCMtdec((u32)delta);
    } else {
        PPCMtdec(0x7fffffff);
    }
}

void OSInitAlarm(void) {
    void *__OSGetExceptionHandler(u8 exception);
    void *__OSSetExceptionHandler(u8 exception, void *handler);
    void DecrementerExceptionHandler(void);
    if (__OSGetExceptionHandler(8) != DecrementerExceptionHandler) {
        AlarmQueue.head = AlarmQueue.tail = NULL;
        __OSSetExceptionHandler(8, DecrementerExceptionHandler);
    }
}

void OSCreateAlarm(s32 *arg0)
{
    *arg0 = 0;
}

void InsertAlarm(OSAlarm *alarm, s64 time, void *(*handler)(void *, void *))
{
    void PPCMtdec(u32 newDec);
    s64 __OSGetSystemTime(void);
    OSAlarm *cursor;
    OSAlarm *prev;
    s64 now;
    s64 delta;

    if (0 < alarm->fire2)
    {
        now = __OSGetSystemTime();
        time = alarm->last;
        if (alarm->last < now)
        {
            delta = (now - alarm->last) / alarm->fire2;
            time += alarm->fire2 * (delta + 1);
        }
    }
    alarm->handler = handler;
    alarm->fire = time;
    cursor = AlarmQueue.head;
    while (cursor != NULL)
    {
        if (time < cursor->fire)
        {
            alarm->prev = cursor->prev;
            cursor->prev = alarm;
            alarm->next = cursor;
            prev = alarm->prev;
            if (prev != NULL)
            {
                prev->next = alarm;
                return;
            }
            AlarmQueue.head = alarm;
            delta = alarm->fire - __OSGetSystemTime();
            if (delta < 0)
            {
                PPCMtdec(0);
                return;
            }
            if (delta < 0x80000000LL)
            {
                PPCMtdec((u32)delta);
                return;
            }
            PPCMtdec(0x7FFFFFFF);
            return;
        }
        cursor = cursor->next;
    }
    alarm->next = NULL;
    prev = AlarmQueue.tail;
    AlarmQueue.tail = alarm;
    alarm->prev = prev;
    if (prev != NULL)
    {
        prev->next = alarm;
        return;
    }
    AlarmQueue.tail = alarm;
    AlarmQueue.head = alarm;
    delta = alarm->fire - __OSGetSystemTime();
    if (delta < 0)
    {
        PPCMtdec(0);
        return;
    }
    if (delta < 0x80000000LL)
    {
        PPCMtdec((u32)delta);
        return;
    }
    PPCMtdec(0x7FFFFFFF);
}

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void OSSetAlarm(OSAlarm *alarm, s64 tick, void *(*handler)(void *, void *))
{
    s64 __OSGetSystemTime(void);
    s32 enabled;

    enabled = OSDisableInterrupts();
    alarm->fire2 = 0;
    InsertAlarm(alarm, __OSGetSystemTime() + tick, handler);
    OSRestoreInterrupts(enabled);
}

void OSCancelAlarm(OSAlarm *alarm) {
    void PPCMtdec(u32 newDec);
    s64 __OSGetSystemTime(void);
    BOOL enabled;
    OSAlarm *next;
    s64 delta;

    enabled = OSDisableInterrupts();
    if (alarm->handler == NULL) {
        OSRestoreInterrupts(enabled);
        return;
    }
    next = alarm->next;
    if (next == NULL) {
        AlarmQueue.tail = alarm->prev;
    } else {
        next->prev = alarm->prev;
    }
    if (alarm->prev) {
        alarm->prev->next = next;
    } else {
        AlarmQueue.head = next;
        if (next) {
            delta = next->fire - __OSGetSystemTime();
            if (delta < 0) {
                PPCMtdec(0);
            } else if (delta < 0x80000000) {
                PPCMtdec((u32)delta);
            } else {
                PPCMtdec(0x7fffffff);
            }
        }
    }
    alarm->handler = NULL;
    OSRestoreInterrupts(enabled);
}

void DecrementerExceptionCallback(u32 arg0, void *arg1)
{
    void OSClearContext(void *context);
    void OSDisableScheduler(void);
    void OSEnableScheduler(void);
    void OSSetCurrentContext(void *context);
    void OSLoadContext(void *context);
    void PPCMtdec(u32 newDec);
    void __OSReschedule(void);
    s64 __OSGetSystemTime(void);
    void InsertAlarm(OSAlarm *alarm, s64 time, void *(*handler)(void *, void *));
    OSAlarm *alarm;
    OSAlarm *next;
    s64 time;
    s64 delta;
    void *(*handler)(void *, void *);
    u64 tmpContext[0x59];
    u64 tmpPad[1];

    time = __OSGetSystemTime();
    alarm = AlarmQueue.head;
    if (alarm == NULL)
    {
        OSLoadContext(arg1);
    }
    if (time < alarm->fire)
    {
        delta = alarm->fire - __OSGetSystemTime();
        if (delta < 0)
        {
            PPCMtdec(0);
        }
        else if (delta < 0x80000000LL)
        {
            PPCMtdec((u32)delta);
        }
        else
        {
            PPCMtdec(0x7FFFFFFF);
        }
        OSLoadContext(arg1);
    }
    next = alarm->next;
    AlarmQueue.head = next;
    if (next == NULL)
    {
        AlarmQueue.tail = NULL;
    }
    else
    {
        next->prev = NULL;
    }
    handler = alarm->handler;
    alarm->handler = NULL;
    if (0 < alarm->fire2)
    {
        InsertAlarm(alarm, 0, handler);
    }
    next = AlarmQueue.head;
    if (next != NULL)
    {
        delta = next->fire - __OSGetSystemTime();
        if (delta < 0)
        {
            PPCMtdec(0);
        }
        else if (delta < 0x80000000LL)
        {
            PPCMtdec((u32)delta);
        }
        else
        {
            PPCMtdec(0x7FFFFFFF);
        }
    }
    OSDisableScheduler();
    OSClearContext(tmpContext);
    OSSetCurrentContext(tmpContext);
    handler(alarm, arg1);
    OSClearContext(tmpContext);
    OSSetCurrentContext(arg1);
    OSEnableScheduler();
    __OSReschedule();
    OSLoadContext(arg1);
}

