#include "common.h"

#pragma use_lmw_stmw on

extern f32 g_zeroVector[4];
extern u8 g_destructibleVTable[];
void BuildOOB(void *oob, void *pos, void *out);
void BuildOConvexHullOffset(void *out, void *hull, void *pos, void *rot);
f32 PhysicalProperties_CollideWithRay(void *props, void *pos, void *start, void *dir, f32 maxDist, void *hit);

f32 MagnitudeVector(Vector4 *v);

f32 GetBoundingBoxRadius(Vector4 *box, Vector4 *offset)
{
    Vector4 corners[8] __attribute__((aligned(16)));
    f32 radius;
    f32 mag;
    u32 i;

    radius = -99999.9f;
    corners[0].x = box[1].x;
    corners[0].y = box[1].y;
    corners[0].z = box[1].z;
    corners[1].x = box[1].x;
    corners[1].y = box[1].y;
    corners[1].z = box[0].z;
    corners[2].x = box[1].x;
    corners[2].y = box[0].y;
    corners[2].z = box[1].z;
    corners[3].x = box[1].x;
    corners[3].y = box[0].y;
    corners[3].z = box[0].z;
    corners[4].x = box[0].x;
    corners[4].y = box[1].y;
    corners[4].z = box[1].z;
    corners[5].x = box[0].x;
    corners[5].y = box[1].y;
    corners[5].z = box[0].z;
    corners[6].x = box[0].x;
    corners[6].y = box[0].y;
    corners[6].z = box[1].z;
    corners[7].x = box[0].x;
    corners[7].y = box[0].y;
    corners[7].z = box[0].z;
    for (i = 0; i < 8; i++)
    {
        corners[i].x += offset->x;
        corners[i].y += offset->y;
        corners[i].z += offset->z;
        mag = MagnitudeVector(&corners[i]);
        if (mag > radius)
        {
            radius = mag;
        }
    }
    return radius;
}

void Destructible_Release(Destructible *destructible) {
    void Trigger_DestructibleKilled(void *thing);
    void OBoxHolder_Release(OBox *obox);
    void OConvexHullHolder_Release(void *hull);
    void Thing_Release(void *thing);
    s8 paused;
    u8 *trigger;

    Trigger_DestructibleKilled(destructible);
    if (*(u8 **)((u8 *)destructible + 0x8C) == g_destructibleVTable) {
        trigger = *(u8 **)((u8 *)destructible + 0x90);
        if (trigger != NULL) {
            *(u8 **)((u8 *)destructible + 0x90) = NULL;
            *(u32 *)(trigger + 0x2C) = 0;
            paused = Trigger_IsTACSystemPaused();
            if (paused == 0) {
                *(u32 *)(trigger + 0x30) = 0;
            }
        }
    }
    OBoxHolder_Release(destructible->orientedBox);
    destructible->orientedBox = NULL;
    OConvexHullHolder_Release(destructible->hull);
    destructible->hull = NULL;
    Thing_Release(destructible);
}

f32 Destructible_GetMass(Destructible *destructible) {
    return destructible->record->info->mass;
}

