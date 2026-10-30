#include "common.h"

typedef struct GXTlutRegion {
    u32 unk0[4];
} GXTlutRegion;

typedef struct GXTexRegion {
    u32 unk0[4];
} GXTexRegion;

typedef struct GXTexObj GXTexObj;

typedef struct GXFifoObj {
    u8 pad[128];
} GXFifoObj;

typedef struct GXData {
    u16 vNumNot;                     /* 0x000 */
    u16 bpSentNot;                   /* 0x002 */
    u16 vNum;                        /* 0x004 */
    u16 vLim;                        /* 0x006 */
    u32 cpEnable;                    /* 0x008 */
    u32 cpStatus;                    /* 0x00C */
    u32 cpClr;                       /* 0x010 */
    u32 vcdLo;                       /* 0x014 */
    u32 vcdHi;                       /* 0x018 */
    u32 vatA[8];                     /* 0x01C */
    u32 vatB[8];                     /* 0x03C */
    u32 vatC[8];                     /* 0x05C */
    u32 lpSize;                      /* 0x07C */
    u32 matIdxA;                     /* 0x080 */
    u32 matIdxB;                     /* 0x084 */
    u32 indexBase[4];                /* 0x088 */
    u32 indexStride[4];              /* 0x098 */
    u32 ambColor[2];                 /* 0x0A8 */
    u32 matColor[2];                 /* 0x0B0 */
    u32 suTs0[8];                    /* 0x0B8 */
    u32 suTs1[8];                    /* 0x0D8 */
    u32 suScis0;                     /* 0x0F8 */
    u32 suScis1;                     /* 0x0FC */
    u32 tref[8];                     /* 0x100 */
    u32 iref;                        /* 0x120 */
    u32 bpMask;                      /* 0x124 */
    u32 IndTexScale0;                /* 0x128 */
    u32 IndTexScale1;                /* 0x12C */
    u32 tevc[16];                    /* 0x130 */
    u32 teva[16];                    /* 0x170 */
    u32 tevKsel[8];                  /* 0x1B0 */
    u32 cmode0;                      /* 0x1D0 */
    u32 cmode1;                      /* 0x1D4 */
    u32 zmode;                       /* 0x1D8 */
    u32 peCtrl;                      /* 0x1DC */
    u32 cpDispSrc;                   /* 0x1E0 */
    u32 cpDispSize;                  /* 0x1E4 */
    u32 cpDispStride;                /* 0x1E8 */
    u32 cpDisp;                      /* 0x1EC */
    u32 cpTexSrc;                    /* 0x1F0 */
    u32 cpTexSize;                   /* 0x1F4 */
    u32 cpTexStride;                 /* 0x1F8 */
    u32 cpTex;                       /* 0x1FC */
    u8 cpTexZ;                       /* 0x200 */
    u32 genMode;                     /* 0x204 */
    GXTexRegion TexRegions[8];       /* 0x208 */
    GXTexRegion TexRegionsCI[4];     /* 0x288 */
    u32 nextTexRgn;                  /* 0x2C8 */
    u32 nextTexRgnCI;                /* 0x2CC */
    GXTlutRegion TlutRegions[20];    /* 0x2D0 */
    void *texRegionCallback;         /* 0x410 */
    void *tlutRegionCallback;        /* 0x414 */
    u32 nrmType;                     /* 0x418 */
    u8 hasNrms;                      /* 0x41C */
    u8 hasBiNrms;                    /* 0x41D */
    u32 projType;                    /* 0x420 */
    f32 projMtx[6];                  /* 0x424 */
    f32 vpLeft;                      /* 0x43C */
    f32 vpTop;                       /* 0x440 */
    f32 vpWd;                        /* 0x444 */
    f32 vpHt;                        /* 0x448 */
    f32 vpNearz;                     /* 0x44C */
    f32 vpFarz;                      /* 0x450 */
    f32 zOffset;                     /* 0x454 */
    f32 zScale;                      /* 0x458 */
    u32 tImage0[8];                  /* 0x45C */
    u32 tMode0[8];                   /* 0x47C */
    u32 texmapId[16];                /* 0x49C */
    u32 tcsManEnab;                  /* 0x4DC */
    u32 tevTcEnab;                   /* 0x4E0 */
    u32 perf0;                       /* 0x4E4 */
    u32 perf1;                       /* 0x4E8 */
    u32 perfSel;                     /* 0x4EC */
    u8 inDispList;                   /* 0x4F0 */
    u8 dlSaveContext;                /* 0x4F1 */
    u8 dirtyVAT;                     /* 0x4F2 */
    u32 dirtyState;                  /* 0x4F4 */
} GXData;

