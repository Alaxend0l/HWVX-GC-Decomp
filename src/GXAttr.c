#include "common.h"

typedef struct GXData {
    u8 unk0[0x2];
    u16 bpSentNot;
    u16 vNum;
    u16 vLim;
    u8 unk8[0x14 - 0x8];
    u32 vcdLo;
    u32 vcdHi;
    u32 vatA[8];
    u32 vatB[8];
    u32 vatC[8];
    u8 unk7C[0x80 - 0x7C];
    u32 matIdxA;
    u32 matIdxB;
    u32 indexBase[4];
    u32 indexStride[4];
    u8 unkA8[0x204 - 0xA8];
    u32 genMode;
    u8 unk208[0x41C - 0x208];
    u8 hasNrms;
    u8 hasBiNrms;
    u8 unk41E[0x4F2 - 0x41E];
    u8 dirtyVAT;
    u8 unk4F3;
    u32 dirtyState;
} GXData;

extern GXData *gx;

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXVtxDescList {
    s32 attr;
    s32 type;
} GXVtxDescList;

typedef struct GXVtxAttrFmtList {
    s32 attr;
    s32 cnt;
    s32 type;
    u8 frac;
} GXVtxAttrFmtList;


void __GXXfVtxSpecs(void);
void __GXXfVtxSpecs(void) {
    u32 nCols;
    u32 nNrm;
    u32 nTex;
    u32 reg;

    nCols = ((gx->vcdLo >> 13) & 3) ? 1 : 0;
    nCols += ((gx->vcdLo >> 15) & 3) ? 1 : 0;
    nNrm = gx->hasBiNrms ? 2 : gx->hasNrms ? 1 : 0;
    nTex = (gx->vcdHi & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 2) & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 4) & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 6) & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 8) & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 10) & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 12) & 3) ? 1 : 0;
    nTex += ((gx->vcdHi >> 14) & 3) ? 1 : 0;
    reg = nCols | (nNrm << 2) | (nTex << 4);
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1008;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 1;
}

void __GXSetVCD(void) {
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x50;
    GXWGFifo.u32 = gx->vcdLo;
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x60;
    GXWGFifo.u32 = gx->vcdHi;
    __GXXfVtxSpecs();
}

void __GXCalculateVLim(void) {
    static u8 tbl1[] = { 0, 4, 1, 2 };
    static u8 tbl2[] = { 0, 8, 1, 2 };
    static u8 tbl3[] = { 0, 12, 1, 2 };
    s32 compCnt;
    u32 vlm;
    u32 vl;
    u32 vh;
    u32 va;

    if (gx->vNum == 0) {
        return;
    }
    vl = gx->vcdLo;
    vh = gx->vcdHi;
    va = gx->vatA[0];
    compCnt = (va & 0x200) >> 9;
    vlm = vl & 1;
    vlm += (vl >> 1) & 1;
    vlm += (vl >> 2) & 1;
    vlm += (vl >> 3) & 1;
    vlm += (vl >> 4) & 1;
    vlm += (vl >> 5) & 1;
    vlm += (vl >> 6) & 1;
    vlm += (vl >> 7) & 1;
    vlm += (vl >> 8) & 1;
    vlm += tbl3[(vl >> 9) & 3];
    vlm += tbl3[(vl >> 11) & 3] * ((compCnt == 1) ? 3 : 1);
    vlm += tbl1[(vl >> 13) & 3];
    vlm += tbl1[(vl >> 15) & 3];
    vlm += tbl2[vh & 3];
    vlm += tbl2[(vh >> 2) & 3];
    vlm += tbl2[(vh >> 4) & 3];
    vlm += tbl2[(vh >> 6) & 3];
    vlm += tbl2[(vh >> 8) & 3];
    vlm += tbl2[(vh >> 10) & 3];
    vlm += tbl2[(vh >> 12) & 3];
    vlm += tbl2[(vh >> 14) & 3];
    gx->vLim = vlm;
}

