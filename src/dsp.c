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

static inline void DSPAssertInt(void) {
    BOOL old;
    u16 tmp;

    old = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~(0x80 | 0x20 | 0x08)) | 0x02);
    __DSPRegs[5] = tmp;
    OSRestoreInterrupts(old);
}


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

#pragma peephole on
#pragma scheduling on
void DSPInit(void) {
    extern s32 __DSP_init_flag;
    extern DSPTaskInfo *__DSP_tmp_task;
    extern DSPTaskInfo *__DSP_curr_task;
    extern DSPTaskInfo *__DSP_last_task;
    extern void __DSPHandler(s16, void *);
    void __DSP_debug_printf(const char *fmt, ...);
    BOOL old;
    u16 tmp;

    __DSP_debug_printf("DSPInit(): Build Date: %s %s\n", "Apr  5 2004", "04:15:32");
    if (__DSP_init_flag == 1) {
        return;
    }
    old = OSDisableInterrupts();
    __OSSetInterruptHandler(7, __DSPHandler);
    __OSUnmaskInterrupts(0x01000000);
    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0xA8) | 0x800);
    __DSPRegs[5] = tmp;
    tmp = __DSPRegs[5];
    tmp = (u16)(tmp & ~0xAC);
    __DSPRegs[5] = tmp;
    __DSP_tmp_task = NULL;
    __DSP_curr_task = NULL;
    __DSP_last_task = NULL;
    __DSP_first_task = NULL;
    __DSP_init_flag = 1;
    OSRestoreInterrupts(old);
}
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

DSPTaskInfo *DSPAddTask(DSPTaskInfo *task) {
    BOOL old;

    old = OSDisableInterrupts();
    __DSP_insert_task(task);
    task->state = 0;
    task->flags = 1;
    OSRestoreInterrupts(old);
    if (task == __DSP_first_task) {
        __DSP_boot_task(task);
    }
    return task;
}

#pragma peephole on
#pragma scheduling on
DSPTaskInfo *DSPAssertTask(DSPTaskInfo *task) {
    extern DSPTaskInfo *__DSP_curr_task;
    extern DSPTaskInfo *__DSP_rude_task;
    extern s32 __DSP_rude_task_pending;
    BOOL old;

    old = OSDisableInterrupts();
    if (__DSP_curr_task == task) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = 1;
        OSRestoreInterrupts(old);
        return task;
    }
    if (task->priority < __DSP_curr_task->priority) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = 1;
        if (__DSP_curr_task->state == 1) {
            BOOL old2;
            u16 tmp;
            old2 = OSDisableInterrupts();
            tmp = __DSPRegs[5];
            tmp = (u16)((tmp & ~0xA8) | 0x02);
            __DSPRegs[5] = tmp;
            OSRestoreInterrupts(old2);
        }
        OSRestoreInterrupts(old);
        return task;
    }
    OSRestoreInterrupts(old);
    return NULL;
}
