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

void FlashHolder_Heartbeat(void) {
    extern u8 g_pFlash[16][0x80];
    extern u32 g_iDeltaTimeMS;
    extern f32 g_fDeltaTimeMS;
    void ExtractColorKey(void *key, void *out, f32 dt);
    void ExtractPointKey(void *key, void *out, f32 dt);
    u8 *flash;
    u32 i;
    u32 delta;

    delta = g_iDeltaTimeMS;
    for (i = 0; i < 16; i++) {
        flash = g_pFlash[i];
        if (*(u8 **)(flash + 0x5C) != NULL) {
            if (*(u32 *)(flash + 0x68) == 0 || *(u32 *)(flash + 0x68) > delta) {
                if (*(u32 *)(flash + 0x68) != 0) {
                    *(u32 *)(flash + 0x68) -= delta;
                }
                ExtractColorKey(*(u8 **)(*(u8 **)(flash + 0x5C) + 0x20) + 4, flash + 0x10, g_fDeltaTimeMS);
                ExtractPointKey(*(u8 **)(flash + 0x5C) + 0x2C, flash + 0x40, g_fDeltaTimeMS);
                if (**(u8 **)(*(u8 **)(flash + 0x5C) + 0x20) == 5) {
                    flash[0x37] = flash[0x37] * *(f32 *)(flash + 0x74);
                } else {
                    flash[0x34] = flash[0x34] * *(f32 *)(flash + 0x74);
                    flash[0x35] = flash[0x35] * *(f32 *)(flash + 0x74);
                    flash[0x36] = flash[0x36] * *(f32 *)(flash + 0x74);
                }
            } else {
                FlashHolder_Remove(i);
            }
        }
    }
}

void FlashHolder_Render(void) {
    extern u8 g_pFlash[16][0x80];
    void Gfx_GetCameraPosition(Vector4 *pos);
    void Gfx_ScreenOverlay(GXColor *color, u8 mode);
    Vector4 camera;
    GXColor color;
    u8 *flash;
    u8 *owner;
    u32 i;

    Gfx_GetCameraPosition(&camera);
    for (i = 0; i < 16; i++) {
        flash = g_pFlash[i];
        owner = *(u8 **)(flash + 0x5C);
        if (owner != NULL && owner[1] != 0) {
            color = *(GXColor *)(flash + 0x34);
            Gfx_ScreenOverlay(&color, **(u8 **)(owner + 0x20));
        }
    }
}

