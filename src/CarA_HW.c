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


s8 CarA_DoFreeStyleStunts(CarA *car) {
    s32 threshold;
    Player *pad;

    threshold = car->vehicleIface->stunt->unk28;
    if (car->player == NULL || !Player_IsHuman(car->player)) {
        return 0;
    }
    pad = car->player;
    if (pad->button10 == 0 && pad->button1A == 0 &&
        pad->stickX > -0x40 && pad->stickX < 0x40 && pad->stickY > -0x40 && pad->stickY < 0x40) {
        return 0;
    }
    if (car->unk460 > threshold && car->lowSpeedFlag > threshold &&
        !(car->transmissionFlags & 0x40002)) {
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
