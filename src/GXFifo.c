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

void GXInitFifoPtrs(GXFifoObj *fifo, void *readPtr, void *writePtr);
void GXInitFifoLimits(GXFifoObj *fifo, u32 hiWatermark, u32 loWatermark);

#pragma peephole on
void GXInitFifoBase(GXFifoObj *fifo, void *base, u32 size) {
    __GXFifoObj *realFifo = (__GXFifoObj *)fifo;

    realFifo->base = base;
    realFifo->top = (u8 *)base + size - 4;
    realFifo->size = size;
    realFifo->count = 0;
    GXInitFifoLimits(fifo, size - 0x4000, (size >> 1) & ~0x1F);
    GXInitFifoPtrs(fifo, base, base);
    return;
}

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void GXInitFifoPtrs(GXFifoObj *fifo, void *readPtr, void *writePtr) {
    __GXFifoObj *realFifo = (__GXFifoObj *)fifo;
    s32 enabled;

    enabled = OSDisableInterrupts();
    realFifo->rdPtr = readPtr;
    realFifo->wrPtr = writePtr;
    realFifo->count = (u8 *)writePtr - (u8 *)readPtr;
    if (realFifo->count < 0) {
        realFifo->count += realFifo->size;
    }
    OSRestoreInterrupts(enabled);
}

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
extern volatile u32 *__piReg;
extern u8 CPGPLinked;
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

void __GXWriteFifoIntReset(u8 hiWatermarkClr, u8 loWatermarkClr) {
    gx->cpClr = (gx->cpClr & ~1) | (u32)hiWatermarkClr;
    gx->cpClr = (gx->cpClr & ~2) | ((u32)loWatermarkClr << 1);
    __cpReg[2] = gx->cpClr;
}

