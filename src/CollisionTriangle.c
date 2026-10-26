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
    u32 time;                    /* 0x28 */
    u8 pad2C[4];                 /* 0x2C */
} CollisionTri;

extern CollisionTri g_aCollisionTriPairCache[4];
extern s32 g_iDeltaTimeMS;

void CollisionTriCache_Clear(void) {
    u32 i;

    for (i = 0; i < 4; i++) {
        g_aCollisionTriPairCache[i].triId0 = 0;
        g_aCollisionTriPairCache[i].triId1 = 0;
        g_aCollisionTriPairCache[i].time = 99999;
    }
}

void CollisionTriCache_Heartbeat(void)
{
    u32 i;

    for (i = 0; i < 4; i++)
    {
        if (g_aCollisionTriPairCache[i].triId0 != 0)
        {
            g_aCollisionTriPairCache[i].time += g_iDeltaTimeMS;
        }
        if (g_aCollisionTriPairCache[i].time > 1000)
        {
            g_aCollisionTriPairCache[i].triId0 = 0;
            g_aCollisionTriPairCache[i].triId1 = 0;
            g_aCollisionTriPairCache[i].time = 99999;
        }
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

void CollisionTriCache_Set(u32 triId0, u32 triId1, Vector4 *pos, Vector4 *normal)
{
    u32 i;
    u32 maxTime;
    u32 slot;

    maxTime = 0;
    slot = 0;
    for (i = 0; i < 4; i++)
    {
        if (triId0 == g_aCollisionTriPairCache[i].triId0 && triId1 == g_aCollisionTriPairCache[i].triId1)
        {
            g_aCollisionTriPairCache[i].time = 0;
            g_aCollisionTriPairCache[i].normal = *normal;
            g_aCollisionTriPairCache[i].pos = *pos;
            return;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (g_aCollisionTriPairCache[i].triId0 == 0)
        {
            slot = i;
            break;
        }
        if (g_aCollisionTriPairCache[i].time > maxTime)
        {
            maxTime = g_aCollisionTriPairCache[i].time;
            slot = i;
        }
    }

    g_aCollisionTriPairCache[slot].time = 0;
    g_aCollisionTriPairCache[slot].normal = *normal;
    g_aCollisionTriPairCache[slot].pos = *pos;
    g_aCollisionTriPairCache[slot].triId0 = triId0;
    g_aCollisionTriPairCache[slot].triId1 = triId1;
}

