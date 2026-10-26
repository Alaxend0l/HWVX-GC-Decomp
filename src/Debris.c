#include "common.h"

typedef struct Debris {
    Vector4 vel;
    u8 mtx[0x30];
    Vector4 pos;
    u8 pad50[0x24];
    u32 flags;
    u8 pad78[0x38];
    Vector4 spinAxis;
    void *visual;
    f32 gravity;
    u8 padC8[4];
    f32 mass;
    Vector4 lastPos;
    Vector4 macroVel;
    s32 macroTime;
} Debris;

void Gfx_DrawGeometry(void *geom, void *mtx);
void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);

s32 Debris_Init(u8 *debris, u8 *data)
{
    Thing_Init(debris, 0);
    *(u8 **)(debris + 0xC0) = *(u8 **)(data + 0x44);
    *(Vector4 *)(debris + 0xB0) = *(Vector4 *)(data + 0x20);
    *(Vector4 *)(debris + 0x50) = *(Vector4 *)(data + 0x10);
    *(Vector4 *)(debris + 0x0) = *(Vector4 *)(data + 0x30);
    ((Vector4 *)data)->w = 1.0f;
    *(Vector4 *)(debris + 0x40) = *(Vector4 *)(data + 0x0);
    *(s32 *)(debris + 0xC8) = *(s32 *)(data + 0x48);
    *(s32 *)(debris + 0xF0) = -1;
    *(Vector4 *)(debris + 0xD0) = *(Vector4 *)(debris + 0x40);
    *(u16 *)(debris + 0x80) = 3;
    *(f32 *)(debris + 0xC4) = *(f32 *)(data + 0x40);
    *(f32 *)(debris + 0xCC) = *(f32 *)(data + 0x4C);
    if (*(u32 *)(data + 0x50) == 0) {
        *(u32 *)(debris + 0x74) |= 2;
    }
    *(u32 *)(debris + 0x74) |= 0x01000000;
    if (*(u8 **)(debris + 0xC0) != NULL) {
        *(f32 *)(debris + 0x70) = *(f32 *)(*(u8 **)(debris + 0xC0) + 0x1C);
    }
    return 1;
}

void Debris_Draw(Debris *debris) {
    if (debris->visual != NULL) {
        if (debris->flags & 0x20000000) {
            Gfx_DrawGeometry(debris->visual, debris->mtx);
        }
        else {
            Gfx_DrawGeometry_MatrixNotCached(debris->visual, debris->mtx);
        }
    }
}

f32 Debris_GetMass(Debris *debris) {
    return debris->mass;
}
