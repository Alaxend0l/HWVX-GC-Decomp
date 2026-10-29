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
    u8 pad4[0x100 - 0x4];
    u32 tref[8];
    u8 pad120[0x130 - 0x120];
    u32 tevc[16];
    u32 teva[16];
    u32 tevKsel[8];
    u8 pad1D0[0x204 - 0x1D0];
    u32 genMode;
    u8 pad208[0x49C - 0x208];
    u32 texmapId[16];
    u8 pad4DC[0x4E0 - 0x4DC];
    u32 tevTcEnab;
    u8 pad4E4[0x4F4 - 0x4E4];
    u32 dirtyState;
} GXData;

extern GXData *gx;

void GXSetTevColorIn(s32 stage, s32 a, s32 b, s32 c, s32 d) {
    u32 *pTevReg;

    pTevReg = &gx->tevc[stage];
    *pTevReg = ((u32)*pTevReg & ~0xF000) | ((u32)a << 12);
    *pTevReg = ((u32)*pTevReg & ~0xF00) | ((u32)b << 8);
    *pTevReg = ((u32)*pTevReg & ~0xF0) | ((u32)c << 4);
    *pTevReg = ((u32)*pTevReg & ~0xF) | (u32)d;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *pTevReg;
    gx->bpSentNot = 0;
}

void GXSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d) {
    u32 *pTevReg;

    pTevReg = &gx->teva[stage];
    *pTevReg = ((u32)*pTevReg & ~0xE000) | ((u32)a << 13);
    *pTevReg = ((u32)*pTevReg & ~0x1C00) | ((u32)b << 10);
    *pTevReg = ((u32)*pTevReg & ~0x380) | ((u32)c << 7);
    *pTevReg = ((u32)*pTevReg & ~0x70) | ((u32)d << 4);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *pTevReg;
    gx->bpSentNot = 0;
}

