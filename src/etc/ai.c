#include "common.h"

volatile u16 __DSPRegs[32] : 0xCC005000;
volatile u32 __AIRegs[8] : 0xCC006C00;

void AISetStreamPlayState(u32 state);
s32 AIGetStreamPlayState(void);
void __AI_SRC_INIT(void);
s32 AIGetStreamSampleRate(void);
u8 AIGetStreamVolLeft(void);
u8 AIGetStreamVolRight(void);
void AISetStreamVolLeft(u8 vol);
void AISetStreamVolRight(u8 vol);


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

void AISetStreamPlayState(u32 state) {
    s32 enabled;
    u8 volRight;
    u8 volLeft;

    if (state == AIGetStreamPlayState()) {
        return;
    }
    if ((u32)AIGetStreamSampleRate() == 0 && state == 1) {
        volRight = AIGetStreamVolRight();
        volLeft = AIGetStreamVolLeft();
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        enabled = OSDisableInterrupts();
        __AI_SRC_INIT();
        __AIRegs[0] = (__AIRegs[0] & ~0x20) | 0x20;
        __AIRegs[0] = (__AIRegs[0] & ~1) | 1;
        OSRestoreInterrupts(enabled);
        AISetStreamVolLeft(volRight);
        AISetStreamVolRight(volLeft);
    } else {
        __AIRegs[0] = (__AIRegs[0] & ~1) | state;
    }
}

s32 AIGetStreamPlayState(void)
{
    return __AIRegs[0] & 1;
}

s32 AIGetDSPSampleRate(void)
{
    return (((u32) *(u32 *)0xCC006C00 >> 6U) & 1) ^ 1;
}

void __AI_set_stream_sample_rate(u32 rate) {
    s32 old;
    s32 state;
    u8 left;
    u8 right;
    u32 temp_r26;

    if (rate != AIGetStreamSampleRate()) {
        state = AIGetStreamPlayState();
        left = AIGetStreamVolLeft();
        right = AIGetStreamVolRight();
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        temp_r26 = __AIRegs[0] & 0x40;
        __AIRegs[0] &= ~0x40;
        old = OSDisableInterrupts();
        __AI_SRC_INIT();
        __AIRegs[0] |= temp_r26;
        __AIRegs[0] = (__AIRegs[0] & ~0x20) | 0x20;
        __AIRegs[0] = (__AIRegs[0] & ~2) | (rate << 1);
        OSRestoreInterrupts(old);
        AISetStreamPlayState(state);
        AISetStreamVolLeft(left);
        AISetStreamVolRight(right);
    }
}

s32 AIGetStreamSampleRate(void)
{
    return ((u32) *(u32 *)0xCC006C00 >> 1U) & 1;
}

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

