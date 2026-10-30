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

#define GXWGFifo (*(volatile PPCWGPipe *)0xCC008000)

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
    __cpReg = (volatile u16 *)0xCC000000;
    __piReg = (volatile u32 *)0xCC003000;
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
    gx->genMode = (gx->genMode & 0x00FFFFFF) | (0 << 24);
    gx->bpMask = 0xFF;
    gx->bpMask = (gx->bpMask & 0x00FFFFFF) | (0x0F << 24);
    gx->lpSize = 0;
    gx->lpSize = (gx->lpSize & 0x00FFFFFF) | (0x22 << 24);
    for (i = 0; i < 16; i++) {
        gx->tevc[i] = 0;
        gx->teva[i] = 0;
        gx->tref[i / 2] = 0;
        gx->texmapId[i] = 0xFF;
        gx->tevc[i] = (gx->tevc[i] & 0x00FFFFFF) | ((0xC0 + i * 2) << 24);
        gx->teva[i] = (gx->teva[i] & 0x00FFFFFF) | ((0xC1 + i * 2) << 24);
        gx->tevKsel[i / 2] = (gx->tevKsel[i / 2] & 0x00FFFFFF) | ((0xF6 + i / 2) << 24);
        gx->tref[i / 2] = (gx->tref[i / 2] & 0x00FFFFFF) | ((0x28 + i / 2) << 24);
    }
    gx->iref = 0;
    gx->iref = (gx->iref & 0x00FFFFFF) | (0x27 << 24);
    for (i = 0; i < 8; i++) {
        gx->suTs0[i] = 0;
        gx->suTs1[i] = 0;
        gx->suTs0[i] = (gx->suTs0[i] & 0x00FFFFFF) | ((0x30 + i * 2) << 24);
        gx->suTs1[i] = (gx->suTs1[i] & 0x00FFFFFF) | ((0x31 + i * 2) << 24);
    }
    gx->suScis0 = (gx->suScis0 & 0x00FFFFFF) | (0x20 << 24);
    gx->suScis1 = (gx->suScis1 & 0x00FFFFFF) | (0x21 << 24);
    gx->cmode0 = (gx->cmode0 & 0x00FFFFFF) | (0x41 << 24);
    gx->cmode1 = (gx->cmode1 & 0x00FFFFFF) | (0x42 << 24);
    gx->zmode = (gx->zmode & 0x00FFFFFF) | (0x40 << 24);
    gx->peCtrl = (gx->peCtrl & 0x00FFFFFF) | (0x43 << 24);
    gx->cpTex = (gx->cpTex & ~0x180) | (0 << 7);
    gx->dirtyState = 0;
    gx->dirtyVAT = 0;

    freqBase = *(u32 *)0x800000F8 / 500;
    __GXFlushTextureState();
    reg = (freqBase >> 11) | 0x400 | 0x69000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    __GXFlushTextureState();
    reg = (freqBase / 4224) | 0x200 | 0x46000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;

    for (i = 0; i < 8; i++) {
        gx->vatA[i] = (gx->vatA[i] & ~0x40000000) | (1 << 30);
        gx->vatB[i] = (gx->vatB[i] & ~0x80000000) | (1 << 31);
        GXWGFifo.u8 = 8;
        GXWGFifo.u8 = i | 0x80;
        GXWGFifo.u32 = gx->vatB[i];
    }
    {
        u32 reg1 = 0;
        u32 reg2 = 0;
        reg1 = (reg1 & ~1) | 1;
        reg1 = (reg1 & ~2) | 2;
        reg1 = (reg1 & ~4) | 4;
        reg1 = (reg1 & ~8) | 8;
        reg1 = (reg1 & ~0x10) | 0x10;
        reg1 = (reg1 & ~0x20) | 0x20;
        GXWGFifo.u8 = 0x10;
        GXWGFifo.u32 = 0x1000;
        GXWGFifo.u32 = reg1;
        reg2 = (reg2 & ~1) | 1;
        GXWGFifo.u8 = 0x10;
        GXWGFifo.u32 = 0x1012;
        GXWGFifo.u32 = reg2;
        reg = 0;
        reg = (reg & ~1) | 1;
        reg = (reg & ~2) | 2;
        reg = (reg & ~4) | 4;
        reg = (reg & ~8) | 8;
        reg = (reg & 0x00FFFFFF) | (0x58 << 24);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = reg;
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
    gx->perfSel = (gx->perfSel & ~0xF0) | (0 << 4);
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x20;
    GXWGFifo.u32 = gx->perfSel;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1006;
    GXWGFifo.u32 = 0;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0x23000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0x24000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0x67000000;
    __GXSetTmemConfig(0);
    __GXInitGX();
    return &FifoObj;
}

