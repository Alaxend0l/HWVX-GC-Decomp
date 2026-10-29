#include "common.h"

#pragma use_lmw_stmw on

s8 IsPointWithinVolume();

/* The transform every Thing starts with. */
typedef struct Thing {
    Vector4 unk0;      /* 0x00 */
    Matrix4 mtx;       /* 0x10 */
    Quaternion quat;   /* 0x50 */
    Vector4 unk60;     /* 0x60 */
    f32 unk70;         /* 0x70 */
    u32 flags;         /* 0x74 */
    u8 pad78[8];
    s16 unk80;         /* 0x80 */
    u8 unk82;          /* 0x82 */
    u8 unk83;          /* 0x83 */
    s32 unk84;         /* 0x84 */
    s32 unk88;         /* 0x88 */
    u8 pad8C[8];
    s32 unk94;         /* 0x94 */
} Thing;

s32 Thing_Init(Thing *thing, s32 arg1) {
    extern Matrix4 g_identity;

    thing->unk0.z = 0.0f;
    thing->unk0.y = 0.0f;
    thing->unk0.x = 0.0f;
    thing->unk60.z = 0.0f;
    thing->unk60.y = 0.0f;
    thing->unk60.x = 0.0f;
    thing->unk70 = 0.0f;
    thing->quat.w = 1.0f;
    thing->quat.x = 0.0f;
    thing->quat.y = 0.0f;
    thing->quat.z = 0.0f;
    thing->flags = 0;
    thing->unk80 = 0;
    thing->mtx = g_identity;
    thing->unk94 = 0;
    thing->unk82 = 0xFF;
    thing->unk83 = 0xFF;
    thing->unk88 = 0;
    thing->unk84 = 0;
    return 1;
}

void Thing_Release(s32 thing) {
    M2C_UNK Grid_FreeObject(void *);
    M2C_UNK ThingManagerList_EmancipateSlaves(s32, s32);
    M2C_UNK ThingManagerList_ReleaseAll(s32);
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    void *player;

    M2C_FIELD(thing, u32 *, 0x74) |= 0x200;
    M2C_FIELD(thing, u32 *, 0x74) |= 2;
    M2C_FIELD(thing, u32 *, 0x74) |= 0x200000;
    if (M2C_FIELD(thing, void **, 0x84) != NULL) {
        Grid_FreeObject(M2C_FIELD(thing, void **, 0x84));
    }
    M2C_FIELD(thing, void **, 0x84) = NULL;
    ThingManagerList_EmancipateSlaves(thing + 0x10, thing + 0x88);
    ThingManagerList_ReleaseAll(thing + 0x88);
    ThingManagerList_RemoveReleasedThings(thing + 0x88);
    player = M2C_FIELD(thing, void **, 0x94);
    if (player != NULL && M2C_FIELD(player, void **, 0x2C) == (void *)thing) {
        Player_DetachThingDeath(player);
    }
}

s32 Thing_PreMove(void) {
    return 1;
}

s32 Thing_Move(void) {
    return 1;
}

void Thing_Draw(void)
{

}

