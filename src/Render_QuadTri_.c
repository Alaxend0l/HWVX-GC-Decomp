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

typedef struct GDL {
    u8 unk0[0x68];
    u32 numPointSprites;
    u8 unk6C[0xEDCC0 - 0x6C];
    GDLPointSprite pointSprites[1];
} GDL;

extern GDL *g_pGDL;

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
void Render_StateBlendMode(u8 mode);
void Render_StateAlphaBlendEnable(s32 enable);
void SetTriVtxDesc(void);

extern GXColor g_textcolor;
void Gfx_SetTextColor(u8 r, u8 g, u8 b, u8 a) {
    g_textcolor.r = r;
    g_textcolor.g = g;
    g_textcolor.b = b;
    g_textcolor.a = a;
}
