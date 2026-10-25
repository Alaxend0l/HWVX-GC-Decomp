#include "common.h"

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

typedef struct GfxState {
    u8 unk0[0x8C];
    u16 debugFont;
    u8 unk8E[0xA0 - 0x8E];
} GfxState;

extern FontManager g_fontManager;
extern GfxState g_state;

void Gfx_SetDebugFont(s16 font) {
    g_state.debugFont = font;
}