f32 Thing_GetMass(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

void Thing_DynamicCollideImpact(void)
{
}

void Thing_DynamicRotation(void)
{

}

s32 Thing_GetSource(void *arg0) {
    void *temp_r3;

    temp_r3 = M2C_FIELD(arg0, void **, 0x94);
    if (temp_r3 != NULL) {
        return M2C_FIELD(temp_r3, s32 *, 0x2C);
    }
    return 0;
}

s32 TriggerAirboxEntered(u32 *arg0) {
    M2C_UNK sp8;
    s32 temp_r30;
    u32 *var_r31;
    u32 temp_r27;
    u32 temp_r3;
    u32 var_r26;
    u32 var_r28;
    void *temp_r29;

    var_r28 = 0U;
    var_r31 = arg0;
    temp_r27 = *arg0;
    temp_r30 = (temp_r27 * 4) + 4;
loop_9:
    if (var_r28 >= temp_r27) {
        return 0;
    }
    temp_r29 = M2C_FIELD(var_r31, void **, 4);
    if ((u32) M2C_FIELD(temp_r29, u32 *, 4) == 1U) {
        var_r26 = 0U;
loop_7:
        if (var_r26 >= (u32) *(arg0 + temp_r30)) {
            goto block_8;
        }
        temp_r3 = M2C_FIELD(*(arg0 + ((temp_r27 + var_r26 + 2) * 4)), u32 *, 0x28);
        if ((temp_r3 != 0U) && (IsPointWithinVolume(temp_r3 + 0x40, temp_r29, &sp8) != 0)) {
            return 1;
        }
        var_r26 += 1;
        goto loop_7;
    }
block_8:
    var_r31 += 4;
    var_r28 += 1;
    goto loop_9;
}

s32 TriggerCreatureVolumeEntered(u32 *trigger) {
    f32 dist;
    u8 *volume;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        if (M2C_FIELD(thing, u32 *, 4) == 1) {
            for (j = 0; j < trigger[numThings + 1]; j++) {
                volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x1C);
                if (volume != NULL && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

s32 TriggerItemVolumeEntered(u32 *trigger) {
    f32 dist;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;
    u8 *volume;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numThings + 1]; j++) {
            volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x24);
            if (volume != NULL) {
                if (M2C_FIELD(volume, u8 **, 0x94) != NULL && M2C_FIELD(M2C_FIELD(volume, u8 **, 0x94), u8 **, 0x2C) != NULL) {
                    volume = M2C_FIELD(M2C_FIELD(volume, u8 **, 0x94), u8 **, 0x2C);
                }
                if (M2C_FIELD(thing, u32 *, 4) == 1 && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

s32 TriggerMovableVolumeEntered(u32 *trigger) {
    f32 dist;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;
    u8 *volume;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numThings + 1]; j++) {
            volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x10);
            if (volume != NULL && M2C_FIELD(thing, u32 *, 4) == 1 && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                return 1;
            }
        }
    }
    return 0;
}

s32 TriggerAirboxStopIn(u32 *trigger) {
    f32 PSVECSquareMag(Vector4 *v);
    f32 dist;
    u8 *volume;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numThings + 1]; j++) {
            volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x28);
            if (M2C_FIELD(thing, u32 *, 4) == 1 && PSVECSquareMag((Vector4 *)volume) < 50.0f && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                return 1;
            }
        }
    }
    return 0;
}

s32 TriggerAirboxExited(u32 *trigger) {
    f32 dist;
    u8 *volume;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numThings + 1]; j++) {
            volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x28);
            if (M2C_FIELD(thing, u32 *, 4) == 1 && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                return 0;
            }
        }
    }
    return 1;
}

s32 ActionAirboxChangeMode(s32 *data) {
    u32 count = data[1];
    s32 mode = data[0];
    u32 i;

    for (i = 0; i < count; i++) {
        u8 *airbox = (u8 *)data[i + 2];
        switch (mode) {
        case 0:
            *(u32 *)(airbox + 4) = 0;
            break;
        case 2:
            if (*(u32 *)(airbox + 4) == 1) {
                *(u32 *)(airbox + 4) = 0;
            } else {
                *(u32 *)(airbox + 4) = 1;
            }
            break;
        case 1:
        default:
            *(u32 *)(airbox + 4) = 1;
            break;
        }
    }
    return 1;
}

s32 TriggerP2PActivated(u32 *data) {
    u32 n = data[0];
    u32 i;
    u32 j;
    u32 k = n + 1;

    for (i = 0; i < n; i++) {
        for (j = 0; j < data[k]; j++) {
            if (M2C_FIELD((data + 1)[j + k], s32 *, 0x74) & 0x800) {
                return 1;
            }
        }
    }
    return 0;
}

