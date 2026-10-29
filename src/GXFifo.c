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
} GXData;

extern GXData *gx;
extern volatile u16 *__cpReg;

void GXInitFifoLimits(GXFifoObj *fifo, u32 hiWatermark, u32 loWatermark) {
    __GXFifoObj *realFifo = (__GXFifoObj *)fifo;

    realFifo->hiWatermark = hiWatermark;
    realFifo->loWatermark = loWatermark;
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

