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
    u8 pad1E0[0x204 - 0x1E0];
    u32 genMode;
    u8 pad208[0x4F1 - 0x208];
    u8 dlSaveContext;
    u8 pad4F2[2];
    u32 dirtyState;
} GXData;

extern GXData *gx;

void __GXFlushTextureState(void);

void GXSetFog(s32 type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color) {
    u32 fogclr, fog0, fog1, fog2, fog3;
    f32 A, B, B_mant, C, a, c;
    s32 B_expn;
    u32 b_m;
    u32 b_s;
    u32 a_hex;
    u32 c_hex;

    if (farz == nearz || endz == startz) {
        A = 0.0f;
        B = 0.5f;
        C = 0.0f;
    } else {
        A = (farz * nearz) / ((farz - nearz) * (endz - startz));
        B = farz / (farz - nearz);
        C = startz / (endz - startz);
    }
    B_mant = B;
    B_expn = 0;
    while (B_mant > 1.0) {
        B_mant /= 2;
        B_expn++;
    }
    while (B_mant > 0 && B_mant < 0.5) {
        B_mant *= 2;
        B_expn--;
    }
    a = A / (f32)(1 << (B_expn + 1));
    b_m = (u32)(8.388638e6f * B_mant);
    b_s = B_expn + 1;
    c = C;

    fog1 = 0;
    fog1 = (fog1 & ~0xFFFFFF) | b_m;
    fog1 = (fog1 & 0xFFFFFF) | 0xEF000000;
    fog2 = 0;
    fog2 = (fog2 & ~0x1F) | b_s;
    fog2 = (fog2 & 0xFFFFFF) | 0xF0000000;
    a_hex = *(u32 *)&a;
    c_hex = *(u32 *)&c;
    fog0 = 0;
    fog0 = (fog0 & ~0x7F800) | (((a_hex >> 23) & 0xFF) << 11);
    fog0 = (fog0 & ~0x7FF) | ((a_hex >> 12) & 0x7FF);
    fog0 = (fog0 & ~0x80000) | ((a_hex >> 31) << 19);
    fog0 = (fog0 & 0xFFFFFF) | 0xEE000000;
    fog3 = 0;
    fog3 = (fog3 & ~0x7F800) | (((c_hex >> 23) & 0xFF) << 11);
    fog3 = (fog3 & ~0x7FF) | ((c_hex >> 12) & 0x7FF);
    fog3 = (fog3 & ~0x80000) | ((c_hex >> 31) << 19);
    fog3 = (fog3 & ~0x100000);
    fog3 = (fog3 & ~0xE00000) | (type << 21);
    fog3 = (fog3 & 0xFFFFFF) | 0xF1000000;
    fogclr = 0;
    fogclr = (fogclr & ~0xFF) | color.b;
    fogclr = (fogclr & ~0xFF00) | (color.g << 8);
    fogclr = (fogclr & ~0xFF0000) | (color.r << 16);
    fogclr = (fogclr & 0xFFFFFF) | 0xF2000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = fog0;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = fog1;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = fog2;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = fog3;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = fogclr;
    gx->bpSentNot = 0;
}

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

void GXSetPixelFmt(s32 pix_fmt, s32 z_fmt) {
    u32 oldPeCtrl;
    u8 aa;
    static u32 p2f[8] = { 0, 1, 2, 3, 4, 4, 4, 5 };

    oldPeCtrl = gx->peCtrl;
    gx->peCtrl = ((u32)gx->peCtrl & ~7) | (u32)p2f[pix_fmt];
    gx->peCtrl = ((u32)gx->peCtrl & ~0x38) | ((u32)z_fmt << 3);
    if (oldPeCtrl != gx->peCtrl) {
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->peCtrl;
        if (pix_fmt == 2) {
            aa = 1;
        } else {
            aa = 0;
        }
        gx->genMode = ((u32)gx->genMode & ~0x200) | ((u32)aa << 9);
        gx->dirtyState |= 4;
    }
    if (p2f[pix_fmt] == 4) {
        gx->cmode1 = ((u32)gx->cmode1 & ~0x600) | ((u32)((pix_fmt - 4) & 3) << 9);
        gx->cmode1 = ((u32)gx->cmode1 & ~0xFF000000) | (0x42 << 24);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->cmode1;
    }
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