void GXGetVtxDesc(s32 attr, s32 *type) {
    s32 cpType;

    switch (attr) {
    case 0:
        cpType = gx->vcdLo & 1;
        break;
    case 1:
        cpType = (gx->vcdLo >> 1) & 1;
        break;
    case 2:
        cpType = (gx->vcdLo >> 2) & 1;
        break;
    case 3:
        cpType = (gx->vcdLo >> 3) & 1;
        break;
    case 4:
        cpType = (gx->vcdLo >> 4) & 1;
        break;
    case 5:
        cpType = (gx->vcdLo >> 5) & 1;
        break;
    case 6:
        cpType = (gx->vcdLo >> 6) & 1;
        break;
    case 7:
        cpType = (gx->vcdLo >> 7) & 1;
        break;
    case 8:
        cpType = (gx->vcdLo >> 8) & 1;
        break;
    case 9:
        cpType = (gx->vcdLo >> 9) & 3;
        break;
    case 10:
        if (gx->hasNrms) {
            cpType = (gx->vcdLo >> 11) & 3;
        } else {
            cpType = 0;
        }
        break;
    case 25:
        if (gx->hasBiNrms) {
            cpType = (gx->vcdLo >> 11) & 3;
        } else {
            cpType = 0;
        }
        break;
    case 11:
        cpType = (gx->vcdLo >> 13) & 3;
        break;
    case 12:
        cpType = (gx->vcdLo >> 15) & 3;
        break;
    case 13:
        cpType = gx->vcdHi & 3;
        break;
    case 14:
        cpType = (gx->vcdHi >> 2) & 3;
        break;
    case 15:
        cpType = (gx->vcdHi >> 4) & 3;
        break;
    case 16:
        cpType = (gx->vcdHi >> 6) & 3;
        break;
    case 17:
        cpType = (gx->vcdHi >> 8) & 3;
        break;
    case 18:
        cpType = (gx->vcdHi >> 10) & 3;
        break;
    case 19:
        cpType = (gx->vcdHi >> 12) & 3;
        break;
    case 20:
        cpType = (gx->vcdHi >> 14) & 3;
        break;
    default:
        cpType = 0;
        break;
    }
    *type = cpType;
}

void GXGetVtxDescv(GXVtxDescList *vcd) {
    enum { GX_VA_PNMTXIDX, GX_VA_MAX_ATTR = 26, GX_VA_NULL = 0xFF } attr;

    for (attr = GX_VA_PNMTXIDX; attr < GX_VA_MAX_ATTR; attr++) {
        vcd[attr].attr = attr;
        GXGetVtxDesc(attr, &vcd[attr].type);
    }
    vcd[attr].attr = GX_VA_NULL;
}

void GXClearVtxDesc(void) {
    gx->vcdLo = 0;
    gx->vcdLo = ((u32)gx->vcdLo & ~0x600) | ((u32)1 << 9);
    gx->vcdHi = 0;
    gx->hasNrms = 0;
    gx->hasBiNrms = 0;
    gx->dirtyState |= 8;
}

