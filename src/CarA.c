#include "common.h"


void CarA_InitInverseBodyInertiaTensor(u8 *car) {
    extern Matrix4 g_identity;
    void PSVECAdd(void *a, void *b, Vector4 *out);
    Vector4 ext[4];
    Vector4 bounds[2];
    u8 *coll;
    u8 *hull;
    u32 i;

    *(Matrix4 *)(car + 0x3C0) = g_identity;
    coll = *(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0);
    switch (coll[0x10]) {
    case 0:
        hull = *(u8 **)(coll + 0x44);
        PSVECAdd(hull + 0x10, car + 0x520, &ext[2]);
        PSVECAdd(hull, car + 0x520, &ext[1]);
        break;
    case 1:
        hull = *(u8 **)(coll + 0x48);
        bounds[0].x = 1000000.0f;
        bounds[0].y = 1000000.0f;
        bounds[0].z = 1000000.0f;
        bounds[1].x = -1000000.0f;
        bounds[1].y = -1000000.0f;
        bounds[1].z = -1000000.0f;
        for (i = 0; i < *(u32 *)(hull + 0xC); i++) {
            if (bounds[0].x > (*(Vector4 **)(hull + 0x1C))[i].x) {
                bounds[0].x = (*(Vector4 **)(hull + 0x1C))[i].x;
            }
            if (bounds[0].y > (*(Vector4 **)(hull + 0x1C))[i].y) {
                bounds[0].y = (*(Vector4 **)(hull + 0x1C))[i].y;
            }
            if (bounds[0].z > (*(Vector4 **)(hull + 0x1C))[i].z) {
                bounds[0].z = (*(Vector4 **)(hull + 0x1C))[i].z;
            }
            if (bounds[1].x < (*(Vector4 **)(hull + 0x1C))[i].x) {
                bounds[1].x = (*(Vector4 **)(hull + 0x1C))[i].x;
            }
            if (bounds[1].y < (*(Vector4 **)(hull + 0x1C))[i].y) {
                bounds[1].y = (*(Vector4 **)(hull + 0x1C))[i].y;
            }
            if (bounds[1].z < (*(Vector4 **)(hull + 0x1C))[i].z) {
                bounds[1].z = (*(Vector4 **)(hull + 0x1C))[i].z;
            }
        }
        PSVECAdd(&bounds[1], car + 0x520, &ext[2]);
        PSVECAdd(&bounds[0], car + 0x520, &ext[1]);
        break;
    }
    coll = *(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0);
    *(f32 *)(car + 0x3C0) = 1.0f / (*(f32 *)(coll + 0x0) * *(f32 *)(coll + 0x20) * (ext[2].y * ext[2].y + ext[2].z * ext[2].z));
    coll = *(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0);
    *(f32 *)(car + 0x3D4) = 1.0f / (*(f32 *)(coll + 0x4) * *(f32 *)(coll + 0x20) * (ext[2].x * ext[2].x + ext[2].z * ext[2].z));
    coll = *(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0);
    *(f32 *)(car + 0x3E8) = 1.0f / (*(f32 *)(coll + 0x8) * *(f32 *)(coll + 0x20) * (ext[2].x * ext[2].x + ext[2].y * ext[2].y));
}

void CarA_Release(u8 *car) {
    typedef struct CarNavPath {
        u8 pad[0x568];
        u8 *path[7];
    } CarNavPath;
    u32 GameSettings_GetCodeVariable(s32, s32);
    u8 *NavHolder_GetNavPoint(s32 index);
    void Trigger_VehicleKilled(void *car);
    void OConvexHullHolder_Release(void *hull);
    void SoundSource_Stop(s32 index);
    void MotionState_Release(void *state);
    void Thing_Release(void *thing);
    u16 i;

    if (GameSettings_GetCodeVariable(0x12D, 0) >= 3 && *(u8 **)(car + 0x94) != NULL) {
        *(s16 *)(*(u8 **)(car + 0x94) + 0xAC) = *(s32 *)(car + 0x3B0);
        for (i = 0; i < 6; i++) {
            if (((CarNavPath *)car)->path[i] == NULL || ((CarNavPath *)car)->path[i + 1] == NULL) {
                break;
            }
            if (NavHolder_GetNavPoint(*(s32 *)(car + 0x3B0)) == ((CarNavPath *)car)->path[i]) {
                *(s16 *)(*(u8 **)(car + 0x94) + 0xAE) = *(s32 *)(((u8 **)(car + 0x568))[i + 1] + 0x28);
                break;
            }
        }
    }
    Trigger_VehicleKilled(car);
    OConvexHullHolder_Release(*(void **)(car + 0x370));
    *(void **)(car + 0x370) = NULL;
    if (*(u32 *)(car + 0x4F0) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F0));
    }
    if (*(u32 *)(car + 0x4F4) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F4));
    }
    if (*(u32 *)(car + 0x4F8) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F8));
    }
    *(s32 *)(car + 0x4F0) = -1;
    *(s32 *)(car + 0x4F4) = -1;
    *(s32 *)(car + 0x4F8) = -1;
    MotionState_Release(car + 0x330);
    Thing_Release(car);
}

