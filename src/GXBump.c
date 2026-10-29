#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u8 pad4[0x120 - 0x4];
    u32 iref;
    u32 bpMask;
} GXData;

extern GXData *gx;

void __GXFlushTextureState(void) {
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->bpMask;
    gx->bpSentNot = 0;
}
