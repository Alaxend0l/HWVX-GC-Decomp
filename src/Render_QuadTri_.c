#include "common.h"

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GDLPointSprite {
    f32 x, y, z; /* 0x00 */
    f32 size;    /* 0x0C */
    u32 color;   /* 0x10 */
    u16 tex;     /* 0x14 */
    u8 blendMode; /* 0x16 */
    u8 unk17;
} GDLPointSprite;

typedef struct GDLTriVtx {
    f32 x, y, z; /* 0x00 */
    f32 w;       /* 0x0C */
    f32 s, t;    /* 0x10 */
    GXColor color; /* 0x18 */
    u8 unk1C[0x20 - 0x1C];
} GDLTriVtx;

typedef struct GDLTri {
    GDLTriVtx v[3]; /* 0x00 */
    u16 tex;        /* 0x60 */
    s16 blendMode;  /* 0x62 */
} GDLTri;

typedef struct GDLSprite {
    Vector4 pos;
    u8 unk10[0x1C - 0x10];
    f32 rot;
    u32 next;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u8 unk28[2];
    u8 blendMode;
    u8 unk2B[0x30 - 0x2B];
} GDLSprite;

typedef struct GDL {
    u8 unk0[0x8];
    u32 numSprites;
    u8 unkC[0x68 - 0xC];
    u32 numPointSprites;
    u8 unk6C[0x215C0 - 0x6C];
    GDLSprite sprites[0x7CF];
    u8 unk38C90[0x38CC0 - 0x38C90];
    GDLTri tris[2000];
    u8 unk69A00[0xEDCC0 - 0x69A00];
    GDLPointSprite pointSprites[1000];
    u8 unkF1B40[0x24CDF4 - 0xF1B40];
    u16 spriteHeads[0x1FF];
} GDL;

extern GDL *g_pGDL;
extern s32 g_debugTriCount;
void GXSetCurrentMtx(u32 id);
void SinCos(f32 *s, f32 *c, f32 angle);
void Set3DRectVtxDesc(void);
void XformVec4x4(Matrix4 *m, Vector4 *v, Vector4 *out);

static inline void GXPosition3f32(f32 x, f32 y, f32 z) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

static inline void GXColor4u8(const u8 r, const u8 g, const u8 b, const u8 a) {
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
    GXWGFifo.u8 = a;
}

static inline void GXTexCoord2f32(f32 s, f32 t) {
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}

void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
void GXSetPointSize(u8 size, s32 fmt);
void Render_SetTextureHandle(s32 handle);
void Render_StateBlendMode(s32 mode);
void Render_StateAlphaBlendEnable(s32 enable);
void SetTriVtxDesc(void);

typedef struct GfxState {
    u8 unk0[0x84];
    s32 unk84;
    u8 unk88[0x8C - 0x88];
    u16 debugFont;
    u8 unk8E[0xA0 - 0x8E];
} GfxState;

extern GfxState g_state;
extern u8 g_screenText[0xA00];
u32 FontManager_Print(char *str, f32 x, f32 y, s32 id);

extern GXColor g_textcolor;

typedef struct GfxWindow {
    u8 unk0[0x2B0];
    u32 unk2B0;
    u8 unk2B4[0x2B8 - 0x2B4];
    u32 unk2B8;
    u8 unk2BC[0x2C0 - 0x2BC];
    GXColor overlayColor;
    u32 overlayMode;
    u8 unk2C8[0x2CC - 0x2C8];
    f32 unk2CC;
} GfxWindow;

GfxWindow *GetRenderWindow(void);
void Render_StateZWriteEnable(s32 enable);
void Set2DRectFlatVtxDesc(void);

static inline void GXPosition2f32(f32 x, f32 y) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
}
#pragma push
#pragma use_lmw_stmw on
void Render_Print(void) {
    f32 y;
    s32 i;
    s32 j;
    char buf[88];

    y = 0.0f;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 80; j++) {
            buf[j] = g_screenText[i * 80 + j];
        }
        buf[j] = 0;
        y += FontManager_Print(buf, 0.0f, y, g_state.debugFont);
    }
}
#pragma pop

void Render_DrawPointSprites(void) {
    u32 i;
    u32 color;

    SetTriVtxDesc();
    Render_StateAlphaBlendEnable(1);
    for (i = 0; i < g_pGDL->numPointSprites; i++) {
        Render_SetTextureHandle(g_pGDL->pointSprites[i].tex);
        Render_StateBlendMode(g_pGDL->pointSprites[i].blendMode);
        GXSetPointSize(255.0f * g_pGDL->pointSprites[i].size, 5);
        GXBegin(0xB8, 0, 1);
        GXPosition3f32(g_pGDL->pointSprites[i].x, g_pGDL->pointSprites[i].y, g_pGDL->pointSprites[i].z);
        color = g_pGDL->pointSprites[i].color;
        GXColor4u8(color >> 24, color >> 16, color >> 8, color);
        GXTexCoord2f32(0.0f, 0.0f);
    }
    Render_StateAlphaBlendEnable(0);
}

