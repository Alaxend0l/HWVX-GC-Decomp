#include "common.h"


void GXInitLightAttnK(GXLightObjInt *lt_obj, f32 k0, f32 k1, f32 k2) {
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

