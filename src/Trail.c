#include "common.h"


void Trail_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Trail_Init(void) {
    extern u8 g_TrialData[1000][0x60];
    extern s32 g_nextTrail;
    extern f32 g_trailTimer;
    s32 i;

    for (i = 0; i < 1000; i++) {
        *(s32 *)(g_TrialData[i] + 0x50) = 0;
    }
    g_nextTrail = 0;
    g_trailTimer = 0.0f;
}

