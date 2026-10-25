#include "common.h"

typedef struct Vector4 { f32 x; f32 y; f32 z; f32 w; } Vector4;


void CarA_Release(u8 *car) {
    typedef struct CarNavPath {
        u8 pad[0x568];
        u8 *path[7];
    } CarNavPath;
    u32 GameSettings_GetCodeVariable(s32, s32);
    u8 *NavHolder_GetNavPoint(s32 index);
    void Trigger_VehicleKilled(void *car);
    void OConvexHullHolder_Release(void *hull);
    void SoundSource_Stop(s32 index);
    void MotionState_Release(void *state);
    void Thing_Release(void *thing);
    u16 i;

    if (GameSettings_GetCodeVariable(0x12D, 0) >= 3 && *(u8 **)(car + 0x94) != NULL) {
        *(s16 *)(*(u8 **)(car + 0x94) + 0xAC) = *(s32 *)(car + 0x3B0);
        for (i = 0; i < 6; i++) {
            if (((CarNavPath *)car)->path[i] == NULL || ((CarNavPath *)car)->path[i + 1] == NULL) {
                break;
            }
            if (NavHolder_GetNavPoint(*(s32 *)(car + 0x3B0)) == ((CarNavPath *)car)->path[i]) {
                *(s16 *)(*(u8 **)(car + 0x94) + 0xAE) = *(s32 *)(((u8 **)(car + 0x568))[i + 1] + 0x28);
                break;
            }
        }
    }
    Trigger_VehicleKilled(car);
    OConvexHullHolder_Release(*(void **)(car + 0x370));
    *(void **)(car + 0x370) = NULL;
    if (*(u32 *)(car + 0x4F0) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F0));
    }
    if (*(u32 *)(car + 0x4F4) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F4));
    }
    if (*(u32 *)(car + 0x4F8) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F8));
    }
    *(s32 *)(car + 0x4F0) = -1;
    *(s32 *)(car + 0x4F4) = -1;
    *(s32 *)(car + 0x4F8) = -1;
    MotionState_Release(car + 0x330);
    Thing_Release(car);
}

void CarA_GetInverseLocalInertiaTensor(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

