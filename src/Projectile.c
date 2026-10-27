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

s32 Projectile_PreMove(u8 *proj)
{
    void GenerateQuaternion(void *axis, void *q, f32 angle);
    void QuaternionMult(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    f32 Sqrt(f32 x);
    void Projectile_ProximityDetection(void *proj);
    extern f32 g_fDeltaTimeSec;
    Vector4 q;
    Vector4 result;
    void *temp;
    f32 step;
    f32 mag;
    f32 scale;

    GenerateQuaternion(proj + 0xB0, &q, ((f32 *)proj)[0x4F] * g_fDeltaTimeSec);
    QuaternionMult((Vector4 *)(proj + 0x50), &q, &result);
    *(Vector4 *)(proj + 0x50) = result;
    ((f32 *)proj)[1] += ((f32 *)proj)[0x52] * g_fDeltaTimeSec;
    temp = ((void **)proj)[0x48];
    if (((f32 *)temp)[0x10])
    {
        step = ((f32 *)temp)[0x10] * g_fDeltaTimeSec;
        ((f32 *)proj)[0] += ((f32 *)(proj + 0x30))[0] * step;
        ((f32 *)proj)[1] += ((f32 *)(proj + 0x30))[1] * step;
        ((f32 *)proj)[2] += ((f32 *)(proj + 0x30))[2] * step;
    }
    mag = PSVECSquareMag((Vector4 *)proj);
    temp = ((void **)proj)[0x48];
    if (mag > (((f32 *)temp)[0x11] * ((f32 *)temp)[0x11]))
    {
        scale = 1.0f / Sqrt(mag);
        ((f32 *)proj)[0] *= scale;
        ((f32 *)proj)[1] *= scale;
        ((f32 *)proj)[2] *= scale;
    }
    temp = ((void **)proj)[0x48];
    if (((f32 *)temp)[0xD])
    {
        Projectile_ProximityDetection(proj);
    }
    return 1;
}

void Projectile_Draw(u8 *proj)
{
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void MultiplyMatrix(Matrix4 *a, void *b, Matrix4 *out);
    void *memset(void *dst, s32 val, u32 n);
    void Gfx_SetBaseColor(u32 *color);
    void Gfx_EnableState(s32 state);
    void Gfx_DisableState(s32 state);
    void Gfx_DrawGeometry(void *geom, void *mtx);
    void *Gfx_AddBillBoardSprite(u16 arg0);
    void *sprite;
    Matrix4 mtx __attribute__((aligned(16)));
    Matrix4 scale __attribute__((aligned(16)));
    u32 color;

    if (((u8 *)((void **)proj)[0x48])[0] == 1)
    {
        QuaternionToSubMatrix(proj + 0x50, proj + 0x10);
        memset(&scale, 0, sizeof(Matrix4));
        scale.v[0].x = ((f32 *)proj)[0x30];
        scale.v[1].y = ((f32 *)proj)[0x31];
        scale.v[2].z = ((f32 *)proj)[0x32];
        scale.v[3].w = 1.0f;
        MultiplyMatrix(&scale, proj + 0x10, &mtx);
        switch (((u8 *)((void **)proj)[0x48])[1])
        {
        case 0:
            if (((u32 *)proj)[0x1d] & 0x4000)
            {
                color = ((u32 *)proj)[0x45];
                Gfx_SetBaseColor(&color);
                Gfx_EnableState(0x10);
            }
            Gfx_DrawGeometry(((void **)((void **)proj)[0x48])[4], &mtx);
            if (((u32 *)proj)[0x1d] & 0x4000)
            {
                Gfx_DisableState(0x10);
            }
            break;
        case 1:
            sprite = Gfx_AddBillBoardSprite(0);
            *(u8 *)((u8 *)sprite + 0x2a) = *(u8 *)((void **)((void **)proj)[0x48])[0x18];
            *(u32 *)((u8 *)sprite + 0x24) = ((u32 *)proj)[0x45];
            *(f32 *)((u8 *)sprite + 0xc) = ((f32 *)proj)[0x30];
            *(f32 *)((u8 *)sprite + 0x0) = ((f32 *)proj)[0x10];
            *(f32 *)((u8 *)sprite + 0x4) = ((f32 *)proj)[0x11];
            *(f32 *)((u8 *)sprite + 0x8) = ((f32 *)proj)[0x12];
            *(f32 *)((u8 *)sprite + 0x1c) = 360.0f;
            break;
        }
    }
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