static GXData gxData;
static GXFifoObj FifoObj;
GXData *gx = &gxData;
volatile u32 *__piReg;
volatile u16 *__cpReg;
volatile u16 *__peReg;
volatile u16 *__memReg;

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s8 s8;
    s16 s16;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

#define SET_REG_FIELD(line, reg, size, shift, val) \
    do { \
        (reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) | ((u32)(val) << (shift)); \
    } while (0)

#define GX_WRITE_U8(ub) GXWGFifo.u8 = (u8)(ub)
#define GX_WRITE_U32(ui) GXWGFifo.u32 = (u32)(ui)

#define GX_WRITE_RAS_REG(value) \
    do { \
        GX_WRITE_U8(0x61); \
        GX_WRITE_U32((value)); \
    } while (0)

#define GX_WRITE_XF_REG(addr, value) \
    do { \
        GX_WRITE_U8(0x10); \
        GX_WRITE_U32(0x1000 + (addr)); \
        GX_WRITE_U32(value); \
    } while (0)

#define GX_WRITE_CP_STRM_REG(addr, vtxfmt, data) \
    do { \
        GX_WRITE_U8(0x08); \
        GX_WRITE_U8((addr) | (vtxfmt)); \
        GX_WRITE_U32(data); \
    } while (0)

#pragma peephole off
s32 GXGetTexObjFmt(GXTexObj *obj);
GXTexRegion *__GXDefaultTexRegionCallback(GXTexObj *t_obj, s32 id)
{
    s32 fmt;

    fmt = GXGetTexObjFmt(t_obj);
    if (fmt != 8 && fmt != 9 && fmt != 10) {
        return &gx->TexRegions[gx->nextTexRgn++ & 7];
    } else {
        return &gx->TexRegionsCI[gx->nextTexRgnCI++ & 3];
    }
}

GXTlutRegion *__GXDefaultTlutRegionCallback(u32 idx)
{
    if (idx >= 20) {
        return NULL;
    }
    return &gx->TlutRegions[idx];
}

