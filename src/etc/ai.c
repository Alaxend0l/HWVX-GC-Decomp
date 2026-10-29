#include "common.h"

volatile u16 __DSPRegs[32] : 0xCC005000;
volatile u32 __AIRegs[8] : 0xCC006C00;


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

s32 OSRestoreInterrupts(s32 level);
void AIInitDMA(u32 start_addr, u32 length) {
    s32 old;

    old = OSDisableInterrupts();
    __DSPRegs[24] = (u16)((__DSPRegs[24] & ~0x3FF) | (start_addr >> 16));
    __DSPRegs[25] = (u16)((__DSPRegs[25] & ~0xFFE0) | (0xFFFF & start_addr));
    __DSPRegs[27] = (u16)((__DSPRegs[27] & ~0x7FFF) | (u16)((length >> 5) & 0xFFFF));
    OSRestoreInterrupts(old);
}

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

void __AIDHandler(s16 interrupt, void *context) {
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    void __AICallbackStackSwitch(void *cb);
    extern void (*__AID_Callback)(void);
    extern u8 *__CallbackStack;
    u8 exceptionContext[0x2C8];
    u16 tmp;

    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0xA0) | 8);
    __DSPRegs[5] = tmp;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AID_Callback) {
        if (__CallbackStack) {
            __AICallbackStackSwitch(__AID_Callback);
        } else {
            __AID_Callback();
        }
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

