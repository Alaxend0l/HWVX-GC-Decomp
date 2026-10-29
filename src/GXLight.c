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
    u8 pad0[0x204];
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

void GXSetNumChans(u8 nChans) {
    gx->genMode = ((u32)gx->genMode & ~(((1 << 3) - 1) << 4)) | ((u32)nChans << 4);
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1000 + 9;
    GXWGFifo.u32 = (u32)nChans;
    gx->dirtyState |= 4;
}