void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac) {
    u32 *va;
    u32 *vb;
    u32 *vc;

    va = &gx->vatA[vtxfmt];
    vb = &gx->vatB[vtxfmt];
    vc = &gx->vatC[vtxfmt];
    switch (attr) {
    case 9:
        *va = ((u32)*va & ~0x1) | (u32)cnt;
        *va = ((u32)*va & ~0xE) | ((u32)type << 1);
        *va = ((u32)*va & ~0x1F0) | ((u32)frac << 4);
        break;
    case 10:
    case 25:
        *va = ((u32)*va & ~0x1C00) | ((u32)type << 10);
        if (cnt == 2) {
            *va = ((u32)*va & ~0x200) | ((u32)1 << 9);
            *va = ((u32)*va & ~0x80000000) | ((u32)1 << 31);
        } else {
            *va = ((u32)*va & ~0x200) | ((u32)cnt << 9);
            *va = ((u32)*va & ~0x80000000) | ((u32)0 << 31);
        }
        break;
    case 11:
        *va = ((u32)*va & ~0x2000) | ((u32)cnt << 13);
        *va = ((u32)*va & ~0x1C000) | ((u32)type << 14);
        break;
    case 12:
        *va = ((u32)*va & ~0x20000) | ((u32)cnt << 17);
        *va = ((u32)*va & ~0x1C0000) | ((u32)type << 18);
        break;
    case 13:
        *va = ((u32)*va & ~0x200000) | ((u32)cnt << 21);
        *va = ((u32)*va & ~0x1C00000) | ((u32)type << 22);
        *va = ((u32)*va & ~0x3E000000) | ((u32)frac << 25);
        break;
    case 14:
        *vb = ((u32)*vb & ~0x1) | (u32)cnt;
        *vb = ((u32)*vb & ~0xE) | ((u32)type << 1);
        *vb = ((u32)*vb & ~0x1F0) | ((u32)frac << 4);
        break;
    case 15:
        *vb = ((u32)*vb & ~0x200) | ((u32)cnt << 9);
        *vb = ((u32)*vb & ~0x1C00) | ((u32)type << 10);
        *vb = ((u32)*vb & ~0x3E000) | ((u32)frac << 13);
        break;
    case 16:
        *vb = ((u32)*vb & ~0x40000) | ((u32)cnt << 18);
        *vb = ((u32)*vb & ~0x380000) | ((u32)type << 19);
        *vb = ((u32)*vb & ~0x7C00000) | ((u32)frac << 22);
        break;
    case 17:
        *vb = ((u32)*vb & ~0x8000000) | ((u32)cnt << 27);
        *vb = ((u32)*vb & ~0x70000000) | ((u32)type << 28);
        *vc = ((u32)*vc & ~0x1F) | (u32)frac;
        break;
    case 18:
        *vc = ((u32)*vc & ~0x20) | ((u32)cnt << 5);
        *vc = ((u32)*vc & ~0x1C0) | ((u32)type << 6);
        *vc = ((u32)*vc & ~0x3E00) | ((u32)frac << 9);
        break;
    case 19:
        *vc = ((u32)*vc & ~0x4000) | ((u32)cnt << 14);
        *vc = ((u32)*vc & ~0x38000) | ((u32)type << 15);
        *vc = ((u32)*vc & ~0x7C0000) | ((u32)frac << 18);
        break;
    case 20:
        *vc = ((u32)*vc & ~0x800000) | ((u32)cnt << 23);
        *vc = ((u32)*vc & ~0x7000000) | ((u32)type << 24);
        *vc = ((u32)*vc & ~0xF8000000) | ((u32)frac << 27);
        break;
    default:
        break;
    }
    gx->dirtyState |= 0x10;
    gx->dirtyVAT |= (u8)(1 << (u8)vtxfmt);
}

void __GXSetVAT(void) {
    u8 i;

    for (i = 0; i < 8; i++) {
        if (gx->dirtyVAT & (1 << i)) {
            GXWGFifo.u8 = 8;
            GXWGFifo.u8 = i | 0x70;
            GXWGFifo.u32 = *(gx->vatA + i);
            GXWGFifo.u8 = 8;
            GXWGFifo.u8 = i | 0x80;
            GXWGFifo.u32 = *(gx->vatB + i);
            GXWGFifo.u8 = 8;
            GXWGFifo.u8 = i | 0x90;
            GXWGFifo.u32 = *(gx->vatC + i);
        }
    }
    gx->dirtyVAT = 0;
}

