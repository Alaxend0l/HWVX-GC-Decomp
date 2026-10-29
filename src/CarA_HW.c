#include "CarA.h"
#include "common.h"

typedef struct CarAShiftSoundParams {
    Vector4 pos;
    Vector4 animRefPos;
    Vector4 vel;
    Vector4 angVel;
    u32 id;
    void *player;
    f32 unk48;
    f32 unk4C;
} CarAShiftSoundParams;

extern f32 g_fDeltaTimeSec;
extern u32 g_iDeltaTimeMS;
extern void *g_pSourcePlayer;
extern Vector4 g_zeroVector;

s8 Player_IsHuman(void *player);
s32 RandomAdd_s32(s32 base, s32 range);
void *ThingManager_DARYL_CreateThing(s32 type, void *params);
f32 CarA_Transmission_GetTorqueAtWheels(CarA *car);


s32 CarA_DoFreeStyleStunts(void *car) {
    s32 threshold;
    s8 *pad;

    threshold = M2C_FIELD(M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), void **, 0x8C), s32 *, 0x28);
    if (M2C_FIELD(car, void **, 0x94) == NULL || !Player_IsHuman(M2C_FIELD(car, void **, 0x94))) {
        return 0;
    }
    pad = M2C_FIELD(car, s8 **, 0x94);
    if (*(u8 *)(pad + 0x10) == 0 && *(u8 *)(pad + 0x1A) == 0 &&
        pad[0] > -0x40 && pad[0] < 0x40 && pad[1] > -0x40 && pad[1] < 0x40) {
        return 0;
    }
    if (M2C_FIELD(car, s32 *, 0x460) > threshold && M2C_FIELD(car, s32 *, 0x404) > threshold &&
        !(M2C_FIELD(car, s32 *, 0x48C) & 0x40002)) {
        return 1;
    }
    return 0;
}

void CarA_StuntImpulse(u8 *car, Vector4 *linImpulse, Vector4 *angImpulse) {
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void MotionState_InitAllSlavesNow(void *motionState, void *slaves, s32 arg2);
    Vector4 lin;
    Vector4 ang;
    Vector4 scaled;
    u8 *data;

    scaled.x = angImpulse->x / *(f32 *)(car + 0x3C0);
    scaled.y = angImpulse->y / *(f32 *)(car + 0x3D4);
    scaled.z = angImpulse->z / *(f32 *)(car + 0x3E8);
    XformVecOrientation(car + 0x10, &scaled, &ang);
    XformVecOrientation(car + 0x10, linImpulse, &lin);
    *(f32 *)(car + 0x0) += lin.x;
    *(f32 *)(car + 0x4) += lin.y;
    *(f32 *)(car + 0x8) += lin.z;
    *(f32 *)(car + 0x380) += ang.x;
    *(f32 *)(car + 0x384) += ang.y;
    *(f32 *)(car + 0x388) += ang.z;
    *(u32 *)(car + 0x48C) |= 2;
    data = *(u8 **)(car + 0x3BC);
    if (*(u32 *)(data + 0xE0) > 4) {
        MotionState_InitAllSlavesNow(car + 0x330, *(u8 **)(data + 0xE4) + 0x34, 4);
    }
}

void CarA_SlaveWeaponStop(CarA *car, s32 weapon) {
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    void ThingManagerList_RemoveReleasedThings(void *list);

    car->slaveWeaponRpm[weapon] = 10001;
    MotionState_ReleaseSlavesByType(&car->motionState, weapon + 0xF0);
    ThingManagerList_RemoveReleasedThings(&car->motionState.slaves);
}
