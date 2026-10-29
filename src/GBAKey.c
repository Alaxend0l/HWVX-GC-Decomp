#include "common.h"

typedef struct DSPTaskInfo {
    u32 state;
    u32 priority;
    u32 flags;
    void *iram_mmem_addr;
    u32 iram_length;
    u32 iram_addr;
    void *dram_mmem_addr;
    u32 dram_length;
    u32 dram_addr;
    u16 dsp_init_vector;
    u16 dsp_resume_vector;
    void (*init_cb)(u32);
    void (*res_cb)(u32);
    void (*done_cb)(u32);
    void (*req_cb)(u32);
    struct DSPTaskInfo *next;
    struct DSPTaskInfo *prev;
    s64 t_context;
    s64 t_task;
} DSPTaskInfo;

typedef struct GBABootInfo {
    s32 paletteColor;
    s32 paletteSpeed;
    s32 unk8;
    s32 length;
    u8 _pad10[0x68 - 0x10];
} GBABootInfo;

typedef struct GBASecParam {
    u8 readbuf[4];
    s32 paletteColor;
    s32 paletteSpeed;
    s32 length;
    u32 *out;
    u8 _pad14[0x20 - 0x14];
    u32 keyA;
} GBASecParam;

typedef struct GBAKeyControl {
    u8 _pad0[0x40];
    GBABootInfo bootInfo;
    DSPTaskInfo task;
    GBASecParam *param;
    u8 _padFC[4];
} GBAKeyControl;

void DCInvalidateRange(void *addr, u32 nBytes);
void DCFlushRange(void *addr, u32 nBytes);
void *memcpy(void *dst, const void *src, u32 n);
void DSPAddTask(DSPTaskInfo *task);
extern u8 D35[0x380];


void F23(u32 arg0) {
    typedef struct GBAControl {
        u8 _pad0[0xa8];
        u32 realLength;
        u8 _padAC[0x4c];
        u32 xferLength;
        u8 _padFC[4];
    } GBAControl;
    extern GBAControl __GBA[4];
    u32 DSPCheckMailToDSP(void);
    void DSPSendMailToDSP(u32 mail);
    void OSPanic(const char *file, s32 line, const char *msg, ...);

    GBAControl *gba;
    u32 idx;
    u64 pad;

    (void)&pad;
    gba = __GBA;
    if ((u32)&gba->realLength == arg0) {
        idx = 0;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 1;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 2;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 3;
    } else {
        OSPanic("GBAKey.c", 169, "GBA - unexpected dsp call");
        idx = -1;
    }

    gba = &__GBA[idx];
    DSPSendMailToDSP(0xABBA0000);
    while (DSPCheckMailToDSP()) {
    }
    DSPSendMailToDSP(gba->xferLength);
    while (DSPCheckMailToDSP()) {
    }
}



void F25(u32 arg0) {
    typedef struct GBAControl {
        u8 _pad0[0xa8];
        u32 realLength;
        u8 _padAC[0x4c];
        u32 xferLength;
        u8 _padFC[4];
    } GBAControl;
    extern GBAControl __GBA[4];
    void __GBAX01(s32 chan, s32 ret);
    void OSPanic(const char *file, s32 line, const char *msg, ...);

    GBAControl *gba;
    s32 idx;
    s32 chan;

    gba = __GBA;
    if ((u32)&gba->realLength == arg0) {
        idx = 0;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 1;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 2;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 3;
    } else {
        OSPanic("GBAKey.c", 169, "GBA - unexpected dsp call");
        idx = -1;
    }

    chan = idx;
    __GBAX01(chan, 0);
}

void __GBAX02(s32 chan, u8 *readbuf) {
    extern GBAKeyControl __GBA[4];
    GBAKeyControl *gba;
    GBABootInfo *bootInfo;
    GBASecParam *param;
    DSPTaskInfo *task;

    gba = &__GBA[chan];
    bootInfo = &gba->bootInfo;
    param = gba->param;
    memcpy(param, readbuf, 4);
    param->paletteColor = bootInfo->paletteColor;
    param->paletteSpeed = bootInfo->paletteSpeed;
    param->length = bootInfo->length;
    param->out = &param->keyA;
    DCInvalidateRange(&param->keyA, 32);
    DCFlushRange(param, 32);

    task = &gba->task;
    task->priority = 255;
    task->iram_mmem_addr = (void *)((u32)D35 - 0x80000000);
    task->iram_length = sizeof(D35);
    task->iram_addr = 0;
    task->dsp_init_vector = 16;
    task->init_cb = F23;
    task->res_cb = NULL;
    task->done_cb = F25;
    task->req_cb = NULL;
    DSPAddTask(task);
}

