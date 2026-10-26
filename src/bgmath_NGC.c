#include "common.h"

typedef struct Vector {
    f32 x;
    f32 y;
    f32 z;
} Vector;

extern f32 __float_nan;
extern f32 __float_huge;

static f32 Sqrt(f64 x)
{
    f64 guess;
    f64 acc;

    if (x > 0.0)
    {
        guess = __frsqrte(x);
        guess = 0.5 * guess * (1.0 - x * guess * guess);
        guess = 0.5 * guess * (1.0 - x * guess * guess);
        guess = 0.5 * guess * (1.0 - x * guess * guess);
        guess = 0.5 * guess * (1.0 - x * guess * guess);
        acc = guess * (1.0 - x * guess * guess);
        return x * acc;
    }
    return 0.0f;
}

void DistanceXYZ(Vector *a, Vector *b, f32 *dist)
{
    *dist = Sqrt((a->y - b->y) * (a->y - b->y) + (a->x - b->x) * (a->x - b->x) + (a->z - b->z) * (a->z - b->z));
}

