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

