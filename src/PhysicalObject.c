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

u8 PhysicalObject_Move(u8 *obj) {
    typedef struct {
        u8 pad0[0x14];
        f32 friction;
        f32 restitution;
    } Material;
    typedef struct {
        u8 pad0[0x4];
        void (*stop)(u8 *obj);
        u8 pad8[0x14 - 0x8];
        f32 (*getMass)(u8 *obj);
        void (*applyImpulse)(u8 *obj, void *hit, s32 arg2, f32 impulse);
        u8 pad1C[0x28 - 0x1C];
        Material *(*getMaterial)(u8 *obj);
        u8 pad2C[0x30 - 0x2C];
        void (*getInverseInertia)(u8 *obj, Vector4 *out);
        void (*integrate)(u8 *obj, f32 dt);
    } VTable;
    typedef struct {
        Vector4 pos;
        Vector4 normal;
        u8 *thing;
        s32 unk24;
        Material *material;
        u8 pad2C[0x40 - 0x2C];
        u8 type;
    } CollisionInfo;
    extern f32 g_fDeltaTimeSec;
    f32 Sqrt(f32 x);
    s8 Timer_Interrupt300(void);
    s32 Thing_HitsTerrain(void *thing, void *arg1, void *arg2);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *src, Vector4 *dst);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *axb);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    CollisionInfo hit;
    Vector4 tmp;
    Vector4 negVel;
    Vector4 r;
    Vector4 rxn;
    Vector4 angVel;
    Vector4 invInertia;
    Vector4 local;
    Vector4 scaled;
    f32 dt;
    f32 elapsed;
    f32 invMass;
    f32 vn;
    f32 e1;
    f32 e2;
    f32 friction;
    f32 inv;
    u32 iter;
    s32 hitTerrain;

    if (!(*(u32 *)(obj + 0x74) & 0xD)) {
        elapsed = 0.0f;
        iter = 0;
        if (Timer_Interrupt300()) {
            inv = 1.0f / Sqrt(((Vector4 *)(obj + 0x50))->x * ((Vector4 *)(obj + 0x50))->x +
                              ((Vector4 *)(obj + 0x50))->y * ((Vector4 *)(obj + 0x50))->y +
                              ((Vector4 *)(obj + 0x50))->z * ((Vector4 *)(obj + 0x50))->z +
                              ((Vector4 *)(obj + 0x50))->w * ((Vector4 *)(obj + 0x50))->w);
            ((Vector4 *)(obj + 0x50))->x *= inv;
            ((Vector4 *)(obj + 0x50))->y *= inv;
            ((Vector4 *)(obj + 0x50))->z *= inv;
            ((Vector4 *)(obj + 0x50))->w *= inv;
        }
        do {
            dt = g_fDeltaTimeSec - elapsed;
            hitTerrain = Thing_HitsTerrain(obj, &dt, &hit);
            ((Vector4 *)(obj + 0x40))->x += ((Vector4 *)obj)->x * dt;
            ((Vector4 *)(obj + 0x40))->y += ((Vector4 *)obj)->y * dt;
            ((Vector4 *)(obj + 0x40))->z += ((Vector4 *)obj)->z * dt;
            *(u32 *)(obj + 0x74) |= 0x100;
            (*(VTable **)(obj + 0x8C))->integrate(obj, dt);
            hit.thing = obj;
            hit.unk24 = 0;
            if (0.0f == dt) {
                (*(VTable **)(obj + 0x8C))->stop(obj);
                return 0;
            }
            if ((s8)hitTerrain) {
                invMass = 1.0f / (*(VTable **)(obj + 0x8C))->getMass(obj);
                vn = PSVECDotProduct(&hit.normal, (Vector4 *)hit.thing);
                e1 = (*(VTable **)(hit.thing + 0x8C))->getMaterial(hit.thing)->restitution;
                e2 = hit.material->restitution;
                friction = 0.3f * hit.material->friction * (*(VTable **)(hit.thing + 0x8C))->getMaterial(hit.thing)->friction;
                negVel.x = -((Vector4 *)obj)->x;
                negVel.y = -((Vector4 *)obj)->y;
                negVel.z = -((Vector4 *)obj)->z;
                if (PSVECSquareMag(&negVel) > 0.0f) {
                    if (friction > 1.0f) {
                        friction = 1.0f;
                    }
                    if (PSVECSquareMag(&negVel)) {
                        PSVECNormalize(&negVel, &negVel);
                    }
                    hit.normal.x = (1.0f - friction) * hit.normal.x + friction * negVel.x;
                    hit.normal.y = (1.0f - friction) * hit.normal.y + friction * negVel.y;
                    hit.normal.z = (1.0f - friction) * hit.normal.z + friction * negVel.z;
                    if (PSVECSquareMag(&hit.normal)) {
                        PSVECNormalize(&hit.normal, &hit.normal);
                    }
                }
                PSVECSubtract(&hit.pos, (Vector4 *)(hit.thing + 0x40), &r);
                PSVECCrossProduct(&r, &hit.normal, &rxn);
                (*(VTable **)(hit.thing + 0x8C))->getInverseInertia(hit.thing, &invInertia);
                InverseXformVecOrientation(hit.thing + 0x10, &rxn, &local);
                scaled.x = invInertia.x * local.x;
                scaled.y = invInertia.y * local.y;
                scaled.z = invInertia.z * local.z;
                XformVecOrientation(hit.thing + 0x10, &scaled, &angVel);
                PSVECCrossProduct(&angVel, &r, &tmp);
                friction = PSVECDotProduct(&tmp, &hit.normal);
                friction = invMass * PSVECDotProduct(&hit.normal, &hit.normal) + friction;
                ((Vector4 *)(obj + 0x40))->x += 0.03f * hit.normal.x;
                ((Vector4 *)(obj + 0x40))->y += 0.03f * hit.normal.y;
                ((Vector4 *)(obj + 0x40))->z += 0.03f * hit.normal.z;
                (*(VTable **)(obj + 0x8C))->applyImpulse(obj, &hit, 0, vn * -(e1 * e2 + 1.0f) / friction);
            }
            if (*(u32 *)(obj + 0x74) & 0x200000) {
                elapsed = g_fDeltaTimeSec;
            }
            if (iter++ > 6) {
                elapsed = g_fDeltaTimeSec;
            }
            elapsed += dt;
        } while (elapsed < g_fDeltaTimeSec);
    }
    return Destructible_Move(obj);
}

