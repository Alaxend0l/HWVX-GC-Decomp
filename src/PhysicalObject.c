#include "common.h"


void Mover_GetTrackPos(void *mover, f32 *pos, f32 time) {
    void Gfx_AnimExtractKeyFrame(void *anim, f32 time, void *keyFrame, u32 count);
    f32 keyFrame[8];

    Gfx_AnimExtractKeyFrame(M2C_FIELD(mover, void **, 0xD4), time, keyFrame, 1);
    pos[0] = keyFrame[4];
    pos[1] = keyFrame[5];
    pos[2] = keyFrame[6];
}

void PhysicalObject_ProcessForce(u8 *obj, Vector4 *force, Vector4 *dir)
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
    Vector4 objVel;
    Vector4 scaled __attribute__((aligned(16)));

    objVel = *(Vector4 *)obj;
    PSVECScale(dir, &scaled, *(u32 *)(obj + 8));
    PSVECSubtract(force, (Vector4 *)(obj + 0x40), &r);
    GetComponentOfVector(dir, &objVel);
    PSVECCrossProduct((Vector4 *)(obj + 0x130), &r, &cross);
    PSVECAdd((Vector4 *)obj, &cross, &pointVel);
    PSVECCrossProduct(&r, &scaled, &torque);

    *(f32 *)(obj + 0xF0) += scaled.x;
    *(f32 *)(obj + 0xF4) += scaled.y;
    *(f32 *)(obj + 0xF8) += scaled.z;
    *(f32 *)(obj + 0x100) += torque.x;
    *(f32 *)(obj + 0x104) += torque.y;
    *(f32 *)(obj + 0x108) += torque.z;
}

