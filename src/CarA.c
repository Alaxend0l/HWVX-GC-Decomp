#include "CarA.h"
#include "common.h"

/* Moves the body offset (centre of mass) to (x, y, z), shifting the car's
   position so the body stays where it is in the world. */
static inline void CarA_SetBodyOffset(CarA *car, f32 x, f32 y, f32 z) {
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    Vector4 delta __attribute__((aligned(16)));
    Vector4 world __attribute__((aligned(16)));

    delta.x = x - car->bodyOffset.x;
    delta.y = y - car->bodyOffset.y;
    delta.z = z - car->bodyOffset.z;
    XformVecOrientation(&car->worldMatrix, &delta, &world);
    car->worldMatrix.v[3].x -= world.x;
    car->worldMatrix.v[3].y -= world.y;
    car->worldMatrix.v[3].z -= world.z;
    car->bodyOffset.x = x;
    car->bodyOffset.y = y;
    car->bodyOffset.z = z;
    car->collDirtyFlags |= 0x100;
}


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

void CarA_GetAngularVelocity(void *car, Vector4 *vel) {
    *vel = M2C_FIELD(car, Vector4 *, 0x410);
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
    Vector4 scaled __attribute__((aligned(16)));

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

void CarA_ProcessImpulse(u8 *car, Vector4 *normal, Vector4 *pos, Vector4 *pointVel, f32 restitution) {
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    f32 PSVECSquareMag(Vector4 *v);
    f32 Sqrt(f32 x);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    Vector4 impulse;
    Vector4 r;
    Vector4 vel = *pointVel;
    Vector4 rxn;
    Vector4 ang;
    Vector4 angxr;
    Vector4 localRxn;
    Vector4 scaledRxn;
    Vector4 torque;
    Vector4 localAng;
    Vector4 scaledAng;
    Vector4 scaled;
    Vector4 local;
    f32 num;
    f32 j;
    f32 maxAng;
    f32 mag;

    PSVECSubtract(pos, (Vector4 *)(car + 0x40), &r);
    num = -(restitution * *(f32 *)(*(u8 **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x30) + 0x18) + 1.0f) * PSVECDotProduct(&vel, normal);
    PSVECCrossProduct(&r, normal, &rxn);
    InverseXformVecOrientation(car + 0x10, &rxn, &localRxn);
    scaledRxn.x = *(f32 *)(car + 0x3C0) * localRxn.x;
    scaledRxn.y = *(f32 *)(car + 0x3D4) * localRxn.y;
    scaledRxn.z = *(f32 *)(car + 0x3E8) * localRxn.z;
    XformVecOrientation(car + 0x10, &scaledRxn, &ang);
    PSVECCrossProduct(&ang, &r, &angxr);
    j = num / (1.0f / *(f32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x20) + PSVECDotProduct(&angxr, normal));
    impulse.x = j * normal->x;
    impulse.y = j * normal->y;
    impulse.z = j * normal->z;
    *(f32 *)(car + 0x0) += impulse.x / *(f32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x20);
    *(f32 *)(car + 0x4) += impulse.y / *(f32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x20);
    *(f32 *)(car + 0x8) += impulse.z / *(f32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x20);
    PSVECCrossProduct(&r, &impulse, &torque);
    *(f32 *)(car + 0x380) += torque.x;
    *(f32 *)(car + 0x384) += torque.y;
    *(f32 *)(car + 0x388) += torque.z;
    if (*(u32 *)(car + 0x3B8) <= 1) {
        maxAng = *(f32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x80) + 0x3C);
        InverseXformVecOrientation(car + 0x10, (Vector4 *)(car + 0x380), &localAng);
        scaledAng.x = *(f32 *)(car + 0x3C0) * localAng.x;
        scaledAng.y = *(f32 *)(car + 0x3D4) * localAng.y;
        scaledAng.z = *(f32 *)(car + 0x3E8) * localAng.z;
        mag = PSVECSquareMag(&scaledAng);
        if (mag > maxAng * maxAng) {
            mag = maxAng / Sqrt(mag);
            scaledAng.x *= mag;
            scaledAng.y *= mag;
            scaledAng.z *= mag;
            localAng.x = scaledAng.x / *(f32 *)(car + 0x3C0);
            localAng.y = scaledAng.y / *(f32 *)(car + 0x3D4);
            localAng.z = scaledAng.z / *(f32 *)(car + 0x3E8);
            XformVecOrientation(car + 0x10, &localAng, (Vector4 *)(car + 0x380));
        }
    }
    InverseXformVecOrientation(car + 0x10, (Vector4 *)(car + 0x380), &local);
    scaled.x = *(f32 *)(car + 0x3C0) * local.x;
    scaled.y = *(f32 *)(car + 0x3D4) * local.y;
    scaled.z = *(f32 *)(car + 0x3E8) * local.z;
    XformVecOrientation(car + 0x10, &scaled, (Vector4 *)(car + 0x410));
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

void CarA_DrawScript(u8 *car) {
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
    void LightManager_LightThing(void *thing);
    void Gfx_DrawAnimGeometryAux(void *geom, void *mtx, s32 skel, s32 arg3);
    void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);
    void Gfx_DrawGeometry(void *geom, void *mtx);
    Matrix4 *matrices;
    u32 i;
    s32 *skel;
    u32 j;
    CarPartList *list;
    CarPart *part;
    u32 bit;
    void *geom;
    Matrix4 mtx;

    matrices = *(Matrix4 **)(car + 0x340);
    if (matrices == NULL) {
        return;
    }
    LightManager_LightThing(car);
    skel = (*(Model **)(car + 0x330))->skel;
    for (i = 0; i < (*(Model **)(car + 0x330))->numGeoms; i++) {
        Gfx_DrawAnimGeometryAux((*(Model **)(car + 0x330))->geoms[i], matrices, *skel, 0);
    }
    for (i = 0; i < (*(Model **)(car + 0x330))->numParts; i++) {
        ModelPart *e = &(*(Model **)(car + 0x330))->parts[i];
        Gfx_DrawGeometry_MatrixNotCached(e->geom, &matrices[e->mtxIndex]);
    }
    list = *(CarPartList **)(*(u8 **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x78) + 0x10) + 0x38);
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
                mtx = matrices[part->mtxIndex];
                XformVec(&mtx, list->parts[j].offset, &mtx.v[3]);
                Gfx_DrawGeometry(geom, &mtx);
            } else {
                Gfx_DrawGeometry_MatrixNotCached(geom, &matrices[part->mtxIndex]);
            }
        }
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

