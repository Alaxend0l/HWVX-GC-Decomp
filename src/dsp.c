#include "common.h"

volatile u16 __DSPRegs[32] : 0xCC005000;

typedef struct DSPTaskInfo {
    volatile u32 state;
    volatile u32 priority;
    volatile u32 flags;
    u8 unkC[0x28 - 0xC];
    void (*init_cb)(void *task);
    void (*res_cb)(void *task);
    void (*done_cb)(void *task);
    void (*req_cb)(void *task);
    struct DSPTaskInfo *next;
    struct DSPTaskInfo *prev;
} DSPTaskInfo;

extern DSPTaskInfo *__DSP_first_task;

void __DSP_insert_task(DSPTaskInfo *task);
void __DSP_boot_task(DSPTaskInfo *task);


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
    return (__DSPRegs[2] << 16) | __DSPRegs[3];
}

#pragma scheduling off
void DSPSendMailToDSP(u16 arg0) {
    *(u16 *)0xCC005000 = (u16) (arg0 >> 0x10U);
    *(u16 *)0xCC005002 = arg0;
}
#pragma scheduling reset
s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag;
    return __DSP_init_flag;
}

void DSPReset(void) {
    extern s32 __DSP_init_flag;
    BOOL old;
    u16 tmp;

    old = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    __DSPRegs[5] = (u16)((tmp & ~(0x08 | 0x20 | 0x80)) | 0x800 | 0x01);
    __DSP_init_flag = 0;
    OSRestoreInterrupts(old);
}

void DSPHalt(void) {
    BOOL old;
    u16 tmp;

    old = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    __DSPRegs[5] = (u16)((tmp & ~(0x08 | 0x20 | 0x80)) | 0x04);
    OSRestoreInterrupts(old);
}

s32 DSPGetDMAStatus(void)
{
    return *(u16 *)0xCC00500A & 0x200;
}

