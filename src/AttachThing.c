#include "common.h"


s32 AttachThing_Init(void *attach, void *data)
{
    f32 RandomAdd(f32, f32);
    void ExtractColorKey(void *, void *, f32);
    void ExtractScaleKey(void *, void *, f32);
    double cos(double);
    extern u8 g_attachGeometryVTable[];
    extern u8 g_attachVTable[];
    typedef struct { u32 x, y, z, w; } Vec4;

    Thing_Init(attach, (s32)data);
    M2C_FIELD(attach, u32 *, 0x94) = M2C_FIELD(data, u32 *, 0x24);
    *(Vec4 *)((u8 *)attach + 0x40) = *(Vec4 *)((u8 *)data + 0x10);
    M2C_FIELD(attach, void **, 0xB4) = M2C_FIELD(data, void **, 0);
    M2C_FIELD(attach, u8 *, 0x124) = 1;
    M2C_FIELD(attach, u8 *, 0xF4) = 1;
    M2C_FIELD(attach, u8 *, 0x15C) = 1;
    if (M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), void **, 0x2C))
    {
        M2C_FIELD(attach, f32 *, 0xC0) = RandomAdd(*(f32 *)((u8 *)M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), void **, 0x2C) + 0x10),
                                                   *(f32 *)((u8 *)M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), void **, 0x2C) + 0x14));
    }
    if ((M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), u32 *, 0x1C) <= 1) &&
        (M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), u32 *, 0x14) <= 1) &&
        (M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), u32 *, 0xC) <= 1))
    {
        M2C_FIELD(attach, u32 *, 0x74) |= 0x4000;
    }
    if (M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), u8 *, 2) == 1)
    {
        M2C_FIELD(attach, f32 *, 0xB0) = (f32)cos(0.01745329238474369f * M2C_FIELD(M2C_FIELD(M2C_FIELD(attach, void **, 0xB4), void **, 0x28), f32 *, 0x10));
    }
    ExtractColorKey((u8 *)M2C_FIELD(attach, void **, 0xB4) + 0x1C, (u8 *)attach + 0x130, 0.0f);
    ExtractScaleKey((u8 *)M2C_FIELD(attach, void **, 0xB4) + 0x14, (u8 *)attach + 0xD0, 0.0f);
    ExtractScaleKey((u8 *)M2C_FIELD(attach, void **, 0xB4) + 0xC, (u8 *)attach + 0x100, 0.0f);
    M2C_FIELD(attach, u32 *, 0x74) |= 0x400000;
    M2C_FIELD(attach, u32 *, 0x74) |= 2;
    M2C_FIELD(attach, u32 *, 0xB8) = 100;
    M2C_FIELD(attach, f32 *, 0xBC) = 1.0f;
    M2C_FIELD(attach, f32 *, 0) = 0.0f;
    M2C_FIELD(attach, f32 *, 4) = 0.0f;
    M2C_FIELD(attach, f32 *, 8) = 1.0f;
    if (M2C_FIELD(data, u32 *, 0x20) != 0)
    {
        M2C_FIELD(attach, void **, 0x8C) = (void *)g_attachGeometryVTable;
    }
    else
    {
        M2C_FIELD(attach, void **, 0x8C) = (void *)g_attachVTable;
    }
    return 1;
}

s32 AttachThing_Move(void *thing) {
    extern f32 g_fDeltaTimeMS;
    if (!(M2C_FIELD(thing, u32 *, 0x74) & 0x4000)) {
        ExtractColorKey(M2C_FIELD(thing, u8 **, 0xB4) + 0x1C, (u8 *)thing + 0x130, g_fDeltaTimeMS);
        ExtractScaleKey(M2C_FIELD(thing, u8 **, 0xB4) + 0x14, (u8 *)thing + 0xD0, g_fDeltaTimeMS);
        ExtractScaleKey(M2C_FIELD(thing, u8 **, 0xB4) + 0xC, (u8 *)thing + 0x100, g_fDeltaTimeMS);
    }
    return 1;
}

void AttachThing_Draw(void *arg0) {
    M2C_UNK AttachThing_DrawCorona(void *);
    void *Gfx_AddBillBoardSprite(u16);
    M2C_UNK Gfx_SetWorldMatrix(M2C_UNK);
    u8 temp_r0;
    void *temp_r3;
    void *temp_r3_2;

    if (!(M2C_FIELD(arg0, s32 *, 0x74) & 0x10)) {
        Gfx_SetWorldMatrix(0);
        temp_r3 = M2C_FIELD(arg0, void **, 0xB4);
        temp_r0 = M2C_FIELD(temp_r3, u8 *, 0);
        switch ((s32) temp_r0) {                    /* irregular */
        case 0:
            temp_r3_2 = Gfx_AddBillBoardSprite((u16) M2C_FIELD(temp_r3, s32 *, 8));
            M2C_FIELD(temp_r3_2, u8 *, 0x2A) = (u8) M2C_FIELD(M2C_FIELD(arg0, void **, 0xB4), u8 *, 1);
            M2C_FIELD(temp_r3_2, s32 *, 0x24) = (s32) M2C_FIELD(arg0, s32 *, 0x154);
            M2C_FIELD(temp_r3_2, f32 *, 0x1C) = (f32) M2C_FIELD(arg0, f32 *, 0x100);
            M2C_FIELD(temp_r3_2, f32 *, 0xC) = (f32) M2C_FIELD(arg0, f32 *, 0xD0);
            M2C_FIELD(temp_r3_2, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x40);
            M2C_FIELD(temp_r3_2, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x44);
            M2C_FIELD(temp_r3_2, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x48);
            return;
        case 1:
            AttachThing_DrawCorona(arg0);
            break;
        }
    }
}

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

s32 RadarBlip_Init(void *blip, void *init) {
    s32 Thing_Init(void *, s32);

    Thing_Init(blip, 0);
    M2C_FIELD(blip, u32 *, 0x40) = M2C_FIELD(init, u32 *, 0x10);
    M2C_FIELD(blip, u32 *, 0x44) = M2C_FIELD(init, u32 *, 0x14);
    M2C_FIELD(blip, u32 *, 0x48) = M2C_FIELD(init, u32 *, 0x18);
    M2C_FIELD(blip, u32 *, 0x4C) = M2C_FIELD(init, u32 *, 0x1C);
    M2C_FIELD(blip, void **, 0xB0) = M2C_FIELD(init, void **, 0x0);
    M2C_FIELD(blip, s32 *, 0xB4) = 0;
    M2C_FIELD(blip, f32 *, 0xB8) = 1.0f;
    if (M2C_FIELD(M2C_FIELD(blip, u8 **, 0xB0), u8 *, 0x3) == 0) {
        M2C_FIELD(blip, u32 *, 0x74) |= 0x200;
    }
    return 1;
}

s32 RadarBlip_Move(void *blip)
{
    typedef struct { u32 x, y, z, w; } Vec4;

    *(Vec4 *)((u8 *)blip + 0xC0) = *(Vec4 *)((u8 *)blip + 0x40);
    return 1;
}

