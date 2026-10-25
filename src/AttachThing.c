#include "common.h"


void AttachGeometry_Draw(void *thing) {
    u8 *data;
    void *geom;
    u32 color;

    data = M2C_FIELD(thing, u8 **, 0xB4);
    geom = M2C_FIELD(data, void **, 0x8);
    if (M2C_FIELD(data, u32 *, 0x1C) != 0) {
        color = M2C_FIELD(thing, u32 *, 0x154);
        Gfx_SetBaseColor(&color);
        Gfx_EnableState(0x10);
    }
    M2C_FIELD(thing, f32 *, 0xDC) = 1.0f;
    Gfx_DrawGeometry_QuaternionScalePos(geom, (u8 *)thing + 0x50, (u8 *)thing + 0xD0, (u8 *)thing + 0x40);
    Gfx_DisableState(0x10);
}

s32 AttachThing_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB4);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}
