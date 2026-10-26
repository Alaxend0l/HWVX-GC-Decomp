#include "common.h"


s32 DSPCheckMailToDSP(void)
{
    return ((u16) *(u16 *)0xCC005000 >> 0xFU) & 1;
}

s32 DSPCheckMailFromDSP(void)
{
    return ((u16) *(u16 *)0xCC005004 >> 0xFU) & 1;
}

s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag;
    return __DSP_init_flag;
}

s32 DSPGetDMAStatus(void)
{
    return *(u16 *)0xCC00500A & 0x200;
}

