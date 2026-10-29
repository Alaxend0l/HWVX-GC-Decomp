#include "common.h"

typedef struct GXData {
    u8 unk0[0x14];
    u32 vcdLo;
    u32 vcdHi;
    u32 vatA[8];
    u32 vatB[8];
    u32 vatC[8];
    u8 unk7C[0x88 - 0x7C];
    u32 indexBase[4];
    u32 indexStride[4];
    u8 unkA8[0x204 - 0xA8];
    u32 genMode;
    u8 unk208[0x41C - 0x208];
    u8 hasNrms;
    u8 hasBiNrms;
    u8 unk41E[0x4F4 - 0x41E];
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
void __GXSetVCD(void) {
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x50;
    GXWGFifo.u32 = gx->vcdLo;
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x60;
    GXWGFifo.u32 = gx->vcdHi;
    __GXXfVtxSpecs();
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

void GXSetNumTexGens(u8 nTexGens) {
    gx->genMode = ((u32)gx->genMode & ~0xF) | (u32)nTexGens;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1000 + 0x3F;
    GXWGFifo.u32 = (u32)nTexGens;
    gx->dirtyState |= 4;
}

