#include "common.h"


s32 Motion_Init(u8 *motion, u8 *init) {
    void MotionState_InitAllSlavesNow(void *motionState, void *slaves, s32 arg2);
    void MotionState_BuildMatricesWorld(void *mtx, void *pos, void *state, f32 scale);
    u8 *state = motion + 0xB0;

    Thing_Init(motion, (s32)init);
    M2C_FIELD(motion, Vector4 *, 0x40) = M2C_FIELD(init, Vector4 *, 0x10);
    M2C_FIELD(motion, Vector4 *, 0x50) = M2C_FIELD(init, Vector4 *, 0x20);
    QuaternionToSubMatrix(motion + 0x50, motion + 0x10);
    M2C_FIELD(state, s32 *, 0x0) = M2C_FIELD(init, s32 *, 0x30);
    M2C_FIELD(state, f32 *, 0x24) = 0.0f;
    MotionState_Init(state, 0);
    MotionState_BuildMatricesWorld(motion + 0x50, motion + 0x40, state, 1.0f);
    if (M2C_FIELD(init, void **, 0x34) != NULL) {
        MotionState_InitAllSlavesNow(state, M2C_FIELD(init, void **, 0x34), 0xFF);
    }
    return 1;
}

void Motion_Release(s32 arg0) {
    MotionState_Release(arg0 + 0xB0);
    Thing_Release(arg0);
}

s32 Motion_Move(u8 *motion) {
    void MotionState_BuildMatricesWorld(void *mtx, void *pos, void *state, f32 scale);
    void MotionState_Heartbeat(void *state, void *arg1, void *thing);
    MotionState_BuildMatricesWorld(motion + 0x50, motion + 0x40, motion + 0xB0, 1.0f);
    MotionState_Heartbeat(motion + 0xB0, motion + 0x10, motion);
    return 1;
}

void Motion_Draw(u8 *motion) {
    extern u8 g_aMatStack[4096];
    void MotionState_Draw2(void *state, void *arg1, void *matStack);
    if (M2C_FIELD(motion, u32 *, 0xC0) != 0) {
        MotionState_Draw(motion + 0xB0);
    } else {
        MotionState_Draw2(motion + 0xB0, motion + 0x10, g_aMatStack);
    }
}

s32 Motion_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}
u8 g_aMatStack[4096];

void Thing_PreMove();
void Thing_GetMass();
void Thing_DynamicCollideImpact();
void Thing_GetCameraOrientationInfo();
void Thing_GetInterfaceInfo();
void Thing_SlaveTransform();
void Thing_GetSurfaceScript();
void Thing_GetNearestNavPoint();
void Thing_GetInverseLocalInertiaTensor();
void Thing_DynamicRotation();
void Thing_CollideWithRay();
void Thing_GetTargetPos();
void Thing_GetOrientedCollision();

void *g_motionVTable[] = {
    Motion_Init,
    Motion_Release,
    Thing_PreMove,
    Motion_Move,
    Motion_Draw,
    Thing_GetMass,
    Thing_DynamicCollideImpact,
    Thing_GetCameraOrientationInfo,
    Thing_GetInterfaceInfo,
    Thing_SlaveTransform,
    Thing_GetSurfaceScript,
    Thing_GetNearestNavPoint,
    Thing_GetInverseLocalInertiaTensor,
    Thing_DynamicRotation,
    Thing_CollideWithRay,
    Thing_GetTargetPos,
    Thing_GetOrientedCollision,
};
