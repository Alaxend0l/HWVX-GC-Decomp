#include "common.h"

typedef struct __GXFifoObj {
    u8 *base;
    u8 *top;
    u32 size;
    u32 hiWatermark;
    u32 loWatermark;
    void *rdPtr;
    void *wrPtr;
    s32 count;
    u8 bind_cpu;
    u8 bind_gp;
} __GXFifoObj;

typedef struct GXFifoObj {
    u8 pad[128];
} GXFifoObj;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u16 vNum;
    u16 vLim;
    u32 cpEnable;
    u32 cpStatus;
    u32 cpClr;
} GXData;

extern GXData *gx;
extern volatile u16 *__cpReg;

void GXInitFifoLimits(GXFifoObj *fifo, u32 hiWatermark, u32 loWatermark) {
    __GXFifoObj *realFifo = (__GXFifoObj *)fifo;

    realFifo->hiWatermark = hiWatermark;
    realFifo->loWatermark = loWatermark;
}

#pragma peephole on
void __OSSetInterruptHandler(s16 interrupt, void *handler);
u32 __OSUnmaskInterrupts(u32 mask);
void *OSGetCurrentThread(void);
void GXCPInterruptHandler(s16 interrupt, void *context);
extern void *__GXCurrentThread;
extern s32 GXOverflowSuspendInProgress;
extern void *CPUFifo;
extern void *GPFifo;
void __GXFifoInit(void) {
    __OSSetInterruptHandler(0x11, GXCPInterruptHandler);
    __OSUnmaskInterrupts(0x4000);
    __GXCurrentThread = OSGetCurrentThread();
    GXOverflowSuspendInProgress = 0;
    CPUFifo = 0;
    GPFifo = 0;
}

void __GXFifoReadEnable(void) {
    gx->cpEnable = (gx->cpEnable & ~1) | 1;
    __cpReg[1] = gx->cpEnable;
}

void __GXFifoReadDisable(void) {
    gx->cpEnable = (gx->cpEnable & ~1) | 0;
    __cpReg[1] = gx->cpEnable;
}

void __GXFifoLink(u8 en) {
    gx->cpEnable = (gx->cpEnable & ~0x10) | ((en ? 1 : 0) << 4);
    __cpReg[1] = gx->cpEnable;
}

void __GXWriteFifoIntEnable(u8 hiWatermarkEn, u8 loWatermarkEn) {
    gx->cpEnable = (gx->cpEnable & ~4) | ((u32)hiWatermarkEn << 2);
    gx->cpEnable = (gx->cpEnable & ~8) | ((u32)loWatermarkEn << 3);
    __cpReg[1] = gx->cpEnable;
}