void Render_ScreenFlash(void) {
    f32 w;
    f32 zero = 0.0f;

    if (GetRenderWindow()->overlayMode != 0) {
        Render_StateZWriteEnable(0);
        Render_StateAlphaBlendEnable(1);
        Render_StateBlendMode(GetRenderWindow()->overlayMode);
        Render_SetTextureHandle(0xFFFF);
        Set2DRectFlatVtxDesc();
        GXBegin(0x80, 0, 4);
        GXPosition2f32(zero, zero);
        GXColor4u8(GetRenderWindow()->overlayColor.r, GetRenderWindow()->overlayColor.g,
                   GetRenderWindow()->overlayColor.b, GetRenderWindow()->overlayColor.a);
        GXPosition2f32(zero, GetRenderWindow()->unk2B8);
        GXColor4u8(GetRenderWindow()->overlayColor.r, GetRenderWindow()->overlayColor.g,
                   GetRenderWindow()->overlayColor.b, GetRenderWindow()->overlayColor.a);
        w = GetRenderWindow()->unk2B0;
        GXPosition2f32(w, GetRenderWindow()->unk2B8);
        GXColor4u8(GetRenderWindow()->overlayColor.r, GetRenderWindow()->overlayColor.g,
                   GetRenderWindow()->overlayColor.b, GetRenderWindow()->overlayColor.a);
        GXPosition2f32(GetRenderWindow()->unk2B0, zero);
        GXColor4u8(GetRenderWindow()->overlayColor.r, GetRenderWindow()->overlayColor.g,
                   GetRenderWindow()->overlayColor.b, GetRenderWindow()->overlayColor.a);
        Render_StateZWriteEnable(1);
        Render_StateAlphaBlendEnable(0);
    }
}

extern s32 g_debugTriCount;
void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
void SetTriFlatVtxDesc(void);
void Render_DrawAlphaTriangle(u16 index) {
    g_debugTriCount++;
    Render_StateBlendMode(g_pGDL->tris[index].blendMode);
    Render_SetTextureHandle(g_pGDL->tris[index].tex);
    Gfx_SetCameraMatrix(NULL, NULL, NULL, NULL);
    if (g_state.unk84 == 0xFFFF) {
        SetTriFlatVtxDesc();
        GXBegin(0x90, 0, 3);
        GXPosition3f32(g_pGDL->tris[index].v[0].x, g_pGDL->tris[index].v[0].y, g_pGDL->tris[index].v[0].z);
        GXColor4u8(g_pGDL->tris[index].v[0].color.r, g_pGDL->tris[index].v[0].color.g, g_pGDL->tris[index].v[0].color.b, g_pGDL->tris[index].v[0].color.a);
        GXPosition3f32(g_pGDL->tris[index].v[1].x, g_pGDL->tris[index].v[1].y, g_pGDL->tris[index].v[1].z);
        GXColor4u8(g_pGDL->tris[index].v[1].color.r, g_pGDL->tris[index].v[1].color.g, g_pGDL->tris[index].v[1].color.b, g_pGDL->tris[index].v[1].color.a);
        GXPosition3f32(g_pGDL->tris[index].v[2].x, g_pGDL->tris[index].v[2].y, g_pGDL->tris[index].v[2].z);
        GXColor4u8(g_pGDL->tris[index].v[2].color.r, g_pGDL->tris[index].v[2].color.g, g_pGDL->tris[index].v[2].color.b, g_pGDL->tris[index].v[2].color.a);
    } else {
        SetTriVtxDesc();
        GXBegin(0x90, 0, 3);
        GXPosition3f32(g_pGDL->tris[index].v[0].x, g_pGDL->tris[index].v[0].y, g_pGDL->tris[index].v[0].z);
        GXColor4u8(g_pGDL->tris[index].v[0].color.r, g_pGDL->tris[index].v[0].color.g, g_pGDL->tris[index].v[0].color.b, g_pGDL->tris[index].v[0].color.a);
        GXTexCoord2f32(g_pGDL->tris[index].v[0].s, g_pGDL->tris[index].v[0].t);
        GXPosition3f32(g_pGDL->tris[index].v[1].x, g_pGDL->tris[index].v[1].y, g_pGDL->tris[index].v[1].z);
        GXColor4u8(g_pGDL->tris[index].v[1].color.r, g_pGDL->tris[index].v[1].color.g, g_pGDL->tris[index].v[1].color.b, g_pGDL->tris[index].v[1].color.a);
        GXTexCoord2f32(g_pGDL->tris[index].v[1].s, g_pGDL->tris[index].v[1].t);
        GXPosition3f32(g_pGDL->tris[index].v[2].x, g_pGDL->tris[index].v[2].y, g_pGDL->tris[index].v[2].z);
        GXColor4u8(g_pGDL->tris[index].v[2].color.r, g_pGDL->tris[index].v[2].color.g, g_pGDL->tris[index].v[2].color.b, g_pGDL->tris[index].v[2].color.a);
        GXTexCoord2f32(g_pGDL->tris[index].v[2].s, g_pGDL->tris[index].v[2].t);
    }
}

void Gfx_SetTextColor(u8 r, u8 g, u8 b, u8 a) {
    g_textcolor.r = r;
    g_textcolor.g = g;
    g_textcolor.b = b;
    g_textcolor.a = a;
}
