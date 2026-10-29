#include "common.h"


void ThingManagerList_Append(void **arg0, void *arg1) {
    void *temp_r5;

    M2C_FIELD(arg1, void **, 0xA4) = (void *) *arg0;
    M2C_FIELD(arg1, s32 *, 0xA0) = 0;
    temp_r5 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, void **, 0xA0) = arg1;
    }
    *arg0 = arg1;
}

/* The fields every pooled Thing shares. */
typedef struct ThingHeader {
    u8 pad0[0x40];
    f32 unk40[4];               /* 0x40 bounding sphere centre */
    u8 pad50[0x20];
    f32 unk70;                  /* 0x70 bounding sphere radius */
    u32 flags;                  /* 0x74 */
    u8 pad78[0x10];
    u32 unk88;                  /* 0x88 */
    void *vtable;               /* 0x8C */
    u8 pad90[0x10];
    struct ThingHeader *prev;   /* 0xA0 */
    struct ThingHeader *next;   /* 0xA4 */
    u32 type;                   /* 0xA8 index into the free lists */
} ThingHeader;

typedef struct ThingSlotProjectile { ThingHeader thing; u8 padAC[0x84]; s32 unk130; u8 pad134[0x2C]; } ThingSlotProjectile;
typedef struct ThingSlotD0 { ThingHeader thing; u8 pad[0xD0 - sizeof(ThingHeader)]; } ThingSlotD0;
typedef struct ThingSlotE0 { ThingHeader thing; u8 pad[0xE0 - sizeof(ThingHeader)]; } ThingSlotE0;
typedef struct ThingSlotF0 { ThingHeader thing; u8 pad[0xF0 - sizeof(ThingHeader)]; } ThingSlotF0;
typedef struct ThingSlot100 { ThingHeader thing; u8 pad[0x100 - sizeof(ThingHeader)]; } ThingSlot100;
typedef struct ThingSlot110 { ThingHeader thing; u8 pad[0x110 - sizeof(ThingHeader)]; } ThingSlot110;
typedef struct ThingSlot120 { ThingHeader thing; u8 pad[0x120 - sizeof(ThingHeader)]; } ThingSlot120;
typedef struct ThingSlot140 { ThingHeader thing; u8 pad[0x140 - sizeof(ThingHeader)]; } ThingSlot140;
typedef struct ThingSlot160 { ThingHeader thing; u8 pad[0x160 - sizeof(ThingHeader)]; } ThingSlot160;
typedef struct ThingSlot1D0 { ThingHeader thing; u8 pad[0x1D0 - sizeof(ThingHeader)]; } ThingSlot1D0;
typedef struct ThingSlot1F0 { ThingHeader thing; u8 pad[0x1F0 - sizeof(ThingHeader)]; } ThingSlot1F0;
typedef struct ThingSlot480 { ThingHeader thing; u8 pad[0x480 - sizeof(ThingHeader)]; } ThingSlot480;
typedef struct ThingSlot5A0 { ThingHeader thing; u8 pad[0x5A0 - sizeof(ThingHeader)]; } ThingSlot5A0;

/* g_thingManager_DARYL's thing pools, one per free list type. */
typedef struct ThingPools {
    ThingSlotProjectile projectiles[140]; /* 0x00000 type 0 */
    ThingSlot100 eventSeqs[192];        /* 0x0C080 type 1 */
    ThingSlotD0 emitters[160];          /* 0x18080 type 2 */
    ThingSlotF0 destructibles[500];     /* 0x20280 type 3 */
    ThingSlotF0 items[96];              /* 0x3D740 type 4 */
    ThingSlot160 attaches[192];         /* 0x43140 type 5 */
    ThingSlot100 debris[128];           /* 0x53940 type 6 */
    ThingSlot1D0 creature;              /* 0x5B940 type 7 */
    ThingSlot480 wildCardRacks[16];     /* 0x5BB10 type 8 */
    ThingSlot5A0 cars[8];               /* 0x60310 type 9 */
    ThingSlot140 physicalObjects[100];  /* 0x63010 type 10 */
    ThingSlotD0 looker;                 /* 0x6AD10 type 11 */
    ThingSlot110 tracers[32];           /* 0x6ADE0 type 12 */
    ThingSlot120 pod;                   /* 0x6CFE0 type 13 */
    ThingSlot1F0 drone;                 /* 0x6D100 type 14 */
    ThingSlotF0 motions[56];            /* 0x6D2F0 type 15 */
    ThingSlotE0 soundSeqs[128];         /* 0x70770 type 16 */
    ThingSlotD0 radarBlips[64];         /* 0x77770 type 17 */
    void *activeList;                   /* 0x7AB70 */
    void *freeList[19];                 /* 0x7AB74 */
} ThingPools;

