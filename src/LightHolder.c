#include "common.h"


void LightHolder_Init(void) {
    extern u8 g_pLight[16][0x80];
    u32 i;

    for (i = 0; i < 16; i++) {
        *(void **)(g_pLight[i] + 0x60) = NULL;
    }
}

void LightHolder_SetTexture(u16 index) {
    void Gfx_SetTextureWrapMode(u16 id, s16 mode);
    extern u16 g_lightTextureIndex;
    g_lightTextureIndex = index;
    Gfx_SetTextureWrapMode(index, 1);
}

s32 LightHolder_Add(u8 *owner, Vector4 *pos, void *thing) {
    extern struct {
        Vector4 pos;
        u8 unk10[0x2C];
        u8 unk3C;
        u8 unk3D[0x13];
        u8 unk50;
        u8 unk51[0xF];
        u8 *owner;
        void *thing;
        u32 unk68;
        u32 life;
        u32 unk70;
        u32 unk74;
        f32 rate;
        u32 unk7C;
    } g_pLight[16];
    s32 i;

    for (i = 0; i < 16; i++) {
        if (g_pLight[i].owner == NULL) {
            g_pLight[i].owner = owner;
            g_pLight[i].pos = *pos;
            g_pLight[i].thing = thing;
            g_pLight[i].life = RandomAdd_s32(*(u32 *)(owner + 4), *(u32 *)(owner + 8));
            g_pLight[i].rate = 1.0f;
            if (*(u32 *)(owner + 4) != 0 && *(u32 *)(owner + 8) != 0) {
                g_pLight[i].rate = 1.0f / ((f32)g_pLight[i].life / (f32)*(u32 *)(owner + 4));
            }
            g_pLight[i].unk50 = 1;
            g_pLight[i].unk3C = 1;
            return i;
        }
    }
    return -1;
}

void LightHolder_Delete(s32 index) {
    extern struct {
        u8 unk0[0x60];
        void *owner;
        u8 unk64[0x1C];
    } g_pLight[16];
    if (index >= 0) {
        g_pLight[index].owner = NULL;
    }
}

