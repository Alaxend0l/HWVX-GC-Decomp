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
