#include "common.h"


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

s32 TriggerCreatureVolumeEntered(u32 *arg0) {
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
        temp_r3 = M2C_FIELD(*(arg0 + ((temp_r27 + var_r26 + 2) * 4)), u32 *, 0x1C);
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

void Thing_GetInterfaceInfo(void *thing, void *info) {
    M2C_FIELD(info, s32 *, 0x10) = M2C_FIELD(thing, s32 *, 0x78);
    M2C_FIELD(info, u32 *, 0x14) = 0.681818f * MagnitudeVector(thing);
    M2C_FIELD(info, s32 *, 0x18) = 0;
    M2C_FIELD(info, s32 *, 0x4) = 0;
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
    Vector4 min;
    Vector4 max;

    Thing_GetDynamicMinMax(thing, &min, &max, 0.0f);
    M2C_FIELD(thing, s32 *, 0x84) = Grid_AddObject(type, thing, &min, &max);
    return M2C_FIELD(thing, s32 *, 0x84);
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
    Vector4 min;
    Vector4 max;

    if (M2C_FIELD(thing, u32 *, 0x84) != 0) {
        Thing_GetDynamicMinMax(thing, &min, &max, expand);
        Grid_MoveObject(M2C_FIELD(thing, s32 *, 0x84), &min, &max);
    }
}

void Thing_StaticGridMove(void *thing) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    Vector4 min;
    Vector4 max;

    if (M2C_FIELD(thing, u32 *, 0x84) != 0) {
        Thing_GetDynamicMinMax(thing, &min, &max, 0.0f);
        Grid_MoveObject(M2C_FIELD(thing, s32 *, 0x84), &min, &max);
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

