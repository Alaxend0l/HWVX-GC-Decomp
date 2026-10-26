#include "common.h"

typedef struct Vector4 { f32 x; f32 y; f32 z; f32 w; } Vector4;

s8 RayHitsTerrain(Vector4 *start, Vector4 *end, void *arg2, void *arg3)
{
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    Vector4 dir __attribute__((aligned(16)));

    PSVECSubtract(end, start, &dir);
    return RayHitsTerrain_Aux3(start, &dir, arg2, arg3);
}
