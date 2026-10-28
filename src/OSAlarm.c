#include "common.h"

typedef struct OSAlarm {
	void *(*handler)(void *, void *);
	u32 tag;
	s64 fire;
	struct OSAlarm *prev;
	struct OSAlarm *next;
	s64 fire2;
	s64 field_20;
} OSAlarm;

typedef struct OSAlarmQueue {
	OSAlarm *head;
	OSAlarm *tail;
} OSAlarmQueue;

OSAlarmQueue AlarmQueue;

void OSInitAlarm(void) {
    extern void *AlarmQueue_803C7210[2];
    void *__OSGetExceptionHandler(u8 exception);
    void *__OSSetExceptionHandler(u8 exception, void *handler);
    void DecrementerExceptionHandler(void);
    if (__OSGetExceptionHandler(8) != DecrementerExceptionHandler) {
        AlarmQueue_803C7210[0] = AlarmQueue_803C7210[1] = NULL;
        __OSSetExceptionHandler(8, DecrementerExceptionHandler);
    }
}

void OSCreateAlarm(s32 *arg0)
{
    *arg0 = 0;
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

