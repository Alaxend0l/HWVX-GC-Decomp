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

void Thing_SetSlavePlayers(void *thing) {
    void *slave;

    for (slave = M2C_FIELD(thing, void **, 0x88); slave != NULL; slave = M2C_FIELD(slave, void **, 0xA4)) {
        M2C_FIELD(slave, s32 *, 0x94) = M2C_FIELD(thing, s32 *, 0x94);
    }
}

void *Thing_GetSurfaceScript(void) {
    extern u8 g_defaultSurfaceScript[];
    return g_defaultSurfaceScript;
}

