#include "common.h"

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s16 s16;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct {
    u32 dummy[8];
} GXTexObj;

void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrap_s, s32 wrap_t, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
void GXLoadTexObj(GXTexObj *obj, s32 id);
void GXBegin(s32 type, s32 vtxfmt, u16 nverts);

void GXSetZMode(int, int, int);
void GXSetBlendMode(int, int, int, int);
void GXSetNumTexGens(int);
void GXSetNumChans(int);
void GXSetNumTevStages(int);
void GXSetTevOrder(int, int, int, int);
void GXSetTevOp(int, int);
void GXSetTevSwapMode(int, int, int);
void GXSetTevSwapModeTable(int, int, int, int, int);

typedef struct GXRenderModeObj {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
} GXRenderModeObj;

void GXSetPixelFmt(int, int);
void C_MTXOrtho(f32 m[4][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void GXSetProjection(f32 mtx[4][4], int type);
void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
void PSMTXIdentity(f32 m[3][4]);
void GXLoadPosMtxImm(f32 mtx[3][4], u32 id);
void GXSetCurrentMtx(u32 id);
void GXSetColorUpdate(int);
void GXSetAlphaUpdate(int);
void GXSetDispCopyGamma(int);
void GXSetTexCoordGen2(int, int, int, u32, int, u32);
void GXInvalidateTexAll(void);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int, int);
void GXSetVtxAttrFmt(int, int, int, int, int);
void GXSetTevColorIn(int, int, int, int, int);
void GXSetTevColorOp(int, int, int, int, int, int);
void GXSetTevAlphaIn(int, int, int, int, int);
void GXSetTevAlphaOp(int, int, int, int, int, int);
void GXSetTevKColorSel(int, int);
void GXSetTevKAlphaSel(int, int);
void GXSetTevColorS10(int, GXColorS10);
void GXSetTevKColor(int, GXColor);
void THPGXRestore(void) {
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0xF);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
}

void THPGXYuv2RgbSetup(GXRenderModeObj *rmode) {
    s32 scrWidth;
    s32 scrHeight;
    f32 pMtx[4][4];
    f32 mMtx[3][4];

    scrWidth = rmode->fbWidth;
    scrHeight = rmode->efbHeight;
    GXSetPixelFmt(0, 0);
    C_MTXOrtho(pMtx, 0.0f, scrHeight, 0.0f, scrWidth, 0.0f, -1.0f);
    GXSetProjection(pMtx, 1);
    GXSetViewport(0.0f, 0.0f, scrWidth, scrHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, scrWidth, scrHeight);
    PSMTXIdentity(mMtx);
    GXLoadPosMtxImm(mMtx, 0);
    GXSetCurrentMtx(0);
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(0);
    GXSetDispCopyGamma(0);
    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(1, 1, 4, 0x3C, 0, 0x7D);
    GXInvalidateTexAll();
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(7, 9, 1, 3, 0);
    GXSetVtxAttrFmt(7, 0xD, 1, 2, 0);
    GXSetNumTevStages(4);
    GXSetTevOrder(0, 1, 1, 0xFF);
    GXSetTevColorIn(0, 0xF, 8, 0xE, 2);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaIn(0, 7, 4, 6, 1);
    GXSetTevAlphaOp(0, 1, 0, 0, 0, 0);
    GXSetTevKColorSel(0, 0xC);
    GXSetTevKAlphaSel(0, 0x1C);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevOrder(1, 1, 2, 0xFF);
    GXSetTevColorIn(1, 0xF, 8, 0xE, 0);
    GXSetTevColorOp(1, 0, 0, 1, 0, 0);
    GXSetTevAlphaIn(1, 7, 4, 6, 0);
    GXSetTevAlphaOp(1, 1, 0, 0, 0, 0);
    GXSetTevKColorSel(1, 0xD);
    GXSetTevKAlphaSel(1, 0x1D);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevOrder(2, 0, 0, 0xFF);
    GXSetTevColorIn(2, 0xF, 8, 0xC, 0);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 4, 7, 7, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevOrder(3, 0xFF, 0xFF, 0xFF);
    GXSetTevColorIn(3, 1, 0, 0xE, 0xF);
    GXSetTevColorOp(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(3, 7, 7, 7, 7);
    GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevKColorSel(3, 0xE);
    GXSetTevColorS10(1, (GXColorS10){ -90, 0, -114, 135 });
    GXSetTevKColor(0, (GXColor){ 0, 0, 226, 88 });
    GXSetTevKColor(1, (GXColor){ 179, 0, 0, 182 });
    GXSetTevKColor(2, (GXColor){ 255, 0, 255, 128 });
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
}

#pragma use_lmw_stmw on
void THPGXYuv2RgbDraw(u32 *yImage, u32 *uImage, u32 *vImage, s16 x, s16 y, s16 textureWidth, s16 textureHeight, s16 polygonWidth, s16 polygonHeight) {
    GXTexObj tobj0;
    GXTexObj tobj1;
    GXTexObj tobj2;

    GXInitTexObj(&tobj0, yImage, (u16)textureWidth, (u16)textureHeight, 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj0, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj0, 0);
    GXInitTexObj(&tobj1, uImage, (u16)(textureWidth >> 1), (u16)(textureHeight >> 1), 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj1, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj1, 1);
    GXInitTexObj(&tobj2, vImage, (u16)(textureWidth >> 1), (u16)(textureHeight >> 1), 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj2, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj2, 2);
    GXBegin(0x80, 7, 4);
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 1;
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 1;
}
#pragma use_lmw_stmw off
