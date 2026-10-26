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

void Gfx_SetEnviroMapMode(u8 mode) {
    g_state.enviroMapMode = mode;
}

void Gfx_SetFarClippingPlane(f32 farPlane) {
    g_state.farClippingPlane = farPlane;
}

void Gfx_SetAmbientColor(GXColor *color) {
    g_state.ambientColor = *color;
}

void Gfx_SetBaseColor(GXColor *color) {
    g_state.unk74 = *color;
}

