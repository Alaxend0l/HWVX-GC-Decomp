#include "common.h"

// DSP task queue: tasks are kept in a doubly linked list sorted by priority,
// walked through next (first -> ... -> last); prev links back.
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

DSPTaskInfo *__DSP_first_task;
DSPTaskInfo *__DSP_last_task;
DSPTaskInfo *__DSP_curr_task;

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

