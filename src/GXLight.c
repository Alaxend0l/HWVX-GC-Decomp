#include "common.h"


void GXInitLightAttnK(GXLightObjInt *lt_obj, f32 k0, f32 k1, f32 k2) {
    lt_obj->k[0] = k0;
    lt_obj->k[1] = k1;
    lt_obj->k[2] = k2;
}
