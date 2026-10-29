#include "common.h"


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
