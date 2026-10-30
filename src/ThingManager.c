#include "common.h"

#pragma use_lmw_stmw on

void ThingManagerList_Append(void **arg0, void *arg1) {
    M2C_FIELD(arg1, void **, 0xA4) = (void *) *arg0;
    M2C_FIELD(arg1, s32 *, 0xA0) = 0;
    if (M2C_FIELD(arg1, void **, 0xA4) != NULL) {
        M2C_FIELD(M2C_FIELD(arg1, void **, 0xA4), void **, 0xA0) = arg1;
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
    u8 pad78[0xA];
    u8 slaveId;                 /* 0x82 trigger id of a slave */
    u8 slavePlayer;             /* 0x83 */
    u8 pad84[4];
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
    ThingSlot1D0 creature[1];           /* 0x5B940 type 7 */
    ThingSlot480 wildCardRacks[16];     /* 0x5BB10 type 8 */
    ThingSlot5A0 cars[8];               /* 0x60310 type 9 */
    ThingSlot140 physicalObjects[100];  /* 0x63010 type 10 */
    ThingSlotD0 looker[1];              /* 0x6AD10 type 11 */
    ThingSlot110 tracers[32];           /* 0x6ADE0 type 12 */
    ThingSlot120 pod[1];                /* 0x6CFE0 type 13 */
    ThingSlot1F0 drone[1];              /* 0x6D100 type 14 */
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

void ThingManagerList_RemoveReleasedThings(void **list) {
    void ThingManagerList_ReleaseAndRemoveAll(void *list);
    ThingHeader *thing;
    ThingHeader *next;

    for (thing = *list; thing != NULL; thing = next) {
        next = thing->next;
        if (thing->flags & 0x200000) {
            ThingManagerList_ReleaseAndRemoveAll(&thing->unk88);
            ThingManagerList_Remove(list, thing);
            ThingManagerList_Append(&g_thingManager_DARYL.freeList[thing->type], thing);
            thing->flags &= ~0x200000;
            thing->flags |= 0x1000;
        }
    }
}

void ThingManagerList_ReleaseAll(void **list);
void ThingManagerList_ReleaseAndRemoveAll(void **list) {
    ThingHeader *thing;
    ThingHeader *next;

    ThingManagerList_ReleaseAll(list);
    for (thing = *list; thing != NULL; thing = next) {
        next = thing->next;
        if ((thing->flags & 0x200000) != 0) {
            void **children;
            ThingManagerList_ReleaseAndRemoveAll(children = (void **)&thing->unk88);
            ThingManagerList_Remove(list, thing);
            ThingManagerList_Append(g_thingManager_DARYL.freeList + thing->type, thing);
            thing->flags &= ~0x200000;
            thing->flags |= 0x1000;
        }
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

void ThingManagerList_EmancipateSlaves(void *mtx, void **list) {
    typedef struct SlaveThing {
        u8 pad0[0x10];
        Matrix4 mtx;
        u8 pad50[0x24];
        u32 flags;
        u8 pad78[8];
        s16 gridCell;
        u8 slaveId;
        u8 slavePlayer;
        u8 pad84[8];
        void *vtable;
        u8 pad90[0x10];
        struct SlaveThing *prev;
        struct SlaveThing *next;
    } SlaveThing;
    extern u8 g_itemVTable[];
    void MultiplyMatrix(void *a, void *b, Matrix4 *out);
    void Thing_AddToGrid(void *thing, s32 type);
    Matrix4 out;
    SlaveThing *thing;
    SlaveThing *next;

    thing = *list;
    while (thing != NULL) {
        next = thing->next;
        if (thing->slavePlayer == 0xEF) {
            if (thing->prev == NULL) {
                *list = thing->next;
            } else {
                thing->prev->next = thing->next;
            }
            if (thing->next != NULL) {
                thing->next->prev = thing->prev;
                thing->next = NULL;
            }
            thing->prev = NULL;
            MultiplyMatrix(&thing->mtx, mtx, &out);
            thing->mtx = out;
            thing->next = g_thingManager_DARYL.activeList;
            thing->prev = NULL;
            if (thing->next != NULL) {
                thing->next->prev = thing;
            }
            g_thingManager_DARYL.activeList = thing;
            if (thing->gridCell != 0 && (u8 *)thing->vtable == g_itemVTable) {
                Thing_AddToGrid(thing, 3);
                thing->flags &= ~8;
            }
        }
        thing = next;
    }
}

void SharedSlaveHolder_Release(void) {
    extern u8 g_aSharedSlaveHolder[0x200];
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    void *thing;
    void *next;
    SharedSlaveHolder *holder;
    u32 i;

    holder = (SharedSlaveHolder *)g_aSharedSlaveHolder;
    i = 0;
    do {
        if (holder->key != NULL) {
            for (thing = holder->slaves; thing != NULL; thing = next) {
                next = M2C_FIELD(thing, void **, 0xA4);
                ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[1])(thing);
            }
            ThingManagerList_RemoveReleasedThings((s32)&holder->slaves);
            holder->key = NULL;
        }
        i++;
        holder++;
    } while (i < 64);
}

s32 SharedSlaveHolder_Get(void *shared) {
    extern u8 g_aSharedSlaveHolder[0x200];
    void ThingManagerList_InitAllSlavesNow(void *list, void *slaves, u8 player);
    s32 i;

    for (i = 0; i < 64; i++) {
        if (((SharedSlaveHolder *)g_aSharedSlaveHolder)[i].key == shared) {
            return i;
        }
    }
    for (i = 0; i < 64; i++) {
        if (((SharedSlaveHolder *)g_aSharedSlaveHolder)[i].key == NULL) {
            ((SharedSlaveHolder *)g_aSharedSlaveHolder)[i].key = shared;
            ThingManagerList_InitAllSlavesNow(&((SharedSlaveHolder *)g_aSharedSlaveHolder)[i].slaves, (u8 *)shared + 4, 0xFF);
            return i;
        }
    }
    return 64;
}

void Thing_DrawSharedSlaves(u8 *thing, u32 *triggers) {
    typedef struct { u8 pad[0x10]; u8 type; u8 pad2[7]; u8 *shared; u8 pad3[4]; } Trigger;
    typedef struct SlaveThing {
        u8 pad0[0x10];
        Matrix4 mtx;
        Quaternion quat;
        u8 pad60[0x2C];
        void (**vtbl)(struct SlaveThing *);
        u8 pad90[0x14];
        struct SlaveThing *next;
    } SlaveThing;
    extern u8 g_aSharedSlaveHolder[0x200];
    u32 i;
    SlaveThing *slave;
    u8 *shared;
    Quaternion quat __attribute__((aligned(16)));
    Vector4 pos __attribute__((aligned(16)));

    for (i = 0; i < triggers[0]; i++) {
        Trigger *trigger = &((Trigger *)triggers[1])[i];
        if (trigger->type == 10) {
            shared = trigger->shared;
            slave = *(SlaveThing **)(g_aSharedSlaveHolder + (*(*(u8 **)(shared + 8) + 0x1D)) * 8 + 4);
            if (*(u32 *)shared != 0) {
                for (; slave != NULL; slave = slave->next) {
                    quat = slave->quat;
                    pos = slave->mtx.v[3];
                    QuaternionMult((Quaternion *)(thing + 0x50), &quat, &slave->quat);
                    XformVec((Matrix4 *)(thing + 0x10), &pos, &slave->mtx.v[3]);
                    slave->vtbl[4](slave);
                    slave->quat = quat;
                    slave->mtx.v[3] = pos;
                }
            }
        }
    }
}

void ThingManagerList_TriggerSlaves(ThingHeader **list, struct SlaveTrigger {
        Vector4 pos;
        u8 type;
        u8 pad11;
        u16 id;
        u8 pad14[4];
        void *data;
    } *trigger, Vector4 *a, Vector4 *b, Vector4 *c, u32 player) {
    typedef struct SharedSlaves {
        u32 active;
        u8 pad4[4];
        u8 *info;
    } SharedSlaves;
    extern Vector4 g_zeroVector;
    extern void *g_pSourcePlayer;
    ThingHeader *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id);
    u32 ViewManager_GetViewFormat(void);
    struct { Vector4 a; Vector4 b; Vector4 c; Vector4 zero; void *data; void *player; f32 f; } p1 __attribute__((aligned(16)));
    struct { Vector4 a; Vector4 b; Vector4 c; void *data; void *player; f32 f; } p2 __attribute__((aligned(16)));
    struct { void *data; u8 pad4[0xC]; Vector4 pos; s32 flag; void *player; } p5 __attribute__((aligned(16)));
    struct { void *data; u8 pad4[0xC]; Vector4 pos; s32 flag; void *player; } p6 __attribute__((aligned(16)));
    struct { Vector4 a; Vector4 b; Vector4 c; void *data; s32 time; void *player; f32 f; } p7 __attribute__((aligned(16)));
    struct { Vector4 unk; Vector4 a; Vector4 b; void *data; s32 flag; } p8 __attribute__((aligned(16)));
    struct { Vector4 a; void *data; u8 flag; } p3 __attribute__((aligned(16)));
    struct { void *data; u8 pad4[0xC]; Vector4 a; } p9 __attribute__((aligned(16)));
    Vector4 p4 __attribute__((aligned(16)));
    u8 id;
    u8 pl;
    SharedSlaves *shared;
    ThingHeader *slave = NULL;
    void *data;
    ThingHeader *thing;

    switch (trigger->type) {
    case 1:
        p1.data = trigger->data;
        p1.c = *c;
        p1.player = g_pSourcePlayer;
        p1.zero = g_zeroVector;
        if (p1.data != NULL && *(u32 *)p1.data != 0) {
            p1.player = g_pSourcePlayer;
            p1.f = 0.0f;
            p1.a = *a;
            p1.b = *b;
            slave = ThingManager_DARYL_CreateSlave(1, &p1, trigger->id);
        }
        break;
    case 2:
        if (trigger->data != NULL) {
            p2.data = trigger->data;
            p2.f = 0.0f;
            p2.c = *c;
            p2.player = g_pSourcePlayer;
            p2.a = *a;
            p2.a.w = 1.0f;
            p2.b = *b;
            slave = ThingManager_DARYL_CreateSlave(2, &p2, trigger->id);
        }
        break;
    case 3:
        data = trigger->data;
        if (data != NULL && data != NULL) {
            p3.a = *a;
            p3.data = data;
            p3.flag = 1;
            slave = ThingManager_DARYL_CreateSlave(0x10, &p3, trigger->id);
        }
        break;
    case 4:
        p4 = trigger->pos;
        p4.w = 1.0f;
        slave = ThingManager_DARYL_CreateSlave(8, &p4, trigger->id);
        break;
    case 5:
        if (trigger->data != NULL) {
            p5.data = trigger->data;
            p5.pos = trigger->pos;
            p5.flag = 1;
            p5.player = g_pSourcePlayer;
            slave = ThingManager_DARYL_CreateSlave(5, &p5, trigger->id);
        }
        break;
    case 6:
        if (ViewManager_GetViewFormat() > 1 && *(u8 *)trigger->data != 0) {
            break;
        }
        if (trigger->data != NULL) {
            p6.data = trigger->data;
            p6.player = g_pSourcePlayer;
            p6.pos = trigger->pos;
            p6.flag = 0;
            slave = ThingManager_DARYL_CreateSlave(5, &p6, trigger->id);
        }
        break;
    case 7:
        if (trigger->data != NULL) {
            p7.data = trigger->data;
            p7.f = 0.0f;
            p7.time = 20;
            p7.c = *c;
            p7.player = g_pSourcePlayer;
            p7.a = *a;
            p7.a.w = 1.0f;
            p7.b = *b;
            slave = ThingManager_DARYL_CreateSlave(0xC, &p7, trigger->id);
        }
        break;
    case 8:
        data = trigger->data;
        if (data != NULL) {
            p8.a = *a;
            p8.b = *b;
            p8.data = data;
            p8.flag = 0;
            slave = ThingManager_DARYL_CreateSlave(0xF, &p8, trigger->id);
        }
        break;
    case 9:
        data = trigger->data;
        if (data != NULL) {
            p9.a = *a;
            p9.data = data;
            slave = ThingManager_DARYL_CreateSlave(0x11, &p9, trigger->id);
        }
        break;
    case 10:
        shared = trigger->data;
        if (shared != NULL && shared->active == 0) {
            shared->active = 1;
            shared->info[0x1D] = SharedSlaveHolder_Get(shared);
        }
        break;
    case 99:
        id = trigger->id;
        pl = player;
        for (thing = *list; thing != NULL; thing = thing->next) {
            if (thing->slaveId == id && thing->slavePlayer == pl) {
                ((void (**)(ThingHeader *))thing->vtable)[1](thing);
            }
        }
        break;
    }

    if (slave != NULL) {
        slave->slavePlayer = player;
        slave->next = *list;
        slave->prev = NULL;
        if (slave->next != NULL) {
            slave->next->prev = slave;
        }
        *list = slave;
    }
}

void ThingManagerList_InitAllSlavesNow(void *list, void *slaves, u8 player) {
    extern Vector4 g_zeroVector;
    void ThingManagerList_TriggerSlaves(void **list, u8 *trigger, Vector4 *a, Vector4 *b, Vector4 *c, u8 player);
    typedef struct { f32 x, y, z, w; } Vec4;
    typedef struct {
        u8 unk0[0x20];
    } SlaveTrigger;
    typedef struct {
        u32 count;
        SlaveTrigger *triggers;
    } SlaveList;
    SlaveList *sl = slaves;
    u32 i;
    void *next;
    void *thing;
    void **freeList;

    thing = *(void **)list;
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        if (M2C_FIELD(thing, u8 *, 0x83) == player) {
            M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *), 4)(thing);
            ThingManagerList_Remove(list, thing);
            M2C_FIELD(thing, u32 *, 0x74) &= ~0x200000;
            M2C_FIELD(thing, u32 *, 0x74) |= 0x1000;
            freeList = g_thingManager_DARYL.freeList + M2C_FIELD(thing, u32 *, 0xA8);
            ThingManagerList_Append(freeList, thing);
        }
        thing = next;
    }
    for (i = 0; i < sl->count; i++) {
        Vec4 dir = {0.0f, 0.0f, 0.0f, 1.0f};
        SlaveTrigger *trigger = &sl->triggers[i];

        ThingManagerList_TriggerSlaves(list, (u8 *)trigger, (Vector4 *)trigger, (Vector4 *)&dir, &g_zeroVector, player);
    }
}

