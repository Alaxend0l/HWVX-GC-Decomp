#include "common.h"

typedef struct Vector {
    f32 x;
    f32 y;
    f32 z;
} Vector;

extern f32 __float_nan;
extern f32 __float_huge;

f32 MagnitudeVector(Vector *v) {
    f32 zero = 0.0f;

    if (v->x != zero || v->y != zero || v->z != zero) {
        return PSVECMag(v);
    }
    return zero;
}

void CopyVector(Vector *dst, Vector *src) {
    *dst = *src;
}

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

void AccumulateVector(void *arg0, void *arg1, void *arg2, f32 farg0) {
    M2C_FIELD(arg2, f32 *, 0) = (f32) ((M2C_FIELD(arg0, f32 *, 0) * farg0) + M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg2, f32 *, 4) = (f32) ((M2C_FIELD(arg0, f32 *, 4) * farg0) + M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg2, f32 *, 8) = (f32) ((M2C_FIELD(arg0, f32 *, 8) * farg0) + M2C_FIELD(arg1, f32 *, 8));
}

void DistanceXYZ(Vector *a, Vector *b, f32 *dist)
{
    f32 dy;
    f32 dx;
    f32 dz;

    dy = a->y - b->y;
    sq:
    dx = a->x - b->x;
    dz = a->z - b->z;
    *dist = Sqrt(dy * dy + dx * dx + dz * dz);
}

void GetComponentOfVector(void *arg0, void *arg1) {
    f32 PSVECDotProduct();
    f32 temp_f1;

    temp_f1 = PSVECDotProduct();
    M2C_FIELD(arg1, f32 *, 0) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 4) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 4));
    M2C_FIELD(arg1, f32 *, 8) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 8));
}

