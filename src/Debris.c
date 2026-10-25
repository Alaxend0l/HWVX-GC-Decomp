#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

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
