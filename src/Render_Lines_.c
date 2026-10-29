#include "common.h"

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct Line2D {
    f32 x0, y0, z0, w0; /* 0x00 */
    f32 x1, y1, z1, w1; /* 0x10 */
    u8 r, g, b, a;      /* 0x20 */
} Line2D;

static inline void GXPosition2f32(f32 x, f32 y) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
}

static inline void GXColor3u8(u8 r, u8 g, u8 b) {
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
}

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);
void GXBegin(s32 type, s32 vtxfmt, u16 nverts);

extern u32 m_VtxDescFlag;

void Set2DLineVtxDesc(void);

void Render_DrawLine2D(Line2D *line) {
    Set2DLineVtxDesc();
    GXBegin(0xA8, 0, 2);
    GXPosition2f32(line->x0, line->y0);
    GXColor3u8(line->r, line->g, line->b);
    GXPosition2f32(line->x1, line->y1);
    GXColor3u8(line->r, line->g, line->b);
}

void Set2DLineVtxDesc(void) {
    if (m_VtxDescFlag != 1) {
        m_VtxDescFlag = 1;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
    }
}

void Set3DLineVtxDesc(void) {
    if (m_VtxDescFlag != 4) {
        m_VtxDescFlag = 4;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
    }
}