void CarA_GetInverseLocalInertiaTensor(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

f32 CarA_Transmission_GetTorqueAtWheels(u8 *car)
{
    u8 *trans;
    f32 scale;
    f32 rpm;
    f32 d;
    f32 c;
    f32 b;
    f32 a;

    a = *(f32 *)(car + 0x4C0);
    b = *(f32 *)(car + 0x4C4);
    c = *(f32 *)(car + 0x4C8);
    d = *(f32 *)(car + 0x4CC);
    rpm = *(f32 *)(car + 0x4E4);
    scale = 1.0f;
    if (*(s32 *)(car + 0x470) < 10000)
    {
        scale = 0.5f;
        rpm *= scale;
    }
    else if (*(u32 *)(car + 0x48C) & 0x1000)
    {
        trans = *(u8 **)(car + 0x3BC);
        rpm /= *(f32 *)(trans + 0x30);
        scale *= *(f32 *)(trans + 0x34);
    }
    if (rpm < 0.0f)
    {
        rpm *= -1.0f;
    }
    trans = *(u8 **)(car + 0x3BC);
    return scale * (rpm * (rpm * (a * rpm + b) + c) + d) /
           ((f32 *)(*(u8 **)(trans + 0xDC)))[*(u8 *)(car + 0x4E8) * 2];
}

void CarA_ProcessForce(u8 *car, Vector4 *force, Vector4 *dir)
{
    void PSVECScale(Vector4 *src, Vector4 *dst, u32 scale);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *dst);
    void PSVECAdd(Vector4 *a, Vector4 *b, Vector4 *dst);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *dst);
    f32 GetComponentOfVector(Vector4 *v, Vector4 *dir);

    Vector4 pointVel;
    Vector4 cross;
    Vector4 r;
    Vector4 torque;
    Vector4 carVel;
    Vector4 scaled;

    carVel = *(Vector4 *)car;
    PSVECScale(dir, &scaled, *(u32 *)(car + 8));
    PSVECSubtract(force, (Vector4 *)(car + 0x40), &r);
    GetComponentOfVector(dir, &carVel);
    PSVECCrossProduct((Vector4 *)(car + 0x410), &r, &cross);
    PSVECAdd((Vector4 *)car, &cross, &pointVel);
    PSVECCrossProduct(&r, &scaled, &torque);

    *(f32 *)(car + 0x390) += scaled.x;
    *(f32 *)(car + 0x394) += scaled.y;
    *(f32 *)(car + 0x398) += scaled.z;
    *(f32 *)(car + 0x3A0) += torque.x;
    *(f32 *)(car + 0x3A4) += torque.y;
    *(f32 *)(car + 0x3A8) += torque.z;
}

void CarA_SetSlavePlayers(u8 *car)
{
    u8 *slave;

    for (slave = *(u8 **)(car + 0x344); slave != NULL; slave = *(u8 **)(slave + 0xA4))
    {
        *(s32 *)(slave + 0x94) = *(s32 *)(car + 0x94);
    }
}

