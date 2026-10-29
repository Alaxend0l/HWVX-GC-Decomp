#include "common.h"

typedef s64 OSTime;
typedef u32 time_t;

OSTime OSGetTime(void);

#define OS_BUS_CLOCK (*(u32 *)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)

time_t __get_time(void) {
    return (time_t)OSTicksToSeconds(OSGetTime()) + 0xBC17C200;
}
