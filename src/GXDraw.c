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

void GXDrawSphere(u8 numMajor, u8 numMinor) {
    s32 ttype;
    f32 radius = 1.0f;
    f32 majorStep = 3.1415927f / numMajor;
    f32 minorStep = 2.0f * 3.1415927f / numMinor;
    s32 i, j;
    f32 a, b, r0, r1, z0, z1, c, x, y;

    GXGetVtxDesc(0xD, &ttype);
    GXGetVtxDescv(vcd);
    GXGetVtxAttrFmtv(3, vat);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xA, 1);
    GXSetVtxAttrFmt(3, 9, 1, 4, 0);
    GXSetVtxAttrFmt(3, 0xA, 0, 4, 0);
    if (ttype != 0) {
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(3, 0xD, 1, 4, 0);
    }
    for (i = 0; i < numMajor; i++) {
        a = i * majorStep;
        b = a + majorStep;
        r0 = radius * sinf(a);
        r1 = radius * sinf(b);
        z0 = radius * cosf(a);
        z1 = radius * cosf(b);
        GXBegin(0x98, 3, (numMinor + 1) * 2);
        for (j = 0; j <= numMinor; j++) {
            c = j * minorStep;
            x = cosf(c);
            y = sinf(c);
            GXWGFifo.f32 = x * r1;
            GXWGFifo.f32 = y * r1;
            GXWGFifo.f32 = z1;
            GXWGFifo.f32 = (x * r1) / radius;
            GXWGFifo.f32 = (y * r1) / radius;
            GXWGFifo.f32 = z1 / radius;
            if (ttype != 0) {
                GXWGFifo.f32 = (f32)j / (f32)numMinor;
                GXWGFifo.f32 = (f32)(i + 1) / (f32)numMajor;
            }
            GXWGFifo.f32 = x * r0;
            GXWGFifo.f32 = y * r0;
            GXWGFifo.f32 = z0;
            GXWGFifo.f32 = (x * r0) / radius;
            GXWGFifo.f32 = (y * r0) / radius;
            GXWGFifo.f32 = z0 / radius;
            if (ttype != 0) {
                GXWGFifo.f32 = (f32)j / (f32)numMinor;
                GXWGFifo.f32 = (f32)i / (f32)numMajor;
            }
        }
    }
    GXSetVtxDescv(vcd);
    GXSetVtxAttrFmtv(3, vat);
}