void Destructible_DynamicCollideImpact(u8 *destructible, u8 *collision, s32 damage, f32 force) {
    typedef struct OBox {
        Vector4 v[3];
    } OBox;
    typedef struct DestructibleInfo {
        f32 inertia[3];
        u8 pad0C[0x10 - 0xC];
        u8 type;
        u8 takesDamage;
        u8 pad12[0x18 - 0x12];
        s32 armour;
        u8 pad1C[0x20 - 0x1C];
        f32 mass;
        u8 pad24[0x44 - 0x24];
        OBox *box;
    } DestructibleInfo;
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void Thing_ProcessEndEvent(u8 *thing, u8 *event, u8 *collision, Vector4 *vel);
    u8 *CollisionInfo_GetOther(void *collision, void *thing);
    s8 Player_IsHuman(void *player);
    DestructibleInfo *info;
    OBox box;
    Vector4 angVel;
    Vector4 torque;
    Vector4 impulse;
    Vector4 r;
    Vector4 local;
    Vector4 scaled;
    f32 scale;
    f32 invX;
    f32 invY;
    f32 invZ;
    f32 inertia;
    u8 *other;

    info = *(DestructibleInfo **)(*(u8 **)(destructible + 0xC0) + 0x10);
    damage -= info->armour;
    if (*(u32 *)(destructible + 0x74) & 0x200000) {
        return;
    }
    if (info->takesDamage && damage > 0) {
        *(s32 *)(destructible + 0x78) -= damage;
    }
    if (*(s32 *)(destructible + 0x78) > 0) {
        return;
    }
    scale = force / (*(DestructibleInfo **)(*(u8 **)(destructible + 0xC0) + 0x10))->mass;
    ((Vector4 *)destructible)->x += ((Vector4 *)(collision + 0x10))->x * scale;
    ((Vector4 *)destructible)->y += ((Vector4 *)(collision + 0x10))->y * scale;
    ((Vector4 *)destructible)->z += ((Vector4 *)(collision + 0x10))->z * scale;
    PSVECSubtract((Vector4 *)collision, (Vector4 *)(destructible + 0x40), &r);
    impulse.x = force * ((Vector4 *)(collision + 0x10))->x;
    impulse.y = force * ((Vector4 *)(collision + 0x10))->y;
    impulse.z = force * ((Vector4 *)(collision + 0x10))->z;
    PSVECCrossProduct(&r, &impulse, &torque);
    info = *(DestructibleInfo **)(*(u8 **)(destructible + 0xC0) + 0x10);
    switch (info->type) {
    case 0:
        box = *info->box;
        invX = 1.0f / (info->inertia[0] * info->mass * (box.v[1].y * box.v[1].y + box.v[1].z * box.v[1].z));
        invY = 1.0f / (info->inertia[1] * info->mass * (box.v[1].x * box.v[1].x + box.v[1].z * box.v[1].z));
        invZ = 1.0f / (info->inertia[2] * info->mass * (box.v[1].x * box.v[1].x + box.v[1].y * box.v[1].y));
        break;
    case 1:
        break;
    case 7:
        inertia = 0.4f * info->mass * *(f32 *)(destructible + 0x70) * *(f32 *)(destructible + 0x70);
        invX = 1.0f / (info->inertia[0] * inertia);
        invY = 1.0f / (info->inertia[1] * inertia);
        invZ = 1.0f / (info->inertia[2] * inertia);
        break;
    }
    InverseXformVecOrientation(destructible + 0x10, &torque, &local);
    scaled.x = invX * local.x;
    scaled.y = invY * local.y;
    scaled.z = invZ * local.z;
    XformVecOrientation(destructible + 0x10, &scaled, &angVel);
    Thing_ProcessEndEvent(destructible, *(u8 **)(*(u8 **)(destructible + 0xC0) + 0x10), collision, &angVel);
    if (*(u8 **)(destructible + 0x90) != NULL && *(u32 *)(*(u8 **)(destructible + 0x90) + 0x30) == 0x1A) {
        *(u32 *)(destructible + 0x74) |= 0x200;
        *(u32 *)(destructible + 0x74) |= 2;
        return;
    }
    other = CollisionInfo_GetOther(collision, destructible);
    if (other != NULL && *(void **)(other + 0x94) != NULL && Player_IsHuman(*(void **)(other + 0x94))) {
        *(u32 *)(destructible + 0x74) |= 0x20;
    }
    Destructible_Release_inline(destructible);
    if (*(u8 **)(destructible + 0x90) != NULL) {
        *(u32 *)(*(u8 **)(destructible + 0x90) + 0x30) = 3;
    }
}

s32 Destructible_GetSurfaceScript(Destructible *destructible) {
    return destructible->record->info->surfaceScript;
}

u32 Destructible_Invalidate(Destructible *destructible, u32 start, u32 end) {
    if ((u32)destructible->record >= start && (u32)destructible->record < end) {
        return 1;
    }
    return 0;
}

