#include "common.h"

void Projectile_Release(void * projectile)
{
    void LensFlare_Delete(s32);
    void Thing_Release(s32);
    void LightHolder_Delete(s32);
    extern u8 g_projectileVTable[];

    LensFlare_Delete(((s32 *)projectile)[0x4c]);
    Thing_Release((s32)projectile);
    LightHolder_Delete(((s32 *)projectile)[0x4d]);
    ((s32 *)projectile)[0x23] = (s32)g_projectileVTable;
}

void Projectile_YAim(u8 *proj, Vector4 *target)
{
    void InverseXformVec(void *mtx, Vector4 *in, Vector4 *out);
    f32 ArcTan(f32 y, f32 x);
    u8 FloatsEqual(f32 a, f32 b, f32 eps);
    f32 MagnitudeVector(void *v);
    void GenerateQuaternion(void *axis, void *q, f32 angle);
    void QuaternionMult(Vector4 *a, Vector4 *b, Vector4 *out);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    Vector4 local __attribute__((aligned(16)));
    Vector4 axis __attribute__((aligned(16))) = {1.0f, 0.0f, 0.0f, 0.0f};
    Vector4 q __attribute__((aligned(16)));
    Vector4 result __attribute__((aligned(16)));
    f32 angle;
    f32 speed;

    InverseXformVec(proj + 0x10, target, &local);
    angle = ArcTan(local.y, local.z);
    if (FloatsEqual(angle, 0.0f, 0.2617993950843811f)) {
        speed = MagnitudeVector(proj);
        GenerateQuaternion(&axis, &q, -angle);
        QuaternionMult((Vector4 *)(proj + 0x50), &q, &result);
        *(Vector4 *)(proj + 0x50) = result;
        QuaternionToSubMatrix(proj + 0x50, proj + 0x10);
        ((Vector4 *)proj)->x = ((Vector4 *)(proj + 0x30))->x * speed;
        ((Vector4 *)proj)->y = ((Vector4 *)(proj + 0x30))->y * speed;
        ((Vector4 *)proj)->z = ((Vector4 *)(proj + 0x30))->z * speed;
    }
}

void Projectile_DynamicCollideImpact(void * proj, void * dir, s32 damage, f32 force)
{
    void Projectile_DynamicCollisionCorrection(void *);
    f32 scale;
    void * temp;

    ((s32 *)proj)[0x1e] -= damage;
    if (dir != NULL)
    {
        temp = ((void **)proj)[0x48];
        scale = force / ((f32 *)temp)[0xb];
        ((f32 *)proj)[0] = ((f32 *)dir)[4] * scale + ((f32 *)proj)[0];
        ((f32 *)proj)[1] = ((f32 *)dir)[5] * scale + ((f32 *)proj)[1];
        ((f32 *)proj)[2] = ((f32 *)dir)[6] * scale + ((f32 *)proj)[2];
        Projectile_DynamicCollisionCorrection(proj);
    }
}

f32 Projectile_GetMass(void * arg0)
{
    void * temp;

    temp = ((void **)arg0)[0x48];
    return ((f32 *)temp)[0xb];
}

s32 Projectile_Invalidate(void * arg0, u32 arg1, u32 arg2)
{
    u32 temp;

    temp = ((u32 *)arg0)[0x48];
    if ((temp >= arg1) && (temp < arg2))
    {
        return 1;
    }
    return 0;
}