#define THINGPOOLS ((ThingPools *)&g_thingManager_DARYL)
#define THINGPOOL_INIT(pool, count, vt, t)                                          \
    for (i = 0; i < (count); i++) {                                                 \
        THINGPOOLS->pool[i].thing.vtable = (vt);                                    \
        thing = &THINGPOOLS->pool[i].thing;                                         \
        THINGPOOLS->pool[i].thing.unk88 = 0;                                        \
        THINGPOOLS->pool[i].thing.flags |= 0x1000;                                  \
        THINGPOOLS->pool[i].thing.prev = NULL;                                      \
        THINGPOOLS->pool[i].thing.next = NULL;                                      \
        THINGPOOLS->pool[i].thing.type = (t);                                       \
        ThingManagerList_Append(&g_thingManager_DARYL.freeList[thing->type], thing); \
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
    ThingHeader *thing;
    u32 i;

    for (i = 0; i < 18; i++) {
        THINGPOOLS->freeList[i] = NULL;
    }
    THINGPOOLS->activeList = NULL;
    for (i = 0; i < 140; i++) {
        THINGPOOLS->projectiles[i].thing.vtable = g_projectileVTable;
        thing = &THINGPOOLS->projectiles[i].thing;
        THINGPOOLS->projectiles[i].unk130 = -1;
        THINGPOOLS->projectiles[i].thing.unk88 = 0;
        THINGPOOLS->projectiles[i].thing.flags |= 0x1000;
        THINGPOOLS->projectiles[i].thing.prev = NULL;
        THINGPOOLS->projectiles[i].thing.next = NULL;
        THINGPOOLS->projectiles[i].thing.type = 0;
        ThingManagerList_Append(&g_thingManager_DARYL.freeList[thing->type], thing);
    }
    THINGPOOL_INIT(eventSeqs, 192, g_eventSeqVTable, 1);
    THINGPOOL_INIT(emitters, 160, g_emitterVTable, 2);
    THINGPOOL_INIT(destructibles, 500, g_destructibleVTable, 3);
    THINGPOOL_INIT(items, 96, g_itemVTable, 4);
    THINGPOOL_INIT(attaches, 192, g_attachVTable, 5);
    THINGPOOL_INIT(debris, 128, g_debrisVTable, 6);
    THINGPOOL_INIT(creature, 1, g_creatureVTable, 7);
    THINGPOOL_INIT(wildCardRacks, 16, g_wildCardRackVTable, 8);
    THINGPOOL_INIT(cars, 8, g_carAVTable, 9);
    THINGPOOL_INIT(physicalObjects, 100, g_physicalObjectVTable, 10);
    THINGPOOL_INIT(looker, 1, g_lookerVTable, 11);
    THINGPOOL_INIT(tracers, 32, g_tracerVTable, 12);
    THINGPOOL_INIT(pod, 1, g_podVTable, 13);
    THINGPOOL_INIT(drone, 1, g_droneVTable, 14);
    THINGPOOL_INIT(motions, 56, g_motionVTable, 15);
    THINGPOOL_INIT(soundSeqs, 128, g_soundSeqVTable, 16);
    THINGPOOL_INIT(radarBlips, 64, g_radarBlipVTable, 17);
}

