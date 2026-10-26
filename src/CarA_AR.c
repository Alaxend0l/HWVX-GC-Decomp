#include "common.h"


void Pod_Release(s32 arg0) {
    MotionState_Release(arg0 + 0xB0);
    Thing_Release(arg0);
}

s32 Pod_Invalidate(s32 arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = *(u32 *)(arg0 + 0x110);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