s32 ActionVehicleTeleport(u32 *action) {
    extern Vector4 g_zeroVector;
    s8 Player_IsHuman(void *player);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    typedef struct {
        Matrix4 mtx;
        u8 pad40[0x44 - 0x40];
        u32 fx;
        u8 pad48[0x54 - 0x48];
        u32 flags;
    } TeleportData;
    typedef struct {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        Vector4 angVel;
        u32 id;
        s32 unk44;
        f32 unk48;
        f32 unk4C;
    } FxParams;
    typedef struct {
        Vector4 vel;
        Matrix4 mtx;
        Vector4 quat;
        u8 pad60[0x74 - 0x60];
        u32 flags;
        u8 pad78[0x8C - 0x78];
        void (**vtbl)(void *);
        u8 pad90[0x94 - 0x90];
        u8 *player;
        u8 pad98[0x3B0 - 0x98];
        s32 unk3B0;
    } Car;
    TeleportData *data;
    u32 i;
    Car *car;
    Vector4 local;
    FxParams params;

    data = (TeleportData *)action[1];
    for (i = 0; i < action[2]; i++) {
        car = *(Car **)(action[i + 3] + 0x28);
        if (data->fx != 0) {
            params.id = data->fx;
            params.pos = data->mtx.v[3];
            SubMatrixToQuaternion(data, &params.quat);
            params.vel.x = params.vel.y = params.vel.z = 0.0f;
            params.unk44 = 0;
            params.unk48 = 0.0f;
            params.unk4C = 0.0f;
            params.angVel = g_zeroVector;
            ThingManager_DARYL_CreateThing(1, &params);
        }
        if (data->flags & 2) {
            InverseXformVecOrientation(&car->mtx, &car->vel, &local);
            XformVecOrientation(data, &local, &car->vel);
        } else if (car->player != NULL && *(Car **)(car->player + 0x2C) != NULL &&
                   *(Car **)(car->player + 0x2C) == car) {
            car->player[0x32] = 5;
        }
        car->mtx = data->mtx;
        SubMatrixToQuaternion(&car->mtx, &car->quat);
        car->vtbl[3](car);
        car->flags |= 0x800;
        car->unk3B0 = -1;
        if (!Player_IsHuman(car->player)) {
            *(s32 *)(car->player + 0x4A4) = 0x20;
            *(s32 *)(car->player + 0x634) = -0x78;
        }
    }
    return 1;
}

f32 MagnitudeVector(void *v);
void Thing_GetInterfaceInfo(void *thing, void *info) {
    typedef struct InterfaceInfo {
        s32 unk0;
        s32 unk4;
        s32 unk8[2];
        s32 unk10;
        u32 speed;
        s32 unk18;
    } InterfaceInfo;
    InterfaceInfo *ii = info;

    ii->unk10 = *(s32 *)((u8 *)thing + 0x78);
    ii->speed = 0.681818f * MagnitudeVector(thing);
    ii->unk18 = 0;
    ii->unk4 = 0;
}

void Thing_SetSlavePlayers(void *thing) {
    void *slave;

    for (slave = M2C_FIELD(thing, void **, 0x88); slave != NULL; slave = M2C_FIELD(slave, void **, 0xA4)) {
        M2C_FIELD(slave, s32 *, 0x94) = M2C_FIELD(thing, s32 *, 0x94);
    }
}

void Thing_InitAllSlavesNow(s32 arg0) {
    M2C_UNK ThingManagerList_InitAllSlavesNow(s32);
    ThingManagerList_InitAllSlavesNow(arg0 + 0x88);
}

void Thing_SlaveTransform(Thing *thing, Matrix4 *mtx) {
    Matrix4 local;

    local = thing->mtx;
    MultiplyMatrix(&local, mtx, &thing->mtx);
    SubMatrixToQuaternion(&thing->mtx, &thing->quat);
}

void Thing_SlaveTransformQuaternionOnly(Thing *thing, Matrix4 *mtx) {
    Quaternion rot __attribute__((aligned(16)));
    Quaternion quat __attribute__((aligned(16)));
    Vector4 pos __attribute__((aligned(16)));

    quat = thing->quat;
    pos = thing->mtx.v[3];
    XformVec(mtx, &pos, &thing->mtx.v[3]);
    SubMatrixToQuaternion(mtx, &rot);
    QuaternionMult(&rot, &quat, &thing->quat);
}

void *Thing_GetSurfaceScript(void) {
    extern u8 g_defaultSurfaceScript[];
    return g_defaultSurfaceScript;
}

void Thing_GetInverseLocalInertiaTensor(void *thing, f32 *tensor) {
    tensor[0] = 0.0f;
    tensor[1] = 0.0f;
    tensor[2] = 0.0f;
}

s32 Thing_GetNearestNavPoint(void) {
    return -1;
}

s32 Thing_AddToGrid(void *thing, s32 type) {
    void Thing_GetDynamicMinMax(void *thing, Vector4 *min, Vector4 *max, f32 expand);
    s32 Grid_AddObject(s32 type, void *thing, Vector4 *min, Vector4 *max);
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));

    Thing_GetDynamicMinMax(thing, &min, &max, 0.0f);
    *(s32 *)((u8 *)thing + 0x84) = Grid_AddObject(type, thing, &min, &max);
    return *(s32 *)((u8 *)thing + 0x84);
}

