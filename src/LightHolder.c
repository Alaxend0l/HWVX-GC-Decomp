#include "common.h"

typedef struct LightHolderDef {
    u8 enabled;
    u8 pointLight;
    u8 unk2;
    u8 overlayMode;
    u32 lifeMin;
    u32 lifeRange;
    u8 radiusKeys[8];
    u8 colorKeys[8];
} LightHolderDef;

typedef struct LightHolderLight {
    Vector4 pos;
    u8 unk10[0x24];
    GXColor color;
    u8 unk38[4];
    u8 unk3C;
    u8 unk3D[3];
    f32 radius;
    u8 unk44[0xC];
    u8 unk50;
    u8 unk51[0xF];
    LightHolderDef *def;
    u8 *thing;
    u32 unk68;
    u32 life;
    u32 unk70;
    u32 unk74;
    f32 rate;
    u32 unk7C;
} LightHolderLight;


void LightHolder_Init(void) {
    extern LightHolderLight g_pLight[16];
    u32 i;

    for (i = 0; i < 16; i++) {
        g_pLight[i].def = NULL;
    }
}

void LightHolder_SetTexture(u16 index) {
    void Gfx_SetTextureWrapMode(u16 id, s16 mode);
    extern u16 g_lightTextureIndex;
    g_lightTextureIndex = index;
    Gfx_SetTextureWrapMode(index, 1);
}

s32 LightHolder_Add(LightHolderDef *def, Vector4 *pos, void *thing) {
    extern LightHolderLight g_pLight[16];
    s32 i;

    for (i = 0; i < 16; i++) {
        if (g_pLight[i].def == NULL) {
            g_pLight[i].def = def;
            g_pLight[i].pos = *pos;
            g_pLight[i].thing = thing;
            g_pLight[i].life = RandomAdd_s32(def->lifeMin, def->lifeRange);
            g_pLight[i].rate = 1.0f;
            if (def->lifeMin != 0 && def->lifeRange != 0) {
                g_pLight[i].rate = 1.0f / ((f32)g_pLight[i].life / (f32)def->lifeMin);
            }
            g_pLight[i].unk50 = 1;
            g_pLight[i].unk3C = 1;
            return i;
        }
    }
    return -1;
}

void LightHolder_Delete(s32 index) {
    extern LightHolderLight g_pLight[16];
    if (index >= 0) {
        g_pLight[index].def = NULL;
    }
}

void LightHolder_Render(void) {
    extern LightHolderLight g_pLight[16];
    void Gfx_GetCameraPosition(Vector4 *pos);
    void Gfx_ScreenOverlay(GXColor *color, u8 mode);
    void DistanceXYZ(void *a, void *b, f32 *dist);
    Vector4 camera __attribute__((aligned(16)));
    f32 dist;
    GXColor color;
    LightHolderLight *light;
    u8 *pos;
    LightHolderDef *def;
    u32 i;

    Gfx_GetCameraPosition(&camera);
    for (i = 0; i < 16; i++) {
        light = &g_pLight[i];
        def = light->def;
        if (def != NULL) {
            if (light->thing != NULL) {
                pos = light->thing + 0x40;
            } else {
                pos = (u8 *)light;
            }
            if (def->enabled != 0) {
                DistanceXYZ(&camera, pos, &dist);
                if (dist <= light->radius) {
                    color = light->color;
                    Gfx_ScreenOverlay(&color, light->def->overlayMode);
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

