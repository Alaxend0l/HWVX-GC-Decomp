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
    u8 pad6[0x7C - 0x6];
    u32 lpSize;
    u8 pad80[0x1D0 - 0x80];
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

void __GXFlushTextureState(void);

void GXSetFogRangeAdj(u8 enable, u16 center, u16 *table) {
    u32 i;
    u32 range_adj;
    u32 range_c;

    if (enable) {
        for (i = 0; i < 10; i += 2) {
            range_adj = 0;
            range_adj = (range_adj & ~0xFFF) | table[i];
            range_adj = (range_adj & ~0xFFF000) | (table[i + 1] << 12);
            range_adj = (range_adj & ~0xFF000000) | ((i / 2 + 0xE9) << 24);
            GXWGFifo.u8 = 0x61;
            GXWGFifo.u32 = range_adj;
        }
    }
    range_c = 0;
    range_c = (range_c & ~0x3FF) | (center + 342);
    range_c = (range_c & ~0x400) | (enable << 10);
    range_c = (range_c & ~0xFF000000) | (0xE8 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = range_c;
    gx->bpSentNot = 0;
}

void GXSetBlendMode(s32 type, s32 src_factor, s32 dst_factor, s32 op) {
    gx->cmode0 = ((u32)gx->cmode0 & ~1) | ((u32)(type == 1 || type == 3));
    gx->cmode0 = ((u32)gx->cmode0 & ~0x800) | ((u32)(type == 3) << 11);
    gx->cmode0 = ((u32)gx->cmode0 & ~2) | ((u32)(type == 2) << 1);
    gx->cmode0 = ((u32)gx->cmode0 & ~0xF000) | ((u32)op << 12);
    gx->cmode0 = ((u32)gx->cmode0 & ~0x700) | ((u32)src_factor << 8);
    gx->cmode0 = ((u32)gx->cmode0 & ~0xE0) | ((u32)dst_factor << 5);
    gx->cmode0 = ((u32)gx->cmode0 & ~0xFF000000) | (0x41 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cmode0;
    gx->bpSentNot = 0;
}

void GXSetColorUpdate(u8 update_enable) {
    gx->cmode0 = (gx->cmode0 & ~8) | (update_enable << 3);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cmode0;
    gx->bpSentNot = 0;
}

void GXSetAlphaUpdate(u8 update_enable) {
    gx->cmode0 = (gx->cmode0 & ~0x10) | (update_enable << 4);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cmode0;
    gx->bpSentNot = 0;
}

void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable) {
    gx->zmode = (gx->zmode & ~1) | compare_enable;
    gx->zmode = (gx->zmode & ~0xE) | (func << 1);
    gx->zmode = (gx->zmode & ~0x10) | (update_enable << 4);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->zmode;
    gx->bpSentNot = 0;
}

void GXSetZCompLoc(u8 before_tex) {
    gx->peCtrl = (gx->peCtrl & ~0x40) | (before_tex << 6);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->peCtrl;
    gx->bpSentNot = 0;
}

void GXSetDither(u8 dither) {
    gx->cmode0 = (gx->cmode0 & ~4) | (dither << 2);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cmode0;
    gx->bpSentNot = 0;
}

void GXSetDstAlpha(u8 enable, u8 alpha) {
    gx->cmode1 = (gx->cmode1 & ~0xFF) | alpha;
    gx->cmode1 = (gx->cmode1 & ~0x100) | (enable << 8);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cmode1;
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

#pragma peephole on
void GXSetFieldMode(u8 field_mode, u8 half_aspect_ratio) {
    gx->lpSize = (gx->lpSize & ~0x400000) | (half_aspect_ratio << 22);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->lpSize;
    __GXFlushTextureState();
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = field_mode | 0x68000000;
    __GXFlushTextureState();
}
