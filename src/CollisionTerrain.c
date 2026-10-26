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

s8 RayHitsTerrain(Vector4 *start, Vector4 *end, void *arg2, void *arg3)
{
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    Vector4 dir __attribute__((aligned(16)));

    PSVECSubtract(end, start, &dir);
    return RayHitsTerrain_Aux3(start, &dir, arg2, arg3);
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

