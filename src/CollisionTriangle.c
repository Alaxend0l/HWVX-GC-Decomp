#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct CollisionTri {
    Vector4 pos;                 /* 0x00 */
    Vector4 normal;              /* 0x10 */
    u32 triId1;                  /* 0x20 */
    u32 triId0;                  /* 0x24 */
    s32 time;                    /* 0x28 */
    u8 pad2C[4];                 /* 0x2C */
} CollisionTri;

extern CollisionTri g_aCollisionTriPairCache[4];