static inline void ThingManager_InitFreeThing(ThingHeader *thing, void *vtable, u32 type) {
    void **freeList;
    ThingHeader *head;

    thing->vtable = vtable;
    thing->unk88 = 0;
    thing->flags |= 0x1000;
    thing->prev = NULL;
    thing->next = NULL;
    thing->type = type;
    freeList = g_thingManager_DARYL.freeList + thing->type;
    thing->next = *freeList;
    thing->prev = NULL;
    head = thing->next;
    if (head != NULL) {
        head->prev = thing;
    }
    *freeList = thing;
}

static inline void ThingManager_InitPool(void *pool, u32 count, u32 size, void *vtable, u32 type) {
    u32 i;

    for (i = 0; i < count; i++) {
        ThingManager_InitFreeThing((ThingHeader *)((u8 *)pool + i * size), vtable, type);
    }
}

void ThingManagerList_Remove(void **arg0, void *arg1) {
    void *temp_r5;
    void *temp_r5_2;

    temp_r5 = M2C_FIELD(arg1, void **, 0xA0);
    if (temp_r5 == NULL) {
        *arg0 = M2C_FIELD(arg1, void **, 0xA4);
    } else {
        M2C_FIELD(temp_r5, void **, 0xA4) = (void *) M2C_FIELD(arg1, void **, 0xA4);
    }
    temp_r5_2 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5_2 != NULL) {
        M2C_FIELD(temp_r5_2, void **, 0xA0) = (void *) M2C_FIELD(arg1, void **, 0xA0);
        M2C_FIELD(arg1, void **, 0xA4) = NULL;
    }
    M2C_FIELD(arg1, void **, 0xA0) = NULL;
}

void *ThingManagerList_GetByType(void *list, s32 type) {
    void *thing;

    for (thing = *(void **)list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (M2C_FIELD(thing, s32 *, 0xA8) == type) {
            return thing;
        }
    }
    return NULL;
}

s32 ThingManagerList_Get(void *list, void *target) {
    void *thing;

    for (thing = *(void **)list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (target == thing) {
            return 1;
        }
    }
    return 0;
}

void ThingManagerList_ReleaseSlavesBySlaveType(void **list, u8 type) {
    void *thing;
    void *next;
    void **freeList;
    void *head;

    thing = *list;
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        if (M2C_FIELD(thing, u8 *, 0x83) == type) {
            M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *), 4)(thing);
            ThingManagerList_Remove(list, thing);
            M2C_FIELD(thing, u32 *, 0x74) &= ~0x200000;
            M2C_FIELD(thing, u32 *, 0x74) |= 0x1000;
            freeList = &g_thingManager_DARYL.freeList[M2C_FIELD(thing, s32 *, 0xA8)];
            M2C_FIELD(thing, void **, 0xA4) = *freeList;
            M2C_FIELD(thing, void **, 0xA0) = NULL;
            head = M2C_FIELD(thing, void **, 0xA4);
            if (head != NULL) {
                M2C_FIELD(head, void **, 0xA0) = thing;
            }
            *freeList = thing;
        }
        thing = next;
    }
}

void ThingManagerList_ReleaseAll(void **list) {
    void *thing;
    void *next;

    thing = *list;
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *), 4)(thing);
        thing = next;
    }
}

