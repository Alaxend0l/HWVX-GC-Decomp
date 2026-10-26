#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct OBox {
    Vector4 v[3];
} OBox;

typedef struct ConvexHull {
    u8 pad0[0x3D0];
    Vector4 point;          /* 0x3D0 */
} ConvexHull;

typedef struct DestructibleInfo {
    f32 inertia[3];         /* 0x00 */
    u8 pad0C[4];            /* 0x0C */
    u8 type;                /* 0x10 */
    u8 takesDamage;         /* 0x11 */
    u8 pad12[6];            /* 0x12 */
    s32 armour;             /* 0x18 */
    u8 pad1C[4];            /* 0x1C */
    f32 mass;               /* 0x20 */
    u8 pad24[0x0C];         /* 0x24 */
    s32 surfaceScript;      /* 0x30 */
    u8 pad34[0x10];         /* 0x34 */
    OBox *box;              /* 0x44 */
    ConvexHull *hull;       /* 0x48 */
} DestructibleInfo;

typedef struct ThingRecord {
    u8 pad00[0x10];
    DestructibleInfo *info;  /* 0x10 */
} ThingRecord;

typedef struct Destructible {
    Vector4 pos;             /* 0x00 */
    f32 matrix[12];          /* 0x10 */
    Vector4 offsetPos;       /* 0x40 */
    u8 pad50[0x24];          /* 0x50 */
    u32 flags;               /* 0x74 */
    u8 pad78[0x48];          /* 0x78 */
    ThingRecord *record;     /* 0xC0 */
    u8 padC4[8];             /* 0xC4 */
    ConvexHull *hull;        /* 0xCC */
    OBox *orientedBox;       /* 0xD0 */
} Destructible;

extern f32 g_zeroVector[4];
extern u8 g_destructibleVTable[];
void BuildOOB(void *oob, void *pos, void *out);
void BuildOConvexHullOffset(void *out, void *hull, void *pos, void *rot);
f32 PhysicalProperties_CollideWithRay(void *props, void *pos, void *start, void *dir, f32 maxDist, void *hit);

f32 MagnitudeVector(Vector4 *v);

f32 GetBoundingBoxRadius(Vector4 *box, Vector4 *offset)
{
    Vector4 corners[8];
    f32 radius;
    f32 mag;
    u32 i;

    radius = 0.0f;
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
    for (i = 0; i < 8; i++) {
        corners[i].x += offset->x;
        corners[i].y += offset->y;
        corners[i].z += offset->z;
        mag = MagnitudeVector(&corners[i]);
        if (mag > radius) {
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

s32 Destructible_GetSurfaceScript(Destructible *destructible) {
    return destructible->record->info->surfaceScript;
}

u32 Destructible_Invalidate(Destructible *destructible, u32 start, u32 end) {
    if ((u32)destructible->record >= start && (u32)destructible->record < end) {
        return 1;
    }
    return 0;
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