#undef THINGPOOL_INIT
#undef THINGPOOLS

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

/* The original calls these rather than inlining them. */
#pragma push
#pragma dont_inline on
void ThingManager_DARYL_Release(void) {
    void ThingManagerList_ReleaseAndRemoveAll(void *list);
    void ThingManager_DARYL_CreateMain(void);

    SharedSlaveHolder_Release();
    ThingManagerList_ReleaseAndRemoveAll(&g_thingManager_DARYL.activeList);
    ThingManagerList_ReleaseAndRemoveAll(&g_thingManager_DARYL.activeList);
    ThingManager_DARYL_CreateMain();
}
#pragma pop

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

typedef struct MovePhys {
    u8 pad0[0x14];
    f32 friction;               /* 0x14 */
    f32 elasticity;             /* 0x18 */
} MovePhys;

typedef struct MoveThing {
    Vector4 vel;                /* 0x00 */
    Matrix4 mtx;                /* 0x10 v[3] = position */
    Vector4 quat;               /* 0x50 */
    Vector4 pushVel;            /* 0x60 */
    f32 radius;                 /* 0x70 */
    u32 flags;                  /* 0x74 */
    s32 unk78;                  /* 0x78 */
    s32 damage;                 /* 0x7C */
    s16 unk80;                  /* 0x80 */
    u8 pad82[2];
    void *gridData;             /* 0x84 */
    u8 pad88[4];
    struct MoveThingVTable *vtable; /* 0x8C */
    u8 pad90[4];
    void *player;               /* 0x94 */
    u8 pad98[0xC];
    struct MoveThing *next;     /* 0xA4 */
    s32 type;                   /* 0xA8 */
} MoveThing;

typedef struct MoveHit {
    Vector4 pos;                /* 0x00 */
    Vector4 normal;             /* 0x10 */
    MoveThing *thing;           /* 0x20 */
    MoveThing *other;           /* 0x24 */
    MovePhys *surface;          /* 0x28 */
    u8 pad2C[0x14];
    u8 type;                    /* 0x40 0xFF: pushed out, 0xFE: embedded */
    u8 pad41[0xF];
} MoveHit;

typedef struct MoveThingVTable {
    void *fn0;
    void *fn4;
    void (*move)(MoveThing *);                          /* 0x08 */
    void (*update)(MoveThing *);                        /* 0x0C */
    void *fn10;
    f32 (*getMass)(MoveThing *);                        /* 0x14 */
    void (*collide)(MoveThing *, MoveHit *, s32, f32);  /* 0x18 */
    void *fn1C;
    void *fn20;
    void *fn24;
    MovePhys *(*getPhys)(MoveThing *);                  /* 0x28 */
    void *fn2C;
    void (*getInvInertia)(MoveThing *, Vector4 *);      /* 0x30 */
    void (*postMove)(MoveThing *, f32);                 /* 0x34 */
} MoveThingVTable;