void ThingManager_DARYL_CreateMain(void) {
    extern u8 g_projectileVTable[];
    extern u8 g_eventSeqVTable[];
    extern u8 g_emitterVTable[];
    extern u8 g_destructibleVTable[];
    extern u8 g_itemVTable[];
    extern u8 g_attachVTable[];
    extern u8 g_debrisVTable[];
    extern u8 g_creatureVTable[];
    extern u8 g_wildCardRackVTable[];
    extern u8 g_physicalObjectVTable[];
    extern u8 g_lookerVTable[];
    extern u8 g_tracerVTable[];
    extern u8 g_podVTable[];
    extern u8 g_droneVTable[];
    extern u8 g_motionVTable[];
    extern u8 g_soundSeqVTable[];
    extern u8 g_radarBlipVTable[];
    ThingPools *tm = (ThingPools *)&g_thingManager_DARYL;
    ThingHeader *thing;
    u32 i;

    for (i = 0; i < 18; i++) {
        tm->freeList[i] = NULL;
    }
    tm->activeList = NULL;
    for (i = 0; i < 140; i++) {
        ThingSlotProjectile *p = &tm->projectiles[i];

        p->thing.vtable = g_projectileVTable;
        p->unk130 = -1;
        p->thing.unk88 = 0;
        p->thing.flags |= 0x1000;
        p->thing.prev = NULL;
        p->thing.next = NULL;
        p->thing.type = 0;
        ThingManagerList_Append(&g_thingManager_DARYL.freeList[p->thing.type], &p->thing);
    }
    for (i = 0; i < 192; i++) {
        thing = &tm->eventSeqs[i].thing;
        ThingManager_InitFreeThing(thing, g_eventSeqVTable, 1);
    }
    for (i = 0; i < 160; i++) {
        thing = &tm->emitters[i].thing;
        ThingManager_InitFreeThing(thing, g_emitterVTable, 2);
    }
    for (i = 0; i < 500; i++) {
        thing = &tm->destructibles[i].thing;
        ThingManager_InitFreeThing(thing, g_destructibleVTable, 3);
    }
    for (i = 0; i < 96; i++) {
        thing = &tm->items[i].thing;
        ThingManager_InitFreeThing(thing, g_itemVTable, 4);
    }
    for (i = 0; i < 192; i++) {
        thing = &tm->attaches[i].thing;
        ThingManager_InitFreeThing(thing, g_attachVTable, 5);
    }
    for (i = 0; i < 128; i++) {
        thing = &tm->debris[i].thing;
        ThingManager_InitFreeThing(thing, g_debrisVTable, 6);
    }
    thing = &tm->creature.thing;
    ThingManager_InitFreeThing(thing, g_creatureVTable, 7);
    for (i = 0; i < 16; i++) {
        thing = &tm->wildCardRacks[i].thing;
        ThingManager_InitFreeThing(thing, g_wildCardRackVTable, 8);
    }
    for (i = 0; i < 8; i++) {
        thing = &tm->cars[i].thing;
        ThingManager_InitFreeThing(thing, g_carAVTable, 9);
    }
    for (i = 0; i < 100; i++) {
        thing = &tm->physicalObjects[i].thing;
        ThingManager_InitFreeThing(thing, g_physicalObjectVTable, 10);
    }
    thing = &tm->looker.thing;
    ThingManager_InitFreeThing(thing, g_lookerVTable, 11);
    for (i = 0; i < 32; i++) {
        thing = &tm->tracers[i].thing;
        ThingManager_InitFreeThing(thing, g_tracerVTable, 12);
    }
    thing = &tm->pod.thing;
    ThingManager_InitFreeThing(thing, g_podVTable, 13);
    thing = &tm->drone.thing;
    ThingManager_InitFreeThing(thing, g_droneVTable, 14);
    for (i = 0; i < 56; i++) {
        thing = &tm->motions[i].thing;
        ThingManager_InitFreeThing(thing, g_motionVTable, 15);
    }
    for (i = 0; i < 128; i++) {
        thing = &tm->soundSeqs[i].thing;
        ThingManager_InitFreeThing(thing, g_soundSeqVTable, 16);
    }
    for (i = 0; i < 64; i++) {
        thing = &tm->radarBlips[i].thing;
        ThingManager_InitFreeThing(thing, g_radarBlipVTable, 17);
    }
}

void OBoxHolder_Init(void);
void OConvexHullHolder_Init(void);
void CollisionPairCache_Clear(void);
void CollisionTriCache_Clear(void);
void *memset(void *dst, int c, u32 n);
extern u8 g_aSharedSlaveHolder[0x200];
void ThingManager_DARYL_Create(void) {
    OBoxHolder_Init();
    OConvexHullHolder_Init();
    ThingManager_DARYL_CreateMain();
    CollisionPairCache_Clear();
    CollisionTriCache_Clear();
    memset(g_aSharedSlaveHolder, 0, sizeof(g_aSharedSlaveHolder));
}

