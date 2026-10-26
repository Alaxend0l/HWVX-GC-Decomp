#include "common.h"


s32 AIGetStreamPlayState(void)
{
    return *(s32 *)0xCC006C00 & 1;
}

s32 AIGetDSPSampleRate(void)
{
    return (((u32) *(u32 *)0xCC006C00 >> 6U) & 1) ^ 1;
}

s32 AIGetStreamSampleRate(void)
{
    return ((u32) *(u32 *)0xCC006C00 >> 1U) & 1;
}

u8 AIGetStreamVolLeft(void)
{
    return (u8) *(s32 *)0xCC006C04;
}

u8 AIGetStreamVolRight(void)
{
    return (u8) ((u32) *(u32 *)0xCC006C04 >> 8U);
}

void AIReset(void) {
    extern s32 __AI_init_flag;
    __AI_init_flag = 0;
}