void GXGetVtxAttrFmt(s32 fmt, s32 attr, s32 *cnt, s32 *type, u8 *frac) {
    u32 *va;
    u32 *vb;
    u32 *vc;

    va = &gx->vatA[fmt];
    vb = &gx->vatB[fmt];
    vc = &gx->vatC[fmt];
    switch (attr) {
    case 9:
        *cnt = *va & 1;
        *type = (*va >> 1) & 7;
        *frac = (*va >> 4) & 0x1F;
        break;
    case 10:
    case 25:
        *cnt = (*va >> 9) & 1;
        if (*cnt == 1 && (*va >> 31)) {
            *cnt = 2;
        }
        *type = (*va >> 10) & 7;
        *frac = 0;
        break;
    case 11:
        *cnt = (*va >> 13) & 1;
        *type = (*va >> 14) & 7;
        *frac = 0;
        break;
    case 12:
        *cnt = (*va >> 17) & 1;
        *type = (*va >> 18) & 7;
        *frac = 0;
        break;
    case 13:
        *cnt = (*va >> 21) & 1;
        *type = (*va >> 22) & 7;
        *frac = (*va >> 25) & 0x1F;
        break;
    case 14:
        *cnt = *vb & 1;
        *type = (*vb >> 1) & 7;
        *frac = (*vb >> 4) & 0x1F;
        break;
    case 15:
        *cnt = (*vb >> 9) & 1;
        *type = (*vb >> 10) & 7;
        *frac = (*vb >> 13) & 0x1F;
        break;
    case 16:
        *cnt = (*vb >> 18) & 1;
        *type = (*vb >> 19) & 7;
        *frac = (*vb >> 22) & 0x1F;
        break;
    case 17:
        *cnt = (*vb >> 27) & 1;
        *type = (*vb >> 28) & 7;
        *frac = *vc & 0x1F;
        break;
    case 18:
        *cnt = (*vc >> 5) & 1;
        *type = (*vc >> 6) & 7;
        *frac = (*vc >> 9) & 0x1F;
        break;
    case 19:
        *cnt = (*vc >> 14) & 1;
        *type = (*vc >> 15) & 7;
        *frac = (*vc >> 18) & 0x1F;
        break;
    case 20:
        *cnt = (*vc >> 23) & 1;
        *type = (*vc >> 24) & 7;
        *frac = (*vc >> 27) & 0x1F;
        break;
    default:
        *cnt = 1;
        *type = 0;
        *frac = 0;
        break;
    }
}

void GXGetVtxAttrFmt(s32 fmt, s32 attr, s32 *cnt, s32 *type, u8 *frac);
void GXGetVtxAttrFmtv(s32 fmt, GXVtxAttrFmtList *list) {
    s32 attr;

    for (attr = 9; attr < 26; attr++) {
        list->attr = attr;
        GXGetVtxAttrFmt(fmt, attr, &list->cnt, &list->type, &list->frac);
        list++;
    }
    list->attr = 0xFF;
}

void GXSetArray(s32 attr, void *base_ptr, u8 stride) {
    s32 cpAttr;
    s32 idx;
    u32 phyAddr;

    if (attr == 0x19) {
        attr = 10;
    }
    cpAttr = attr - 9;
    phyAddr = (u32)base_ptr & 0x3FFFFFFF;
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = cpAttr | 0xA0;
    GXWGFifo.u32 = phyAddr;
    idx = cpAttr - 12;
    if (idx >= 0 && idx < 4) {
        gx->indexBase[idx] = phyAddr;
    }
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = cpAttr | 0xB0;
    GXWGFifo.u32 = stride;
    idx = cpAttr - 12;
    if (idx >= 0 && idx < 4) {
        gx->indexStride[idx] = stride;
    }
}

void GXInvalidateVtxCache(void) {
    *(volatile u8 *)0xCC008000 = 0x48;
}