void CarA_DrawReflection(void *thing) {
    typedef struct { void *geom; u16 mtxIndex; u8 pad6[2]; } ModelPart;
    typedef struct {
        u32 numGeoms;
        void **geoms;
        u32 numParts;
        ModelPart *parts;
        u8 pad10[4];
        s32 *skel;
    } Model;
    typedef struct CarPart {
        u8 flags;
        u8 xform;
        u8 pad2[2];
        u16 mtxIndex;
        u8 pad6[2];
        void *geom;
        Vector4 *offset;
        u8 pad10[0xC];
    } CarPart;
    typedef struct CarPartList {
        u32 count;
        CarPart *parts;
    } CarPartList;
    typedef struct CarTrailBone {
        f32 offset;
        f32 width0;
        f32 width1;
        u8 padC[0x3F - 0xC];
        u8 parent;
        u16 bone;
        u8 pad42[0x50 - 0x42];
    } CarTrailBone;
    void LightManager_LightThing(void *thing);
    void Gfx_DrawGeometry(void *geom, void *mtx);
    void CarA_Extract_Keyframes(u8 *car, void *frames, void *state);
    void Create_Plane3(Vector4 *plane, Vector4 *normal, Vector4 *point);
    f32 Distance_Plane(void *plane, void *point);
    void Gfx_AnimTraverseHeirarchyToMatrices(void *frames, s32 skel, Matrix4 *matrices);
    void MotionState_DrawSlaves(void *state, s32 arg1);
    void MotionState_DrawSlavesAttachCoronaOnly(void *state, s32 arg1);
    Matrix4 matrices[64];
    u8 frames[0x800];
    u8 frames2[0x800];
    Matrix4 partMtx;
    Matrix4 mtx;
    Matrix4 tmp;
    Matrix4 partMtx2;
    Vector4 plane;
    Vector4 pos;
    u8 *car = thing;
    f32 offset = 0.0f;
    Matrix4 *oldMatrices = *(Matrix4 **)(car + 0x340);
    f32 h;

    if (*(Model **)(car + 0x330) != NULL) {
        LightManager_LightThing(car);
        *(Matrix4 **)(car + 0x340) = matrices;
        {
            s32 *skel = (*(Model **)(car + 0x330))->skel;
            CarA_Extract_Keyframes(car, frames, *(Model **)(car + 0x330));
            XformVec(car + 0x10, (Vector4 *)(car + 0x520), &pos);
            TEST_AnimTraverseHeirarchyToMatricesWorld((Vector4 *)(car + 0x50), &pos, frames, *skel, matrices, 1.0f);
        }
        Create_Plane3(&plane, (Vector4 *)(car + 0x20), &pos);
        h = -Distance_Plane(&plane, &matrices[(*(CarTrailBone **)(*(u8 **)(car + 0x3BC) + 0xD4))->bone].v[3]);
        offset += 2.0f * h + 2.0f * (*(CarTrailBone **)(*(u8 **)(car + 0x3BC) + 0xD4))->offset;
        {
            u32 i;
            for (i = 0; i < (*(Model **)(car + 0x330))->numParts; i++) {
                ModelPart *e = &(*(Model **)(car + 0x330))->parts[i];
                Gfx_DrawGeometry(e->geom, &matrices[e->mtxIndex]);
            }
        }
        {
            u32 j;
            CarPartList *list = *(CarPartList **)(*(u8 **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x78) + 0x10) + 0x38);
            CarPart *part;
            u32 bit;
            void *geom;
            if (list != NULL) {
                for (j = 0; j < list->count; j++) {
                    part = &list->parts[j];
                    geom = part->geom;
                    bit = *(u32 *)(car + 0x374) & (1 << j);
                    if (bit && !(part->flags & 1)) {
                        continue;
                    }
                    if (!bit && (part->flags & 1)) {
                        continue;
                    }
                    if (part->xform) {
                        partMtx = matrices[part->mtxIndex];
                        XformVec(&partMtx, list->parts[j].offset, &partMtx.v[3]);
                        Gfx_DrawGeometry(geom, &partMtx);
                    } else {
                        Gfx_DrawGeometry(geom, &matrices[part->mtxIndex]);
                    }
                }
            }
        }
        MotionState_DrawSlaves(car + 0x330, 0);
        mtx = *(Matrix4 *)(car + 0x10);
        {
            u32 i;
            s32 *skel = (*(Model **)(car + 0x330))->skel;
            CarA_Extract_Keyframes(car, frames2, *(Model **)(car + 0x330));
            Gfx_AnimTraverseHeirarchyToMatrices(frames2, *skel, matrices);
            mtx.v[1].x *= -1.0f;
            mtx.v[1].y *= -1.0f;
            mtx.v[1].z *= -1.0f;
            mtx.v[0].x *= -1.0f;
            mtx.v[0].y *= -1.0f;
            mtx.v[0].z *= -1.0f;
            XformVec(car + 0x10, (Vector4 *)(car + 0x520), &mtx.v[3]);
            mtx.v[3].x += mtx.v[1].x * offset;
            mtx.v[3].y += mtx.v[1].y * offset;
            mtx.v[3].z += mtx.v[1].z * offset;
            for (i = 0; i < *skel; i++) {
                tmp = matrices[i];
                MultiplyMatrix(&tmp, &mtx, &matrices[i]);
            }
        }
        {
            u32 i;
            for (i = 0; i < (*(Model **)(car + 0x330))->numParts; i++) {
                ModelPart *e = &(*(Model **)(car + 0x330))->parts[i];
                Gfx_DrawGeometry(e->geom, &matrices[e->mtxIndex]);
            }
        }
        {
            u32 j;
            CarPartList *list = *(CarPartList **)(*(u8 **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x78) + 0x10) + 0x38);
            CarPart *part;
            u32 bit;
            void *geom;
            if (list != NULL) {
                for (j = 0; j < list->count; j++) {
                    part = &list->parts[j];
                    geom = part->geom;
                    bit = *(u32 *)(car + 0x374) & (1 << j);
                    if (bit && !(part->flags & 1)) {
                        continue;
                    }
                    if (!bit && (part->flags & 1)) {
                        continue;
                    }
                    if (part->xform) {
                        partMtx2 = matrices[part->mtxIndex];
                        XformVec(&partMtx2, list->parts[j].offset, &partMtx2.v[3]);
                        Gfx_DrawGeometry(geom, &partMtx2);
                    } else {
                        Gfx_DrawGeometry(geom, &matrices[part->mtxIndex]);
                    }
                }
            }
        }
        MotionState_DrawSlavesAttachCoronaOnly(car + 0x330, 0);
        *(Matrix4 **)(car + 0x340) = oldMatrices;
    }
}

