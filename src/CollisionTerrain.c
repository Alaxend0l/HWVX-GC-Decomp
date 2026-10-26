#include "common.h"




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
s8 OBoxHitsCellZeroTimeEdgesOnly(void *obox, void *cell, void *arg2)
{
    s8 OBoxHitsBSPZeroTimeEdgesOnly(void *, void *, void *);
    s8 OBoxHitsPolyZeroTime(void *, void *, void *);
    s8 hit = 0;
    u32 i;
    u8 *poly;

    *(f32 *)((char *)obox + 0xb0) = 0.0f;
    *(f32 *)((char *)obox + 0xb4) = 0.0f;
    *(f32 *)((char *)obox + 0xb8) = 0.0f;
    *(f32 *)((char *)obox + 0xc0) = 0.0f;
    *(f32 *)((char *)obox + 0xc4) = 0.0f;
    *(f32 *)((char *)obox + 0xc8) = 0.0f;
    if (*(u32 *)((char *)(*(u32 **)g_worldData.world) + 0x58) == 1)
    {
        hit = OBoxHitsBSPZeroTimeEdgesOnly(obox, cell, arg2);
    }
    else
    {
        for (i = 0; i < *(u32 *)((char *)cell + 0x3c); i++)
        {
            poly = (u8 *)(*(u32 *)((char *)(*(u32 **)g_worldData.world) + 0x4c) +
                          (*(u32 **)((char *)cell + 0x38))[i] * 0x20);
            if (*(u32 *)(poly + 0x18) != g_worldData.unk4)
            {
                *(u32 *)(poly + 0x18) = g_worldData.unk4;
                hit = OBoxHitsPolyZeroTime(obox, poly, arg2);
                if (hit)
                {
                    return 1;
                }
            }
        }
    }
    return hit;
}
s8 OBoxHitsBSP(void *obox, Vector4 *vel, void *bsp, f32 *time, void *arg4)
{
    Vector4 corner __attribute__((aligned(16)));
    Vector4 hitNormal __attribute__((aligned(16)));
    Vector4 normal __attribute__((aligned(16)));
    u8 order[8] = {2, 3, 6, 7, 0, 1, 4, 5};
    u8 *edge;
    f32 t;
    f32 hit;
    u32 i;
    s8 hitCorner;
    s8 hitEdge;
    u32 m;
    u32 material;

    t = *time;
    hitCorner = 0;
    hitEdge = 0;
    if (*(u32 *)((u8 *)bsp + 0x3c) == 0 && *(u32 *)((u8 *)bsp + 0x40) == 0)
    {
        return 0;
    }
    for (i = 0; i < *(u32 *)((u8 *)bsp + 0x40) && t > 0.0f; i++)
    {
        edge = (u8 *)(*(u8 **)((u8 *)*(void **)g_worldData.world + 0x54) +
                      (*(u16 **)((u8 *)bsp + 0x44))[i] * 0x30);
        if (*(u32 *)(edge + 0x20) != g_worldData.unk4)
        {
            *(u32 *)(edge + 0x20) = g_worldData.unk4;
            hit = OBoxHitsSegment(obox, vel, edge, t, arg4);
            if (hit < t)
            {
                t = hit;
                hitEdge = 1;
                if (arg4 != NULL)
                {
                    ((CollisionInfo *)arg4)->material =
                        (*(u32 **)((u8 *)*(void **)g_worldData.world + 0x5c))[*(u32 *)(edge + 0x24)];
                }
            }
        }
    }
    for (i = 0; i < 8 && t > 0.0f; i++)
    {
        m = RayHitsBSP(&((OBoxCorners *)obox)->corners[order[i]], vel, bsp, &t, &hitNormal);
        if (m != 0)
        {
            material = m;
            hitCorner = 1;
            corner = ((OBoxCorners *)obox)->corners[order[i]];
            normal = hitNormal;
        }
    }
    if (!hitEdge && !hitCorner)
    {
        return 0;
    }
    if (arg4 != NULL && hitCorner)
    {
        ((CollisionInfo *)arg4)->type = 0xfe;
        ((CollisionInfo *)arg4)->material = material;
        ((CollisionInfo *)arg4)->pos.x = vel->x * t + corner.x;
        ((CollisionInfo *)arg4)->pos.y = vel->y * t + corner.y;
        ((CollisionInfo *)arg4)->pos.z = vel->z * t + corner.z;
        ((CollisionInfo *)arg4)->unk20 = 0;
        ((CollisionInfo *)arg4)->unk24 = 0;
        ((CollisionInfo *)arg4)->normal = normal;
    }
    *time = t;
    return 1;
}
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
