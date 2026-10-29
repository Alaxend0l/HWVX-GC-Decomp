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
    u32 IndTexScale0;
    u32 IndTexScale1;
    u8 pad130[0x204 - 0x130];
    u32 genMode;
    u8 pad208[0x4F4 - 0x208];
    u32 dirtyState;
} GXData;

extern GXData *gx;

void GXSetTevIndirect(s32 tev_stage, s32 ind_stage, s32 format, s32 bias_sel, s32 matrix_sel, s32 wrap_s, s32 wrap_t, u8 add_prev, u8 utc_lod, s32 alpha_sel) {
    u32 reg;

    reg = 0;
    reg = (reg & ~0x3) | ind_stage;
    reg = (reg & ~0xC) | (format << 2);
    reg = (reg & ~0x70) | (bias_sel << 4);
    reg = (reg & ~0x180) | (alpha_sel << 7);
    reg = (reg & ~0x1E00) | (matrix_sel << 9);
    reg = (reg & ~0xE000) | (wrap_s << 13);
    reg = (reg & ~0x70000) | (wrap_t << 16);
    reg = (reg & ~0x80000) | (utc_lod << 19);
    reg = (reg & ~0x100000) | (add_prev << 20);
    reg = (reg & ~0xFF000000) | ((tev_stage + 0x10) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 0;
}

void GXSetIndTexCoordScale(s32 ind_state, s32 scale_s, s32 scale_t) {
    switch (ind_state) {
    case 0:
        gx->IndTexScale0 = (gx->IndTexScale0 & ~0xF) | scale_s;
        gx->IndTexScale0 = (gx->IndTexScale0 & ~0xF0) | (scale_t << 4);
        gx->IndTexScale0 = (gx->IndTexScale0 & ~0xFF000000) | (0x25 << 24);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->IndTexScale0;
        break;
    case 1:
        gx->IndTexScale0 = (gx->IndTexScale0 & ~0xF00) | (scale_s << 8);
        gx->IndTexScale0 = (gx->IndTexScale0 & ~0xF000) | (scale_t << 12);
        gx->IndTexScale0 = (gx->IndTexScale0 & ~0xFF000000) | (0x25 << 24);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->IndTexScale0;
        break;
    case 2:
        gx->IndTexScale1 = (gx->IndTexScale1 & ~0xF) | scale_s;
        gx->IndTexScale1 = (gx->IndTexScale1 & ~0xF0) | (scale_t << 4);
        gx->IndTexScale1 = (gx->IndTexScale1 & ~0xFF000000) | (0x26 << 24);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->IndTexScale1;
        break;
    case 3:
        gx->IndTexScale1 = (gx->IndTexScale1 & ~0xF00) | (scale_s << 8);
        gx->IndTexScale1 = (gx->IndTexScale1 & ~0xF000) | (scale_t << 12);
        gx->IndTexScale1 = (gx->IndTexScale1 & ~0xFF000000) | (0x26 << 24);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->IndTexScale1;
        break;
    }
    gx->bpSentNot = 0;
}

void GXSetNumIndStages(u8 nIndStages) {
    gx->genMode = (gx->genMode & ~0x70000) | (nIndStages << 16);
    gx->dirtyState |= 6;
}

#pragma push
#pragma dont_inline on
void GXSetTevDirect(s32 tev_stage) {
    GXSetTevIndirect(tev_stage, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
#pragma pop

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