typedef struct MoveGridEntry {
    u32 unk0;
    MoveThing *thing;
} MoveGridEntry;

typedef struct MoveCarTuning {
    u8 pad0[0x38];
    f32 maxImpactVel;           /* 0x38 */
    u8 pad3C[0x20];
    f32 damageScale;            /* 0x5C */
} MoveCarTuning;

typedef struct MoveCarBody {
    u8 pad0[0x18];
    f32 bumperHeight;           /* 0x18 */
} MoveCarBody;

typedef struct MoveCarIface {
    u8 pad0[0x80];
    MoveCarTuning *tuning;      /* 0x80 */
    u8 pad84[0x1C];
    struct { u8 pad0[0x44]; MoveCarBody *body; } *model; /* 0xA0 */
} MoveCarIface;

typedef struct MoveCar {
    MoveThing thing;
    u8 padAC[0x3BC - 0xAC];
    MoveCarIface *iface;        /* 0x3BC */
    u8 pad3C0[0x50C - 0x3C0];
    f32 drag;                   /* 0x50C */
    u8 pad510[0x18];
    f32 rideHeight;             /* 0x528 */
} MoveCar;

typedef struct MovePlayer {
    u8 pad0[0xD0];
    struct { u8 pad0[0x1C]; f32 damageScale; } *difficulty; /* 0xD0 */
} MovePlayer;

typedef struct MoveImpactFx {
    u8 pad0[0x68];
    f32 minVel;                 /* 0x68 */
    f32 maxVel;                 /* 0x6C */
    u8 pad70[0x80];
    u32 count;                  /* 0xF0 */
    u32 *ids;                   /* 0xF4 */
} MoveImpactFx;

extern MoveHit g_aCI[8];
void Dprintf(const char *fmt, ...);

static inline u32 CI_Find(MoveThing *a, MoveThing *b) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((g_aCI[i].thing == a && g_aCI[i].other == b) || (g_aCI[i].other == a && g_aCI[i].thing == b)) {
            return TRUE;
        }
    }
    return FALSE;
}

static inline void CI_Add(MoveThing *a, MoveThing *b) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (g_aCI[i].thing == NULL) {
            g_aCI[i].thing = a;
            g_aCI[i].other = b;
            return;
        }
    }
    Dprintf("out of CI slots\n");
}

