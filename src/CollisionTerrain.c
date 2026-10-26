#include "common.h"

typedef struct Vector4 { f32 x; f32 y; f32 z; f32 w; } Vector4;
typedef struct Matrix4 { Vector4 v[4]; } Matrix4;

typedef struct WorldData
{
    void *world;
    s32 unk4;
    s32 unk8[4];
} WorldData;
extern WorldData g_worldData;

s8 OBoxHitsSegmentZeroTime(void *, void *, void *);

typedef struct OBoxCorners
{
    u8 pad[0x10];
    Vector4 corners[8];
} OBoxCorners;
typedef struct CollisionInfo
{
    Vector4 pos;
    Vector4 normal;
    s32 unk20;
    s32 unk24;
    u32 material;
    u8 pad2C[0x40 - 0x2C];
    u8 type;
} CollisionInfo;

f32 OBoxHitsSegment(void *, Vector4 *, void *, f32, void *);
u32 RayHitsBSP(Vector4 *, Vector4 *, void *, f32 *, void *);

s8 RayHitsTerrain(Vector4 *start, Vector4 *end, void *arg2, void *arg3)
{
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    Vector4 dir __attribute__((aligned(16)));

    PSVECSubtract(end, start, &dir);
    return RayHitsTerrain_Aux3(start, &dir, arg2, arg3);
}

#pragma push
#pragma use_lmw_stmw on
s8 OBoxHitsBSPZeroTimeEdgesOnly(void *obox, void *bsp, void *arg2)
{
    u8 *edge;
    u32 i;

    if (*(u32 *)((char *)bsp + 0x3c) == 0 && *(u32 *)((char *)bsp + 0x40) == 0)
    {
        return 0;
    }
    for (i = 0; i < *(u32 *)((char *)bsp + 0x40); i++)
    {
        edge = (u8 *)(*(u8 **)((char *)*(void **)g_worldData.world + 0x54) +
                      (*(u16 **)((char *)bsp + 0x44))[i] * 0x30);
        if (*(u32 *)(edge + 0x20) != g_worldData.unk4)
        {
            *(u32 *)(edge + 0x20) = g_worldData.unk4;
            if (OBoxHitsSegmentZeroTime(obox, edge, arg2))
            {
                return 1;
            }
        }
    }
    return 0;
}
void BuildOOBOffset(void *oob, void *pos, void *rot, void *out)
{
    void XformVec(void *, void *, void *);
    void BuildOOB(void *, void *, void *);
    struct Local {
        double d[8];
    };
    struct Local local __attribute__((aligned(16)));

    local = *(struct Local *)pos;
    XformVec(pos, rot, &local.d[6]);
    BuildOOB(oob, &local, out);
    *(void **)((char *)out + 0xa0) = (char *)pos + 0x20;
    *(void **)((char *)out + 0xa4) = (char *)pos;
    *(void **)((char *)out + 0xa8) = (char *)pos + 0x10;
}