void ThingManager_DARYL_Destroy(void) {
    void *thing;
    u32 i;

    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[1])(thing);
    }
    g_thingManager_DARYL.activeList = NULL;
    for (i = 0; i < 18; i++) {
        g_thingManager_DARYL.freeList[i] = NULL;
    }
}

void ThingManager_DARYL_Init(void) {
    M2C_UNK ThingManager_DARYL_Release();
    M2C_UNK MatrixCache_Init();
    ThingManager_DARYL_Release();
    MatrixCache_Init();
}

void ThingManager_DARYL_Release(void) {
    void ThingManagerList_ReleaseAndRemoveAll(void *list);
    void ThingManager_DARYL_CreateMain(void);

    SharedSlaveHolder_Release();
    ThingManagerList_ReleaseAndRemoveAll(&g_thingManager_DARYL.activeList);
    ThingManagerList_ReleaseAndRemoveAll(&g_thingManager_DARYL.activeList);
    ThingManager_DARYL_CreateMain();
}

void ThingManager_MoveLookerOnly(void) {
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    extern u8 g_soundSeqVTable[];
    void *thing;

    ((void (*)(void *))(M2C_FIELD(&g_thingManager_DARYL, void ***, 0x6AD9C))[2])(g_thingManager_DARYL.looker);
    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (M2C_FIELD(thing, u8 **, 0x8C) == g_soundSeqVTable && (M2C_FIELD(thing, u32 *, 0x74) & 0x100000)) {
            ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[3])(thing);
        }
    }
    ThingManagerList_RemoveReleasedThings((s32)&g_thingManager_DARYL.activeList);
}

void ThingManager_DARYL_Heartbeat(void) {
    M2C_UNK ThingManager_MoveLookerOnly();
    M2C_UNK ThingManager_MoveThings_DARYL_SUBTICK();
    extern u8 g_bFreezeWorld;
    if ((s8) g_bFreezeWorld != 0) {
        ThingManager_MoveLookerOnly();
        return;
    }
    ThingManager_MoveThings_DARYL_SUBTICK();
}

void ThingManager_MoveThings_DARYL_SUBTICK(void)
{
}

void ThingManager_DARYL_Render(void) {
    extern void *g_pNoRayCollideThing;
    s32 Gfx_FrustrumCullSphereTestAllWindows(void *, f32);
    void Thing_DrawSlaves(void *);
    ThingHeader *thing;

    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = thing->next) {
        if (!(thing->flags & 0x200200) && Gfx_FrustrumCullSphereTestAllWindows(thing->unk40, thing->unk70)) {
            ((void (**)(void *))thing->vtable)[4](thing);
            g_pNoRayCollideThing = thing;
            Thing_DrawSlaves(thing);
        }
    }
    g_pNoRayCollideThing = NULL;
}

void *ThingManager_DARYL_CreateThing(s32 type, void *params) {
    void *thing = g_thingManager_DARYL.freeList[type];

    if (thing == NULL) {
        thing = NULL;
    } else {
        g_thingManager_DARYL.freeList[type] = M2C_FIELD(thing, void **, 0xA4);
        if (g_thingManager_DARYL.freeList[type] != NULL) {
            M2C_FIELD(g_thingManager_DARYL.freeList[type], void **, 0xA0) = NULL;
        }
        M2C_FIELD(thing, void **, 0xA4) = NULL;
    }
    if (thing != NULL) {
        M2C_FIELD(thing, void **, 0xA4) = g_thingManager_DARYL.activeList;
        M2C_FIELD(thing, void **, 0xA0) = NULL;
        if (M2C_FIELD(thing, void **, 0xA4) != NULL) {
            M2C_FIELD(M2C_FIELD(thing, void **, 0xA4), void **, 0xA0) = thing;
        }
        g_thingManager_DARYL.activeList = thing;
        ((void (*)(void *, void *))(M2C_FIELD(thing, void ***, 0x8C))[0])(thing, params);
        return thing;
    }
    return NULL;
}

