#include "common.h"

#pragma use_lmw_stmw on

typedef struct FontChar {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    s8 spacingX;
    s8 spacingY;
    u8 unk12[0x14 - 0x12];
} FontChar;

typedef struct Font {
    f32 height;
    f32 width;
    char *letters;
    u16 texture;
    u8 caseMode;
    u8 unkF;
    s8 inUse;
    u8 unk11;
    u16 refCount;
    FontChar chars[128];
} Font;

typedef struct FontManager {
    u32 numFonts;
    Font fonts[16];
} FontManager;

typedef struct GDLRectVtx {
    f32 x;
    f32 y;
    f32 z;
    u8 unkC[0x10 - 0xC];
    f32 s;
    f32 t;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u8 unk1C[0x20 - 0x1C];
} GDLRectVtx;

typedef struct GDLRect {
    GDLRectVtx v[4];
    u16 tex;
    u8 blendMode;
    u8 window;
} GDLRect;

typedef struct GfxState {
    u8 unk0[0x8C];
    u16 debugFont;
    u8 unk8E[0xA0 - 0x8E];
} GfxState;

extern FontManager g_fontManager;
extern GfxState g_state;

unsigned long strlen(const char *s);

void FontManager_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void FontManager_Destroy(void) {
}

void Gfx_FontManager_Init(void) {
    u32 i;

    g_state.debugFont = 0;
    g_fontManager.numFonts = 0;
    for (i = 0; i < 16; i++) {
        g_fontManager.fonts[i].inUse = 0;
    }
}

void Gfx_SetDebugFont(s16 font) {
    g_state.debugFont = font;
}

s32 Gfx_FontManager_ReleaseFont(s32 id) {
    Font *font = &g_fontManager.fonts[id];
    s32 ret = 0;

    if (!font->inUse) {
        return 1;
    }
    if (--font->refCount == 0) {
        font->inUse = 0;
        g_fontManager.numFonts--;
        ret = 1;
    }
    return ret;
}

u32 Gfx_FontManager_Print(char *str, u32 id, u8 blendMode, f32 x, f32 y, f32 spacing) {
    extern GXColor g_textcolor;
    GDLRect *Gfx_Add2DRectangle(void);
    u32 i;
    Font *font = &g_fontManager.fonts[id];
    GXColor color = g_textcolor;
    u8 c;
    GDLRect *rect;

    if (id == 0 && g_fontManager.numFonts == 0) {
        return 0;
    }
    for (i = 0; i < strlen(str); i++) {
        c = str[i];
        if (font->caseMode == 1 || font->caseMode == 3) {
            if (c >= 'a' && c <= 'z') {
                c -= 0x20;
            }
        }
        if (c != ' ') {
            f32 x1 = x + (u8)font->chars[c].spacingX;
            f32 y1 = y + font->height;
            rect = Gfx_Add2DRectangle();
            rect->blendMode = blendMode;
            rect->tex = font->texture;
            *(GXColor *)&rect->v[0].r = color;
            *(GXColor *)&rect->v[1].r = color;
            *(GXColor *)&rect->v[2].r = color;
            *(GXColor *)&rect->v[3].r = color;
            rect->v[0].s = font->chars[c].unk0;
            rect->v[0].t = font->chars[c].unk4;
            rect->v[1].s = font->chars[c].unk0;
            rect->v[1].t = font->chars[c].unkC;
            rect->v[2].s = font->chars[c].unk8;
            rect->v[2].t = font->chars[c].unkC;
            rect->v[3].s = font->chars[c].unk8;
            rect->v[3].t = font->chars[c].unk4;
            rect->v[0].x = x;
            rect->v[0].y = y;
            rect->v[1].x = x;
            rect->v[1].y = y1;
            rect->v[2].x = x1;
            rect->v[2].y = y1;
            rect->v[3].x = x1;
            rect->v[3].y = y;
        }
        x += (u8)font->chars[c].spacingX + spacing;
    }
    return g_fontManager.fonts[id].height;
}

void Gfx_FontManager_PrintScale(char *str, s32 id, f32 *scale, u8 blendMode, f32 x, f32 y, f32 spacing) {
    extern GXColor g_textcolor;
    GDLRect *Gfx_Add2DRectangle(void);
    u32 i;
    Font *font = &g_fontManager.fonts[id];
    GXColor color = g_textcolor;
    u8 c;
    GDLRect *rect;

    for (i = 0; i < strlen(str); i++) {
        c = str[i];
        if (font->caseMode == 1 || font->caseMode == 3) {
            if (c >= 'a' && c <= 'z') {
                c -= 0x20;
            }
        }
        if (c != ' ') {
            f32 x1 = (u8)font->chars[c].spacingX * scale[0] + x;
            f32 y1 = font->height * scale[1] + y;
            rect = Gfx_Add2DRectangle();
            rect->blendMode = blendMode;
            rect->tex = font->texture;
            *(GXColor *)&rect->v[0].r = color;
            *(GXColor *)&rect->v[1].r = color;
            *(GXColor *)&rect->v[2].r = color;
            *(GXColor *)&rect->v[3].r = color;
            rect->v[0].s = font->chars[c].unk0;
            rect->v[0].t = font->chars[c].unk4;
            rect->v[1].s = font->chars[c].unk0;
            rect->v[1].t = font->chars[c].unkC;
            rect->v[2].s = font->chars[c].unk8;
            rect->v[2].t = font->chars[c].unkC;
            rect->v[3].s = font->chars[c].unk8;
            rect->v[3].t = font->chars[c].unk4;
            rect->v[0].x = x;
            rect->v[0].y = y;
            rect->v[1].x = x;
            rect->v[1].y = y1;
            rect->v[2].x = x1;
            rect->v[2].y = y1;
            rect->v[3].x = x1;
            rect->v[3].y = y;
            rect->v[0].z = 0.0f;
            rect->v[1].z = 0.0f;
            rect->v[2].z = 0.0f;
            rect->v[3].z = 0.0f;
        }
        x += scale[0] * ((u8)font->chars[c].spacingX + spacing);
    }
}

unsigned long strlen(const char *s);
void Gfx_FontManager_GetPrintRect(char *str, s32 id, f32 *width, f32 *height, f32 spacing) {
    Font *font = &g_fontManager.fonts[id];
    u32 w = 0;
    u32 i = 0;
    u32 len = strlen(str);
    u8 c;

    while (i < len) {
        c = str[i++];
        if (font->caseMode == 1 || font->caseMode == 3) {
            if (c >= 'a' && c <= 'z') {
                c -= 0x20;
            }
        }
        w += (u8)font->chars[c].spacingX + spacing;
    }
    *width = (u32)(w - spacing);
    *height = font->height;
}

