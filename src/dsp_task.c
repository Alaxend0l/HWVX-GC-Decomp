#include "common.h"

// DSP task queue: tasks are kept in a doubly linked list sorted by priority,
// walked through next (first -> ... -> last); prev links back.
typedef struct DSPTaskInfo {
    volatile u32 state;
    volatile u32 priority;
    volatile u32 flags;
    u8 *iram_mmem_addr;
    u32 iram_length;
    u32 iram_addr;
    u8 *dram_mmem_addr;
    u32 dram_length;
    u32 dram_addr;
    u16 dsp_init_vector;
    u16 dsp_resume_vector;
    void (*init_cb)(void *task);
    void (*res_cb)(void *task);
    void (*done_cb)(void *task);
    void (*req_cb)(void *task);
    struct DSPTaskInfo *next;
    struct DSPTaskInfo *prev;
} DSPTaskInfo;

DSPTaskInfo *__DSP_first_task;
DSPTaskInfo *__DSP_last_task;
DSPTaskInfo *__DSP_curr_task;

extern volatile u16 __DSPRegs[32] : 0xCC005000;
extern DSPTaskInfo *__DSP_rude_task;
extern s32 __DSP_rude_task_pending;

void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
u32 DSPCheckMailFromDSP(void);
u32 DSPReadMailFromDSP(void);
void DSPSendMailToDSP(u32 mail);
u32 DSPCheckMailToDSP(void);
void __DSP_exec_task(DSPTaskInfo *curr, DSPTaskInfo *next);
void __DSP_remove_task(DSPTaskInfo *task);
void __DSP_exec_task(DSPTaskInfo *curr, DSPTaskInfo *next)
{
    if (curr != NULL)
    {
        DSPSendMailToDSP((u32)curr->dram_mmem_addr);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(curr->dram_length);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(curr->dram_addr);
        while (DSPCheckMailToDSP())
            ;
    }
    else
    {
        DSPSendMailToDSP(0);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(0);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(0);
        while (DSPCheckMailToDSP())
            ;
    }

    DSPSendMailToDSP((u32)next->iram_mmem_addr);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(next->iram_length);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(next->iram_addr);
    while (DSPCheckMailToDSP())
        ;

    if (next->state == 0)
    {
        DSPSendMailToDSP((u32)next->dsp_init_vector);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(0);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(0);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(0);
        while (DSPCheckMailToDSP())
            ;
    }
    else
    {
        DSPSendMailToDSP((u32)next->dsp_resume_vector);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP((u32)next->dram_mmem_addr);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(next->dram_length);
        while (DSPCheckMailToDSP())
            ;
        DSPSendMailToDSP(next->dram_addr);
        while (DSPCheckMailToDSP())
            ;
    }
}

u32 DSPCheckMailFromDSP(void);
u32 DSPReadMailFromDSP(void);
void __DSP_debug_printf(const char *fmt, ...);
void __DSP_boot_task(DSPTaskInfo *task)
{
    volatile u32 mail;

    while (!DSPCheckMailFromDSP())
        ;
    mail = DSPReadMailFromDSP();

    DSPSendMailToDSP(0x80F3A001);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP((u32)task->iram_mmem_addr);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(0x80F3C002);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(task->iram_addr & 0xFFFF);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(0x80F3A002);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(task->iram_length);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(0x80F3B002);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(0);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(0x80F3D001);
    while (DSPCheckMailToDSP())
        ;
    DSPSendMailToDSP(task->dsp_init_vector);
    while (DSPCheckMailToDSP())
        ;

    __DSP_debug_printf("DSP is booting task: 0x%08X\n", task);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM MMEM ADDR: 0x%08X\n", (u32)task->iram_mmem_addr);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM DSP ADDR : 0x%08X\n", task->iram_addr);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM LENGTH   : 0x%08X\n", task->iram_length);
    __DSP_debug_printf("__DSP_boot_task()  : DRAM MMEM ADDR : 0x%08X\n", task->dram_length);
    __DSP_debug_printf("__DSP_boot_task()  : Start Vector   : 0x%08X\n", task->dsp_init_vector);
}

void __DSP_insert_task(DSPTaskInfo *task)
{
    DSPTaskInfo *temp;

    if (__DSP_first_task == NULL)
    {
        __DSP_first_task = __DSP_last_task = __DSP_curr_task = task;
        task->next = task->prev = NULL;
    }
    else
    {
        temp = __DSP_first_task;
        while (temp)
        {
            if (task->priority < temp->priority)
            {
                task->prev = temp->prev;
                temp->prev = task;
                task->next = temp;
                if (task->prev == NULL)
                    __DSP_first_task = task;
                else
                    task->prev->next = task;
                break;
            }
            temp = temp->next;
        }
        if (temp == NULL)
        {
            __DSP_last_task->next = task;
            task->next = NULL;
            task->prev = __DSP_last_task;
            __DSP_last_task = task;
        }
    }
}

void __DSP_remove_task(DSPTaskInfo *task)
{
    task->flags = 0;
    task->state = 3;

    if (__DSP_first_task == task)
    {
        if (task->next)
        {
            __DSP_first_task = task->next;
            task->next->prev = NULL;
        }
        else
        {
            __DSP_first_task = __DSP_last_task = __DSP_curr_task = NULL;
        }
    }
    else if (__DSP_last_task == task)
    {
        __DSP_last_task = task->prev;
        task->prev->next = NULL;
        __DSP_curr_task = __DSP_first_task;
    }
    else
    {
        __DSP_curr_task = task->next;
        task->prev->next = task->next;
        task->next->prev = task->prev;
    }
}

