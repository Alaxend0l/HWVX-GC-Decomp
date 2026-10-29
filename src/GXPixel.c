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
    u16 vNum;
    u8 pad6[0x1D0 - 0x6];
    u32 cmode0;
    u32 cmode1;
    u32 zmode;
    u32 peCtrl;
    u8 pad1E0[0x4F1 - 0x1E0];
    u8 dlSaveContext;
    u8 pad4F2[2];
    u32 dirtyState;
} GXData;

extern GXData *gx;

void GXSetColorUpdate(u8 update_enable) {
    gx->cmode0 = (gx->cmode0 & ~8) | (update_enable << 3);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cmode0;
    gx->bpSentNot = 0;
}

void GXSetFieldMask(u8 odd_mask, u8 even_mask) {
    u32 reg;

    reg = even_mask;
    reg = (reg & ~2) | (odd_mask << 1);
    reg = (reg & 0xFFFFFF) | 0x44000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 0;
}
