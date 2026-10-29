#include "common.h"

volatile u16 __DSPRegs[32] : 0xCC005000;


s32 ARRegisterDMACallback(s32 cb) {
    extern s32 __AR_Callback;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(void);
    s32 old;

    old = __AR_Callback;
    OSDisableInterrupts();
    __AR_Callback = cb;
    OSRestoreInterrupts();
    return old;
}

void OSRestoreInterrupts(s32 level);
void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length) {
    s32 enabled = OSDisableInterrupts();
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3ff) | (mainmem_addr >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xffe0) | (0xffff & mainmem_addr));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3ff) | (aram_addr >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xffe0) | (0xffff & aram_addr));
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x8000) | (type << 15));
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3ff) | (length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xffe0) | (0xffff & length));
    OSRestoreInterrupts(enabled);
}

void ARReset(void) {
    extern s32 __AR_init_flag;
    __AR_init_flag = 0;
}

s32 ARGetBaseAddress(void)
{
    return 0x4000;
}

s32 ARGetSize(void) {
    extern s32 __AR_Size;
    return __AR_Size;
}

void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
static void __ARHandler(s16 interrupt, void *context) {
    extern void (*__AR_Callback)(void);
    u8 exceptionContext[0x2C8];
    u16 tmp;

    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0x88) | 0x20);
    __DSPRegs[5] = tmp;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AR_Callback) {
        __AR_Callback();
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