s32 OBox_CollideAllThingsZeroTime(void *obox, void *thing, void *info) {
    u32 Grid_GetObjects(Vector4 *min, Vector4 *max, u32 typeMask, GridResult *results, u32 maxResults);
    s8 OBoxHitsOBoxZeroTime(void *a, void *b, void *info);
    f32 PSVECSquareDistance(f32 *a, f32 *b);
    Vector4 min;
    Vector4 max;
    GridResult results[256];
    u32 count;
    u32 i;
    u8 *other;
    f32 t;
    f32 dist;
    f32 r;
    void *otherBox;

    Thing_GetStaticRotationalMinMax(thing, &min, &max);
    count = Grid_GetObjects(&min, &max, 5, results, 0x100);
    for (i = 0; i < count; i++) {
        other = (u8 *)results[i].id;
        if (!(*(u32 *)(other + 0x74) & 2) && *(s16 *)(other + 0x80) != 0 && other != thing) {
            dist = PSVECSquareDistance((f32 *)((u8 *)thing + 0x40), (f32 *)(other + 0x40));
            r = *(f32 *)(other + 0x70) + *(f32 *)((u8 *)thing + 0x70);
            if (r * r > dist) {
                t = 1.0f;
                if (*(s16 *)(other + 0x80) == 1) {
                    otherBox = (*(void *(**)(void *))(*(u8 **)(other + 0x8C) + 0x40))(other);
                    if (OBoxHitsOBoxZeroTime(obox, otherBox, info)) {
                        if (info != NULL) {
                            *(void **)((u8 *)info + 0x20) = thing;
                            *(void **)((u8 *)info + 0x24) = other;
                        }
                        t = 0.0f;
                    } else {
                        t = 1.0f;
                    }
                }
                if (t < 1.0f) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

s8 RayHitsObjects_Aux(Vector4 *start, Vector4 *dir, f32 *t, void *info) {
    extern void *g_pNoRayCollideThing;
    u32 Grid_GetObjects(Vector4 *min, Vector4 *max, u32 typeMask, GridResult *results, u32 maxResults);
    s8 SegmentVsSphere(void *center, Vector4 *start, Vector4 *end, f32 radius);
    Vector4 min;
    Vector4 max;
    Vector4 end;
    GridResult results[512];
    u32 count;
    u32 i;
    u8 *other;
    f32 best;
    f32 hit;

    best = *t;
    max = *start;
    end.x = dir->x * best + start->x;
    end.y = dir->y * best + start->y;
    end.z = dir->z * best + start->z;
    min = end;
    if (start->x < min.x) {
        min.x = start->x;
    }
    if (start->y < min.y) {
        min.y = start->y;
    }
    if (start->z < min.z) {
        min.z = start->z;
    }
    if (end.x > max.x) {
        max.x = end.x;
    }
    if (end.y > max.y) {
        max.y = end.y;
    }
    if (end.z > max.z) {
        max.z = end.z;
    }
    count = Grid_GetObjects(&min, &max, 5, results, 0x200);
    for (i = 0; i < count; i++) {
        other = (u8 *)results[i].id;
        if (!(*(u32 *)(other + 0x74) & 2) && other != g_pNoRayCollideThing &&
            SegmentVsSphere(other + 0x40, start, &end, *(f32 *)(other + 0x70))) {
            hit = (*(f32 (**)(void *, Vector4 *, Vector4 *, void *, f32))(*(u8 **)(other + 0x8C) + 0x38))(other, start, dir, info, best);
            if (hit < *t) {
                best = hit;
            }
        }
    }
    if (best < *t) {
        *t = best;
        return 1;
    }
    return 0;
}

s8 RayHitsTerrainAndObjects(void *arg0, void *arg1, void *arg2, void *arg3) {
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    s8 RayHitsObjects_Aux(void *, void *, void *, void *);
    s8 hitTerrain;

    if (g_worldData.world == NULL || *(void **)g_worldData.world == NULL) {
        return 0;
    }
    hitTerrain = RayHitsTerrain_Aux3(arg0, arg1, arg2, arg3);
    if (RayHitsObjects_Aux(arg0, arg1, arg2, arg3)) {
        return 1;
    }
    return hitTerrain;
}

s8 OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(void *obox, void *thing, void *arg2) {
    s8 OBoxHitsTerrainZeroTimeEdgesOnly(void *, void *);
    s32 OBox_CollideAllThingsZeroTime(void *, void *, void *);
    if (!(M2C_FIELD(thing, u32 *, 0x74) & 0x40000) && OBoxHitsTerrainZeroTimeEdgesOnly(obox, arg2)) {
        return 1;
    }
    return OBox_CollideAllThingsZeroTime(obox, thing, arg2);
}

s32 ThingManager_GetActiveCars(void **cars) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 8; i++) {
        u8 *car = g_thingManager_DARYL.unk0 + 0x60310 + i * 0x5A0;
        if (!(M2C_FIELD(car, u32 *, 0x74) & 0x201000)) {
            cars[count++] = car;
        }
    }
    return count;
}

s32 ThingManager_GetActiveItems(void **items) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 96; i++) {
        u8 *item = g_thingManager_DARYL.unk0 + 0x3D740 + i * 0xF0;
        if (!(*(u32 *)(item + 0x74) & 0x201000)) {
            items[count++] = item;
        }
    }
    return count;
}