void GXSetTevColor(s32 id, GXColor color) {
    u32 regRA;
    u32 regBG;

    regRA = 0;
    regRA = ((u32)regRA & ~0x7FF) | (u32)color.r;
    regRA = ((u32)regRA & ~0x7FF000) | ((u32)color.a << 12);
    regRA = ((u32)regRA & ~0xFF000000) | ((u32)(0xE0 + id * 2) << 24);
    regBG = 0;
    regBG = ((u32)regBG & ~0x7FF) | (u32)color.b;
    regBG = ((u32)regBG & ~0x7FF000) | ((u32)color.g << 12);
    regBG = ((u32)regBG & ~0xFF000000) | ((u32)(0xE1 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regRA;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    gx->bpSentNot = 0;
}

void GXSetTevColorS10(s32 id, GXColorS10 color) {
    u32 regRA;
    u32 regBG;

    regRA = 0;
    regRA = ((u32)regRA & ~0x7FF) | ((u32)color.r & 0x7FF);
    regRA = ((u32)regRA & ~0x7FF000) | (((u32)color.a & 0x7FF) << 12);
    regRA = ((u32)regRA & ~0xFF000000) | ((u32)(0xE0 + id * 2) << 24);
    regBG = 0;
    regBG = ((u32)regBG & ~0x7FF) | ((u32)color.b & 0x7FF);
    regBG = ((u32)regBG & ~0x7FF000) | (((u32)color.g & 0x7FF) << 12);
    regBG = ((u32)regBG & ~0xFF000000) | ((u32)(0xE1 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regRA;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    gx->bpSentNot = 0;
}

void GXSetTevKColor(s32 id, GXColor color) {
    u32 regRA;
    u32 regBG;

    regRA = 0;
    regRA = ((u32)regRA & ~0xFF) | (u32)color.r;
    regRA = ((u32)regRA & ~0xFF000) | ((u32)color.a << 12);
    regRA = ((u32)regRA & ~0xF00000) | ((u32)8 << 20);
    regRA = ((u32)regRA & ~0xFF000000) | ((u32)(0xE0 + id * 2) << 24);
    regBG = 0;
    regBG = ((u32)regBG & ~0xFF) | (u32)color.b;
    regBG = ((u32)regBG & ~0xFF000) | ((u32)color.g << 12);
    regBG = ((u32)regBG & ~0xF00000) | ((u32)8 << 20);
    regBG = ((u32)regBG & ~0xFF000000) | ((u32)(0xE1 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regRA;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    gx->bpSentNot = 0;
}

void GXSetTevKColorSel(s32 stage, s32 sel) {
    u32 *Kreg;

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        *Kreg = ((u32)*Kreg & ~0x7C000) | ((u32)sel << 14);
    } else {
        *Kreg = ((u32)*Kreg & ~0x1F0) | ((u32)sel << 4);
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *Kreg;
    gx->bpSentNot = 0;
}

void GXSetTevKAlphaSel(s32 stage, s32 sel) {
    u32 *Kreg;

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        *Kreg = ((u32)*Kreg & ~0xF80000) | ((u32)sel << 19);
    } else {
        *Kreg = ((u32)*Kreg & ~0x3E00) | ((u32)sel << 9);
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *Kreg;
    gx->bpSentNot = 0;
}

void GXSetTevSwapMode(s32 stage, s32 ras_sel, s32 tex_sel) {
    u32 *pTevReg;

    pTevReg = &gx->teva[stage];
    *pTevReg = ((u32)*pTevReg & ~0x3) | (u32)ras_sel;
    *pTevReg = ((u32)*pTevReg & ~0xC) | ((u32)tex_sel << 2);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *pTevReg;
    gx->bpSentNot = 0;
}

void GXSetAlphaCompare(s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1) {
    u32 reg = 0;

    reg = ((u32)reg & ~0xFF) | (u32)ref0;
    reg = ((u32)reg & ~0xFF00) | ((u32)ref1 << 8);
    reg = ((u32)reg & ~0x70000) | ((u32)comp0 << 16);
    reg = ((u32)reg & ~0x380000) | ((u32)comp1 << 19);
    reg = ((u32)reg & ~0xC00000) | ((u32)op << 22);
    reg = ((u32)reg & ~0xFF000000) | ((u32)0xF3 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 0;
}

void GXSetZTexture(s32 op, s32 fmt, u32 bias) {
    u32 zenv0;
    u32 zenv1;
    u32 type;

    zenv0 = 0;
    zenv0 = ((u32)zenv0 & ~0xFFFFFF) | (u32)bias;
    zenv0 = ((u32)zenv0 & 0xFFFFFF) | 0xF4000000;
    zenv1 = 0;
    switch (fmt) {
    case 0x11:
        type = 0;
        break;
    case 0x13:
        type = 1;
        break;
    case 0x16:
        type = 2;
        break;
    default:
        type = 2;
        break;
    }
    zenv1 = ((u32)zenv1 & ~3) | (u32)type;
    zenv1 = ((u32)zenv1 & ~0xC) | ((u32)op << 2);
    zenv1 = ((u32)zenv1 & 0xFFFFFF) | 0xF5000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = zenv0;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = zenv1;
    gx->bpSentNot = 0;
}

void GXSetTevOrder(s32 stage, s32 coord, s32 map, s32 color) {
    static s32 c2r[] = { 0, 1, 0, 1, 0, 1, 7, 5, 6 };
    u32 *ptref;
    u32 tmap;
    u32 tcoord;

    ptref = &gx->tref[stage / 2];
    gx->texmapId[stage] = map;
    tmap = map & ~0x100;
    tmap = (tmap >= 8) ? 0 : tmap;
    if (coord >= 8) {
        tcoord = 0;
        gx->tevTcEnab = gx->tevTcEnab & ~(1 << stage);
    } else {
        tcoord = coord;
        gx->tevTcEnab = gx->tevTcEnab | (1 << stage);
    }
    if (stage & 1) {
        *ptref = (*ptref & ~0x7000) | (tmap << 12);
        *ptref = (*ptref & ~0x38000) | (tcoord << 15);
        *ptref = (*ptref & ~0x380000) | (((color == 0xFF) ? 7 : c2r[color]) << 19);
        *ptref = (*ptref & ~0x40000) | ((map != 0xFF && !(map & 0x100)) << 18);
    } else {
        *ptref = (*ptref & ~0x7) | tmap;
        *ptref = (*ptref & ~0x38) | (tcoord << 3);
        *ptref = (*ptref & ~0x380) | (((color == 0xFF) ? 7 : c2r[color]) << 7);
        *ptref = (*ptref & ~0x40) | ((map != 0xFF && !(map & 0x100)) << 6);
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *ptref;
    gx->bpSentNot = 0;
    gx->dirtyState |= 1;
}

void GXSetNumTevStages(u8 nStages) {
    gx->genMode = ((u32)gx->genMode & ~(((1 << 4) - 1) << 10)) | ((u32)(nStages - 1) << 10);
    gx->dirtyState |= 4;
}

