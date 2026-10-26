#include "common.h"


void VolumeManager_Create(void) {
    extern s32 g_VolumeHolder;
    g_VolumeHolder = 0;
}

void VolumeManager_Release(void) {
    extern s32 g_VolumeHolder;
    void SetReverb(s32 a, s32 b, s32 c, s32 d);
    g_VolumeHolder = 0;
    SetReverb(0, 0, 0, 0);
    SetReverb(1, 0, 0, 0);
}

s32 VolumeManager_GetVolumeCellIndex(Vector4 *point) {
    extern u8 *g_VolumeHolder;
    u8 *holder = g_VolumeHolder;
    s32 x;
    s32 y;
    s32 z;

    if (holder == NULL) {
        return 0;
    }
    x = (point->x - M2C_FIELD(holder, f32 *, 0x0)) / M2C_FIELD(holder, f32 *, 0x20);
    y = (point->y - M2C_FIELD(holder, f32 *, 0x4)) / M2C_FIELD(holder, f32 *, 0x24);
    z = (point->z - M2C_FIELD(holder, f32 *, 0x8)) / M2C_FIELD(holder, f32 *, 0x28);
    if (x < 0 || x >= M2C_FIELD(holder, s32 *, 0x2C)) {
        return 0;
    }
    if (y < 0 || y >= M2C_FIELD(holder, s32 *, 0x30)) {
        return 0;
    }
    if (z < 0 || z >= M2C_FIELD(holder, s32 *, 0x34)) {
        return 0;
    }
    return M2C_FIELD(holder, s32 **, 0x14)[z * M2C_FIELD(holder, s32 *, 0x2C) + M2C_FIELD(holder, s32 *, 0x34) * (y * M2C_FIELD(holder, s32 *, 0x2C)) + x];
}

s32 VolumeManager_GetNumVolumesOfTypeByCell(s32 type, s32 cell) {
    extern s32 g_VolumeHolder;
    if ((void *)g_VolumeHolder != NULL) {
        return M2C_FIELD(M2C_FIELD(g_VolumeHolder, u8 **, 0x10) + cell * 0x38 + type * 8, s32 *, 0x0);
    }
    return 0;
}