void Thing_GetStaticRotationalMinMax(void *thing, Vector4 *min, Vector4 *max) {
    *max = *(Vector4 *)((u8 *)thing + 0x40);
    *min = *max;
    min->x -= M2C_FIELD(thing, f32 *, 0x70);
    min->y -= M2C_FIELD(thing, f32 *, 0x70);
    min->z -= M2C_FIELD(thing, f32 *, 0x70);
    max->x += M2C_FIELD(thing, f32 *, 0x70);
    max->y += M2C_FIELD(thing, f32 *, 0x70);
    max->z += M2C_FIELD(thing, f32 *, 0x70);
}

void Thing_DynamicGridMove(void *thing, f32 expand) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));

    if (*(u32 *)((u8 *)thing + 0x84) != 0) {
        Thing_GetDynamicMinMax(thing, &min, &max, expand);
        Grid_MoveObject(*(s32 *)((u8 *)thing + 0x84), &min, &max);
    }
}

void Thing_StaticGridMove(void *thing) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    void Thing_GetDynamicMinMax(void *thing, Vector4 *min, Vector4 *max, f32 expand);
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));

    if (*(u32 *)((u8 *)thing + 0x84) != 0) {
        Thing_GetDynamicMinMax(thing, &min, &max, 0.0f);
        Grid_MoveObject(*(s32 *)((u8 *)thing + 0x84), &min, &max);
    }
}

void Thing_RotationalGridMove(void *thing) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    Vector4 min;
    Vector4 max;

    if (M2C_FIELD(thing, u32 *, 0x84) != 0) {
        Thing_GetStaticRotationalMinMax(thing, &min, &max);
        Grid_MoveObject(M2C_FIELD(thing, s32 *, 0x84), &min, &max);
    }
}

void Thing_ProcessEndEvent(u8 *thing, u8 *event, u8 *collision, Vector4 *vel) {
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    s32 RandomAdd_s32(s32 base, s32 range);
    extern Vector4 g_zeroVector;
    struct {
        Vector4 pos;
        Vector4 quat;
        Vector4 origin;
        Vector4 vel;
        void *data;
        s32 owner;
        f32 a;
        f32 b;
    } params;
    void *data;
    u8 *other;

    data = NULL;
    if (event != NULL && *(u32 *)(event + 0x34) != 0) {
        data = (*(void ***)(event + 0x38))[RandomAdd_s32(0, *(u32 *)(event + 0x34))];
    }
    if (data != NULL) {
        params.data = data;
        params.pos = *(Vector4 *)(thing + 0x40);
        params.quat = *(Vector4 *)(thing + 0x50);
        params.origin = *(Vector4 *)(thing + 0x0);
        params.owner = *(s32 *)(thing + 0x94);
        params.a = 0.0f;
        params.b = 0.0f;
        if (vel != NULL) {
            params.vel = *vel;
        } else {
            params.vel = g_zeroVector;
        }
        if (collision != NULL) {
            other = *(u8 **)(collision + 0x20);
            if (other != NULL && other != thing) {
                params.owner = *(s32 *)(other + 0x94);
            } else {
                other = *(u8 **)(collision + 0x24);
                if (other != NULL && other != thing) {
                    params.owner = *(s32 *)(other + 0x94);
                }
            }
        }
        ThingManager_DARYL_CreateThing(1, &params);
    }
}

void Thing_CollideWithRay(void)
{
}

void Thing_GetTargetPos(void *thing, Quaternion *pos) {
    *pos = *(Quaternion *)((u8 *)thing + 0x40);
}

s32 Thing_GetOrientedCollision(void) {
    return 0;
}

s32 Thing_HitsTerrain(void *thing, void *arg1, void *arg2) {
    s32 OBoxHitsTerrain(void *obox, void *thing, void *arg1, void *arg2);
    s32 RayHitsTerrain_Aux3(void *pos, void *thing, void *arg1, void *arg2);

    if (g_worldData.world == NULL) {
        return 0;
    }
    switch (M2C_FIELD(thing, s16 *, 0x80)) {
    case 3:
        return RayHitsTerrain_Aux3((u8 *)thing + 0x40, thing, arg1, arg2);
    case 1:
        return OBoxHitsTerrain(M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void *(**)(void *), 0x40)(thing), thing, arg1, arg2);
    }
    return 0;
}

void Thing_PassiveTargeting(void)
{
}

u32 CollisionInfo_GetOther(void *arg0, u32 arg1) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x20);
    if (temp_r0 == arg1) {
        return M2C_FIELD(arg0, u32 *, 0x24);
    }
    return temp_r0;
}