void CarA_Draw(u8 *car)
{
    s8 Player_IsHuman(void *player);
    s32 StateManager_CurrState(void);
    u32 ViewManager_GetViewFormat(void);
    void Gfx_DrawGeometry(void *, Matrix4 *);
    void CarA_DrawScript(u8 *);
    void XformVec(void *, Vector4 *, Vector4 *);
    void MotionState_DrawSlaves(void *, s32);
    extern void *g_pNoRayCollideThing;
    Matrix4 mtx __attribute__((aligned(16)));
    f64 b;
    f64 a;
    u8 *p2;

    if (*(u8 **)(car + 0x94) != NULL && (*(u32 *)(*(u8 **)(car + 0x94) + 0x6C) & 0x10) &&
        !(*(u32 *)(*(u8 **)(car + 0x94) + 0x6C) & 0x80) && ViewManager_GetViewFormat() <= 1) {
        p2 = *(u8 **)(*(u8 **)(car + 0x3BC) + 0x80);
        if ((*((u8 **)(p2 + 0x84)))[((*(u8 **)(car + 0x94))[0x31] % *(u32 *)(p2 + 0x80)) * 0xC] == 4) {
            goto skipscript;
        }
    }
    CarA_DrawScript(car);
skipscript:
    if (*(void **)(*(u8 **)(car + 0x3BC) + 0x54) != NULL && StateManager_CurrState() == 5) {
        a = ((f64 *)(car + 0x10))[0];
        b = ((f64 *)(car + 0x10))[1];
        ((f64 *)&mtx)[0] = a;
        ((f64 *)&mtx)[1] = b;
        b = ((f64 *)(car + 0x10))[3];
        a = ((f64 *)(car + 0x10))[2];
        ((f64 *)&mtx)[2] = a;
        ((f64 *)&mtx)[3] = b;
        b = ((f64 *)(car + 0x10))[5];
        a = ((f64 *)(car + 0x10))[4];
        ((f64 *)&mtx)[4] = a;
        ((f64 *)&mtx)[5] = b;
        b = ((f64 *)(car + 0x10))[7];
        a = ((f64 *)(car + 0x10))[6];
        ((f64 *)&mtx)[6] = a;
        ((f64 *)&mtx)[7] = b;
        XformVec(car + 0x10, (Vector4 *)(car + 0x520), &mtx.v[3]);
        if (Player_IsHuman(*(void **)(car + 0x94))) {
            Gfx_DrawGeometry(*(void **)(*(u8 **)(car + 0x3BC) + 0x54), &mtx);
        }
    }
    g_pNoRayCollideThing = car;
    MotionState_DrawSlaves(car + 0x330, 0);
    g_pNoRayCollideThing = NULL;
}

f32 CarA_GetMass(u8 *car)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    return *(f32 *)(coll + 0x20);
}

void CarA_GetTargetPos(u8 *car, Vector4 *pos)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    switch (*(u8 *)(coll + 0x10))
    {
    case 0:
        *pos = *(Vector4 *)(car + 0xC0);
        break;
    case 1:
        *pos = *(Vector4 *)(*(u8 **)(car + 0x370) + 0x3D0);
        break;
    }
}

void *CarA_GetOrientedCollision(u8 *car)
{
    void BuildOOBOffset(void *, void *, void *, void *);
    void BuildOConvexHullOffset(void *, void *, void *, void *);
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    switch (*(u8 *)(coll + 0x10))
    {
    case 0:
        if (*(u32 *)(car + 0x74) & 0x180)
        {
            BuildOOBOffset(*(u8 **)(coll + 0x44), car + 0x10, car + 0x520, car + 0xC0);
            *(u32 *)(car + 0x74) &= ~0x180;
        }
        return car + 0xC0;
    case 1:
        BuildOConvexHullOffset(*(u8 **)(car + 0x370), *(u8 **)(coll + 0x48), car + 0x10, car + 0x520);
        return *(u8 **)(car + 0x370);
    }
    return NULL;
}

s32 CarA_GetSurfaceScript(u8 *car)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    return *(s32 *)(coll + 0x30);
}

s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}