s32 ActionDestructibleChangeMode(u32 *action) {
    void Thing_ProcessEndEvent(u8 *thing, u8 *event, u8 *collision, Vector4 *vel);
    void ThingManagerList_ReleaseSlavesBySlaveType(void **list, u8 type);
    s32 mode;
    u32 count;
    u32 i;
    u8 *thing;
    Destructible *destructible;

    mode = action[0];
    count = action[1];
    for (i = 0; i < count; i++) {
        thing = (u8 *)action[i + 2];
        destructible = *(Destructible **)(thing + 0x2C);
        if (destructible != NULL) {
            switch (mode) {
            case 3:
                Thing_ProcessEndEvent((u8 *)destructible, (u8 *)destructible->record->info, NULL, NULL);
                Destructible_Release(destructible);
                break;
            case 0:
                ThingManagerList_ReleaseSlavesBySlaveType(&destructible->slaves, 0xFF);
                destructible->flags |= 0x200;
                destructible->flags |= 0x2;
                break;
            case 1:
                Thing_InitAllSlavesNow(destructible, (u8 *)destructible->record + 0x18, 0xFF);
                destructible->flags &= ~0x200;
                destructible->flags &= ~0x2;
                break;
            case 25:
                Destructible_Release(destructible);
                break;
            case 26:
                ThingManagerList_ReleaseSlavesBySlaveType(&destructible->slaves, 0xFF);
                Thing_ProcessEndEvent((u8 *)destructible, (u8 *)destructible->record->info, NULL, NULL);
                destructible->flags |= 0x200;
                destructible->flags |= 0x2;
                break;
            }
        }
        *(s32 *)(thing + 0x30) = mode;
    }
    return 1;
}

s32 ActionMoveableChangeMode(u32 *action) {
    typedef struct {
        u8 pad0[0x10];
        u8 *mover;
    } Thing;
    typedef struct {
        u8 pad0[0x8];
        f32 start;
        f32 end;
    } Anim;
    u32 i;
    u8 *mover;

    for (i = 0; i < action[4]; i++) {
        mover = ((Thing *)action[i + 5])->mover;
        if (mover != NULL) {
            switch (action[0]) {
            case 6:
                *(f32 *)(mover + 0xD8) = action[2];
                break;
            case 12:
                *(u32 *)(mover + 0x74) |= 0x4000;
                break;
            case 11:
                *(u32 *)(mover + 0x74) &= ~0x4000;
                *(f32 *)(mover + 0xD8) = action[2];
                break;
            case 3:
                (*(void (**)(void *))(*(u8 **)(mover + 0x8C) + 4))(mover);
                break;
            }
            switch (action[1]) {
            case 13:
                if (*(Anim **)(mover + 0xD4) != NULL) {
                    *(f32 *)(mover + 0xDC) = (*(Anim **)(mover + 0xD4))->start;
                }
                Mover_GetTrackPos(mover, (f32 *)(mover + 0x40), *(f32 *)(mover + 0xDC));
                break;
            case 14:
                if (*(Anim **)(mover + 0xD4) != NULL) {
                    *(f32 *)(mover + 0xDC) = (*(Anim **)(mover + 0xD4))->end;
                }
                Mover_GetTrackPos(mover, (f32 *)(mover + 0x40), *(f32 *)(mover + 0xDC));
                break;
            }
        }
    }
    return 1;
}

void Destructible_CollideWithRay(Destructible *destructible, Vector4 *start, Vector4 *end, Vector4 *hit, f32 maxDist) {
    f32 dist;

    dist = PhysicalProperties_CollideWithRay(destructible->record->info, destructible->matrix, start, end, maxDist, hit);
    if (hit != NULL && dist < maxDist) {
        *(Vector4 *)((u8 *)hit + 0x30) = destructible->pos;
    }
}

void Destructible_GetTargetPos(Destructible *destructible, Vector4 *pos) {
    switch (destructible->record->info->type) {
    case 0:
    case 3:
        *pos = destructible->orientedBox->v[0];
        break;
    case 1:
    case 6:
        *pos = destructible->hull->point;
        break;
    default:
        *pos = destructible->offsetPos;
        break;
    }
}

void *Destructible_GetOrientedCollision(Destructible *destructible) {
    DestructibleInfo *info;

    info = destructible->record->info;
    switch (info->type) {
    case 0:
    case 3:
        if (destructible->flags & 0x180) {
            BuildOOB(info->box, destructible->matrix, destructible->orientedBox);
            destructible->flags &= ~0x180;
        }
        return destructible->orientedBox;
    case 1:
    case 6:
        BuildOConvexHullOffset(destructible->hull, info->hull, destructible->matrix, g_zeroVector);
        return destructible->hull;
    default:
        return NULL;
    }
}
