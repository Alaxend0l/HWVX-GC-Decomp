#include "common.h"

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe *)0xCC008000)

typedef struct Line2D {
    f32 x0, y0, z0, w0; /* 0x00 */
    f32 x1, y1, z1, w1; /* 0x10 */
    u8 r, g, b, a;      /* 0x20 */
} Line2D;

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);
void GXBegin(s32 type, s32 vtxfmt, u16 nverts);

extern u32 m_VtxDescFlag;

void Set2DLineVtxDesc(void);
