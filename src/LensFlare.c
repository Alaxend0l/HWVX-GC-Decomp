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

s32 LensFlare_Add(void *flare, s32 arg1, Vector4 *pos) {
    typedef struct Flare {
        void *flare;
        s32 unk4;
        u8 unk8[0x10 - 0x8];
        Vector4 pos;
        u8 unk20;
        u8 unk21[0x30 - 0x21];
    } Flare;
    extern Flare g_pFlare[64];
    extern u32 g_activeFlares;
    u32 i;

    if (flare != NULL) {
        for (i = 0; i < 64; i++) {
            if (g_pFlare[i].flare == NULL) {
                g_pFlare[i].unk4 = arg1;
                g_pFlare[i].flare = flare;
                g_pFlare[i].unk20 = 0;
                if (pos != NULL) {
                    g_pFlare[i].pos = *pos;
                }
                break;
            }
        }
        if (i != 64) {
            if (i >= g_activeFlares) {
                g_activeFlares = i + 1;
            }
            return i;
        }
    }
    return -1;
}

void LensFlare_Delete(s32 arg0) {
    extern u8 g_pFlare[64][0x30];
    extern s32 g_activeFlares;
    s32 i;

    if (arg0 != -1) {
        g_activeFlares = 0;
        *(s32 *)g_pFlare[arg0] = 0;
        for (i = 0; i < 64; i++) {
            if (*(u32 *)g_pFlare[i] != 0) {
                g_activeFlares = i + 1;
            }
        }
    }
}

