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

double atan2(double x);
float ArcTan(float x) {
    return (float)atan2(x);
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
  f32 dx;
  f32 dy;
  f32 dz;
  ;
  dx = a->x - b->x;
  dz = a->z - b->z;
  *dist = Sqrt((((a->y - b->y) * (a->y - b->y)) + (dx * dx)) + (dz * dz));
}

s32 CompareDistances(Vector *a, Vector *b, f32 dist)
{
    f32 dy = a->y - b->y;
    f32 dx = a->x - b->x;
    f32 dz = a->z - b->z;
    f32 sq = dist * dist;
    f32 len = dy * dy + dx * dx + dz * dz;

    if (len > sq) {
        return 1;
    }
    if (len < sq) {
        return -1;
    }
    return 0;
}

void XformVec(Matrix4 *m, Vector *v, Vector *out)
{
    f32 tx = m->v[2].x * v->z + (m->v[0].x * v->x + m->v[1].x * v->y) + m->v[3].x;
    f32 ty = m->v[2].y * v->z + (m->v[0].y * v->x + m->v[1].y * v->y) + m->v[3].y;
    f32 tz = m->v[2].z * v->z + (m->v[0].z * v->x + m->v[1].z * v->y) + m->v[3].z;

    out->x = tx;
    out->y = ty;
    out->z = tz;
}

void XformVec4x4(Matrix4 *m, Vector *v, Vector *out)
{
    f32 tx = m->v[2].x * v->z + (m->v[0].x * v->x + m->v[1].x * v->y) + m->v[3].x;
    f32 ty = m->v[2].y * v->z + (m->v[0].y * v->x + m->v[1].y * v->y) + m->v[3].y;
    f32 tz = m->v[2].z * v->z + (m->v[0].z * v->x + m->v[1].z * v->y) + m->v[3].z;

    out->x = tx;
    out->y = ty;
    out->z = tz;
}

void InverseXformVec(Matrix4 *m, Vector *v, Vector *out)
{
    f32 dx = v->x - m->v[3].x;
    f32 dy = v->y - m->v[3].y;
    f32 dz = v->z - m->v[3].z;

    f32 tx = m->v[0].x * dx + m->v[1].x * dy + m->v[2].x * dz;
    f32 ty = m->v[0].y * dx + m->v[1].y * dy + m->v[2].y * dz;
    f32 tz = m->v[0].z * dx + m->v[1].z * dy + m->v[2].z * dz;

    out->x = tx;
    out->y = ty;
    out->z = tz;
}

void XformVecOrientation(Matrix4 *m, Vector *v, Vector *out)
{
    f32 tx = m->v[2].x * v->z + (m->v[0].x * v->x + m->v[1].x * v->y);
    f32 ty = m->v[2].y * v->z + (m->v[0].y * v->x + m->v[1].y * v->y);
    f32 tz = m->v[2].z * v->z + (m->v[0].z * v->x + m->v[1].z * v->y);

    out->x = tx;
    out->y = ty;
    out->z = tz;
}

void InverseXformVecOrientation(Matrix4 *m, Vector *v, Vector *out)
{
    f32 tx = m->v[0].z * v->z + (m->v[0].x * v->x + m->v[0].y * v->y);
    f32 ty = m->v[1].z * v->z + (m->v[1].x * v->x + m->v[1].y * v->y);
    f32 tz = m->v[2].z * v->z + (m->v[2].x * v->x + m->v[2].y * v->y);

    out->x = tx;
    out->y = ty;
    out->z = tz;
}

void TransposeSubMatrix(Matrix4 *m)
{
    f32 temp;

    temp = m->v[0].y;
    m->v[0].y = m->v[1].x;
    m->v[1].x = temp;

    temp = m->v[0].z;
    m->v[0].z = m->v[2].x;
    m->v[2].x = temp;

    temp = m->v[1].z;
    m->v[1].z = m->v[2].y;
    m->v[2].y = temp;
}

s32 FloatsEqual(f32 a, f32 b, f32 c)
{
    f32 diff = a - b;

    if (diff < 0.0f)
    {
        diff *= -1.0f;
    }
    return diff < c;
}

void GenerateQuaternion(Vector4 *axis, Quaternion *q, f32 angle)
{
    f64 sin(f32);
    f64 cos(f32);
    f32 s;

    if (angle) {
        angle *= 0.5f;
        q->w = cos(angle);
        s = sin(angle);
        q->x = axis->x * s;
        q->y = axis->y * s;
        q->z = axis->z * s;
    } else {
        q->w = 1.0f;
        q->x = q->y = q->z = 0.0f;
    }
}

void GenerateSpinAngleFromQuaternion(Quaternion *q, Vector4 *axis, f32 *angle)
{
    f64 acos(f32);
    f64 sin(f32);
    f32 s;

    *angle = acos(q->w);
    s = sin(*angle);
    if (0.0f == s) {
        axis->x = 1.0f;
        axis->y = 0.0f;
        axis->z = 0.0f;
    } else {
        axis->x = q->x / s;
        axis->y = q->y / s;
        axis->z = q->z / s;
        *angle *= 2.0f;
    }
}

void QuaternionMult(Quaternion *a, Quaternion *b, Quaternion *out)
{
    out->w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    out->x = a->y * b->z - a->z * b->y + a->w * b->x + a->x * b->w;
    out->y = a->z * b->x - a->x * b->z + a->w * b->y + a->y * b->w;
    out->z = a->x * b->y - a->y * b->x + a->w * b->z + a->z * b->w;
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

void MinMaxVec(Vector4 *min, Vector4 *max, Vector4 *v)
{
    if (min->x > v->x) {
        min->x = v->x;
    }
    if (min->y > v->y) {
        min->y = v->y;
    }
    if (min->z > v->z) {
        min->z = v->z;
    }
    if (min->w > v->w) {
        min->w = v->w;
    }
    if (max->x < v->x) {
        max->x = v->x;
    }
    if (max->y < v->y) {
        max->y = v->y;
    }
    if (max->z < v->z) {
        max->z = v->z;
    }
    if (max->w < v->w) {
        max->w = v->w;
    }
}