void GXSetTexCoordGen2(u32 dst_coord, s32 func, s32 src_param, u32 mtx, u8 normalize, u32 pt_texmtx) {
    void __GXSetMatrixIndex(u32 attr);
    u32 reg = 0;
    u32 row;
    u32 form;
    u32 mtxIdAttr;

    form = 0;
    row = 5;
    switch (src_param) {
    case 0: row = 0; form = 1; break;
    case 1: row = 1; form = 1; break;
    case 2: row = 3; form = 1; break;
    case 3: row = 4; form = 1; break;
    case 19: row = 2; break;
    case 20: row = 2; break;
    case 4: row = 5; break;
    case 5: row = 6; break;
    case 6: row = 7; break;
    case 7: row = 8; break;
    case 8: row = 9; break;
    case 9: row = 10; break;
    case 10: row = 11; break;
    case 11: row = 12; break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        break;
    default:
        break;
    }

    switch (func) {
    case 1:
        reg = ((u32)reg & ~0x2) | ((u32)0 << 1);
        reg = ((u32)reg & ~0x4) | ((u32)form << 2);
        reg = ((u32)reg & ~0x70) | ((u32)0 << 4);
        reg = ((u32)reg & ~0xF80) | ((u32)row << 7);
        break;
    case 0:
        reg = ((u32)reg & ~0x2) | ((u32)1 << 1);
        reg = ((u32)reg & ~0x4) | ((u32)form << 2);
        reg = ((u32)reg & ~0x70) | ((u32)0 << 4);
        reg = ((u32)reg & ~0xF80) | ((u32)row << 7);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        reg = ((u32)reg & ~0x2) | ((u32)0 << 1);
        reg = ((u32)reg & ~0x4) | ((u32)form << 2);
        reg = ((u32)reg & ~0x70) | ((u32)1 << 4);
        reg = ((u32)reg & ~0xF80) | ((u32)row << 7);
        reg = ((u32)reg & ~0x7000) | ((u32)(src_param - 12) << 12);
        reg = ((u32)reg & ~0x38000) | ((u32)(func - 2) << 15);
        break;
    case 10:
        reg = ((u32)reg & ~0x2) | ((u32)0 << 1);
        reg = ((u32)reg & ~0x4) | ((u32)form << 2);
        if (src_param == 19) {
            reg = ((u32)reg & ~0x70) | ((u32)2 << 4);
        } else {
            reg = ((u32)reg & ~0x70) | ((u32)3 << 4);
        }
        reg = ((u32)reg & ~0xF80) | ((u32)2 << 7);
        break;
    default:
        break;
    }

    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = dst_coord + 0x1040;
    GXWGFifo.u32 = reg;

    reg = 0;
    reg = ((u32)reg & ~0x3F) | ((u32)(pt_texmtx - 64));
    reg = ((u32)reg & ~0x100) | ((u32)normalize << 8);
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = dst_coord + 0x1050;
    GXWGFifo.u32 = reg;

    switch (dst_coord) {
    case 0:
        gx->matIdxA = ((u32)gx->matIdxA & ~0xFC0) | ((u32)mtx << 6);
        break;
    case 1:
        gx->matIdxA = ((u32)gx->matIdxA & ~0x3F000) | ((u32)mtx << 12);
        break;
    case 2:
        gx->matIdxA = ((u32)gx->matIdxA & ~0xFC0000) | ((u32)mtx << 18);
        break;
    case 3:
        gx->matIdxA = ((u32)gx->matIdxA & ~0x3F000000) | ((u32)mtx << 24);
        break;
    case 4:
        gx->matIdxB = ((u32)gx->matIdxB & ~0x3F) | (u32)mtx;
        break;
    case 5:
        gx->matIdxB = ((u32)gx->matIdxB & ~0xFC0) | ((u32)mtx << 6);
        break;
    case 6:
        gx->matIdxB = ((u32)gx->matIdxB & ~0x3F000) | ((u32)mtx << 12);
        break;
    default:
        gx->matIdxB = ((u32)gx->matIdxB & ~0xFC0000) | ((u32)mtx << 18);
        break;
    }

    mtxIdAttr = dst_coord + 1;
    __GXSetMatrixIndex(mtxIdAttr);
}

void GXSetNumTexGens(u8 nTexGens) {
    gx->genMode = ((u32)gx->genMode & ~0xF) | (u32)nTexGens;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1000 + 0x3F;
    GXWGFifo.u32 = (u32)nTexGens;
    gx->dirtyState |= 4;
}

