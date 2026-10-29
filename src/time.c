#include "common.h"

typedef s32 time_t;

time_t __get_time(void);

time_t time(time_t *timer) {
    time_t result = __get_time();

    if (timer) {
        *timer = result;
    }
    return result;
}
