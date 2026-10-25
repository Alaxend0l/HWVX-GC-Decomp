#include "common.h"


void Drone_GetCameraOrientationInfo(void *drone, void *camera, void *info) {
    M2C_FIELD(info, s32 *, 0x88) = 1;
    M2C_FIELD(info, void **, 0x84) = (u8 *)drone + 0x154;
    M2C_FIELD(info, f32 *, 0x8C) = 1.0f;
    M2C_FIELD(info, f32 *, 0x30) = 0.0f;
    M2C_FIELD(info, f32 *, 0x34) = 1.0f;
    M2C_FIELD(info, f32 *, 0x38) = 0.0f;
}
