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

void LightHolder_Render(void) {
    extern u8 g_pLight[16][0x80];
    void Gfx_GetCameraPosition(Vector4 *pos);
    void Gfx_ScreenOverlay(GXColor *color, u8 mode);
    void DistanceXYZ(void *a, void *b, f32 *dist);
    Vector4 camera;
    f32 dist;
    GXColor color;
    u8 *light;
    u8 *pos;
    u8 *owner;
    u32 i;

    Gfx_GetCameraPosition(&camera);
    for (i = 0; i < 16; i++) {
        light = g_pLight[i];
        owner = *(u8 **)(light + 0x60);
        if (owner != NULL) {
            if (*(u8 **)(light + 0x64) != NULL) {
                pos = *(u8 **)(light + 0x64) + 0x40;
            } else {
                pos = light;
            }
            if (owner[0] != 0) {
                DistanceXYZ(&camera, pos, &dist);
                if (dist <= *(f32 *)(light + 0x40)) {
                    color = *(GXColor *)(light + 0x34);
                    Gfx_ScreenOverlay(&color, (*(u8 **)(light + 0x60))[3]);
                }
            }
        }
    }
}

void LightHolder_Heartbeat(void) {
    extern u8 g_pLight[16][0x80];
    extern u32 g_iDeltaTimeMS;
    extern f32 g_fDeltaTimeMS;
    void ExtractColorKey(void *keys, void *out, f32 time);
    void ExtractF32Key(void *keys, void *out, f32 time);
    u8 *light;
    u32 i;
    u32 delta;

    delta = g_iDeltaTimeMS;
    for (i = 0; i < 16; i++) {
        light = g_pLight[i];
        if (*(u8 **)(light + 0x60) != NULL) {
            if (*(u32 *)(light + 0x6C) == 0 || *(u32 *)(light + 0x6C) > delta) {
                if (*(u32 *)(light + 0x6C) != 0) {
                    *(u32 *)(light + 0x6C) -= delta;
                }
                ExtractColorKey(*(u8 **)(light + 0x60) + 0x14, light + 0x10, g_fDeltaTimeMS * *(f32 *)(light + 0x78));
                ExtractF32Key(*(u8 **)(light + 0x60) + 0xC, light + 0x40, g_fDeltaTimeMS * *(f32 *)(light + 0x78));
            } else {
                LightHolder_Delete(i);
            }
        }
    }
}

void LightHolder_SetRenderState(void *camera) {
    extern u8 g_pLight[16][0x80];
    u32 CompareDistances(void *arg0, void *arg1, f32 farg0);
    void Gfx_AddPointLight(GXColor *color, void *position, f32 radius);
    void Gfx_RemovePointLights(void);
    u8 *light;
    u32 i;
    u8 *pos;
    u8 *owner;
    GXColor color;

    Gfx_RemovePointLights();
    for (i = 0; i < 16; i++) {
        light = g_pLight[i];
        owner = *(u8 **)(light + 0x60);
        if (owner != NULL && owner[1] != 0) {
            if (*(u8 **)(light + 0x64) != NULL) {
                pos = *(u8 **)(light + 0x64) + 0x40;
            } else {
                pos = light;
            }
            if (CompareDistances(camera, pos, *(f32 *)(light + 0x40)) != 1) {
                color = *(GXColor *)(light + 0x34);
                Gfx_AddPointLight(&color, pos, *(f32 *)(light + 0x40));
            }
        }
    }
}

