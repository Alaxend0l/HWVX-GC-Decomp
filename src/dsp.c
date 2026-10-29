#include "common.h"

volatile u16 __DSPRegs[32] : 0xCC005000;


s32 DSPCheckMailToDSP(void)
{
    return ((u16) *(u16 *)0xCC005000 >> 0xFU) & 1;
}

s32 DSPCheckMailFromDSP(void)
{
    return ((u16) *(u16 *)0xCC005004 >> 0xFU) & 1;
}

u32 DSPReadMailFromDSP(void)
{
    u16 hi;
    u16 lo;
    hi = ((volatile u16 *)0xCC005000)[2];
    lo = ((volatile u16 *)0xCC005000)[3];
    return (hi << 16) | lo;
}

#pragma scheduling off
void DSPSendMailToDSP(u32 mail)
{
    *(u16 *)0xCC005000 = mail >> 16;
    *(u16 *)0xCC005002 = mail;
}
#pragma scheduling reset
s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag;
    return __DSP_init_flag;
}

s32 DSPGetDMAStatus(void)
{
    return *(u16 *)0xCC00500A & 0x200;
}

