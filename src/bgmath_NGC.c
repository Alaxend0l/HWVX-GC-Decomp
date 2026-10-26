#include "common.h"

typedef struct Vector {
    f32 y;
    f32 x;
    f32 z;
} Vector;

extern f32 __float_nan;
extern f32 __float_huge;

static f32 Sqrt(f64 x)
{
    f64 guess;
    f64 t;
    f64 result;
    s32 i;

    if (x > 0.0)
    {
        guess = __frsqrte(x);
        for (i = 0; i < 4; i++)
        {
            t = 1.0 - x * guess * guess;
            guess = 0.5 * guess * t;
        }
        result = x * guess;
    }
    else if (x == 0.0)
    {
        result = 0.0;
    }
    else if (x != x)
    {
        result = __float_nan;
    }
    else
    {
        result = __float_huge;
    }
    return result;
}

void DistanceXYZ(Vector *a, Vector *b, f32 *dist)
{
    f32 dy;
    f32 dx;
    f32 dz;

    dy = a->y - b->y;
    dx = a->x - b->x;
    dz = a->z - b->z;
    *dist = Sqrt(dy * dy + dx * dx + dz * dz);
}

