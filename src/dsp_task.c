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
