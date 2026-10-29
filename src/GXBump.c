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
    u8 pad128[0x204 - 0x128];
    u32 genMode;
} GXData;

extern GXData *gx;

void __GXUpdateBPMask(void) {
    u32 nIndStages;
    u32 i;
    u32 tmap;
    u32 new_imask;

    new_imask = 0;
    nIndStages = (gx->genMode >> 16) & 7;
    for (i = 0; i < nIndStages; i++) {
        switch (i) {
        case 0:
            tmap = gx->iref & 7;
            break;
        case 1:
            tmap = (gx->iref >> 6) & 7;
            break;
        case 2:
            tmap = (gx->iref >> 12) & 7;
            break;
        case 3:
            tmap = (gx->iref >> 18) & 7;
            break;
        }
        new_imask |= 1 << tmap;
    }
    if ((u8)gx->bpMask != new_imask) {
        gx->bpMask = (gx->bpMask & ~0xFF) | new_imask;
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->bpMask;
        gx->bpSentNot = 0;
    }
}

void __GXFlushTextureState(void) {
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->bpMask;
    gx->bpSentNot = 0;
}