GXFifoObj *GXInit(void *base, u32 size)
{
    void GXSetMisc(u32 token, u32 val);
    void __GXFifoInit(void);
    void GXInitFifoBase(GXFifoObj *fifo, void *base, u32 size);
    void GXSetCPUFifo(GXFifoObj *fifo);
    void GXSetGPFifo(GXFifoObj *fifo);
    void __GXPEInit(void);
    u32 PPCMfhid2(void);
    void PPCMtwpar(u32 val);
    void PPCMthid2(u32 val);
    void __GXFlushTextureState(void);
    void GXInitTexCacheRegion(GXTexRegion *region, u8 is32bMipmap, u32 tmemEven, u32 sizeEven, u32 tmemOdd, u32 sizeOdd);
    void GXInitTlutRegion(GXTlutRegion *region, u32 tmemAddr, u32 tlutSize);
    void __GXSetTmemConfig(u32 config);
    void __GXInitGX(void);
    u32 i;
    u32 reg;
    u32 freqBase;

    gx->inDispList = 0;
    gx->dlSaveContext = 1;
    gx->tcsManEnab = 0;
    gx->tevTcEnab = 0;
    GXSetMisc(1, 0);
    __piReg = (volatile u32 *)0xCC003000;
    __cpReg = (volatile u16 *)0xCC000000;
    __peReg = (volatile u16 *)0xCC001000;
    __memReg = (volatile u16 *)0xCC004000;
    __GXFifoInit();
    GXInitFifoBase(&FifoObj, base, size);
    GXSetCPUFifo(&FifoObj);
    GXSetGPFifo(&FifoObj);
    __GXPEInit();
    reg = PPCMfhid2();
    PPCMtwpar(0x0C008000);
    PPCMthid2(reg | 0x40000000);

    gx->genMode = 0;
    SET_REG_FIELD(0, gx->genMode, 8, 24, 0);
    gx->bpMask = 0xFF;
    SET_REG_FIELD(0, gx->bpMask, 8, 24, 0x0F);
    gx->lpSize = 0;
    SET_REG_FIELD(0, gx->lpSize, 8, 24, 0x22);
    for (i = 0; i < 16; i++) {
        gx->tevc[i] = 0;
        gx->teva[i] = 0;
        gx->tref[i / 2] = 0;
        gx->texmapId[i] = 0xFF;
        SET_REG_FIELD(0, gx->tevc[i], 8, 24, 0xC0 + i * 2);
        SET_REG_FIELD(0, gx->teva[i], 8, 24, 0xC1 + i * 2);
        SET_REG_FIELD(0, gx->tevKsel[i / 2], 8, 24, 0xF6 + i / 2);
        SET_REG_FIELD(0, gx->tref[i / 2], 8, 24, 0x28 + i / 2);
    }
    gx->iref = 0;
    SET_REG_FIELD(0, gx->iref, 8, 24, 0x27);
    for (i = 0; i < 8; i++) {
        gx->suTs0[i] = 0;
        gx->suTs1[i] = 0;
        SET_REG_FIELD(0, gx->suTs0[i], 8, 24, 0x30 + i * 2);
        SET_REG_FIELD(0, gx->suTs1[i], 8, 24, 0x31 + i * 2);
    }
    SET_REG_FIELD(0, gx->suScis0, 8, 24, 0x20);
    SET_REG_FIELD(0, gx->suScis1, 8, 24, 0x21);
    SET_REG_FIELD(0, gx->cmode0, 8, 24, 0x41);
    SET_REG_FIELD(0, gx->cmode1, 8, 24, 0x42);
    SET_REG_FIELD(0, gx->zmode, 8, 24, 0x40);
    SET_REG_FIELD(0, gx->peCtrl, 8, 24, 0x43);
    SET_REG_FIELD(0, gx->cpTex, 2, 7, 0);
    gx->dirtyState = 0;
    gx->dirtyVAT = 0;

    freqBase = *(u32 *)0x800000F8 / 500;
    __GXFlushTextureState();
    reg = (freqBase >> 11) | 0x400 | 0x69000000;
    GX_WRITE_RAS_REG(reg);
    __GXFlushTextureState();
    reg = (freqBase / 4224) | 0x200 | 0x46000000;
    GX_WRITE_RAS_REG(reg);

    for (i = 0; i < 8; i++) {
        SET_REG_FIELD(0, gx->vatA[i], 1, 30, 1);
        SET_REG_FIELD(0, gx->vatB[i], 1, 31, 1);
        do {
            s32 regAddr;
            GX_WRITE_U8(0x08);
            GX_WRITE_U8(i | 0x80);
            GX_WRITE_U32(gx->vatB[i]);
            regAddr = i - 12;
        } while (0);
    }
    {
        u32 reg1 = 0;
        u32 reg2 = 0;
        SET_REG_FIELD(0, reg1, 1, 0, 1);
        SET_REG_FIELD(0, reg1, 1, 1, 1);
        SET_REG_FIELD(0, reg1, 1, 2, 1);
        SET_REG_FIELD(0, reg1, 1, 3, 1);
        SET_REG_FIELD(0, reg1, 1, 4, 1);
        SET_REG_FIELD(0, reg1, 1, 5, 1);
        do {
            s32 regAddr;
            GX_WRITE_U8(0x10);
            GX_WRITE_U32(0x1000);
            GX_WRITE_U32(reg1);
            regAddr = 0;
        } while (0);
        SET_REG_FIELD(0, reg2, 1, 0, 1);
        do {
            s32 regAddr;
            GX_WRITE_U8(0x10);
            GX_WRITE_U32(0x1012);
            GX_WRITE_U32(reg2);
            regAddr = 0x12;
        } while (0);
        reg = 0;
        SET_REG_FIELD(0, reg, 1, 0, 1);
        SET_REG_FIELD(0, reg, 1, 1, 1);
        SET_REG_FIELD(0, reg, 1, 2, 1);
        SET_REG_FIELD(0, reg, 1, 3, 1);
        SET_REG_FIELD(0, reg, 8, 24, 0x58);
        GX_WRITE_RAS_REG(reg);
    }

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion(&gx->TexRegions[i], 0, i * 0x8000, 0, 0x80000 + i * 0x8000, 0);
    }
    for (i = 0; i < 4; i++) {
        GXInitTexCacheRegion(&gx->TexRegionsCI[i], 0, (i * 2 + 8) << 15, 0, (i * 2 + 9) << 15, 0);
    }
    for (i = 0; i < 16; i++) {
        GXInitTlutRegion(&gx->TlutRegions[i], 0xC0000 + i * 0x2000, 16);
    }
    for (i = 0; i < 4; i++) {
        GXInitTlutRegion(&gx->TlutRegions[i + 16], 0xE0000 + i * 0x8000, 64);
    }

    __cpReg[3] = 0;
    SET_REG_FIELD(0, gx->perfSel, 4, 4, 0);
    do {
        s32 regAddr;
        GX_WRITE_U8(0x08);
        GX_WRITE_U8(0x20);
        GX_WRITE_U32(gx->perfSel);
        regAddr = 0x20 - 12;
    } while (0);
    do {
        s32 regAddr;
        GX_WRITE_U8(0x10);
        GX_WRITE_U32(0x1006);
        GX_WRITE_U32(0);
        regAddr = 6;
    } while (0);
    GX_WRITE_RAS_REG(0x23000000);
    GX_WRITE_RAS_REG(0x24000000);
    GX_WRITE_RAS_REG(0x67000000);
    __GXSetTmemConfig(0);
    __GXInitGX();
    return &FifoObj;
}

