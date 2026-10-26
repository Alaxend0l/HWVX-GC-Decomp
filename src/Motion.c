#include "common.h"


s32 Motion_Init(u8 *motion, u8 *init) {
    void MotionState_InitAllSlavesNow(void *motionState, void *slaves, s32 arg2);
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