void ThingManager_MoveThings_DARYL_SUBTICK(void)
{
    extern f32 g_fDeltaTimeSec;
    extern u32 g_frameCounter;
    extern u8 g_carAVTable[];
    extern Matrix4 g_identity;
    extern Vector4 g_zeroVector;
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    void MemClr32(void *dst, u32 size);
    void CollisionPairCache_Heartbeat(void);
    void CollisionTriCache_Heartbeat(void);
    void Thing_DynamicGridMove(void *thing, f32 dt);
    void Thing_RotationalGridMove(void *thing);
    void Thing_StaticGridMove(void *thing);
    void Thing_MoveSlaves(void *thing);
    MoveThing *Thing_GetSource(void *thing);
    s8 Thing_HitsTerrain(void *thing, f32 *dt, MoveHit *hit);
    s8 Thing_TestThingCollision(void *a, void *b, f32 *dt, MoveHit *hit);
    s8 RayHitsTerrain(Vector4 *pos, MoveHit *hit, f32 *dist, MoveHit *out);
    void Thing_GetDynamicMinMax(void *thing, Vector4 *min, Vector4 *max, f32 dt);
    s32 Grid_GetObjects(Vector4 *min, Vector4 *max, u32 flags, MoveGridEntry *out, u32 max_count);
    f32 PSVECDotProduct(void *a, void *b);
    void PSVECScale(void *in, void *out, f32 scale);
    f32 PSVECSquareMag(void *v);
    void PSVECNormalize(void *in, void *out);
    void PSVECAdd(void *a, void *b, void *out);
    void PSVECSubtract(void *a, void *b, void *out);
    void PSVECCrossProduct(void *a, void *b, void *out);
    f32 Distance_Plane(Vector4 *normal, MoveHit *hit);
    void XformVecOrientation(Matrix4 *m, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(Matrix4 *m, Vector4 *in, Vector4 *out);
    void TransposeSubMatrix(Matrix4 *m);
    void MultiplySubMatrix(Matrix4 *a, Matrix4 *b, Matrix4 *out);
    f32 CarA_GetImpactMultiplier(void *car, MoveHit *hit, f32 scale);
    u8 FloatsEqual(f32 a, f32 b, f32 eps);
    s8 Player_IsHuman(void *player);
    s8 Timer_Interrupt300(void);
    f32 _fabs(f32 x);
    f32 Sqrt(f64 x);
    void Dprintf(const char *fmt, ...);
    static f32 offset = 0.2f;
    static f32 mult = 10.0f;
    MoveThing *moved[0x200];
    MoveGridEntry objs[0x200];
    MoveHit hit;
    MoveHit rayHit;
    FxParams fx;
    Matrix4 invInertia;
    Matrix4 rot;
    Matrix4 inertia;
    Matrix4 tmp;
    Vector4 push;
    Vector4 min;
    Vector4 max;
    f32 dt;
    f32 dist;
    MoveThing *thing;
    MoveThing *t;
    MoveThing *other;
    MoveThing *source;
    MoveThing *otherSource;
    SharedSlaveHolder *holder;
    s32 numMoved;
    s32 count;
    s32 i;
    s32 j;
    u32 k;
    u32 gridFlags;
    u32 stuck;
    u32 iters;
    f32 elapsed;
    f32 dtStep;
    f32 dtSave;
    f32 depth;

    stuck = 0;
    iters = 0;
    elapsed = 0.0f;
    numMoved = 0;
    MemClr32(g_aCI, sizeof(g_aCI));
    CollisionPairCache_Heartbeat();
    CollisionTriCache_Heartbeat();
    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = thing->next) {
        if (!(thing->flags & 0x280008)) {
            thing->vtable->move(thing);
        }
        thing->flags &= ~0x1A040;
        if (thing->unk80 == 0) {
            continue;
        }
        if (!(thing->flags & 4) || (thing->flags & 0x200000)) {
            continue;
        }
        moved[numMoved++] = thing;
        if (thing->gridData == NULL || (thing->flags & 8)) {
            continue;
        }
        Thing_DynamicGridMove(thing, g_fDeltaTimeSec);
    }

    do {
        hit.thing = NULL;
        hit.other = NULL;
        dt = dtStep = g_fDeltaTimeSec - elapsed;
        hit.surface = NULL;
        for (i = 0; i < numMoved; i++) {
            t = moved[i];
            dtSave = dt;
            source = Thing_GetSource(t);
            t->flags |= 0x8000;
            if (t->flags & 0x40) {
                continue;
            }
            if (t->flags & 0x200000) {
                continue;
            }
            hit.type = 0;
            if (dt && !(t->flags & 0x24000B) && Thing_HitsTerrain(t, &dt, &hit)) {
                if (hit.type == 0xFF) {
                    if (PSVECDotProduct(t, &hit.normal) < 0.0f) {
                        f32 d;

                        depth = 0.0f;
                        d = PSVECDotProduct(t, &hit.normal);
                        if (d <= 15.5f) {
                            depth = -(d - 15.5f);
                        }
                        t->flags |= 0x12000;
                        PSVECScale(&hit.normal, &push, depth);
                        if (PSVECSquareMag(&t->pushVel) < 40000.0f) {
                            t->vel.x += push.x;
                            t->vel.y += push.y;
                            t->vel.z += push.z;
                            t->pushVel.x += push.x;
                            t->pushVel.y += push.y;
                            t->pushVel.z += push.z;
                        }
                    }
                    dt = dtSave;
                } else if (hit.type == 0xFE) {
                    if (Distance_Plane(&hit.normal, &hit) < -offset) {
                        dist = 1.5f;
                        rayHit.type = 0;
                        offset = 0.3f;
                        if (RayHitsTerrain(&t->mtx.v[3], &hit, &dist, &rayHit)) {
                            if (PSVECSquareMag(&t->pushVel) < 40000.0f) {
                                t->vel.x += rayHit.normal.x * mult;
                                t->vel.y += rayHit.normal.y * mult;
                                t->vel.z += rayHit.normal.z * mult;
                                t->pushVel.x += rayHit.normal.x * mult;
                                t->pushVel.y += rayHit.normal.y * mult;
                                t->pushVel.z += rayHit.normal.z * mult;
                            }
                            hit.type = 0xFF;
                        } else {
                            if (PSVECSquareMag(&t->pushVel) < 40000.0f) {
                                t->vel.x += hit.normal.x * mult;
                                t->vel.y += hit.normal.y * mult;
                                t->vel.z += hit.normal.z * mult;
                                t->pushVel.x += hit.normal.x * mult;
                                t->pushVel.y += hit.normal.y * mult;
                                t->pushVel.z += hit.normal.z * mult;
                            }
                            hit.type = 0xFF;
                        }
                        t->flags |= 0x10080;
                        dt = dtSave;
                    } else {
                        t->mtx.v[3].x += hit.normal.x * offset;
                        t->mtx.v[3].y += hit.normal.y * offset;
                        t->mtx.v[3].z += hit.normal.z * offset;
                        t->flags |= 0x80;
                        hit.type = 0;
                    }
                    hit.thing = t;
                    hit.other = NULL;
                } else {
                    hit.thing = t;
                    hit.other = NULL;
                }
            }
            if (!(t->flags & 0x20002) && dt > 0.0f) {
                gridFlags = 4;
                if (t->flags & 0x4000009) {
                    gridFlags = 0;
                }
                Thing_GetDynamicMinMax(t, &min, &max, dtStep);
                count = Grid_GetObjects(&min, &max, gridFlags | 3, objs, 0x200);
                hit.type = 0;
                for (j = 0; j < count; j++) {
                    other = objs[j].thing;
                    dtSave = dt;
                    otherSource = Thing_GetSource(other);
                    if (other->flags & 0x8000) {
                        continue;
                    }
                    if (source != NULL && source == other) {
                        continue;
                    }
                    if (otherSource != NULL && otherSource == t) {
                        continue;
                    }
                    if (!(other->flags & 0x40000004)) {
                        continue;
                    }
                    if (other->flags & 0x200042) {
                        continue;
                    }
                    if ((t->flags & 0x4000009) && (other->flags & 0x4000009)) {
                        continue;
                    }
                    if ((other->flags & 8) && (t->flags & 8)) {
                        continue;
                    }
                    if (CI_Find(t, other)) {
                        continue;
                    }
                    if (!Thing_TestThingCollision(t, other, &dt, &hit)) {
                        continue;
                    }
                    if (hit.type == 0xFF && t->vtable->getPhys(t)->elasticity > 0.0f &&
                        other->vtable->getPhys(other)->elasticity > 0.0f && t->unk78 > 0 && other->unk78 > 0) {
                        f32 pushA = 0.0f;
                        f32 pushB = 0.0f;
                        f32 dA = PSVECDotProduct(t, &hit.normal);
                        f32 dB = PSVECDotProduct(other, &hit.normal);

                        if (dA - 5.5f <= 0.0f) {
                            pushA = -(dA - 5.5f);
                        }
                        if (dB + 5.5f >= 0.0f) {
                            pushB = dB + 5.5f;
                        }
                        t->flags |= 0x10000;
                        other->flags |= 0x10000;
                        if (t->flags & 0x4002008) {
                            pushB += pushA;
                            t->flags &= ~0x10000;
                            pushA = 0.0f;
                        } else if (other->flags & 0x4002008) {
                            pushA += pushB;
                            other->flags &= ~0x10000;
                            pushB = 0.0f;
                        }
                        if (PSVECSquareMag(&t->pushVel) < 40000.0f) {
                            t->vel.x += hit.normal.x * pushA;
                            t->vel.y += hit.normal.y * pushA;
                            t->vel.z += hit.normal.z * pushA;
                            t->pushVel.x += hit.normal.x * pushA;
                            t->pushVel.y += hit.normal.y * pushA;
                            t->pushVel.z += hit.normal.z * pushA;
                        }
                        if (PSVECSquareMag(&other->pushVel) < 40000.0f) {
                            other->vel.x -= hit.normal.x * pushB;
                            other->vel.y -= hit.normal.y * pushB;
                            other->vel.z -= hit.normal.z * pushB;
                            other->pushVel.x -= hit.normal.x * pushB;
                            other->pushVel.y -= hit.normal.y * pushB;
                            other->pushVel.z -= hit.normal.z * pushB;
                        }
                        CI_Add(t, other);
                        dt = dtSave;
                    } else {
                        hit.thing = t;
                        hit.other = other;
                        hit.surface = NULL;
                    }
                }
            }
        }

        if (dt <= 0.0f) {
            stuck++;
            dt = 0.0f;
            if (stuck > 10) {
                if (!(hit.thing->flags & 0x4000008)) {
                    hit.thing->vel.x = 0.0f;
                    hit.thing->vel.y = 0.0f;
                    hit.thing->vel.z = 0.0f;
                    hit.thing->flags |= 0x40;
                }
                if (hit.other != NULL && !(hit.other->flags & 0x4000008)) {
                    hit.other->vel.x = 0.0f;
                    hit.other->vel.y = 0.0f;
                    hit.other->vel.z = 0.0f;
                    hit.other->flags |= 0x40;
                }
            }
        } else {
            stuck = 0;
            for (i = 0; i < numMoved; i++) {
                t = moved[i];
                if (!(t->flags & 8)) {
                    t->mtx.v[3].x += t->vel.x * dt;
                    t->mtx.v[3].y += t->vel.y * dt;
                    t->mtx.v[3].z += t->vel.z * dt;
                    t->flags |= 0x80;
                }
            }
        }

        if (hit.thing != NULL) {
            f32 vA = PSVECDotProduct(&hit.normal, hit.thing);
            f32 vB = 0.0f;
            f32 invMassA = 0.0f;
            f32 invMassB = 0.0f;
            f32 elastA = hit.thing->vtable->getPhys(hit.thing)->elasticity;
            s32 damageB = 0;
            s32 damageA = hit.thing->damage;
            Vector4 crossA;
            Vector4 crossB = {0.0f, 0.0f, 0.0f, 0.0f};
            f32 elastB = 1.0f;
            Vector4 negVel;
            Vector4 rA;
            Vector4 rxnA;
            Vector4 wA;
            Vector4 diagA;
            Vector4 localA;
            Vector4 scaledA;
            Vector4 rB;
            Vector4 rxnB;
            Vector4 wB;
            Vector4 diagB;
            Vector4 sum;
            Vector4 armA;
            Vector4 armB __attribute__((aligned(16)));

            if (hit.other == NULL && hit.thing->vtable == (MoveThingVTable *)g_carAVTable &&
                PSVECDotProduct(&hit.thing->mtx.v[1], &hit.normal) < 0.5f) {
                elastA *= CarA_GetImpactMultiplier(hit.thing, &hit, 1.0f);
                if (hit.thing->mtx.v[1].y < 0.0f) {
                    f32 blend = 0.6f * hit.surface->friction * hit.thing->vtable->getPhys(hit.thing)->friction;

                    negVel.x = -hit.thing->vel.x;
                    negVel.y = -hit.thing->vel.y;
                    negVel.z = -hit.thing->vel.z;
                    if (blend > 1.0f) {
                        blend = 1.0f;
                    }
                    if (PSVECSquareMag(&negVel) != 0.0f) {
                        PSVECNormalize(&negVel, &negVel);
                    }
                    hit.normal.x = (1.0f - blend) * hit.normal.x + blend * negVel.x;
                    hit.normal.y = (1.0f - blend) * hit.normal.y + blend * negVel.y;
                    hit.normal.z = (1.0f - blend) * hit.normal.z + blend * negVel.z;
                    if (PSVECSquareMag(&hit.normal) != 0.0f) {
                        PSVECNormalize(&hit.normal, &hit.normal);
                    }
                }
            }
            if (!(hit.thing->flags & 0x4000008) || hit.thing->unk78 == 0) {
                invMassA = 1.0f / hit.thing->vtable->getMass(hit.thing);
            }
            if (hit.thing->vtable == (MoveThingVTable *)g_carAVTable &&
                (hit.other == NULL || (hit.other->unk78 != 0 && (hit.other->flags & 8) &&
                                       hit.other->vtable->getPhys(hit.other)->elasticity > 0.0f))) {
                MoveCar *car = (MoveCar *)hit.thing;
                f32 lim = car->iface->tuning->maxImpactVel * (0.7f * CarA_GetImpactMultiplier(car, &hit, 1.0f) + 0.3f);

                if (vA > lim) {
                    f32 d = lim - vA;

                    vA = lim;
                    hit.thing->vel.x += hit.normal.x * d;
                    hit.thing->vel.y += hit.normal.y * d;
                    hit.thing->vel.z += hit.normal.z * d;
                } else if (vA < -lim) {
                    f32 d = -lim - vA;

                    vA = -lim;
                    hit.thing->vel.x += hit.normal.x * d;
                    hit.thing->vel.y += hit.normal.y * d;
                    hit.thing->vel.z += hit.normal.z * d;
                }
            }
            PSVECSubtract(&hit.pos, &hit.thing->mtx.v[3], &rA);
            PSVECCrossProduct(&rA, &hit.normal, &rxnA);
            hit.thing->vtable->getInvInertia(hit.thing, &diagA);
            InverseXformVecOrientation(&hit.thing->mtx, &rxnA, &localA);
            scaledA.x = diagA.x * localA.x;
            scaledA.y = diagA.y * localA.y;
            scaledA.z = diagA.z * localA.z;
            XformVecOrientation(&hit.thing->mtx, &scaledA, &wA);
            PSVECCrossProduct(&wA, &rA, &crossA);
            if (hit.other != NULL) {
                vB = PSVECDotProduct(&hit.normal, hit.other);
                elastB = hit.other->vtable->getPhys(hit.other)->elasticity;
                if (!(hit.other->flags & 0x4000008) || hit.other->unk78 == 0) {
                    invMassB = 1.0f / hit.other->vtable->getMass(hit.other);
                }
                if (hit.other->type == 9) {
                    elastB *= CarA_GetImpactMultiplier(hit.other, &hit, -1.0f);
                    if (hit.thing->type == 9) {
                        MoveImpactFx *fx_ = *(MoveImpactFx **)((u8 *)g_worldData.world + 0x9C);
                        f32 rel = vB - vA;
                        f32 lo = fx_->minVel;
                        u32 n = fx_->count;
                        f32 hi = fx_->maxVel;

                        if (rel > lo && hi - lo != 0.0f && n != 0) {
                            u32 idx = (f32)n * ((rel - lo) / (hi - lo));
                            u32 id;

                            if (idx > n) {
                                idx = n;
                            }
                            id = fx_->ids[idx - 1];
                            if (id != 0) {
                                fx.id = id;
                                fx.pos = hit.pos;
                                fx.vel = g_zeroVector;
                                fx.unk44 = 0;
                                fx.unk48 = 0.0f;
                                fx.unk4C = 0.0f;
                                fx.angVel = g_zeroVector;
                                fx.quat = g_zeroVector;
                                ThingManager_DARYL_CreateThing(1, &fx);
                            }
                        }
                    }
                }
                if (hit.other->type == 9 && (hit.thing->flags & 8) && hit.thing->unk78 != 0 && elastA > 0.0f) {
                    f32 lim = ((MoveCar *)hit.other)->iface->tuning->maxImpactVel;

                    if (vB > lim) {
                        f32 d = lim - vB;

                        vB = lim;
                        hit.other->vel.x += hit.normal.x * d;
                        hit.other->vel.y += hit.normal.y * d;
                        hit.other->vel.z += hit.normal.z * d;
                    } else if (vB < -lim) {
                        f32 d = -lim - vB;

                        vB = -lim;
                        hit.other->vel.x += hit.normal.x * d;
                        hit.other->vel.y += hit.normal.y * d;
                        hit.other->vel.z += hit.normal.z * d;
                    }
                }
                PSVECSubtract(&hit.pos, &hit.other->mtx.v[3], &rB);
                rot = hit.other->mtx;
                inertia = g_identity;
                hit.other->vtable->getInvInertia(hit.other, &diagB);
                TransposeSubMatrix(&rot);
                inertia.v[0].x = diagB.x;
                inertia.v[1].y = diagB.y;
                inertia.v[2].z = diagB.z;
                MultiplySubMatrix(&inertia, &hit.other->mtx, &tmp);
                MultiplySubMatrix(&rot, &tmp, &invInertia);
                PSVECCrossProduct(&rB, &hit.normal, &rxnB);
                XformVecOrientation(&invInertia, &rxnB, &wB);
                PSVECCrossProduct(&wB, &rB, &crossB);
                damageB = hit.other->damage;
            } else if (hit.surface != NULL) {
                elastB = hit.surface->elasticity;
            }
            vA = (vA - vB) * -(1.0f + _fabs(elastA * elastB));
            PSVECAdd(&crossA, &crossB, &sum);
            vA /= (invMassA + invMassB) * PSVECDotProduct(&hit.normal, &hit.normal) + PSVECDotProduct(&sum, &hit.normal);
            if (elastB < 0.0f) {
                elastB = 0.0f;
            } else {
                elastB = vA;
            }
            if (elastA < 0.0f) {
                elastA = 0.0f;
            } else {
                elastA = -vA;
            }
            if (hit.other != NULL && hit.thing->vtable == (MoveThingVTable *)g_carAVTable &&
                hit.other->vtable == (MoveThingVTable *)g_carAVTable) {
                MoveCar *car = (MoveCar *)hit.thing;
                MoveCarBody *body = car->iface->model->body;

                armA.x = hit.pos.x - car->thing.mtx.v[3].x;
                armA.y = hit.pos.y - car->thing.mtx.v[3].y;
                armA.z = hit.pos.z - car->thing.mtx.v[3].z;
                if (FloatsEqual(PSVECDotProduct(&car->thing.mtx.v[2], &armA) - car->rideHeight, body->bumperHeight, 0.3f)) {
                    damageA = -vA * car->iface->tuning->damageScale * PSVECDotProduct(&hit.thing->mtx.v[2], &hit.normal);
                    if (hit.thing->player != NULL && !Player_IsHuman(hit.thing->player)) {
                        damageA = (f32)damageA * ((MovePlayer *)hit.thing->player)->difficulty->damageScale;
                    }
                }
                car = (MoveCar *)hit.other;
                body = car->iface->model->body;
                armB.x = hit.pos.x - car->thing.mtx.v[3].x;
                armB.y = hit.pos.y - car->thing.mtx.v[3].y;
                armB.z = hit.pos.z - car->thing.mtx.v[3].z;
                if (FloatsEqual(PSVECDotProduct(&car->thing.mtx.v[2], &armB) - car->rideHeight, body->bumperHeight, 0.3f)) {
                    damageB = vA * car->iface->tuning->damageScale * PSVECDotProduct(&hit.other->mtx.v[2], &hit.normal);
                    if (hit.other->player != NULL && !Player_IsHuman(hit.other->player)) {
                        damageB = (f32)damageB * ((MovePlayer *)hit.other->player)->difficulty->damageScale;
                    }
                }
            }
            hit.thing->vtable->collide(hit.thing, &hit, damageB, elastB);
            Thing_DynamicGridMove(hit.thing, dtStep);
            if (hit.other != NULL) {
                hit.other->vtable->collide(hit.other, &hit, damageA, elastA);
                Thing_DynamicGridMove(hit.other, dtStep);
            }
        }

        for (i = 0; i < numMoved; i++) {
            t = moved[i];
            t->vtable->postMove(t, dt);
            t->flags &= ~0x8000;
        }
        if (dt < 0.00001f && iters > 100) {
            elapsed = g_fDeltaTimeSec;
        }
        elapsed += dt;
        iters++;
    } while (elapsed < g_fDeltaTimeSec);

    g_frameCounter++;
    for (t = g_thingManager_DARYL.activeList; t != NULL; t = t->next) {
        if (t->gridData != NULL && !(t->flags & 0x200008)) {
            Thing_RotationalGridMove(t);
        }
    }
    for (t = g_thingManager_DARYL.activeList; t != NULL; t = t->next) {
        if (!(t->flags & 0x10000)) {
            t->vel.x -= t->pushVel.x;
            t->vel.y -= t->pushVel.y;
            t->vel.z -= t->pushVel.z;
            t->pushVel.x = 0.0f;
            t->pushVel.y = 0.0f;
            t->pushVel.z = 0.0f;
        }
        if (t->vtable == (MoveThingVTable *)g_carAVTable) {
            t->vel.x -= g_fDeltaTimeSec * (((MoveCar *)t)->drag * t->mtx.v[0].x);
            t->vel.y -= g_fDeltaTimeSec * (((MoveCar *)t)->drag * t->mtx.v[0].y);
            t->vel.z -= g_fDeltaTimeSec * (((MoveCar *)t)->drag * t->mtx.v[0].z);
        }
        if (!(t->flags & 0x200000)) {
            t->vtable->update(t);
            if (!(t->flags & 8) && t->gridData != NULL) {
                Thing_StaticGridMove(t);
            }
            Thing_MoveSlaves(t);
        }
    }
    for (k = 0; k < 0x40; k++) {
        if (((SharedSlaveHolder *)g_aSharedSlaveHolder)[k].key != NULL) {
            MoveThing *slave = ((SharedSlaveHolder *)g_aSharedSlaveHolder)[k].slaves;

            while (slave != NULL) {
                MoveThing *next = slave->next;

                slave->vtable->update(slave);
                if (Timer_Interrupt300()) {
                    f32 inv = 1.0f / Sqrt(slave->quat.x * slave->quat.x + slave->quat.y * slave->quat.y +
                                          slave->quat.z * slave->quat.z + slave->quat.w * slave->quat.w);

                    slave->quat.x *= inv;
                    slave->quat.y *= inv;
                    slave->quat.z *= inv;
                    slave->quat.w *= inv;
                }
                slave = next;
            }
        }
    }
    ThingManagerList_RemoveReleasedThings((s32)&g_thingManager_DARYL.activeList);
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

void *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id) {
    void *slave = g_thingManager_DARYL.freeList[type];

    if (slave == NULL) {
        slave = NULL;
    } else {
        g_thingManager_DARYL.freeList[type] = M2C_FIELD(slave, void **, 0xA4);
        if (g_thingManager_DARYL.freeList[type] != NULL) {
            M2C_FIELD(g_thingManager_DARYL.freeList[type], void **, 0xA0) = NULL;
        }
        M2C_FIELD(slave, void **, 0xA4) = NULL;
    }
    if (slave != NULL) {
        ((void (*)(void *, void *))(M2C_FIELD(slave, void ***, 0x8C))[0])(slave, params);
        M2C_FIELD(slave, u8 *, 0x82) = id;
        M2C_FIELD(slave, u32 *, 0x74) |= 0x20000000;
        return slave;
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
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));
    Vector4 end __attribute__((aligned(16)));
    GridResult results[512];
    u32 count;
    u32 i;
    ThingHeader *other;
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
        other = (ThingHeader *)results[i].id;
        if (!(other->flags & 2) && other != g_pNoRayCollideThing &&
            SegmentVsSphere(other->unk40, start, &end, other->unk70)) {
            hit = (*(f32 (**)(void *, Vector4 *, Vector4 *, void *, f32))((u8 *)other->vtable + 0x38))(other, start, dir, info, best);
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

s8 OBoxHitsTerrainAndObjectsZeroTime(void *obox, void *thing, void *info) {
    typedef struct { f32 x, y, z, w; } Vec4;
    u32 Grid_GetObjects(Vec4 *min, Vec4 *max, u32 typeMask, GridResult *results, u32 maxResults);
    s8 OBoxHitsTerrain(void *obox, Vector4 *vel, f32 *t, void *info);
    f32 OBoxHitsOBox(void *a, Vector4 *velA, void *b, Vector4 *velB, s32 arg4, f32 t);
    f32 PSVECSquareDistance(void *a, void *b);
    extern Vector4 g_zeroVector;
    f32 time;
    Vec4 max;
    Vec4 min;
    GridResult results[96];
    u32 count;
    u32 i;
    u8 *other;
    f32 t;
    s32 hit;

    time = 1.0f;
    if (!(*(u32 *)((u8 *)thing + 0x74) & 0x40000)) {
        if (OBoxHitsTerrain(obox, &g_zeroVector, &time, info)) {
            return 1;
        }
    }
    if (*(u32 *)((u8 *)thing + 0x74) & 0x20000) {
        hit = 0;
    } else {
        Thing_GetStaticRotationalMinMax(thing, (Vector4 *)&min, (Vector4 *)&max);
        count = Grid_GetObjects(&min, &max, 5, results, 0x60);
        for (i = 0; i < count; i++) {
            other = (u8 *)results[i].id;
            if (!(*(u32 *)(other + 0x74) & 2) && *(s16 *)(other + 0x80) != 0 && other != thing &&
                (*(f32 *)(other + 0x70) + *(f32 *)((u8 *)thing + 0x70)) * (*(f32 *)(other + 0x70) + *(f32 *)((u8 *)thing + 0x70)) >
                    PSVECSquareDistance((u8 *)thing + 0x40, other + 0x40)) {
                if (*(s16 *)(other + 0x80) == 1) {
                    t = OBoxHitsOBox(obox, &g_zeroVector, (*(void *(**)(void *))(*(u8 **)(other + 0x8C) + 0x40))(other), &g_zeroVector, 0, 1.0f);
                }
                if (t < 1.0f) {
                    hit = 1;
                    goto done;
                }
            }
        }
        hit = 0;
    }
done:
    return hit;
}

s8 OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(void *obox, void *thing, void *arg2) {
    s8 OBoxHitsTerrainZeroTimeEdgesOnly(void *, void *);
    s32 OBox_CollideAllThingsZeroTime(void *, void *, void *);
    if (!(M2C_FIELD(thing, u32 *, 0x74) & 0x40000) && OBoxHitsTerrainZeroTimeEdgesOnly(obox, arg2)) {
        return 1;
    }
    return OBox_CollideAllThingsZeroTime(obox, thing, arg2);
}

s32 RayHitsObjects_GetObject(Vector4 *start, Vector4 *end, f32 *t, s8 statics, s8 dynamics) {
    u32 Grid_GetObjects(Vector4 *min, Vector4 *max, u32 typeMask, GridResult *results, u32 maxResults);
    s8 SegmentVsSphere(void *center, Vector4 *start, Vector4 *end, f32 radius);
    GridResult results[256];
    u8 info[0x50] __attribute__((aligned(16)));
    Vector4 hitEnd __attribute__((aligned(16)));
    Vector4 dir __attribute__((aligned(16)));
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));
    u32 count;
    u32 i;
    Destructible *other;
    f32 best;
    f32 hit;

    best = *t;
    dir.x = end->x - start->x;
    dir.y = end->y - start->y;
    dir.z = end->z - start->z;
    hitEnd.x = dir.x * best + start->x;
    hitEnd.y = dir.y * best + start->y;
    hitEnd.z = dir.z * best + start->z;
    min = *end;
    max = *start;
    if (start->x < min.x) {
        min.x = start->x;
    }
    if (start->y < min.y) {
        min.y = start->y;
    }
    if (start->z < min.z) {
        min.z = start->z;
    }
    if (end->x > max.x) {
        max.x = end->x;
    }
    if (end->y > max.y) {
        max.y = end->y;
    }
    if (end->z > max.z) {
        max.z = end->z;
    }
    count = Grid_GetObjects(&min, &max, 4, results, 0x100);
    for (i = 0; i < count; i++) {
        other = (Destructible *)results[i].id;
        if (other->flags & 2) {
            continue;
        }
        if ((statics == 1 && other->record->info->takesDamage == 0 && other->record->info->armour == 0) ||
            (dynamics == 1 && (other->record->info->takesDamage == 1 || other->record->info->armour != 0))) {
            if (SegmentVsSphere(&other->offsetPos, start, &hitEnd, *(f32 *)((u8 *)other + 0x70))) {
                hit = (*(f32 (**)(void *, Vector4 *, Vector4 *, void *, f32))((u8 *)other->vtable + 0x38))(other, start, &dir, info, best);
                if (hit < *t) {
                    best = hit;
                }
            }
        }
    }
    if (best < *t) {
        *t = best;
        return 0;
    }
    return 0;
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