s32 ThingManager_GetActiveSoundSequences(void **seqs) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 128; i++) {
        u8 *seq = g_thingManager_DARYL.unk0 + 0x70770 + i * 0xE0;
        if (!(M2C_FIELD(seq, u32 *, 0x74) & 0x201000)) {
            seqs[count++] = seq;
        }
    }
    return count;
}

s32 GetAllTargets(void **targets) {
    return ThingManager_GetActiveCars(targets);
}

void *FindHost(Vector4 *pos, Vector4 *dir) {
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *src, Vector4 *dst);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void *items[500];
    void *cars[8];
    void *pods[8];
    Vector4 dir1;
    Vector4 diff1;
    Vector4 dir2;
    Vector4 diff2;
    Vector4 dir3;
    Vector4 diff3;
    Vector4 dir4;
    Vector4 diff4;
    void *bases[4];
    u32 count;
    u32 i;
    u32 j;
    u32 k;
    u32 m;

    count = ThingManager_GetActiveCars(cars);
    for (i = 0; i < count; i++) {
        PSVECSubtract((Vector4 *)((u8 *)cars[i] + 0x40), pos, &diff1);
        dir1 = diff1;
        if (!(PSVECSquareMag(&diff1) < 1.0f)) {
            if (PSVECSquareMag(&dir1)) {
                PSVECNormalize(&dir1, &dir1);
            }
            if (PSVECDotProduct(dir, &dir1) > 0.8f) {
                return cars[i];
            }
        }
    }
    count = ThingManager_GetActiveUnk6D100(pods);
    for (j = 0; j < count; j++) {
        PSVECSubtract((Vector4 *)((u8 *)pods[j] + 0x40), pos, &diff2);
        dir2 = diff2;
        if (!(PSVECSquareMag(&diff2) < 1.0f)) {
            if (PSVECSquareMag(&dir2)) {
                PSVECNormalize(&dir2, &dir2);
            }
            if (PSVECDotProduct(dir, &dir2) > 0.8f) {
                return pods[j];
            }
        }
    }
    count = ThingManager_GetActiveUnk5B940(bases);
    for (k = 0; k < count; k++) {
        PSVECSubtract((Vector4 *)((u8 *)bases[k] + 0x40), pos, &diff3);
        dir3 = diff3;
        if (!(PSVECSquareMag(&diff3) < 1.0f)) {
            if (PSVECSquareMag(&dir3)) {
                PSVECNormalize(&dir3, &dir3);
            }
            if (PSVECDotProduct(dir, &dir3) > 0.8f) {
                return bases[k];
            }
        }
    }
    count = ThingManager_GetActiveUnk20280(items);
    for (m = 0; m < count; m++) {
        PSVECSubtract((Vector4 *)((u8 *)items[m] + 0x40), pos, &diff4);
        dir4 = diff4;
        if (!(PSVECSquareMag(&diff4) < 1.0f)) {
            if (PSVECSquareMag(&dir4)) {
                PSVECNormalize(&dir4, &dir4);
            }
            if (PSVECDotProduct(dir, &dir4) > 0.8f) {
                return items[m];
            }
        }
    }
    return g_thingManager_DARYL.looker;
}

