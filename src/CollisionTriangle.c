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

void CollisionTriCache_Clear(void) {
    u32 i;

    for (i = 0; i < 4; i++) {
        g_aCollisionTriPairCache[i].triId0 = 0;
        g_aCollisionTriPairCache[i].triId1 = 0;
        g_aCollisionTriPairCache[i].time = 99999;
    }
}

s32 CollisionTriCache_Get(u32 triId0, u32 triId1, Vector4 *pos, Vector4 *normal) {
    u32 i;

    for (i = 0; i < 4; i++) {
        if (triId0 == g_aCollisionTriPairCache[i].triId0 && triId1 == g_aCollisionTriPairCache[i].triId1) {
            *pos = g_aCollisionTriPairCache[i].pos;
            *normal = g_aCollisionTriPairCache[i].normal;
            g_aCollisionTriPairCache[i].time = 0;
            return 1;
        }
    }
    return 0;
}

