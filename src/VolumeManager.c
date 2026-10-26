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