void ThingManager_Invalidate(u32 start, u32 end) {
    typedef struct SoundSeqThing {
        u8 pad0[0xD0];
        s32 sources[4];
    } SoundSeqThing;
    s32 Projectile_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 EventSeq_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Emitter_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Destructible_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Item_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 AttachThing_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Creature_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 WildCardRack_Invalidate(u8 *rack, u32 start, u32 end);
    s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Tracer_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Pod_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 Motion_Invalidate(void *arg0, u32 arg1, u32 arg2);
    s32 SoundSeq_Invalidate(void *arg0, u32 arg1, u32 arg2);
    void SoundSource_Stop(s32 index);
    u32 i;
    u8 *thing;
    SoundSeqThing *seq;
    u32 j;

    if (g_thingManager_DARYL.activeList == NULL) {
        return;
    }
    for (i = 0; i < 140; i++) {
        thing = g_thingManager_DARYL.unk0 + i * 0x160;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Projectile_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 192; i++) {
        thing = g_thingManager_DARYL.unk0 + 0xC080 + i * 0x100;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)EventSeq_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 160; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x18080 + i * 0xD0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Emitter_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 500; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x20280 + i * 0xF0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Destructible_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 96; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x3D740 + i * 0xF0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Item_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 192; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x43140 + i * 0x160;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)AttachThing_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 1; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x5B940 + i * 0x5A0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Creature_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 16; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x5BB10 + i * 0x480;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)WildCardRack_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 8; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x60310 + i * 0x5A0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)CarA_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 100; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x63010 + i * 0x140;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Destructible_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 32; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x6ADE0 + i * 0x110;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Tracer_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 1; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x6CFE0 + i * 0x120;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Pod_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 1; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x6D100 + i * 0x5A0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Destructible_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 56; i++) {
        thing = g_thingManager_DARYL.unk0 + 0x6D2F0 + i * 0xF0;
        if (!(*(u32 *)(thing + 0x74) & 0x1000) && (s8)Motion_Invalidate(thing, start, end)) {
            (*(void (**)(void *))(*(u8 **)(thing + 0x8C) + 4))(thing);
        }
    }
    for (i = 0; i < 128; i++) {
        seq = (SoundSeqThing *)(g_thingManager_DARYL.unk0 + 0x70770 + i * 0xE0);
        if (!(*(u32 *)((u8 *)seq + 0x74) & 0x1000) && (s8)SoundSeq_Invalidate(seq, start, end)) {
            for (j = 0; j < 4; j++) {
                if (seq->sources[j] >= 0) {
                    SoundSource_Stop(seq->sources[j]);
                }
                seq->sources[j] = -1;
            }
            Thing_Release((s32)seq);
        }
    }
    ThingManagerList_RemoveReleasedThings(&g_thingManager_DARYL.activeList);
}

s8 TriggerVehicleDamage(u32 *params) {
    u32 i;
    u32 count;
    u8 **things;
    s32 *saved;
    u32 type;
    s32 amount;
    s32 result;

    count = params[0];
    things = (u8 **)&params[1];
    result = 0;
    saved = (s32 *)&params[count + 3];
    type = params[count + 1];
    amount = params[count + 2];
    for (i = 0; i < count; i++) {
        u8 *vehicle = M2C_FIELD(*things, u8 **, 0x28);
        if (type == 0x2A) {
            s32 damage = *(s32 *)(vehicle + 0x78);
            if (damage < *saved - amount) {
                result = 1;
            }
            *saved = damage;
        }
        if (type == 0x2B && *(s32 *)(vehicle + 0x78) < *(s32 *)(*(u8 **)(*(u8 **)(vehicle + 0x3BC) + 0xA0) + 0x14) / amount) {
            result = 1;
        }
        things++;
        saved++;
    }
    return result;
}

s32 TriggerPlayerVehicleGear(u32 *params) {
    extern u8 g_cameraManager[];
    u8 *vehicle = M2C_FIELD(*(u8 **)g_cameraManager, u8 **, 0x2C);

    if (vehicle != NULL && vehicle[0x4E8] == params[1]) {
        return 1;
    }
    return 0;
}

s32 TriggerPlayerVehicleInAir(u32 *params) {
    extern u8 g_cameraManager[];
    u8 *vehicle;
    u32 inAir;

    if (*(u8 **)g_cameraManager != NULL) {
        vehicle = M2C_FIELD(*(u8 **)g_cameraManager, u8 **, 0x2C);
    } else {
        vehicle = NULL;
    }
    inAir = params[2];
    if (inAir == 0 && vehicle != NULL && *(s32 *)(vehicle + 0x460) >= (s32)params[1]) {
        params[2] = 1;
        return 1;
    }
    if (inAir != 0 && vehicle != NULL && *(s32 *)(vehicle + 0x460) == 0) {
        params[2] = 0;
        return 1;
    }
    return 0;
}

