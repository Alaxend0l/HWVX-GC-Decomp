#include "common.h"

typedef struct OSCalendarTime {
    s32 sec;
    s32 min;
    s32 hour;
    s32 mday;
    s32 mon;
    s32 year;
    s32 wday;
    s32 yday;
    s32 msec;
    s32 usec;
} OSCalendarTime;

u32 OSGetTick(void);
s64 OSGetTime(void);
void OSTicksToCalendarTime(s64 ticks, OSCalendarTime *td);

OSCalendarTime g_todaysDate;
u32 max_time;
s32 m_iDeltaTimeUs;
u32 m_iLastTimeUs;

void Performance_Start(void) {

}

void Performance_End(void) {

}

void Release_Performance_Get(u32 *arg0) {
    u32 OSGetTick(void);
    *arg0 = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

void Release_Performance_Diff(u32 *arg0) {
    u32 OSGetTick(void);
    extern u32 max_time;
    u32 now;

    now = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
    if (now < *arg0) {
        now += max_time;
    }
    *arg0 = now - *arg0;
}

