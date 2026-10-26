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

