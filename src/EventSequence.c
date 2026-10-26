

#include "common.h"

typedef struct
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

extern u8 g_CurrentViews;

s32 Gfx_GetCameraPositions(Vector4 *positions);
f32 PSVECSquareDistance(f32 *a, f32 *b);

void Thing_Release(void *);
void EventSeq_Release(void *arg0)
{
    Thing_Release(arg0);
}

s32 EventSeq_InCameraRange(u8 *seq, f32 range)
{
    Vector4 positions[5] __attribute__((aligned(16)));
    Vector4 *pos;
    f32 dist;
    u32 count;
    u32 i;

    if (range == 0.0f)
    {
        return 1;
    }
    if (*(s8 *)(seq + 0xF1))
    {
        count = Gfx_GetCameraPositions(positions);
        *(f32 *)(seq + 0xF4) = PSVECSquareDistance((f32 *)(seq + 0x40), (f32 *)positions);
        if (count > 1)
        {
            for (i = 0, pos = positions; i < 5; i++, pos++)
            {
                if (g_CurrentViews & (1 << i))
                {
                    dist = PSVECSquareDistance((f32 *)(seq + 0x40), (f32 *)pos);
                    if (dist < *(f32 *)(seq + 0xF4))
                    {
                        *(f32 *)(seq + 0xF4) = dist;
                    }
                }
            }
        }
        *(s8 *)(seq + 0xF1) = 0;
    }
    return range * range > *(f32 *)(seq + 0xF4);
}

s32 EventSeq_Invalidate(void *arg0, u32 arg1, u32 arg2)
{
    u32 field;

    field = M2C_FIELD(arg0, u32 *, 0xD0);
    if (field >= arg1 && field < arg2)
    {
        return 1;
    }
    return 0;
}