void PhysicalObject_DynamicCollideImpact(u8 *obj, u8 *collision, f32 force) {
    extern Vector4 g_zeroVector;
    f32 PSVECSquareMag(void *v);
    f32 _fabs(f32 x);
    void Create_Plane(Vector4 *plane, Vector4 *normal, Vector4 *point);
    f32 Distance_Plane(void *plane, void *point);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void Destructible_Release(u8 *destructible);
    typedef struct {
        Vector4 center;
        Vector4 corners[8];
    } OBox;
    struct { Vector4 pos; Vector4 vel; Vector4 spin; Vector4 quat; void *info; u32 unk44; u32 unk48; u32 unk4C; u32 unk50; } params;
    Quaternion normal;
    Quaternion pos;
    f32 scale;
    u8 *other;

    normal = *(Quaternion *)(collision + 0x10);
    pos = *(Quaternion *)collision;
    scale = force / *(f32 *)(*(u8 **)(*(u8 **)(obj + 0xC0) + 0x10) + 0x20);
    if (*(u32 *)(obj + 0x74) & 0x200000) {
        return;
    }
    if ((*(u32 *)(obj + 0x74) & 0x100000) && normal.y > 0.9f && _fabs(scale) < 2.0f &&
        ((other = *(u8 **)(collision + 0x24)) == NULL || (*(u32 *)(other + 0x74) & 8))) {
        Vector4 center = {0.0f, 0.0f, 0.0f, 0.0f};
        Vector4 plane;
        u32 i;
        u8 *info;
        u8 *hull;
        f32 weight;
        f32 d;
        f32 w;

        weight = 0.0f;
        Create_Plane(&plane, (Vector4 *)&normal, (Vector4 *)collision);
        info = *(u8 **)(*(u8 **)(obj + 0xC0) + 0x10);
        switch (info[0x10]) {
        case 0:
            for (i = 0; i < 8; i++) {
                d = Distance_Plane(&plane, &(*(OBox **)(obj + 0xD0))->corners[i]);
                if (d < 0.15f) {
                    w = (0.15f - d) / 0.15f;
                    center.x += (*(OBox **)(obj + 0xD0))->corners[i].x * w;
                    center.y += (*(OBox **)(obj + 0xD0))->corners[i].y * w;
                    center.z += (*(OBox **)(obj + 0xD0))->corners[i].z * w;
                    weight += w;
                }
            }
            break;
        case 1:
        case 6:
            hull = *(u8 **)(info + 0x48);
            for (i = 0; i < *(u32 *)(hull + 0xC); i++) {
                d = Distance_Plane(&plane, &(*(Vector4 **)(obj + 0xCC))[i]);
                if (d < 0.15f) {
                    w = (0.15f - d) / 0.15f;
                    center.x += (*(Vector4 **)(obj + 0xCC))[i].x * w;
                    center.y += (*(Vector4 **)(obj + 0xCC))[i].y * w;
                    center.z += (*(Vector4 **)(obj + 0xCC))[i].z * w;
                    weight += w;
                }
            }
            break;
        }
        if (weight >= 3.0f) {
            PhysicalObject_ProjectSpin(obj, (Vector4 *)(collision + 0x10));
            if (PSVECSquareMag(obj + 0x130) < 1.0f && PSVECSquareMag(obj) < 25.0f) {
                params.pos = *(Vector4 *)(obj + 0x40);
                params.unk50 = 0;
                params.unk4C = 0;
                params.vel = g_zeroVector;
                params.info = *(void **)(obj + 0xC0);
                params.quat = *(Vector4 *)(obj + 0x50);
                params.spin = *(Vector4 *)(obj + 0x130);
                ThingManager_DARYL_CreateThing(3, &params);
                Destructible_Release(obj);
            }
        }
    }
    ((Vector4 *)obj)->x += normal.x * scale;
    ((Vector4 *)obj)->y += normal.y * scale;
    ((Vector4 *)obj)->z += normal.z * scale;
    PhysicalObject_ApplyAngularImpulse(obj, (Vector4 *)&pos, (Vector4 *)&normal, force);
}

void PhysicalObject_GetInverseLocalInertiaTensor(void *obj, Quaternion *out) {
    *out = *(Quaternion *)((u8 *)obj + 0x120);
}

