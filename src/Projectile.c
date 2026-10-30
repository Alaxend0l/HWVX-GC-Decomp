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

void Projectile_MoveBase(u8 *proj)
{
extern f32 g_fDeltaTimeSec;
extern f32 g_fDeltaTimeMS;
extern s32 g_iDeltaTimeMS;
extern Vector4 g_zeroVector;
extern u8 g_projectileVTable[];
void LensFlare_Delete(s32 flare);
void Thing_Release(s32 thing);
void LightHolder_Delete(s32 light);
void ThingManager_DARYL_CreateThing(s32 type, void *params);
void PSVECScale(void *src, void *dst, f32 scale);
void PSVECAdd(void *a, void *b, void *out);
f32 MagnitudeVector(void *v);
void ExtractColorKey(void *keys, void *out, f32 t);
void ExtractScaleKey(void *keys, void *out, f32 t);
f32 dt;
u8 type;
u32 id;
Vector4 cur;
Vector4 step;
Vector4 move;
struct ThingParams
{
Vector4 a;
Vector4 b;
Vector4 c;
Vector4 zero;
u32 id;
s32 player;
f32 f0;
f32 f1;
} q __attribute__((aligned(16)));
struct ThingParams p __attribute__((aligned(16)));
dt = g_fDeltaTimeSec;
*(u16 *)(proj + 0x15C) = 0;
if (((s32 *)proj)[0x49] < 0) {
id = ((u32 *)((void **)proj)[0x48])[0x1E];
if (id != 0) {
q.id = id;
*(Vector4 *)&q.a = *(Vector4 *)(proj + 0x40);
q.b = *(Vector4 *)(proj + 0x50);
q.c = *(Vector4 *)(proj + 0x00);
q.player = ((s32 *)proj)[0x25];
q.f0 = 0.0f;
q.f1 = 0.0f;
q.zero = g_zeroVector;
ThingManager_DARYL_CreateThing(1, &q);
}
LensFlare_Delete(((s32 *)proj)[0x4C]);
Thing_Release((s32)proj);
LightHolder_Delete(((s32 *)proj)[0x4D]);
*(void **)(proj + 0x8C) = g_projectileVTable;
}
((s32 *)proj)[0x49] -= g_iDeltaTimeMS;
type = *(u8 *)(*(u8 **)(proj + 0x120) + 0x04);
if (type != 0) {
if (type == 1) {
*(f32 *)(proj + 0x128) += 1000.0f * dt;
}
else if (type == 2) {
PSVECScale(proj, &move, dt);
*(f32 *)(proj + 0x128) += MagnitudeVector(&move);
}
if (*(f32 *)(proj + 0x128) >= 0.0f) {
cur = *(Vector4 *)(proj + 0x40);
PSVECScale(proj, &step, -*(f32 *)(proj + 0x12C));
if (*(f32 *)(proj + 0x12C)) {
while (*(f32 *)(proj + 0x128) > *(f32 *)(proj + 0x12C)) {
id = *(u32 *)(*(u8 **)(proj + 0x120) + 0x70);
if (id != 0) {
p.id = id;
p.a = cur;
p.b = *(Vector4 *)(proj + 0x50);
p.c = *(Vector4 *)(proj + 0x00);
p.player = ((s32 *)proj)[0x25];
p.f0 = 0.0f;
p.f1 = 0.0f;
p.zero = g_zeroVector;
ThingManager_DARYL_CreateThing(1, &p);
}
*(f32 *)(proj + 0x128) -= *(f32 *)(proj + 0x12C);
PSVECAdd(&cur, &step, &cur);
}
}
}
}
ExtractColorKey(*(u8 **)(proj + 0x120) + 0x94, proj + 0xF0, g_fDeltaTimeMS * *(f32 *)(proj + 0x140));
ExtractScaleKey(*(u8 **)(proj + 0x120) + 0x8C, proj + 0xC0, g_fDeltaTimeMS * *(f32 *)(proj + 0x140));
}

