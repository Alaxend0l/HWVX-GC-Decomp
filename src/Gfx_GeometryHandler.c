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

typedef struct GDLDrawEntry {
    s32 geometry;
    GXColor unk4;
    s32 matrix;
    s32 lightState;
    u8 unk10;
    u8 unk11;
    u8 envmapState;
    u8 pad13;
} GDLDrawEntry;

typedef struct GDL {
    u8 unk0[0x54];
    s32 numDrawEntries;
    u8 unk58[0xDFCC0 - 0x58];
    GDLDrawEntry drawEntries[(0xE9CC0 - 0xDFCC0) / 0x14];
} GDL;

typedef struct GeometryGroup {
    u16 unk0;
    u16 flags;
    u8 unk4[0x10 - 0x4];
    void *data;
    u16 unk14;
    u8 unk16[0x18 - 0x16];
    u16 unk18;
    u8 unk1A[0x20 - 0x1A];
} GeometryGroup;

typedef struct Geometry {
    u16 unk0;
    u16 numGroups;
    u16 unk4;
    u16 unk6;
    GeometryGroup *groups;
    f32 *lodDistances;
    f32 bboxMin[4];
    f32 bboxMax[4];
    f32 center[4];
} Geometry;

extern GDL *g_pGDL;
extern GfxState g_state;
extern u8 g_CurrentViews;

void *GetRenderWindow();
void Gfx_SetRenderWindow(u32 view);
s32 Gfx_AddWorldMatrixState_CacheMatrix(u32 matrix);
s32 Gfx_AddWorldMatrixState_NonCached(u32 matrix);
s32 Gfx_AddWorldMatrixState_CacheMatrixQuaternionScalePos(s32 quat, s32 scale, s32 pos);
s32 Gfx_GetLightState(void);
s32 Gfx_GetEnvmapState(void);
void Gfx_EnableState(u32 state);
void XformVec(Vector *m, Vector *v, Vector *out);
f32 PSVECSquareMag(Vector *v);
void PSVECAdd(Vector *a, Vector *b, Vector *ab);
void PSVECScale(Vector *src, Vector *dst, f32 scale);
void PSVECSubtract(Vector *a, Vector *b, Vector *a_b);
f32 PSVECDotProduct(Vector *a, Vector *b);
f32 Sqrt(f32 x);

void Gfx_DrawGeometry(s32 geometry, s32 matrix) {
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].geometry = geometry;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].matrix = Gfx_AddWorldMatrixState_CacheMatrix(matrix);
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk10 = (g_state.unk7C >> 4) & 1;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk4 = g_state.unk74;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].lightState = Gfx_GetLightState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].envmapState = Gfx_GetEnvmapState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk11 = 0;
    g_pGDL->numDrawEntries++;
}

void Gfx_DrawGeometry_MatrixNotCached(s32 geometry, s32 matrix) {
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].geometry = geometry;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].matrix = Gfx_AddWorldMatrixState_NonCached(matrix);
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk10 = (g_state.unk7C >> 4) & 1;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk4 = g_state.unk74;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].lightState = Gfx_GetLightState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].envmapState = Gfx_GetEnvmapState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk11 = 0;
    g_pGDL->numDrawEntries++;
}