static inline s32 ThingManager_GetActiveUnk6D100(void **things) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 1; i++) {
        u8 *thing = g_thingManager_DARYL.unk0 + 0x6D100 + i * 0x5A0;
        if (!(*(u32 *)(thing + 0x74) & 0x201000)) {
            things[count++] = thing;
        }
    }
    return count;
}

static inline s32 ThingManager_GetActiveUnk5B940(void **things) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 1; i++) {
        u8 *thing = g_thingManager_DARYL.unk0 + 0x5B940 + i * 0x5A0;
        if (!(*(u32 *)(thing + 0x74) & 0x201000)) {
            things[count++] = thing;
        }
    }
    return count;
}

static inline s32 ThingManager_GetActiveUnk20280(void **things) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 500; i++) {
        u8 *thing = g_thingManager_DARYL.unk0 + 0x20280 + i * 0xF0;
        if (!(*(u32 *)(thing + 0x74) & 0x201000)) {
            things[count++] = thing;
        }
    }
    return count;
}

void *FindHost(Vector4 *pos, Vector4 *dir) {
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *src, Vector4 *dst);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void *items[500];
    void *cars[8];
    void *pods[8];
    Vector4 dir1 __attribute__((aligned(16)));
    Vector4 diff1 __attribute__((aligned(16)));
    Vector4 dir2 __attribute__((aligned(16)));
    Vector4 diff2 __attribute__((aligned(16)));
    Vector4 dir3 __attribute__((aligned(16)));
    Vector4 diff3 __attribute__((aligned(16)));
    Vector4 dir4 __attribute__((aligned(16)));
    Vector4 diff4 __attribute__((aligned(16)));
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

#pragma push
#pragma dont_inline on
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
#pragma pop

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