#pragma push
#pragma use_lmw_stmw on
void Projectile_ProximityDetection(u8 *proj)
{
    extern s32 g_iDeltaTimeMS;
    extern Vector4 g_zeroVector;
    extern u8 g_projectileVTable[];
    void *Thing_GetSource(void *thing);
    s32 ThingManager_GetActiveCars(u8 **cars, s32 max);
    f32 PSVECSquareDistance(void *a, void *b);
    void InverseXformVec(void *mtx, Vector4 *in, Vector4 *out);
    void *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void ThingManagerList_Append(void *list, void *thing);
    void LensFlare_Delete(s32 flare);
    void Thing_Release(s32 thing);
    void LightHolder_Delete(s32 light);
    struct { Vector4 a; Vector4 b; Vector4 c; Vector4 zero; u32 id; s32 player; f32 f0; f32 f1; } p __attribute__((aligned(16)));
    u8 *cars[16];
    u32 count, i;
    void *source;
    f32 radius;
    u8 *info;
    u32 id;
    u8 *slave;
    radius = ((f32 *)((void **)proj)[0x48])[0xD];
    source = Thing_GetSource(proj);
    ((s32 *)proj)[0x51] -= g_iDeltaTimeMS;
    if (((s32 *)proj)[0x51] < 0)
    {
        count = ThingManager_GetActiveCars(cars, 16);
        for (i = 0; i < count; i++)
        {
            if (cars[i] != source)
            {
                if (PSVECSquareDistance(proj + 0x40, cars[i] + 0x40) < (radius + ((f32 *)cars[i])[0x1C]) * (radius + ((f32 *)cars[i])[0x1C]))
                {
                    info = ((void **)proj)[0x48];
                    id = ((u32 *)info)[0x1D];
                    if (id != 0)
                    {
                        p.f0 = 0.0f;
                        p.f1 = 0.0f;
                        p.player = ((s32 *)proj)[0x25];
                        p.id = id;
                        p.zero = g_zeroVector;
                        if (((u8 *)info)[7] != 0)
                        {
                            p.b = *(Vector4 *)(proj + 0x50);
                            p.c.x = 0.0f;
                            p.c.y = 0.0f;
                            p.c.z = 0.0f;
                            InverseXformVec(cars[i] + 0x10, (Vector4 *)(proj + 0x40), &p.a);
                            slave = ThingManager_DARYL_CreateSlave(1, &p, 0xFF);
                            if (slave != NULL)
                            {
                                slave[0x83] = 0;
                                ThingManagerList_Append(cars[i] + 0x88, slave);
                            }
                        }
                        else
                        {
                            p.c = *(Vector4 *)(proj + 0x00);
                            p.a = *(Vector4 *)(proj + 0x40);
                            p.b.w = 1.0f;
                            p.b.x = 0.0f;
                            p.b.y = 0.0f;
                            p.b.z = 0.0f;
                            ThingManager_DARYL_CreateThing(1, &p);
                        }
                    }
                    ((Vector4 *)proj)->x = 0.0f;
                    ((Vector4 *)proj)->y = 0.0f;
                    ((Vector4 *)proj)->z = 0.0f;
                    LensFlare_Delete(((s32 *)proj)[0x4C]);
                    Thing_Release((s32)proj);
                    LightHolder_Delete(((s32 *)proj)[0x4D]);
                    *(void **)(proj + 0x8C) = g_projectileVTable;
                    ((u32 *)proj)[0x1D] |= 2;
                    return;
                }
            }
        }
        ((s32 *)proj)[0x51] += ((s32 *)((void **)proj)[0x48])[0xE];
    }
}
#pragma pop
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

