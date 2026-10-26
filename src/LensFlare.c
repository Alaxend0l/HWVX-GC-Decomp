#include "common.h"


void LensFlare_Init(void) {
    extern u8 g_pFlare[64][0x30];
    extern s32 g_activeFlares;
    extern s32 g_LensFlareRayTest;
    s32 i;

    for (i = 0; i < 64; i++) {
        *(s32 *)g_pFlare[i] = 0;
    }
    g_activeFlares = 0;
    g_LensFlareRayTest = 1;
}