void __GXInitGX(void) {
    typedef struct GXRenderModeObj {
        s32 viTVmode;
        u16 fbWidth;
        u16 efbHeight;
        u16 xfbHeight;
        u16 viXOrigin;
        u16 viYOrigin;
        u16 viWidth;
        u16 viHeight;
        s32 xFBmode;
        u8 field_rendering;
        u8 aa;
        u8 sample_pattern[12][2];
        u8 vfilter[7];
    } GXRenderModeObj;
    extern GXRenderModeObj GXNtsc480IntDf;
    extern GXRenderModeObj GXPal528IntDf;
    extern GXRenderModeObj GXEurgb60Hz480IntDf;
    extern GXRenderModeObj GXMpal480IntDf;
    u32 VIGetTvFormat(void);
    void GXSetCopyClear(GXColor clear_clr, u32 clear_z);
    void GXSetTexCoordGen2(u32 dst_coord, s32 func, s32 src_param, u32 mtx, u8 normalize, u32 pt_texmtx);
    void GXSetNumTexGens(u8 nTexGens);
    void GXClearVtxDesc(void);
    void GXInvalidateVtxCache(void);
    void GXSetArray(s32 attr, void *base_ptr, u8 stride);
    void GXSetLineWidth(u8 width, u32 texOffsets);
    void GXSetPointSize(u8 pointSize, u32 texOffsets);
    void GXEnableTexOffsets(s32 coord, u8 line_enable, u8 point_enable);
    void GXLoadPosMtxImm(f32 mtx[3][4], u32 id);
    void GXLoadNrmMtxImm(f32 mtx[3][4], u32 id);
    void GXSetCurrentMtx(u32 id);
    void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type);
    void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
    void GXSetCoPlanar(u8 enable);
    void GXSetCullMode(s32 mode);
    void GXSetClipMode(s32 mode);
    void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
    void GXSetScissorBoxOffset(s32 x_off, s32 y_off);
    void GXSetNumChans(u8 nChans);
    void GXSetChanCtrl(s32 chan, u8 enable, s32 amb_src, s32 mat_src, u32 light_mask, s32 diff_fn, s32 attn_fn);
    void GXSetChanAmbColor(s32 chan, GXColor amb_color);
    void GXSetChanMatColor(s32 chan, GXColor mat_color);
    void GXInvalidateTexAll(void);
    void *GXSetTexRegionCallback(GXTexRegion *(*f)(GXTexObj *, s32));
    void *GXSetTlutRegionCallback(GXTlutRegion *(*f)(u32));
    void GXSetTevOrder(s32 stage, s32 coord, u32 map, s32 color);
    void GXSetNumTevStages(u8 nStages);
    void GXSetTevOp(s32 id, s32 mode);
    void GXSetAlphaCompare(s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1);
    void GXSetZTexture(s32 op, s32 fmt, u32 bias);
    void GXSetTevKColorSel(s32 stage, s32 sel);
    void GXSetTevKAlphaSel(s32 stage, s32 sel);
    void GXSetTevSwapMode(s32 stage, s32 ras_sel, s32 tex_sel);
    void GXSetTevSwapModeTable(s32 table, s32 red, s32 green, s32 blue, s32 alpha);
    void GXSetTevDirect(s32 tev_stage);
    void GXSetNumIndStages(u8 nIndStages);
    void GXSetIndTexCoordScale(s32 ind_state, s32 scale_s, s32 scale_t);
    void GXSetFog(s32 type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color);
    void GXSetFogRangeAdj(u8 enable, u16 center, u16 *table);
    void GXSetBlendMode(s32 type, s32 src_factor, s32 dst_factor, s32 op);
    void GXSetColorUpdate(u8 updateEnable);
    void GXSetAlphaUpdate(u8 updateEnable);
    void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable);
    void GXSetZCompLoc(u8 before_tex);
    void GXSetDither(u8 dither);
    void GXSetDstAlpha(u8 enable, u8 alpha);
    void GXSetPixelFmt(s32 pix_fmt, s32 z_fmt);
    void GXSetFieldMask(u8 odd_mask, u8 even_mask);
    void GXSetFieldMode(u8 field_mode, u8 half_aspect_ratio);
    void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht);
    void GXSetDispCopyDst(u16 wd, u16 ht);
    u32 GXSetDispCopyYScale(f32 vscale);
    void GXSetCopyClamp(s32 clamp);
    void GXSetCopyFilter(u8 aa, const u8 sample_pattern[12][2], u8 vf, const u8 vfilter[7]);
    void GXSetDispCopyGamma(s32 gamma);
    void GXSetDispCopyFrame2Field(s32 mode);
    void GXClearBoundingBox(void);
    void GXPokeColorUpdate(u8 update_enable);
    void GXPokeAlphaUpdate(u8 update_enable);
    void GXPokeDither(u8 dither);
    void GXPokeBlendMode(s32 type, s32 src_factor, s32 dst_factor, s32 op);
    void GXPokeAlphaMode(s32 func, u8 threshold);
    void GXPokeAlphaRead(s32 mode);
    void GXPokeDstAlpha(u8 enable, u8 alpha);
    void GXPokeZMode(u8 compare_enable, s32 func, u8 update_enable);
    void GXSetGPMetric(s32 perf0, s32 perf1);
    void GXClearGPMetric(void);
    GXRenderModeObj *rmode;
    f32 identity_mtx[3][4];
    GXColor clear = {64, 64, 64, 255};
    GXColor black = {0, 0, 0, 0};
    GXColor white = {255, 255, 255, 255};
    u32 i;

    switch (VIGetTvFormat()) {
    case 0:
        rmode = &GXNtsc480IntDf;
        break;
    case 1:
        rmode = &GXPal528IntDf;
        break;
    case 5:
        rmode = &GXEurgb60Hz480IntDf;
        break;
    case 2:
        rmode = &GXMpal480IntDf;
        break;
    default:
        rmode = &GXNtsc480IntDf;
        break;
    }

    GXSetCopyClear(clear, 0xFFFFFF);
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetTexCoordGen2(1, 1, 5, 60, 0, 125);
    GXSetTexCoordGen2(2, 1, 6, 60, 0, 125);
    GXSetTexCoordGen2(3, 1, 7, 60, 0, 125);
    GXSetTexCoordGen2(4, 1, 8, 60, 0, 125);
    GXSetTexCoordGen2(5, 1, 9, 60, 0, 125);
    GXSetTexCoordGen2(6, 1, 10, 60, 0, 125);
    GXSetTexCoordGen2(7, 1, 11, 60, 0, 125);
    GXSetNumTexGens(1);
    GXClearVtxDesc();
    GXInvalidateVtxCache();
    for (i = 9; i <= 24; i++) {
        GXSetArray(i, gx, 0);
    }
    GXSetLineWidth(6, 0);
    GXSetPointSize(6, 0);
    GXEnableTexOffsets(0, 0, 0);
    GXEnableTexOffsets(1, 0, 0);
    GXEnableTexOffsets(2, 0, 0);
    GXEnableTexOffsets(3, 0, 0);
    GXEnableTexOffsets(4, 0, 0);
    GXEnableTexOffsets(5, 0, 0);
    GXEnableTexOffsets(6, 0, 0);
    GXEnableTexOffsets(7, 0, 0);
    identity_mtx[0][0] = 1.0f;
    identity_mtx[0][1] = 0.0f;
    identity_mtx[0][2] = 0.0f;
    identity_mtx[0][3] = 0.0f;
    identity_mtx[1][0] = 0.0f;
    identity_mtx[1][1] = 1.0f;
    identity_mtx[1][2] = 0.0f;
    identity_mtx[1][3] = 0.0f;
    identity_mtx[2][0] = 0.0f;
    identity_mtx[2][1] = 0.0f;
    identity_mtx[2][2] = 1.0f;
    identity_mtx[2][3] = 0.0f;
    GXLoadPosMtxImm(identity_mtx, 0);
    GXLoadNrmMtxImm(identity_mtx, 0);
    GXSetCurrentMtx(0);
    GXLoadTexMtxImm(identity_mtx, 60, 0);
    GXLoadTexMtxImm(identity_mtx, 125, 0);
    GXSetViewport(0.0f, 0.0f, rmode->fbWidth, rmode->xfbHeight, 0.0f, 1.0f);
    GXSetCoPlanar(0);
    GXSetCullMode(2);
    GXSetClipMode(0);
    GXSetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetScissorBoxOffset(0, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(4, black);
    GXSetChanMatColor(4, white);
    GXSetChanCtrl(5, 0, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(5, black);
    GXSetChanMatColor(5, white);
    GXInvalidateTexAll();
    gx->nextTexRgn = 0;
    gx->nextTexRgnCI = 0;
    GXSetTexRegionCallback(__GXDefaultTexRegionCallback);
    GXSetTlutRegionCallback(__GXDefaultTlutRegionCallback);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOrder(1, 1, 1, 4);
    GXSetTevOrder(2, 2, 2, 4);
    GXSetTevOrder(3, 3, 3, 4);
    GXSetTevOrder(4, 4, 4, 4);
    GXSetTevOrder(5, 5, 5, 4);
    GXSetTevOrder(6, 6, 6, 4);
    GXSetTevOrder(7, 7, 7, 4);
    GXSetTevOrder(8, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(9, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(10, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(11, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(12, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(13, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(14, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(15, 0xFF, 0xFF, 0xFF);
    GXSetNumTevStages(1);
    GXSetTevOp(0, 3);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZTexture(0, 0x11, 0);
    for (i = 0; i < 16; i++) {
        GXSetTevKColorSel(i, 6);
        GXSetTevKAlphaSel(i, 0);
        GXSetTevSwapMode(i, 0, 0);
    }
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
    for (i = 0; i < 16; i++) {
        GXSetTevDirect(i);
    }
    GXSetNumIndStages(0);
    GXSetIndTexCoordScale(0, 0, 0);
    GXSetIndTexCoordScale(1, 0, 0);
    GXSetIndTexCoordScale(2, 0, 0);
    GXSetIndTexCoordScale(3, 0, 0);
    GXSetFog(0, 0.0f, 1.0f, 0.1f, 1.0f, black);
    GXSetFogRangeAdj(0, 0, 0);
    GXSetBlendMode(0, 4, 5, 0);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetZMode(1, 3, 1);
    GXSetZCompLoc(1);
    GXSetDither(1);
    GXSetDstAlpha(0, 0);
    GXSetPixelFmt(0, 0);
    GXSetFieldMask(1, 1);
    GXSetFieldMode(rmode->field_rendering, (rmode->viHeight == 2 * rmode->xfbHeight) ? (u8)1 : (u8)0);
    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyDst(rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyYScale((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
    GXSetCopyClamp(3);
    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, 1, rmode->vfilter);
    GXSetDispCopyGamma(0);
    GXSetDispCopyFrame2Field(0);
    GXClearBoundingBox();
    GXPokeColorUpdate(1);
    GXPokeAlphaUpdate(1);
    GXPokeDither(0);
    GXPokeBlendMode(0, 0, 1, 15);
    GXPokeAlphaMode(7, 0);
    GXPokeAlphaRead(1);
    GXPokeDstAlpha(0, 0);
    GXPokeZMode(1, 7, 1);
    GXSetGPMetric(0x23, 0x16);
    GXClearGPMetric();
}