s32 PhysicalObject_Init(u8 *obj, u8 *data) {
    typedef struct OBox {
        Vector4 v[3];
    } OBox;
    typedef struct Mesh {
        u8 pad0[0xC];
        u32 count;
        u8 pad10[0x1C - 0x10];
        Vector4 *verts;
    } Mesh;
    typedef struct DestructibleInfo {
        f32 inertia[3];
        u8 pad0C[0x10 - 0xC];
        u8 type;
        u8 pad11[0x20 - 0x11];
        f32 mass;
        u8 pad24[0x44 - 0x24];
        OBox *box;
        Mesh *mesh;
    } DestructibleInfo;
    void Destructible_Init(u8 *obj, u8 *data);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    s32 Thing_HitsTerrain(void *thing, void *arg1, void *arg2);
    u8 hit[0x50];
    OBox box;
    Vector4 scaled;
    Vector4 local;
    f32 dist;
    DestructibleInfo *info;
    Mesh *mesh;
    u32 i;

    *(f32 *)(data + 4) += 0.2f;
    Destructible_Init(obj, data);
    *(u32 *)(obj + 0x74) &= ~8;
    *(u32 *)(obj + 0x74) &= ~0x200;
    if (*(u8 **)(data + 0x5C) != NULL) {
        *(f32 *)(obj + 0xDC) = 1.0f + *(f32 *)(*(u8 **)(data + 0x5C) + 0xC);
    }
    info = *(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10);
    switch (info->type) {
    case 0:
        box = *info->box;
        *(f32 *)(obj + 0x120) = 1.0f / (info->inertia[0] * info->mass * (box.v[1].y * box.v[1].y + box.v[1].z * box.v[1].z));
        *(f32 *)(obj + 0x124) = 1.0f / ((*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->inertia[1] * (*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->mass * (box.v[1].x * box.v[1].x + box.v[1].z * box.v[1].z));
        *(f32 *)(obj + 0x128) = 1.0f / ((*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->inertia[2] * (*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->mass * (box.v[1].x * box.v[1].x + box.v[1].y * box.v[1].y));
        break;
    case 1:
    case 6:
        mesh = info->mesh;
        box.v[0].x = 9999.9f;
        box.v[0].y = 9999.9f;
        box.v[0].z = 9999.9f;
        box.v[1].x = -9999.9f;
        box.v[1].y = -9999.9f;
        box.v[1].z = -9999.9f;
        for (i = 0; i < mesh->count; i++) {
            if (box.v[0].x > mesh->verts[i].x) {
                box.v[0].x = mesh->verts[i].x;
            }
            if (box.v[0].y > mesh->verts[i].y) {
                box.v[0].y = mesh->verts[i].y;
            }
            if (box.v[0].z > mesh->verts[i].z) {
                box.v[0].z = mesh->verts[i].z;
            }
            if (box.v[1].x < mesh->verts[i].x) {
                box.v[1].x = mesh->verts[i].x;
            }
            if (box.v[1].y < mesh->verts[i].y) {
                box.v[1].y = mesh->verts[i].y;
            }
            if (box.v[1].z < mesh->verts[i].z) {
                box.v[1].z = mesh->verts[i].z;
            }
        }
        *(f32 *)(obj + 0x120) = 1.0f / ((*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->inertia[0] * (*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->mass * (box.v[1].y * box.v[1].y + box.v[1].z * box.v[1].z));
        *(f32 *)(obj + 0x124) = 1.0f / ((*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->inertia[1] * (*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->mass * (box.v[1].x * box.v[1].x + box.v[1].z * box.v[1].z));
        *(f32 *)(obj + 0x128) = 1.0f / ((*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->inertia[2] * (*(DestructibleInfo **)(*(u8 **)(obj + 0xC0) + 0x10))->mass * (box.v[1].x * box.v[1].x + box.v[1].y * box.v[1].y));
        break;
    }
    if (((Vector4 *)(data + 0x20))->x || ((Vector4 *)(data + 0x20))->y || ((Vector4 *)(data + 0x20))->z) {
        InverseXformVecOrientation(obj + 0x10, (Vector4 *)(data + 0x20), &local);
        scaled.x = local.x / *(f32 *)(obj + 0x120);
        scaled.y = local.y / *(f32 *)(obj + 0x124);
        scaled.z = local.z / *(f32 *)(obj + 0x128);
        XformVecOrientation(obj + 0x10, &scaled, (Vector4 *)(obj + 0x110));
    }
    if (!(*(u32 *)(obj + 0x74) & 0x40002)) {
        dist = 1.0f;
        if ((s8)Thing_HitsTerrain(obj, &dist, hit) && 0.0f == dist) {
            (*(void (**)(u8 *))(*(u8 **)(obj + 0x8C) + 4))(obj);
        }
    }
    return 1;
}

void Mover_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void PhysicalObject_IntegrateNonLateral(u8 *obj) {
    extern Vector4 g_zeroVector;
    extern f32 g_fDeltaTimeSec;
    void XformVecOrientation(void *mtx, f32 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, f32 *out);
    f32 scaled[4];
    f32 local[4];
    f32 invMass;

    invMass = g_fDeltaTimeSec / *(f32 *)(*(u8 **)(*(u8 **)(obj + 0xC0) + 0x10) + 0x20);
    ((Vector4 *)obj)->x += invMass * ((Vector4 *)(obj + 0xF0))->x;
    ((Vector4 *)obj)->y += invMass * ((Vector4 *)(obj + 0xF0))->y;
    ((Vector4 *)obj)->z += invMass * ((Vector4 *)(obj + 0xF0))->z;
    ((Vector4 *)(obj + 0x110))->x += g_fDeltaTimeSec * ((Vector4 *)(obj + 0x100))->x;
    ((Vector4 *)(obj + 0x110))->y += g_fDeltaTimeSec * ((Vector4 *)(obj + 0x100))->y;
    ((Vector4 *)(obj + 0x110))->z += g_fDeltaTimeSec * ((Vector4 *)(obj + 0x100))->z;
    *(Vector4 *)(obj + 0x100) = g_zeroVector;
    *(Vector4 *)(obj + 0xF0) = *(Vector4 *)(obj + 0x100);
    InverseXformVecOrientation(obj + 0x10, (Vector4 *)(obj + 0x110), local);
    scaled[0] = ((Vector4 *)(obj + 0x120))->x * local[0];
    scaled[1] = ((Vector4 *)(obj + 0x120))->y * local[1];
    scaled[2] = ((Vector4 *)(obj + 0x120))->z * local[2];
    XformVecOrientation(obj + 0x10, scaled, (Vector4 *)(obj + 0x130));
}

s32 PhysicalObject_PreMove(u8 *obj) {
    extern Vector4 g_zeroVector;
    extern f32 g_fDeltaTimeSec;
    void XformVecOrientation(void *mtx, f32 *in, f32 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, f32 *out);
    f32 angVel[4];
    f32 scaled[4];
    f32 local[4];
    f32 damping;
    f32 scaled2[4];
    f32 local2[4];
    f32 invMass;

    if (!(*(u32 *)(obj + 0x74) & 8)) {
        *(Vector4 *)(obj + 0x100) = g_zeroVector;
        *(Vector4 *)(obj + 0xF0) = *(Vector4 *)(obj + 0x100);
        ((Vector4 *)(obj + 0xF0))->y += *(f32 *)(*(u8 **)(*(u8 **)(obj + 0xC0) + 0x10) + 0x24) *
                                        *(f32 *)(*(u8 **)(*(u8 **)(obj + 0xC0) + 0x10) + 0x20);
        damping = *(f32 *)(*(u8 **)(*(u8 **)(obj + 0xC0) + 0x10) + 0x2C);
        InverseXformVecOrientation(obj + 0x10, (Vector4 *)(obj + 0x110), local);
        scaled[0] = ((Vector4 *)(obj + 0x120))->x * local[0];
        scaled[1] = ((Vector4 *)(obj + 0x120))->y * local[1];
        scaled[2] = ((Vector4 *)(obj + 0x120))->z * local[2];
        XformVecOrientation(obj + 0x10, scaled, angVel);
        ((Vector4 *)(obj + 0x100))->x += -damping * angVel[0];
        ((Vector4 *)(obj + 0x100))->y += -damping * angVel[1];
        ((Vector4 *)(obj + 0x100))->z += -damping * angVel[2];
        invMass = g_fDeltaTimeSec / *(f32 *)(*(u8 **)(*(u8 **)(obj + 0xC0) + 0x10) + 0x20);
        ((Vector4 *)obj)->x += invMass * ((Vector4 *)(obj + 0xF0))->x;
        ((Vector4 *)obj)->y += invMass * ((Vector4 *)(obj + 0xF0))->y;
        ((Vector4 *)obj)->z += invMass * ((Vector4 *)(obj + 0xF0))->z;
        ((Vector4 *)(obj + 0x110))->x += g_fDeltaTimeSec * ((Vector4 *)(obj + 0x100))->x;
        ((Vector4 *)(obj + 0x110))->y += g_fDeltaTimeSec * ((Vector4 *)(obj + 0x100))->y;
        ((Vector4 *)(obj + 0x110))->z += g_fDeltaTimeSec * ((Vector4 *)(obj + 0x100))->z;
        *(Vector4 *)(obj + 0x100) = g_zeroVector;
        *(Vector4 *)(obj + 0xF0) = *(Vector4 *)(obj + 0x100);
        InverseXformVecOrientation(obj + 0x10, (Vector4 *)(obj + 0x110), local2);
        scaled2[0] = ((Vector4 *)(obj + 0x120))->x * local2[0];
        scaled2[1] = ((Vector4 *)(obj + 0x120))->y * local2[1];
        scaled2[2] = ((Vector4 *)(obj + 0x120))->z * local2[2];
        XformVecOrientation(obj + 0x10, scaled2, (f32 *)(obj + 0x130));
    }
    return 1;
}

