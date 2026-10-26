#include "common.h"


s32 next;
void srand(s32 arg0) {
    next = arg0;
}

s32 rand(void) {
    u32 temp_r0;

    temp_r0 = (next * 0x41C64E6D) + 0x3039;
    next = temp_r0;
    return (temp_r0 >> 0x10U) & 0x7FFF;
}

