#include "common.h"

#pragma fp_contract off

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u8 pad4[0x80 - 0x4];
    u32 matIdxA;
    u32 matIdxB;
    u8 pad88[0xF8 - 0x88];
    u32 suScis0;
    u32 suScis1;
    u8 pad100[0x43C - 0x100];
    f32 vpLeft;
    f32 vpTop;
    f32 vpWd;
    f32 vpHt;
    f32 vpNearz;
    f32 vpFarz;
} GXData;


void GXProject(f32 x, f32 y, f32 z, f32 mtx[3][4], f32 *pm, f32 *vp, f32 *sx, f32 *sy, f32 *sz) {
    f32 peye_x;
    f32 peye_y;
    f32 peye_z;
    f32 xc;
    f32 yc;
    f32 zc;
    f32 wc;

    peye_x = mtx[0][3] + ((mtx[0][2] * z) + ((mtx[0][0] * x) + (mtx[0][1] * y)));
    peye_y = mtx[1][3] + ((mtx[1][2] * z) + ((mtx[1][0] * x) + (mtx[1][1] * y)));
    peye_z = mtx[2][3] + ((mtx[2][2] * z) + ((mtx[2][0] * x) + (mtx[2][1] * y)));
    if (pm[0] == 0.0f) {
        xc = (peye_x * pm[1]) + (peye_z * pm[2]);
        yc = (peye_y * pm[3]) + (peye_z * pm[4]);
        zc = pm[6] + (peye_z * pm[5]);
        wc = 1.0f / -peye_z;
    } else {
        xc = pm[2] + (peye_x * pm[1]);
        yc = pm[4] + (peye_y * pm[3]);
        zc = pm[6] + (peye_z * pm[5]);
        wc = 1.0f;
    }
    *sx = (vp[2] / 2.0f) + (vp[0] + (wc * (xc * vp[2] / 2.0f)));
    *sy = (vp[3] / 2.0f) + (vp[1] + (wc * (-yc * vp[3] / 2.0f)));
    *sz = vp[5] + (wc * (zc * (vp[5] - vp[4])));
}

void WriteMTXPS4x3(f32 mtx[3][4], volatile f32 *wgpipe);
#pragma peephole off
void GXLoadPosMtxImm(f32 mtx[3][4], u32 id) {
    u32 reg;
    GXWGFifo.u8 = 0x10;
    reg = (id * 4) | 0xB0000;
    GXWGFifo.u32 = reg;
    WriteMTXPS4x3(mtx, (volatile f32 *)0xCC008000);
}

void GXSetViewportJitter(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz, u32 field);
#pragma peephole off
void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz) {
    GXSetViewportJitter(left, top, wd, ht, nearz, farz, 1);
}
void GXGetViewportv(void *arg0) {
    extern void *gx;
    M2C_FIELD(arg0, f32 *, 0) = (f32) M2C_FIELD(gx, f32 *, 0x43C);
    M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(gx, f32 *, 0x440);
    M2C_FIELD(arg0, f32 *, 8) = (f32) M2C_FIELD(gx, f32 *, 0x444);
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) M2C_FIELD(gx, f32 *, 0x448);
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) M2C_FIELD(gx, f32 *, 0x44C);
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) M2C_FIELD(gx, f32 *, 0x450);
}

#pragma peephole off
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht) {
    extern GXData *gx;
    u32 tp, lf, bm, rt;

    tp = top + 342;
    lf = left + 342;
    bm = tp + ht - 1;
    rt = lf + wd - 1;
    gx->suScis0 = (gx->suScis0 & ~0x7FF) | tp;
    gx->suScis0 = (gx->suScis0 & ~0x7FF000) | (lf << 12);
    gx->suScis1 = (gx->suScis1 & ~0x7FF) | bm;
    gx->suScis1 = (gx->suScis1 & ~0x7FF000) | (rt << 12);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->suScis0;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->suScis1;
    gx->bpSentNot = 0;
}

void GXSetClipMode(u32 mode) {
    extern GXData *gx;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1005;
    GXWGFifo.u32 = mode;
    gx->bpSentNot = 1;
}

