#include "common.h"


void FlashHolder_Init(void) {
    extern u8 g_pFlash[16][0x80];
    u32 i;

    for (i = 0; i < 16; i++) {
        *(void **)(g_pFlash[i] + 0x5C) = NULL;
    }
}

s32 FlashHolder_Add(void *flash, Vector4 *pos, s32 arg2, f32 time) {
    typedef struct Flash {
        Vector4 pos;
        u8 unk10[0x3C - 0x10];
        u8 active;
        u8 unk3D[0x58 - 0x3D];
        u8 visible;
        u8 unk59[0x5C - 0x59];
        void *flash;
        s32 unk60;
        u8 unk64[0x68 - 0x64];
        s32 unk68;
        u8 unk6C[0x74 - 0x6C];
        f32 time;
        u8 unk78[0x80 - 0x78];
    } Flash;
    extern Flash g_pFlash[16];
    s32 i;

    for (i = 0; i < 16; i++) {
        if (g_pFlash[i].flash == NULL) {
            g_pFlash[i].flash = flash;
            g_pFlash[i].pos = *pos;
            g_pFlash[i].unk60 = arg2;
            g_pFlash[i].unk68 = M2C_FIELD(flash, s32 *, 0xC);
            g_pFlash[i].active = 1;
            g_pFlash[i].visible = 1;
            g_pFlash[i].time = time;
            return i;
        }
    }
    return -1;
}

