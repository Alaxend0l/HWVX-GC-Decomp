#include "common.h"

u32 __OSBusClock : 0x800000F8;

static s32 YearDays[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static s32 LeapYearDays[12] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

static inline BOOL IsLeapYear(s32 year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static inline s32 GetLeapDays(s32 year) {
    if (year < 1) {
        return 0;
    }
    return (year + 3) / 4 - (year - 1) / 100 + (year - 1) / 400;
}

s64 OSGetTime(void) {
    u32 OSGetTick(void);
    u32 hi;
    u32 lo;
    u32 hi2;

    do {
        hi = __mftbu();
        lo = OSGetTick();
        hi2 = __mftbu();
    } while (hi != hi2);
    return ((s64)hi << 32) | lo;
}

#pragma peephole off
static void GetDates(s32 days, s32 *td) {
    s32 year;
    s32 n;
    s32 month;
    s32 *p;

    td[6] = (days + 6) % 7;
    for (year = days / 365; days < (n = year * 365 + GetLeapDays(year)); year--) {
    }
    days -= n;
    td[5] = year;
    td[7] = days;
    p = IsLeapYear(year) ? LeapYearDays : YearDays;
    for (month = 12; days < p[--month];) {
    }
    td[4] = month;
    td[3] = days - p[month] + 1;
}

#pragma push
#pragma dont_inline on
void OSTicksToCalendarTime(s64 ticks, s32 *td)
{
    s32 days;
    s32 secs;
    s64 d;

    d = ticks % (__OSBusClock / 4);
    if (d < 0) {
        d += __OSBusClock / 4;
    }
    td[9] = (s32)(((d) * 8) / ((__OSBusClock / 4) / 125000) % 1000);
    td[8] = (s32)((d) / ((__OSBusClock / 4) / 1000) % 1000);

    ticks -= d;
    days = (s32)((ticks) / (__OSBusClock / 4) / 86400 + 0xB2575);
    secs = (s32)((ticks) / (__OSBusClock / 4) % 86400);
    if (secs < 0) {
        days -= 1;
        secs += 86400;
    }

    GetDates(days, td);
    td[2] = secs / 60 / 60;
    td[1] = (secs / 60) % 60;
    td[0] = secs % 60;
}
#pragma pop