s32 Projectile_Move(u8 *proj)
{
extern f32 g_fDeltaTimeSec;
extern f32 g_fDeltaTimeMS;
extern s32 g_iDeltaTimeMS;
extern Vector4 g_zeroVector;
extern u8 g_projectileVTable[];
void LensFlare_Delete(s32 flare);
void Thing_Release(s32 thing);
void LightHolder_Delete(s32 light);
void ThingManager_DARYL_CreateThing(s32 type, void *params);
void PSVECScale(void *src, void *dst, f32 scale);
void PSVECAdd(void *a, void *b, void *out);
f32 MagnitudeVector(void *v);
void ExtractColorKey(void *keys, void *out, f32 t);
void ExtractScaleKey(void *keys, void *out, f32 t);
void Thing_CollideWithForceBoxes(void *thing);
f32 dt;
u8 type;
u32 id;
Vector4 cur;
Vector4 step;
Vector4 move;
struct ThingParams
{
Vector4 a;
Vector4 b;
Vector4 c;
Vector4 zero;
u32 id;
s32 player;
f32 f0;
f32 f1;
} q __attribute__((aligned(16)));
struct ThingParams p __attribute__((aligned(16)));
dt = g_fDeltaTimeSec;
*(u16 *)(proj + 0x15C) = 0;
if (((s32 *)proj)[0x49] < 0) {
id = ((u32 *)((void **)proj)[0x48])[0x1E];
if (id != 0) {
q.id = id;
*(Vector4 *)&q.a = *(Vector4 *)(proj + 0x40);
q.b = *(Vector4 *)(proj + 0x50);
q.c = *(Vector4 *)(proj + 0x00);
q.player = ((s32 *)proj)[0x25];
q.f0 = 0.0f;
q.f1 = 0.0f;
q.zero = g_zeroVector;
ThingManager_DARYL_CreateThing(1, &q);
}
LensFlare_Delete(((s32 *)proj)[0x4C]);
Thing_Release((s32)proj);
LightHolder_Delete(((s32 *)proj)[0x4D]);
*(void **)(proj + 0x8C) = g_projectileVTable;
}
((s32 *)proj)[0x49] -= g_iDeltaTimeMS;
type = *(u8 *)(*(u8 **)(proj + 0x120) + 0x04);
if (type != 0) {
if (type == 1) {
*(f32 *)(proj + 0x128) += 1000.0f * dt;
}
else if (type == 2) {
PSVECScale(proj, &move, dt);
*(f32 *)(proj + 0x128) += MagnitudeVector(&move);
}
if (*(f32 *)(proj + 0x128) >= 0.0f) {
cur = *(Vector4 *)(proj + 0x40);
PSVECScale(proj, &step, -*(f32 *)(proj + 0x12C));
if (*(f32 *)(proj + 0x12C)) {
while (*(f32 *)(proj + 0x128) > *(f32 *)(proj + 0x12C)) {
id = *(u32 *)(*(u8 **)(proj + 0x120) + 0x70);
if (id != 0) {
p.id = id;
p.a = cur;
p.b = *(Vector4 *)(proj + 0x50);
p.c = *(Vector4 *)(proj + 0x00);
p.player = ((s32 *)proj)[0x25];
p.f0 = 0.0f;
p.f1 = 0.0f;
p.zero = g_zeroVector;
ThingManager_DARYL_CreateThing(1, &p);
}
*(f32 *)(proj + 0x128) -= *(f32 *)(proj + 0x12C);
PSVECAdd(&cur, &step, &cur);
}
}
}
}
ExtractColorKey(*(u8 **)(proj + 0x120) + 0x94, proj + 0xF0, g_fDeltaTimeMS * *(f32 *)(proj + 0x140));
ExtractScaleKey(*(u8 **)(proj + 0x120) + 0x8C, proj + 0xC0, g_fDeltaTimeMS * *(f32 *)(proj + 0x140));
Thing_CollideWithForceBoxes(proj);
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

#pragma push
#pragma use_lmw_stmw on
void Projectile_DynamicCollisionCorrection(u8 *proj, u8 *hit) {
    extern Vector4 g_zeroVector;
    extern u8 g_projectileVTable[];
    void *Thing_GetSource(void *thing);
    f32 PSVECDotProduct(void *a, void *b);
    void PSVECCrossProduct(void *a, void *b, void *out);
    void PSVECNormalize(void *a, void *out);
    f32 PSVECSquareMag(void *v);
    void GenerateQuaternion(void *axis, void *q, f32 angle);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void TransposeSubMatrix(Matrix4 *mtx);
    void MultiplySubMatrix(Matrix4 *a, Matrix4 *b, void *out);
    void InverseXformVec(void *mtx, void *in, void *out);
    void Grid_FreeObject(void *object);
    void ThingManagerList_Remove(void *list, void *thing);
    void ThingManagerList_Append(void *list, void *thing);
    void *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    f32 RandomAdd(f32 base, f32 range);
    s32 RandomAdd_s32(s32 base, s32 range);
    void BulletHole_AddSegment(void *mtx, u16 texture, GXColor color, u32 life, f32 size);
    void LensFlare_Delete(s32 flare);
    void LightHolder_Delete(s32 light);
    double acos(double x);
    struct { Vector4 a; Vector4 b; Vector4 c; Vector4 zero; u32 id; s32 player; f32 f0; f32 f1; } p;
    Matrix4 mtx;
    Matrix4 other;
    Matrix4 hole;
    Vector4 normal __attribute__((aligned(16)));
    Vector4 q __attribute__((aligned(16)));
    Vector4 axis __attribute__((aligned(16)));
    u8 *target;
    u8 *surface;
    u8 *slave;
    u32 id;
    f32 dot;
    f32 angle;
    f32 size;

    Thing_GetSource(proj);
    if (!(*(u32 *)(proj + 0x74) & 0x40) && !(*(u32 *)(proj + 0x74) & 0x200000) && hit != NULL) {
        *(u16 *)(proj + 0x15C) += 1;
        if (*(u16 *)(proj + 0x15C) > 8) {
            *(u32 *)(proj + 0x74) |= 0x40;
            return;
        }
        if (*(u8 **)(hit + 0x20) == proj) {
            target = *(u8 **)(hit + 0x24);
            normal = *(Vector4 *)(hit + 0x10);
        } else {
            target = *(u8 **)(hit + 0x24);
            normal.x = -*(f32 *)(hit + 0x10);
            normal.y = -*(f32 *)(hit + 0x14);
            normal.z = -*(f32 *)(hit + 0x18);
        }
        switch ((*(u8 **)(proj + 0x120))[3]) {
        case 6:
            if (target != NULL) {
                mtx = *(Matrix4 *)(proj + 0x10);
                other = *(Matrix4 *)(target + 0x10);
                TransposeSubMatrix(&other);
                MultiplySubMatrix(&other, &mtx, proj + 0x10);
                SubMatrixToQuaternion(proj + 0x10, (Vector4 *)(proj + 0x50));
                InverseXformVec(target + 0x10, &mtx.v[3], proj + 0x40);
                if (*(void **)(proj + 0x84) != NULL) {
                    Grid_FreeObject(*(void **)(proj + 0x84));
                }
                *(void **)(proj + 0x84) = NULL;
                proj[0x83] = 0xEF;
                ThingManagerList_Remove(&g_thingManager_DARYL.activeList, proj);
                ThingManagerList_Append(target + 0x88, proj);
                *(Vector4 *)proj = g_zeroVector;
            } else {
                *(u32 *)(proj + 0x74) |= 0x80000;
                *(Vector4 *)proj = g_zeroVector;
            }
            break;
        case 0:
        case 3:
            id = *(u32 *)(*(u8 **)(proj + 0x120) + 0x74);
            if (id != 0) {
                dot = PSVECDotProduct(&normal, proj + 0x30);
                switch ((*(u8 **)(proj + 0x120))[8]) {
                case 0:
                    p.b = *(Vector4 *)(proj + 0x50);
                    break;
                case 1:
                    if (dot < -0.97f) {
                        Vector4 flip __attribute__((aligned(16))) = {0.0f, 1.0f, 0.0f, 0.0f};
                        GenerateQuaternion(&flip, &q, 3.1415927f);
                        QuaternionMult(&q, (Vector4 *)(proj + 0x50), &p.b);
                    } else {
                        angle = acos(dot);
                        PSVECCrossProduct(proj + 0x30, &normal, &axis);
                        if (dot > 0.99f) {
                            axis = *(Vector4 *)(proj + 0x10);
                        } else if (PSVECSquareMag(&axis)) {
                            PSVECNormalize(&axis, &axis);
                        }
                        GenerateQuaternion(&axis, &q, angle);
                        QuaternionMult(&q, (Vector4 *)(proj + 0x50), &p.b);
                    }
                    break;
                case 2:
                    if (dot < -0.97f) {
                        Vector4 flip __attribute__((aligned(16))) = {0.0f, 1.0f, 0.0f, 0.0f};
                        GenerateQuaternion(&flip, &q, 3.1415927f);
                        QuaternionMult(&q, (Vector4 *)(proj + 0x50), &p.b);
                    } else {
                        if (dot > 0.97f) {
                            angle = -1.5707964f;
                        } else {
                            angle = (f32)acos(dot) - 1.5707964f;
                        }
                        PSVECCrossProduct(proj + 0x30, &normal, &axis);
                        if (dot > 0.99f) {
                            axis = *(Vector4 *)(proj + 0x10);
                        } else if (PSVECSquareMag(&axis)) {
                            PSVECNormalize(&axis, &axis);
                        }
                        GenerateQuaternion(&axis, &q, 2.0f * angle);
                        QuaternionMult(&q, (Vector4 *)(proj + 0x50), &p.b);
                    }
                    break;
                }
                p.id = id;
                p.c = g_zeroVector;
                p.player = *(s32 *)(proj + 0x94);
                p.f0 = 0.0f;
                p.f1 = 0.0f;
                p.zero = g_zeroVector;
                if (target != NULL) {
                    InverseXformVec(target + 0x10, proj + 0x40, &p.a);
                    slave = ThingManager_DARYL_CreateSlave(1, &p, 0xFF);
                    if (slave != NULL) {
                        slave[0x83] = 0xEF;
                        ThingManagerList_Append(target + 0x88, slave);
                    }
                } else {
                    p.a = *(Vector4 *)(proj + 0x40);
                    p.a.x = 0.5f * normal.x + p.a.x;
                    p.a.y = 0.5f * normal.y + p.a.y;
                    p.a.z = 0.5f * normal.z + p.a.z;
                    ThingManager_DARYL_CreateThing(1, &p);
                }
            }
            ((Vector4 *)proj)->x = 0.0f;
            ((Vector4 *)proj)->y = 0.0f;
            ((Vector4 *)proj)->z = 0.0f;
            if (*(u8 **)(hit + 0x24) == NULL && (surface = *(u8 **)(hit + 0x28)) != NULL && *(u32 *)(*(u8 **)(proj + 0x120) + 0x5C) != 0) {
                switch (surface[1]) {
                case 0: {
                    Vector4 rot __attribute__((aligned(16)));
                    Vector4 q2 __attribute__((aligned(16)));
                    Vector4 axis2 __attribute__((aligned(16)));
                    dot = PSVECDotProduct(&normal, proj + 0x30);
                    if (dot < -0.985f) {
                        Vector4 flip __attribute__((aligned(16))) = {0.0f, 1.0f, 0.0f, 0.0f};
                        GenerateQuaternion(&flip, &q2, 3.1415927f);
                        QuaternionMult(&q2, (Vector4 *)(proj + 0x50), &rot);
                    } else {
                        angle = acos(dot);
                        PSVECCrossProduct(proj + 0x30, &normal, &axis2);
                        if (dot > 0.99f) {
                            axis2 = *(Vector4 *)(proj + 0x10);
                        } else if (PSVECSquareMag(&axis2)) {
                            PSVECNormalize(&axis2, &axis2);
                        }
                        GenerateQuaternion(&axis2, &q2, angle);
                        QuaternionMult(&q2, (Vector4 *)(proj + 0x50), &rot);
                    }
                    QuaternionToSubMatrix(&rot, &hole);
                    hole.v[3].x = 0.5f * *(f32 *)(hit + 0x10) + *(f32 *)(hit + 0x0);
                    hole.v[3].y = 0.5f * *(f32 *)(hit + 0x14) + *(f32 *)(hit + 0x4);
                    hole.v[3].z = 0.5f * *(f32 *)(hit + 0x18) + *(f32 *)(hit + 0x8);
                    size = RandomAdd(*(f32 *)(*(u8 **)(proj + 0x120) + 0x4C), *(f32 *)(*(u8 **)(proj + 0x120) + 0x50));
                    angle = RandomAdd_s32(*(s32 *)(*(u8 **)(proj + 0x120) + 0x54), *(s32 *)(*(u8 **)(proj + 0x120) + 0x58));
                    BulletHole_AddSegment(&hole, *(u32 *)(*(u8 **)(proj + 0x120) + 0x5C), **(GXColor **)(*(u8 **)(proj + 0x120) + 0xAC), angle, size);
                    break;
                }
                case 1:
                case 2:
                    break;
                }
            }
            LensFlare_Delete(*(s32 *)(proj + 0x130));
            Thing_Release((s32)proj);
            LightHolder_Delete(*(s32 *)(proj + 0x134));
            *(void **)(proj + 0x8C) = g_projectileVTable;
            *(u32 *)(proj + 0x74) |= 2;
            break;
        case 1:
        case 4:
            ((Vector4 *)(proj + 0x40))->x = 0.02f * normal.x + ((Vector4 *)(proj + 0x40))->x;
            ((Vector4 *)(proj + 0x40))->y = 0.02f * normal.y + ((Vector4 *)(proj + 0x40))->y;
            ((Vector4 *)(proj + 0x40))->z = 0.02f * normal.z + ((Vector4 *)(proj + 0x40))->z;
        case 2:
        case 5:
            break;
        }
    }
}
#pragma pop
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

