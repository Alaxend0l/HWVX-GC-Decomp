#include "common.h"

typedef struct Vector {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector;

typedef struct GfxPointLight {
    Vector position;
    f32 radius;
    GXColor color;
    u8 unk18[0x20 - 0x18];
} GfxPointLight;

typedef struct GfxState {
    GfxPointLight pointLights[3];
    Vector lightDir;
    GXColor lightColor;
    GXColor unk74;
    GXColor ambientColor;
    u32 unk7C;
    f32 unk80;
    s32 unk84;
    s32 unk88;
    u16 debugFont;
    s8 lightChanged;
    s8 unk8F;
    u8 numPointLights;
    u8 unk91;
    u8 unk92;
    u8 unk93;
    u8 enviroMapMode;
    u8 unk95[0x98 - 0x95];
    f32 farClippingPlane;
    u8 unk9C[0xA0 - 0x9C];
} GfxState;

extern GfxState g_state;

typedef struct GfxWindow {
    u8 unk0[0x180];
    Vector cameraPos;
    u8 unk190[0x2D0 - 0x190];
} GfxWindow;

f32 PSVECSquareMag(Vector *v);
void PSVECNormalize(Vector *src, Vector *dst);


void Gfx_Destroy(void) {
    M2C_UNK Gfx_DisplayList_Destroy();
    M2C_UNK Gfx_TextureHandler_Destroy();
    M2C_UNK Render_Destroy();
    M2C_UNK FontManager_Destroy();
    FontManager_Destroy();
    Gfx_DisplayList_Destroy();
    Gfx_TextureHandler_Destroy();
    Render_Destroy();
}

void Gfx_CreateView(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_EnableState(s32 arg0) {
    M2C_FIELD(&g_state, s32 *, 0x7C) = (s32) (M2C_FIELD(&g_state, s32 *, 0x7C) | arg0);
}

void Gfx_DisableState(s32 arg0) {
    M2C_FIELD(&g_state, s32 *, 0x7C) = (s32) (M2C_FIELD(&g_state, s32 *, 0x7C) & ~arg0);
}

void Gfx_SetEnviroMapMode(u8 mode) {
    g_state.enviroMapMode = mode;
}

void Gfx_SetFarClippingPlane(f32 farPlane) {
    g_state.farClippingPlane = farPlane;
}

void Gfx_RemovePointLights(void) {
    if ((u8) M2C_FIELD(&g_state, u8 *, 0x90) != 0) {
        M2C_FIELD(&g_state, u8 *, 0x90) = 0U;
        M2C_FIELD(&g_state, s8 *, 0x8E) = 1;
    }
}

void Gfx_AddPointLight(GXColor *color, Vector *pos, f32 radius) {
    if (g_state.numPointLights < 3) {
        g_state.pointLights[g_state.numPointLights].color = *color;
        g_state.pointLights[g_state.numPointLights].position = *pos;
        g_state.pointLights[g_state.numPointLights].radius = radius;
        g_state.lightChanged = 1;
        g_state.numPointLights++;
    }
}

void Gfx_SetParallelLight(GXColor *color, Vector *dir) {
    Vector *lightDir;

    g_state.lightColor = *color;
    if (dir != NULL) {
        lightDir = &g_state.lightDir;
        *lightDir = *dir;
        if (PSVECSquareMag(lightDir) != 0.0f) {
            PSVECNormalize(lightDir, lightDir);
            g_state.lightChanged = 1;
        }
    }
}

void Gfx_SetAmbientColor(GXColor *color) {
    g_state.ambientColor = *color;
}

void Gfx_SetBaseColor(GXColor *color) {
    g_state.unk74 = *color;
}

void Gfx_SetRenderWindow(s32 window) {
    extern s32 g_currentWindow;
    extern void *g_pWindow;
    extern u8 g_window[][0x2D0];

    g_currentWindow = window;
    g_pWindow = g_window[window];
}

s32 Gfx_GetRenderWindowIndex(void) {
    extern s32 g_currentWindow;
    return g_currentWindow;
}

GfxWindow *GetRenderWindow(void) {
    extern s32 g_currentWindow;
    extern GfxWindow g_window[];
    return &g_window[g_currentWindow];
}

void Gfx_ClearScreen(void)
{
    Render_ClearScreen();
}

