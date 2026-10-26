#include "common.h"

typedef struct Vector {
    f32 x;
    f32 y;
    f32 z;
} Vector;

extern s32 __float_nan[];
extern s32 __float_huge[];

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

void CopyMatrix(Matrix4 *dst, Matrix4 *src)
{
    void *memcpy(void *dst, const void *src, u32 n);

    memcpy(dst, src, 0x40);
}

void SinCos(f32 *s, f32 *c, f32 angle)
{
    f64 sin(f32);
    f64 cos(f32);
    f32 r;

    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    r = 0.017453292f * angle;
    *s = sin(r);
    *c = cos(r);
}

void SinCosRad(f32 *s, f32 *c, f32 rad)
{
    f64 sin(f32);
    f64 cos(f32);
    f32 angle = rad * 57.29578f;
    f32 r;

    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    r = 0.017453292f * angle;
    *s = sin(r);
    *c = cos(r);
}

f32 Sqrt(f64 x)
{
    f64 guess;
    f64 result;
    s32 i;

    if (x > 0.0)
    {
        guess = __frsqrte(x);
        for (i = 0; i < 4; i++)
        {
            guess = 0.5 * guess * (3.0 - x * (guess * guess));
        }
        result = x * guess;
    }
    else if (x == 0.0)
    {
        result = 0.0;
    }
    else if (x)
    {
        result = *(f32 *)__float_nan;
    }
    else
    {
        result = *(f32 *)__float_huge;
    }
    return result;
}

f32 _fabs(f32 x) {
    f64 result = __fabs((f64)x);
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

s32 CompareDistances(Vector *a, Vector *b, f32 dist)
{
    f32 dy = a->y - b->y;
    f32 dx = a->x - b->x;
    f32 dz = a->z - b->z;
    f32 len = dy * dy + dx * dx + dz * dz;

    if (len > dist * dist) {
        return 1;
    }
    if (len < dist * dist) {
        return -1;
    }
    return 0;
}

void GetComponentOfVector(void *arg0, void *arg1) {
    f32 PSVECDotProduct();
    f32 temp_f1;

    temp_f1 = PSVECDotProduct();
    M2C_FIELD(arg1, f32 *, 0) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 4) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 4));
    M2C_FIELD(arg1, f32 *, 8) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 8));
}

void ProjectVectorToNormal(Vector *n, Vector *v) {
    f32 PSVECDotProduct();
    f32 scale;

    scale = -PSVECDotProduct(n, v);
    v->x = scale * n->x + v->x;
    v->y = scale * n->y + v->y;
    v->z = scale * n->z + v->z;
}

