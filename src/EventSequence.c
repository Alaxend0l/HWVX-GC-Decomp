

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

