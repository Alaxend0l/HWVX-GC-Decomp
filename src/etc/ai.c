#include "common.h"


s32 AIRegisterDMACallback(s32 cb) {
    extern s32 __AID_Callback;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(void);
    s32 old;

    old = __AID_Callback;
    OSDisableInterrupts();
    __AID_Callback = cb;
    OSRestoreInterrupts();
    return old;
}

volatile u16 __DSPRegs[32] : 0xCC005000;
void AIStartDMA(void) {
    __DSPRegs[27] |= 0x8000;
}

void AIStopDMA(void) {
    __DSPRegs[27] &= ~0x8000;
}

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

volatile u32 __AIRegs[8] : 0xCC006C00;
void AISetStreamVolLeft(u8 vol) {
    __AIRegs[1] = (__AIRegs[1] & ~0xFF) | (vol & 0xFF);
}

u8 AIGetStreamVolLeft(void)
{
    return (u8) *(s32 *)0xCC006C04;
}

void AISetStreamVolRight(u8 vol) {
    __AIRegs[1] = (__AIRegs[1] & ~0xFF00) | (((u32)vol & 0xFF) << 8);
}

u8 AIGetStreamVolRight(void)
{
    return (u8) ((u32) *(u32 *)0xCC006C04 >> 8U);
}

void AIReset(void) {
    extern s32 __AI_init_flag;
    __AI_init_flag = 0;
}

void OSClearContext(void *);
void OSSetCurrentContext(void *);
extern void (*__AIS_Callback)(u32);
void __AISHandler(s16 interrupt, void *context) {
    u8 exceptionContext[0x2C8];

    __AIRegs[0] |= 8;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AIS_Callback) {
        __AIS_Callback(__AIRegs[2]);
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

