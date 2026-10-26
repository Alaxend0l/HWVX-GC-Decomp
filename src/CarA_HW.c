#include "CarA.h"
#include "common.h"


void CarA_SlaveWeaponStop(CarA *car, s32 weapon) {
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    void ThingManagerList_RemoveReleasedThings(void *list);

    car->slaveWeaponRpm[weapon] = 10001;
    MotionState_ReleaseSlavesByType(&car->motionState, weapon + 0xF0);
    ThingManagerList_RemoveReleasedThings(&car->motionState.slaves);
}
