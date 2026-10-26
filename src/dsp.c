#include "common.h"


s32 DSPCheckMailToDSP(void)
{
    return ((u16) *(u16 *)0xCC005000 >> 0xFU) & 1;
}

s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag;
    return __DSP_init_flag;
}