void CarA_DynamicRotation(u8 *car, f32 dt) {
    typedef struct OBox {
        Vector4 center;
        Vector4 corners[8];
        u8 pad[0x60];
    } OBox;
    typedef struct RayHit {
        Vector4 pos;
        Vector4 normal;
        u8 pad20[8];
        u8 *surface;
        u8 pad2C[0x24];
    } RayHit;
    extern u8 *g_pNoRayCollideThing;
    f32 MagnitudeVector(Vector4 *v);
    void GenerateQuaternion(Vector4 *axis, Quaternion *q, f32 angle);
    void QuaternionToSubMatrix(Quaternion *q, Matrix4 *m);
    void CarA_StuntsGetNewOrientation(u8 *car, Quaternion *q, f32 t);
    void *CarA_GetOrientedCollision(void *car);
    void BuildOOBOffset(void *oob, void *pos, void *rot, void *out);
    s8 RayHitsTerrainAndObjects(void *pos, void *vel, f32 *t, RayHit *hit);
    s8 OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(void *obox, void *thing, void *hit);
    void PSVECSubtract(void *a, void *b, Vector4 *out);
    void PSVECCrossProduct(void *a, void *b, Vector4 *out);
    OBox obox;
    RayHit hit;
    Matrix4 mtx;
    Quaternion q;
    Quaternion newQ;
    Vector4 axis;
    Vector4 dir;
    Vector4 r;
    Vector4 v;
    f32 t;
    f32 mag;
    u32 i;
    OBox *old;
    void *box;
    s8 hitAny;

    t = dt;
    axis = *(Vector4 *)(car + 0x410);
    mag = MagnitudeVector(&axis);
    if (mag > 0.0001f) {
        axis.x /= mag;
        axis.y /= mag;
        axis.z /= mag;
    }
    GenerateQuaternion(&axis, &q, mag * t);
    QuaternionMult((Vector4 *)&q, (Vector4 *)(car + 0x50), (Vector4 *)&newQ);
    CarA_StuntsGetNewOrientation(car, &newQ, t);
    QuaternionToSubMatrix(&newQ, &mtx);
    mtx.v[3] = *(Vector4 *)(car + 0x40);
    switch ((*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0))[0x10]) {
    case 0:
        box = *(void **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x44);
        old = CarA_GetOrientedCollision(car);
        hitAny = 0;
        BuildOOBOffset(box, &mtx, car + 0x520, &obox);
        g_pNoRayCollideThing = car;
        for (i = 0; i < 8; i++) {
            if (t > 0.0f) {
                dir.x = (obox.corners[i].x - old->corners[i].x) / dt;
                dir.y = (obox.corners[i].y - old->corners[i].y) / dt;
                dir.z = (obox.corners[i].z - old->corners[i].z) / dt;
                if (RayHitsTerrainAndObjects(&old->corners[i], &dir, &t, &hit)) {
                    hitAny = 1;
                    CarA_ProcessImpulse(car, &hit.normal, &hit.pos, &dir, *(f32 *)(hit.surface + 0x18));
                }
            }
        }
        g_pNoRayCollideThing = NULL;
        if (!hitAny) {
            if (OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(&obox, car, &hit)) {
                PSVECSubtract(&hit.pos, car + 0x40, &r);
                PSVECCrossProduct(car + 0x410, &r, &v);
                CarA_ProcessImpulse(car, &hit.normal, &hit.pos, &v, 0.5f);
            } else {
                *(Quaternion *)(car + 0x50) = newQ;
                *(Matrix4 *)(car + 0x10) = mtx;
                *(OBox *)(car + 0xC0) = obox;
                *(u8 **)(car + 0x160) = car + 0x30;
                *(u8 **)(car + 0x164) = car + 0x10;
                *(u8 **)(car + 0x168) = car + 0x20;
                *(u32 *)(car + 0x74) &= ~0x180;
            }
        }
        break;
    case 1:
        break;
    }
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

