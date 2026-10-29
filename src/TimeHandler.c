#include "common.h"


extern s32 g_timer300;
s8 Timer_Interrupt300(void) {
    return (u32)g_timer300 >> 31;
}
