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
    u8 pad0[2];
    u16 bpSentNot;
    u8 pad4[0xA8 - 0x4];
    u32 ambColor[2];
    u32 matColor[2];
    u8 padB8[0x204 - 0xB8];
    u32 genMode;
    u8 pad208[0x4F4 - 0x208];
    u32 dirtyState;
} GXData;

extern GXData *gx;

void GXInitLightAttnK(GXLightObjInt *lt_obj, f32 k0, f32 k1, f32 k2) {
    lt_obj->k[0] = k0;
    lt_obj->k[1] = k1;
    lt_obj->k[2] = k2;
}

void GXInitLightDistAttn(GXLightObjInt *lt_obj, f32 ref_dist, f32 ref_br, s32 dist_func) {
    f32 k0;
    f32 k1;
    f32 k2;

    if (ref_dist < 0.0f) {
        dist_func = 0;
    }
    if (ref_br <= 0.0f || ref_br >= 1.0f) {
        dist_func = 0;
    }
    switch (dist_func) {
    case 1:
        k0 = 1.0f;
        k1 = (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.0f;
        break;
    case 2:
        k0 = 1.0f;
        k1 = 0.5f * (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.5f * (1.0f - ref_br) / (ref_dist * (ref_br * ref_dist));
        break;
    case 3:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = (1.0f - ref_br) / (ref_dist * (ref_br * ref_dist));
        break;
    case 0:
    default:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = 0.0f;
        break;
    }
    lt_obj->k[0] = k0;
    lt_obj->k[1] = k1;
    lt_obj->k[2] = k2;
}

void GXInitLightPos(GXLightObjInt *lt_obj, f32 x, f32 y, f32 z) {
    lt_obj->lpos[0] = x;
    lt_obj->lpos[1] = y;
    lt_obj->lpos[2] = z;
}

void GXInitLightColor(GXLightObjInt *lt_obj, GXColor color) {
    lt_obj->Color = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}

void GXLoadLightObjImm(GXLightObjInt *lt_obj, s32 light) {
    u32 addr;
    u32 idx;

    switch (light) {
    case 0x1:
        idx = 0;
        break;
    case 0x2:
        idx = 1;
        break;
    case 0x4:
        idx = 2;
        break;
    case 0x8:
        idx = 3;
        break;
    case 0x10:
        idx = 4;
        break;
    case 0x20:
        idx = 5;
        break;
    case 0x40:
        idx = 6;
        break;
    case 0x80:
        idx = 7;
        break;
    default:
        idx = 0;
        break;
    }
    addr = idx * 0x10 + 0x600;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = addr | 0xF0000;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = lt_obj->Color;
    GXWGFifo.f32 = lt_obj->a[0];
    GXWGFifo.f32 = lt_obj->a[1];
    GXWGFifo.f32 = lt_obj->a[2];
    GXWGFifo.f32 = lt_obj->k[0];
    GXWGFifo.f32 = lt_obj->k[1];
    GXWGFifo.f32 = lt_obj->k[2];
    GXWGFifo.f32 = lt_obj->lpos[0];
    GXWGFifo.f32 = lt_obj->lpos[1];
    GXWGFifo.f32 = lt_obj->lpos[2];
    GXWGFifo.f32 = lt_obj->ldir[0];
    GXWGFifo.f32 = lt_obj->ldir[1];
    GXWGFifo.f32 = lt_obj->ldir[2];
    gx->bpSentNot = 1;
}

void GXSetChanAmbColor(s32 chan, GXColor amb_color) {
    u32 reg = 0;
    u32 colIdx;
    u32 alpha;

    switch (chan) {
    case 0:
        alpha = gx->ambColor[0] & 0xFF;
        reg = ((u32)reg & ~0xFF) | ((u32)alpha);
        reg = ((u32)reg & ~0xFF00) | ((u32)amb_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)amb_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)amb_color.r << 24);
        colIdx = 0;
        break;
    case 1:
        alpha = gx->ambColor[1] & 0xFF;
        reg = ((u32)reg & ~0xFF) | ((u32)alpha);
        reg = ((u32)reg & ~0xFF00) | ((u32)amb_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)amb_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)amb_color.r << 24);
        colIdx = 1;
        break;
    case 2:
        reg = gx->ambColor[0];
        reg = ((u32)reg & ~0xFF) | ((u32)amb_color.a);
        colIdx = 0;
        break;
    case 3:
        reg = gx->ambColor[1];
        reg = ((u32)reg & ~0xFF) | ((u32)amb_color.a);
        colIdx = 1;
        break;
    case 4:
        reg = ((u32)reg & ~0xFF) | ((u32)amb_color.a);
        reg = ((u32)reg & ~0xFF00) | ((u32)amb_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)amb_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)amb_color.r << 24);
        colIdx = 0;
        break;
    case 5:
        reg = ((u32)reg & ~0xFF) | ((u32)amb_color.a);
        reg = ((u32)reg & ~0xFF00) | ((u32)amb_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)amb_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)amb_color.r << 24);
        colIdx = 1;
        break;
    default:
        return;
    }
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = colIdx + 0x100A;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 1;
    gx->ambColor[colIdx] = reg;
}

void GXSetChanMatColor(s32 chan, GXColor mat_color) {
    u32 reg = 0;
    u32 colIdx;
    u32 alpha;

    switch (chan) {
    case 0:
        alpha = gx->matColor[0] & 0xFF;
        reg = ((u32)reg & ~0xFF) | ((u32)alpha);
        reg = ((u32)reg & ~0xFF00) | ((u32)mat_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)mat_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)mat_color.r << 24);
        colIdx = 0;
        break;
    case 1:
        alpha = gx->matColor[1] & 0xFF;
        reg = ((u32)reg & ~0xFF) | ((u32)alpha);
        reg = ((u32)reg & ~0xFF00) | ((u32)mat_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)mat_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)mat_color.r << 24);
        colIdx = 1;
        break;
    case 2:
        reg = gx->matColor[0];
        reg = ((u32)reg & ~0xFF) | ((u32)mat_color.a);
        colIdx = 0;
        break;
    case 3:
        reg = gx->matColor[1];
        reg = ((u32)reg & ~0xFF) | ((u32)mat_color.a);
        colIdx = 1;
        break;
    case 4:
        reg = ((u32)reg & ~0xFF) | ((u32)mat_color.a);
        reg = ((u32)reg & ~0xFF00) | ((u32)mat_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)mat_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)mat_color.r << 24);
        colIdx = 0;
        break;
    case 5:
        reg = ((u32)reg & ~0xFF) | ((u32)mat_color.a);
        reg = ((u32)reg & ~0xFF00) | ((u32)mat_color.b << 8);
        reg = ((u32)reg & ~0xFF0000) | ((u32)mat_color.g << 16);
        reg = ((u32)reg & ~0xFF000000) | ((u32)mat_color.r << 24);
        colIdx = 1;
        break;
    default:
        return;
    }
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = colIdx + 0x100C;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 1;
    gx->matColor[colIdx] = reg;
}

void GXSetNumChans(u8 nChans) {
    gx->genMode = ((u32)gx->genMode & ~(((1 << 3) - 1) << 4)) | ((u32)nChans << 4);
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1000 + 9;
    GXWGFifo.u32 = (u32)nChans;
    gx->dirtyState |= 4;
}

