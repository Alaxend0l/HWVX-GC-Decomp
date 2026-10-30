#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

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

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u16 vNum;
    u8 pad6[0x1D0 - 0x6];
    u32 cmode0;
    u32 cmode1;
    u32 zmode;
    u32 peCtrl;
    u32 cpDispSrc;
    u32 cpDispSize;
    u32 cpDispStride;
    u32 cpDisp;
    u32 cpTexSrc;
    u32 cpTexSize;
    u32 cpTexStride;
    u32 cpTex;
    u8 cpTexZ;
} GXData;

void __GetImageTileCount(s32 fmt, u16 wd, u16 ht, u32 *rowTiles, u32 *colTiles, u32 *cmpTiles);

extern GXData *gx;

static u32 __GXGetNumXfbLines(u32 efbHt, u32 iScale) {
    u32 count;
    u32 realHt;
    u32 iScaleD;

    count = (efbHt - 1) * 0x100;
    realHt = (count / iScale) + 1;
    iScaleD = iScale;
    if (iScaleD > 0x80 && iScaleD < 0x100) {
        while (iScaleD % 2 == 0) {
            iScaleD /= 2;
        }
        if (efbHt % iScaleD == 0) {
            realHt++;
        }
    }
    if (realHt > 0x400) {
        realHt = 0x400;
    }
    return realHt;
}

void GXAdjustForOverscan(GXRenderModeObj *rmin, GXRenderModeObj *rmout, u16 hor, u16 ver) {
    u16 hor2 = (u16)(hor * 2);
    u16 ver2 = (u16)(ver * 2);
    u32 verf;

    if (rmin != rmout) {
        *rmout = *rmin;
    }
    rmout->fbWidth = (u16)(rmin->fbWidth - hor2);
    verf = (u32)(ver2 * rmin->efbHeight) / rmin->xfbHeight;
    rmout->efbHeight = (u16)(rmin->efbHeight - verf);
    if (rmin->xFBmode == 0 && (rmin->viTVmode & 2) != 2) {
        rmout->xfbHeight = (u16)(rmin->xfbHeight - ver);
    } else {
        rmout->xfbHeight = (u16)(rmin->xfbHeight - ver2);
    }
    rmout->viWidth = (u16)(rmin->viWidth - hor2);
    rmout->viHeight = (u16)(rmin->viHeight - ver2);
    rmout->viXOrigin = (u16)(rmin->viXOrigin + hor);
    rmout->viYOrigin = (u16)(rmin->viYOrigin + ver);
}

void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    gx->cpDispSrc = 0;
    gx->cpDispSrc = ((u32)gx->cpDispSrc & ~0x3FF) | ((u32)left);
    gx->cpDispSrc = ((u32)gx->cpDispSrc & ~0xFFC00) | ((u32)top << 10);
    gx->cpDispSrc = ((u32)gx->cpDispSrc & ~0xFF000000) | ((u32)0x49 << 24);
    gx->cpDispSize = 0;
    gx->cpDispSize = ((u32)gx->cpDispSize & ~0x3FF) | ((u32)(wd - 1));
    gx->cpDispSize = ((u32)gx->cpDispSize & ~0xFFC00) | ((u32)(ht - 1) << 10);
    gx->cpDispSize = ((u32)gx->cpDispSize & ~0xFF000000) | ((u32)0x4A << 24);
}

void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    gx->cpTexSrc = 0;
    gx->cpTexSrc = ((u32)gx->cpTexSrc & ~0x3FF) | ((u32)left);
    gx->cpTexSrc = ((u32)gx->cpTexSrc & ~0xFFC00) | ((u32)top << 10);
    gx->cpTexSrc = ((u32)gx->cpTexSrc & ~0xFF000000) | ((u32)0x49 << 24);
    gx->cpTexSize = 0;
    gx->cpTexSize = ((u32)gx->cpTexSize & ~0x3FF) | ((u32)(wd - 1));
    gx->cpTexSize = ((u32)gx->cpTexSize & ~0xFFC00) | ((u32)(ht - 1) << 10);
    gx->cpTexSize = ((u32)gx->cpTexSize & ~0xFF000000) | ((u32)0x4A << 24);
}

void GXSetDispCopyDst(u16 wd, u16 ht) {
    u16 stride;

    stride = (u16)(wd * 2);
    gx->cpDispStride = 0;
    gx->cpDispStride = (gx->cpDispStride & ~0x3FF) | (stride >> 5);
    gx->cpDispStride = (gx->cpDispStride & ~0xFF000000) | (0x4D << 24);
}

