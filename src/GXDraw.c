#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXVtxDescList {
    s32 attr;
    s32 type;
} GXVtxDescList;

typedef struct GXVtxAttrFmtList {
    s32 attr;
    s32 cnt;
    s32 type;
    u8 frac;
} GXVtxAttrFmtList;

static GXVtxDescList vcd[27];
static GXVtxAttrFmtList vat[27];

void GXGetVtxDesc(s32 attr, s32 *type);
void GXGetVtxDescv(GXVtxDescList *vcd);
void GXGetVtxAttrFmtv(s32 fmt, GXVtxAttrFmtList *vat);
void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);
void GXSetVtxDescv(GXVtxDescList *vcd);
void GXSetVtxAttrFmtv(s32 fmt, GXVtxAttrFmtList *vat);
void GXBegin(u8 type, u8 vtxfmt, u16 nverts);
f32 sinf(f32 x);
f32 cosf(f32 x);