void CarA_GetInterfaceInfo(u8 *car, u8 *info) {
    void *ThingManagerList_GetByType(void *list, s32 type);
    s8 GameSettings_IsCheatEnabled(s32 cheat);
    u32 GameSettings_GetCodeVariable(s32, s32);
    u32 GameSettings_GetNumOfPlayers(void);
    u32 CameraManager_GetActiveCameras(void);
    f32 _fabs(f32 x);
    typedef struct {
        u32 active;
        u8 unk4[0x3C];
        s16 ammo;
        u8 unk42[0xA];
        u8 *item;
    } RackSlot;
    typedef struct {
        u8 unk0[0xB0];
        RackSlot slots[12];
        u8 current;
    } WeaponRack;
    WeaponRack *rack;
    u32 count;

    rack = ThingManagerList_GetByType(car + 0x344, 8);
    *(u32 *)(info + 0x10) = 100.0f * ((f32)*(s32 *)(car + 0x78) / (f32)*(s32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x14));
    if (*(u32 *)(info + 0x10) > 100.0f) {
        *(u32 *)(info + 0x10) = 100;
    }
    if (rack != NULL && rack->slots[rack->current].item != NULL) {
        if (GameSettings_IsCheatEnabled(0)) {
            rack->slots[rack->current].ammo = *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x28);
        }
        if (*(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x24) != 0) {
            *(u32 *)(info + 0x38) = 100.0f * ((f32)rack->slots[rack->current].ammo / (f32)*(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x28));
            *(u32 *)(info + 0x3C) = *(u32 *)(info + 0x38) / *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x24);
        } else {
            *(u32 *)(info + 0x38) = 0;
            *(u32 *)(info + 0x3C) = 0;
        }
        if (*(u8 **)(rack->slots[rack->current].item + 0x30) != NULL) {
            *(u32 *)(info + 0x68) = *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x10);
        }
    }
    *(u32 *)(info + 0x14) = _fabs(0.681818f * *(f32 *)(car + 0x458));
    if (*(s8 *)(car + 0x584) && *(u32 *)(info + 0x14) < *(u32 *)(car + 0x58C)) {
        (*(u32 *)(car + 0x588))++;
        if (*(u32 *)(car + 0x588) > 5) {
            *(s8 *)(car + 0x584) = 0;
            *(u32 *)(car + 0x588) = 0;
            *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
        } else {
            *(u32 *)(info + 0x14) = *(u32 *)(car + 0x58C);
        }
    } else if (*(s8 *)(car + 0x584)) {
        *(u32 *)(car + 0x588) = 0;
        *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
    }
    if (!*(s8 *)(car + 0x584) && *(u32 *)(info + 0x14) > *(u32 *)(car + 0x58C)) {
        (*(u32 *)(car + 0x588))++;
        if (*(u32 *)(car + 0x588) > 5) {
            *(s8 *)(car + 0x584) = 1;
            *(u32 *)(car + 0x588) = 0;
            *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
        } else {
            *(u32 *)(info + 0x14) = *(u32 *)(car + 0x58C);
        }
    } else if (!*(s8 *)(car + 0x584)) {
        *(u32 *)(car + 0x588) = 0;
        *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
    }
    *(u32 *)(info + 0x18) = *(f32 *)(car + 0x4E4);
    *(f32 *)(info + 0x20) = *(s32 *)(car + 0x400) / 300;
    *(f32 *)(info + 0x2C) = 0.2f * (100.0f + *(f32 *)(car + 0x44));
    if (*(u8 **)(car + 0x94) != NULL && (*(u32 *)(*(u8 **)(car + 0x94) + 0x6C) & 0x200)) {
        if (GameSettings_GetCodeVariable(0x198, 0)) {
            count = CameraManager_GetActiveCameras();
        } else {
            count = GameSettings_GetNumOfPlayers();
        }
        if (count > 1) {
            if (count == 2) {
                *(u32 *)(info + 0x4) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x84) + 0x18);
                if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 1) {
                    *(s32 *)(info + 0x8) = 0;
                    *(s32 *)(info + 0xC) = 10;
                } else {
                    *(s32 *)(info + 0x8) = 0;
                    *(s32 *)(info + 0xC) = 230;
                }
            } else {
                *(u32 *)(info + 0x4) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x84) + 0x1C);
                if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 1) {
                    *(s32 *)(info + 0x8) = 10;
                    *(s32 *)(info + 0xC) = 10;
                } else if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 2) {
                    *(s32 *)(info + 0x8) = 310;
                    *(s32 *)(info + 0xC) = 10;
                } else if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 3) {
                    *(s32 *)(info + 0x8) = 10;
                    *(s32 *)(info + 0xC) = 230;
                } else {
                    *(s32 *)(info + 0x8) = 310;
                    *(s32 *)(info + 0xC) = 230;
                }
            }
        } else {
            *(u32 *)(info + 0x4) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x84) + 0x10);
            *(s32 *)(info + 0x8) = 0;
            *(s32 *)(info + 0xC) = 0;
        }
    }
    if (rack != NULL && rack->slots[rack->current].active != 0) {
        info[0x34] = 1;
        *(s16 *)(info + 0x6C) = *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x18);
    }
    if (*(u32 *)(car + 0x494) != 0) {
        *(u32 *)(info + 0x44) = *(u32 *)(car + 0x494);
        *(u32 *)(car + 0x494) = 0;
    }
    if (*(u32 *)(car + 0x498) != 0) {
        *(u32 *)(info + 0x40) = *(u32 *)(car + 0x498);
        *(u32 *)(car + 0x498) = 0;
    }
    if (*(u32 *)(car + 0x49C) != 0) {
        *(u32 *)(info + 0x48) = *(u32 *)(car + 0x49C);
        *(u32 *)(car + 0x49C) = 0;
    }
    if (*(u32 *)(car + 0x4A0) != 0) {
        *(u32 *)(info + 0x4C) = *(u32 *)(car + 0x4A0);
        *(u32 *)(car + 0x4A0) = 0;
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