void GXSetTexCopyDst(u16 wd, u16 ht, s32 fmt, u8 mipmap) {
    u32 rowTiles;
    u32 colTiles;
    u32 cmpTiles;
    u32 peTexFmt;
    u32 peTexFmtH;

    gx->cpTexZ = 0;
    peTexFmt = fmt & 0xF;
    if (fmt == 0x13) {
        peTexFmt = 0xB;
    }
    switch (fmt) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 0x26:
        gx->cpTex = ((u32)gx->cpTex & ~0x18000) | ((u32)3 << 15);
        break;
    default:
        gx->cpTex = ((u32)gx->cpTex & ~0x18000) | ((u32)2 << 15);
        break;
    }
    gx->cpTexZ = 0x10 == (fmt & 0x10);
    peTexFmtH = (peTexFmt >> 3) & 1;
    gx->cpTex = ((u32)gx->cpTex & ~8) | ((u32)peTexFmtH << 3);
    peTexFmt = peTexFmt & 7;
    __GetImageTileCount(fmt, wd, ht, &rowTiles, &colTiles, &cmpTiles);
    gx->cpTexStride = 0;
    gx->cpTexStride = ((u32)gx->cpTexStride & ~0x3FF) | ((u32)(rowTiles * cmpTiles));
    gx->cpTexStride = ((u32)gx->cpTexStride & ~0xFF000000) | ((u32)0x4D << 24);
    gx->cpTex = ((u32)gx->cpTex & ~0x200) | ((u32)mipmap << 9);
    gx->cpTex = ((u32)gx->cpTex & ~0x70) | ((u32)peTexFmt << 4);
}

void GXSetDispCopyFrame2Field(s32 mode) {
    gx->cpDisp = (gx->cpDisp & ~0x3000) | (mode << 12);
    gx->cpTex = gx->cpTex & ~0x3000;
}

void GXSetCopyClamp(s32 clamp) {
    u8 clmpB;
    u8 clmpT;

    clmpT = (u8)(1 == (clamp & 1));
    clmpB = (u8)(2 == (clamp & 2));
    gx->cpDisp = (gx->cpDisp & ~1) | clmpT;
    gx->cpDisp = (gx->cpDisp & ~2) | (clmpB << 1);
    gx->cpTex = (gx->cpTex & ~1) | clmpT;
    gx->cpTex = (gx->cpTex & ~2) | (clmpB << 1);
}

u32 GXSetDispCopyYScale(f32 vscale) {
    u8 enable;
    u32 iScale;
    u32 ht;
    u32 reg;

    iScale = (u32)(256.0f / vscale) & 0x1FF;
    enable = (iScale != 256);
    reg = 0;
    reg = ((u32)reg & ~0x1FF) | ((u32)iScale);
    reg = ((u32)reg & ~0xFF000000) | ((u32)0x4E << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 0;
    gx->cpDisp = ((u32)gx->cpDisp & ~0x400) | ((u32)enable << 10);
    ht = (u32)((gx->cpDispSize >> 10) & 0x3FF) + 1;
    return __GXGetNumXfbLines(ht, iScale);
}

void GXSetCopyClear(GXColor clear_clr, u32 clear_z) {
    u32 reg;

    reg = 0;
    reg = ((u32)reg & ~0xFF) | ((u32)clear_clr.r);
    reg = ((u32)reg & ~0xFF00) | ((u32)clear_clr.a << 8);
    reg = ((u32)reg & ~0xFF000000) | ((u32)0x4F << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    reg = 0;
    reg = ((u32)reg & ~0xFF) | ((u32)clear_clr.b);
    reg = ((u32)reg & ~0xFF00) | ((u32)clear_clr.g << 8);
    reg = ((u32)reg & ~0xFF000000) | ((u32)0x50 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    reg = 0;
    reg = ((u32)reg & ~0xFFFFFF) | ((u32)clear_z);
    reg = ((u32)reg & ~0xFF000000) | ((u32)0x51 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 0;
}

void GXSetDispCopyGamma(s32 gamma) {
    gx->cpDisp = (gx->cpDisp & ~0x180) | (gamma << 7);
}

void GXCopyDisp(void *dest, u8 clear) {
    u32 reg;
    u32 tempPeCtrl;
    u8 changePeCtrl;

    if (clear) {
        reg = gx->zmode;
        reg = ((u32)reg & ~1) | ((u32)1);
        reg = ((u32)reg & ~0xE) | ((u32)7 << 1);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = reg;
        reg = gx->cmode0;
        reg = ((u32)reg & ~1) | ((u32)0);
        reg = ((u32)reg & ~2) | ((u32)0 << 1);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = reg;
    }
    changePeCtrl = 0;
    if ((clear || (u32)(gx->peCtrl & 7) == 3) && (u32)((gx->peCtrl >> 6) & 1) == 1) {
        changePeCtrl = 1;
        tempPeCtrl = gx->peCtrl;
        tempPeCtrl = ((u32)tempPeCtrl & ~0x40) | ((u32)0 << 6);
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = tempPeCtrl;
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cpDispSrc;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cpDispSize;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cpDispStride;
    reg = 0;
    reg = ((u32)reg & ~0xFFFFFF) | ((u32)dest >> 5);
    reg = ((u32)reg & ~0xFF000000) | ((u32)0x4B << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->cpDisp = ((u32)gx->cpDisp & ~0x800) | ((u32)clear << 11);
    gx->cpDisp = ((u32)gx->cpDisp & ~0x4000) | ((u32)1 << 14);
    gx->cpDisp = ((u32)gx->cpDisp & ~0xFF000000) | ((u32)0x52 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->cpDisp;
    if (clear) {
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->zmode;
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->cmode0;
    }
    if (changePeCtrl) {
        GXWGFifo.u8 = 0x61;
        GXWGFifo.u32 = gx->peCtrl;
    }
    gx->bpSentNot = 0;
}

void GXClearBoundingBox(void) {
    u32 reg;

    reg = 0x550003FF;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    reg = 0x560003FF;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    gx->bpSentNot = 0;
}

