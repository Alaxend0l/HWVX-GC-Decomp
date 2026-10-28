#include "common.h"

typedef struct OSAlarm {
	void *handler;
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

void OSCreateAlarm(s32 *arg0)
{
    *arg0 = 0;
}
