#include "common.h"

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);

extern u32 m_VtxDescFlag;

void *Allocate(u32 size, u32 flags, const char *name);
void *memset(void *dst, int c, u32 n);
void PrintDVDMessage(void);
void GXSetBlendMode(s32 type, s32 src, s32 dst, s32 op);
void GXSetColorUpdate(u8 enable);
void GXCopyDisp(void *dest, u8 clear);
void GXDrawDone(void);
void VISetNextFrameBuffer(void *fb);
void VISetBlack(s32 black);
void VIFlush(void);
void VIWaitForRetrace(void);

extern void *g_pDL;
extern void *g_currentBuffer;
extern void *g_frameBuffer1;
extern void *g_frameBuffer2;
extern u8 m_firstFrame;
extern s32 m_srcBlend;
extern s32 m_dstBlend;

void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable);
void GXSetCullMode(s32 mode);
void GXSetFog(s32 type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color);

extern s32 m_zwrite;
extern s32 m_zstate;
extern s32 m_zmode;
extern f32 m_fogRange2;
extern f32 m_fogRange1;
extern u8 m_fogType;
extern GXColor m_fogColor;
extern s32 m_fogMode;
extern s32 m_cullMode;
extern s32 m_blendMode;

typedef struct GfxWindow {
    f32 unk0[4][4];
    f32 unk40[4][4];
    f32 unk80[4][4];
    f32 unkC0[4][4];
    f32 unk100[4][4];
    f32 unk140[4][4];
    u8 unk180[0x270 - 0x180];
    f32 fov;
    u8 unk274[0x278 - 0x274];
    f32 unk278;
    f32 unk27C;
    u32 width;
    u32 height;
    u32 halfWidth;
    u32 halfHeight;
    f32 unk290;
    f32 unk294;
    f32 unk298;
    f32 unk29C;
    f32 unk2A0;
    f32 unk2A4;
    f32 unk2A8;
    f32 unk2AC;
    u32 unk2B0;
    u32 unk2B4;
    u32 unk2B8;
    u32 unk2BC;
    u8 unk2C0[4];
    u32 unk2C4;
    u8 unk2C8[0x2D0 - 0x2C8];
} GfxWindow;

typedef struct GXRenderModeObj {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xFBmode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
} GXRenderModeObj;

extern GXRenderModeObj *rmode;
extern void *m_defaultFifo;
extern void *m_defaultFifoObj;
extern u8 ShadowTexture[];
extern u8 pShadowImage[];

void GXSetViewport(f32 xOrig, f32 yOrig, f32 wd, f32 ht, f32 nearZ, f32 farZ);
void GXSetScissor(u32 xOrig, u32 yOrig, u32 wd, u32 ht);
void *GXInit(void *base, u32 size);
void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht);
void GXSetDispCopyDst(u16 wd, u16 ht);
u32 GXSetDispCopyYScale(f32 vscale);
void GXSetCopyFilter(u8 aa, u8 sample_pattern[12][2], u8 vf, u8 vfilter[7]);
void GXSetPixelFmt(s32 pix_fmt, s32 z_fmt);
void GXSetDispCopyGamma(s32 gamma);
void Gfx_SetRenderWindow(s32 window);
void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
void GXSetNumChans(u8 n);
void GXSetNumTexGens(u8 n);
void GXSetNumTevStages(u8 n);
void GXSetTevOrder(s32 stage, s32 coord, s32 map, s32 color);
void GXEnableTexOffsets(s32 coord, u8 line, u8 point);
void GXSetPointSize(u8 size, s32 fmt);
void GXInitTexObj(void *obj, void *image, u16 width, u16 height, s32 format, s32 wrapS, s32 wrapT, u8 mipmap);
void Dprintf(const char *fmt, ...);
s32 DisplayList_Create(void);

typedef struct GfxState {
    u8 unk0[0x60];
    f32 lightDir[4];
    GXColor lightColor;
    GXColor unk74;
    GXColor ambientColor;
    u8 unk7C[0x93 - 0x7C];
    u8 unk93;
    u8 unk94[0xA0 - 0x94];
} GfxState;

extern GfxState g_state;
extern f32 g_identity[4][4];
extern GXColor g_textcolor;
extern GXColor m_clearColor;
extern s32 m_fillMode;
extern s32 m_currentView;
extern s32 m_curLightState;
extern s32 m_curTexture;
typedef struct OSFontHeader {
    u16 fontType;
    u16 firstChar;
    u16 lastChar;
    u16 invalChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
    u16 cellWidth;
    u16 cellHeight;
    u32 sheetSize;
    u16 sheetFormat;
    u16 sheetColumn;
    u16 sheetRow;
    u16 sheetWidth;
    u16 sheetHeight;
    u16 widthTable;
    u32 sheetImage;
    u32 sheetFullSize;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;
} OSFontHeader;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

extern OSFontHeader *FontData;
extern s32 __OSCurrHeap;

GfxWindow *GetRenderWindow(void);
void *memcpy(void *dst, const void *src, u32 n);
void *OSAllocFromHeap(s32 heap, u32 size);
BOOL OSInitFont(void *fontData);

void GXSetTevOp(s32 id, s32 mode);
void Render_StateFogEnable(s32 enable);
void DisplayList_Init(void);

typedef struct GDL {
    u8 unk0[0x14];
    s32 numTris;
} GDL;

typedef struct RenderStats {
    s32 numTris;
    s32 debugTriCount;
    u8 unk8[0x100 - 0x8];
} RenderStats;

typedef struct SilhouetteEdge {
    f32 normal0[3];
    u16 v0;
    u16 v1;
    f32 normal1[3];
    s32 type;
} SilhouetteEdge;

f32 PSVECDotProduct(const f32 *a, const f32 *b);

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s16 s16;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

static inline void GXPosition3f32(f32 x, f32 y, f32 z) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

static inline void GXColor1u32(u32 clr) {
    GXWGFifo.u32 = clr;
}

static inline void GXPosition3s16(s16 x, s16 y, s16 z) {
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = z;
}

static inline void GXTexCoord2s16(const s16 s, const s16 t) {
    GXWGFifo.s16 = s;
    GXWGFifo.s16 = t;
}

void GXBegin(s32 type, s32 vtxfmt, u16 nverts);

extern GDL *g_pGDL;
extern RenderStats g_renderStats;
extern s32 g_debugTriCount;

void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
void GXLoadTexObj(GXTexObj *obj, s32 id);
void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type);
void GXSetTexCoordGen2(s32 dst, s32 func, s32 src, u32 mtx, u8 normalize, u32 pt);
void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
char *OSGetFontTexture(char *string, void **image, s32 *x, s32 *y, s32 *width);
#pragma push
#pragma use_lmw_stmw on
s32 DrawChar(s32 x, s32 y, u16 code) {
    typedef struct {
        u32 dummy[8];
    } GXTexObj;
    typedef struct {
        u8 unk0[0x10];
        u16 cellWidth;
        u16 cellHeight;
        u8 unk14[4];
        u16 sheetFormat;
        u8 unk1A[4];
        u16 sheetWidth;
        u16 sheetHeight;
    } FontHeader;
    extern FontHeader *FontData_803C76A4;
    void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrap_s, s32 wrap_t, u8 mipmap);
    void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
    void GXLoadTexObj(GXTexObj *obj, s32 id);
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type);
    void GXSetTexCoordGen2(s32 dst, s32 func, s32 src, u32 mtx, u8 normalize, u32 pt);
    void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
    char *OSGetFontTexture(char *string, void **image, s32 *x, s32 *y, s32 *width);
    f32 mtx[3][4];
    GXTexObj tobj;
    char str[4];
    s32 width;
    void *image;
    s32 s;
    s32 t;
    s16 y1;
    s16 y0;
    s16 x1;
    s16 x0;
    s32 s0;
    s32 t0;
    s32 cw;
    s32 ch;

    if (code == 0) {
        return 0;
    }
    if (code < 0x100) {
        str[0] = code;
        str[1] = 0;
    } else {
        str[0] = code >> 8;
        str[1] = code;
        str[2] = 0;
    }
    OSGetFontTexture(str, &image, &s, &t, &width);
    GXInitTexObj(&tobj, image, FontData_803C76A4->sheetWidth, FontData_803C76A4->sheetHeight, FontData_803C76A4->sheetFormat, 0, 0, 0);
    GXInitTexObjLOD(&tobj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj, 0);
    PSMTXScale(mtx, 1.0f / FontData_803C76A4->sheetWidth, 1.0f / FontData_803C76A4->sheetHeight, 1.0f);
    GXLoadTexMtxImm(mtx, 30, 1);
    GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
    x0 = x;
    y0 = y;
    t0 = t;
    cw = FontData_803C76A4->cellWidth;
    ch = FontData_803C76A4->cellHeight;
    s0 = s;
    x1 = x0 + cw;
    y1 = y0 + ch;
    GXBegin(0x80, 0, 4);
    GXWGFifo.s16 = x0;
    GXWGFifo.s16 = y0;
    GXWGFifo.s16 = 0;
    GXWGFifo.s16 = s0;
    GXWGFifo.s16 = t0;
    GXWGFifo.s16 = x1;
    GXWGFifo.s16 = y0;
    GXWGFifo.s16 = 0;
    GXWGFifo.s16 = s0 + cw;
    GXWGFifo.s16 = t0;
    GXWGFifo.s16 = x1;
    GXWGFifo.s16 = y1;
    GXWGFifo.s16 = 0;
    GXWGFifo.s16 = s0 + cw;
    GXWGFifo.s16 = t0 + ch;
    GXWGFifo.s16 = x0;
    GXWGFifo.s16 = y1;
    GXWGFifo.s16 = 0;
    GXWGFifo.s16 = s0;
    GXWGFifo.s16 = t0 + ch;
    return width;
}

#pragma push
#pragma use_lmw_stmw on
s32 DrawString(s32 x, s32 y, char *string) {
    f32 mtx[3][4];
    GXTexObj tobj;
    void *image;
    s32 s;
    s32 t;
    s32 width;
    s16 y1;
    s16 y0;
    s16 x1;
    s16 x0;
    s32 s0;
    s32 t0;
    s32 cw;
    s32 ch;
    s32 total = 0;

    while (*string != 0) {
        string = OSGetFontTexture(string, &image, &s, &t, &width);
        GXInitTexObj(&tobj, image, FontData->sheetWidth, FontData->sheetHeight, FontData->sheetFormat, 0, 0, 0);
        GXInitTexObjLOD(&tobj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        GXLoadTexObj(&tobj, 0);
        PSMTXScale(mtx, 1.0f / FontData->sheetWidth, 1.0f / FontData->sheetHeight, 1.0f);
        GXLoadTexMtxImm(mtx, 30, 1);
        GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
        x0 = x + total;
        y0 = y;
        t0 = t;
        cw = FontData->cellWidth;
        ch = FontData->cellHeight;
        s0 = s;
        x1 = x0 + cw;
        y1 = y0 + ch;
        GXBegin(0x80, 0, 4);
        GXWGFifo.s16 = x0;
        GXWGFifo.s16 = y0;
        GXWGFifo.s16 = 0;
        GXWGFifo.s16 = s0;
        GXWGFifo.s16 = t0;
        GXWGFifo.s16 = x1;
        GXWGFifo.s16 = y0;
        GXWGFifo.s16 = 0;
        GXWGFifo.s16 = s0 + cw;
        GXWGFifo.s16 = t0;
        GXWGFifo.s16 = x1;
        GXWGFifo.s16 = y1;
        GXWGFifo.s16 = 0;
        GXWGFifo.s16 = s0 + cw;
        GXWGFifo.s16 = t0 + ch;
        GXWGFifo.s16 = x0;
        GXWGFifo.s16 = y1;
        GXWGFifo.s16 = 0;
        GXWGFifo.s16 = s0;
        GXWGFifo.s16 = t0 + ch;
        total += width;
    }
    return total;
}
#pragma pop

#pragma pop

void Render_Create(void) {
    m_fogMode = 0;
    m_fillMode = 0;
    m_currentView = 0;
    GetRenderWindow()->fov = 90.0f;
    memset(GetRenderWindow()->unkC0, 0, 0x40);
    g_state.unk93 = 0;
    memset(g_identity, 0, 0x40);
    g_identity[3][3] = 1.0f;
    g_identity[2][2] = 1.0f;
    g_identity[1][1] = 1.0f;
    g_identity[0][0] = 1.0f;
    memcpy(GetRenderWindow()->unk0, g_identity, 0x40);
    memcpy(GetRenderWindow()->unk40, g_identity, 0x40);
    memcpy(GetRenderWindow()->unk80, g_identity, 0x40);
    memcpy(GetRenderWindow()->unk100, g_identity, 0x40);
    m_blendMode = 1;
    g_textcolor.a = 0xFF;
    g_textcolor.b = 0xFF;
    g_textcolor.g = 0xFF;
    g_textcolor.r = 0xFF;
    m_clearColor.b = 0;
    m_clearColor.g = 0;
    m_clearColor.r = 0;
    m_clearColor.a = 0;
    g_state.ambientColor.r = 0x30;
    g_state.ambientColor.g = 0x30;
    g_state.ambientColor.b = 0x30;
    g_state.ambientColor.a = 0xFF;
    g_state.lightColor.r = 0xC8;
    g_state.lightColor.g = 0xC8;
    g_state.lightColor.b = 0xC8;
    g_state.lightColor.a = 0xFF;
    g_state.lightDir[0] = 0.5f;
    g_state.lightDir[1] = 0.5f;
    g_state.lightDir[2] = 0.0f;
    m_curLightState = -1;
    m_curTexture = 0;
    m_zmode = 3;
    m_zstate = 1;
    m_zwrite = 1;
    FontData = OSAllocFromHeap(__OSCurrHeap, 0x20120);
    OSInitFont(FontData);
}

s32 Render_CreateView(s32 view, u32 width, u32 height) {
    m_defaultFifo = OSAllocFromHeap(__OSCurrHeap, 0x40000);
    m_defaultFifoObj = GXInit(m_defaultFifo, 0x40000);
    GXSetViewport(0.0f, 0.0f, rmode->fbWidth, rmode->xfbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyDst(rmode->fbWidth, rmode->xfbHeight);
    GXSetDispCopyYScale((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, 1, rmode->vfilter);
    GXSetPixelFmt(1, 0);
    GXCopyDisp(g_currentBuffer, 1);
    GXSetDispCopyGamma(1);
    Gfx_SetRenderWindow(0);
    GetRenderWindow()->width = width;
    GetRenderWindow()->halfWidth = width >> 1;
    GetRenderWindow()->height = height;
    GetRenderWindow()->halfHeight = height >> 1;
    GetRenderWindow()->unk278 = 0.0f;
    GetRenderWindow()->unk27C = 0.0f;
    GetRenderWindow()->unk298 = (width * 16) / 640.0f;
    GetRenderWindow()->unk29C = (height * 16) / 480.0f;
    GetRenderWindow()->unk290 = ((0x1000 - width) >> 1) * 16;
    GetRenderWindow()->unk294 = ((0x1000 - height) >> 1) * 16;
    GetRenderWindow()->unk2A0 = GetRenderWindow()->unk298;
    GetRenderWindow()->unk2A4 = GetRenderWindow()->unk29C;
    GetRenderWindow()->unk2A8 = GetRenderWindow()->unk290;
    GetRenderWindow()->unk2AC = GetRenderWindow()->unk294;
    GetRenderWindow()->unk2B0 = 0;
    GetRenderWindow()->unk2B8 = 0;
    GetRenderWindow()->unk2B4 = width - 1;
    GetRenderWindow()->unk2BC = height - 1;
    GetRenderWindow()->unk2C4 = 0;
    Gfx_SetCameraMatrix(NULL, NULL, NULL, NULL);
    memcpy(GetRenderWindow()->unk140, GetRenderWindow()->unk40, 0x40);
    DisplayList_Create();
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXEnableTexOffsets(0, 0, 1);
    GXSetPointSize(0xFF, 5);
    GXInitTexObj(ShadowTexture, pShadowImage, 0x140, 0xE0, 1, 0, 0, 0);
    Dprintf("Render Init\n");
    return 1;
}

void Gfx_SetProjectionMatrix(f32 fov);
void GXSetViewport(f32 xOrig, f32 yOrig, f32 wd, f32 ht, f32 nearZ, f32 farZ);
void GXSetScissor(u32 xOrig, u32 yOrig, u32 wd, u32 ht);
void Render_StateSetScissor(void) {
    f32 top;
    f32 wd;
    f32 ht;

    Gfx_SetProjectionMatrix(GetRenderWindow()->fov);
    ht = 0.93333334f * GetRenderWindow()->height;
    wd = GetRenderWindow()->width;
    top = 0.93333334f * GetRenderWindow()->unk2B8;
    GXSetViewport(GetRenderWindow()->unk2B0, top, wd, ht, 0.0f, 1.0f);
    GXSetScissor(GetRenderWindow()->unk2B0, 0.93333334f * GetRenderWindow()->unk2B8, (f32)GetRenderWindow()->width, 0.93333334f * GetRenderWindow()->height);
}

extern u8 g_CurrentViews;
extern u32 *m_vertexBuffer;
void GXInvalidateVtxCache(void);
void GXInvalidateTexAll(void);
void GXSetCopyClear(GXColor clear_clr, u32 clear_z);
void GXSetAlphaUpdate(u8 enable);
void Render_StateBlendMode(u32 mode);
void Gfx_StateTextureBlendMode(s32 mode);
void Render_StateAlphaBlendEnable(s32 enable);
void Render_StateCullMode(s32 mode);
void Render_StateZEnable(s32 enable);
void Render_StateZWriteEnable(s32 enable);
void Render_BeginScene(void) {
    f32 top;
    f32 wd;
    f32 ht;

    Gfx_SetProjectionMatrix(GetRenderWindow()->fov);
    ht = 0.93333334f * GetRenderWindow()->height;
    wd = GetRenderWindow()->width;
    top = 0.93333334f * GetRenderWindow()->unk2B8;
    GXSetViewport(GetRenderWindow()->unk2B0, top, wd, ht, 0.0f, 1.0f);
    GXSetScissor(GetRenderWindow()->unk2B0, 0.93333334f * GetRenderWindow()->unk2B8, (f32)GetRenderWindow()->width, 0.93333334f * GetRenderWindow()->height);
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    GXSetCopyClear(m_clearColor, 0xFFFFFF);
    Render_StateBlendMode(1);
    Gfx_StateTextureBlendMode(2);
    Render_StateAlphaBlendEnable(0);
    Render_StateCullMode(1);
    Render_StateZEnable(1);
    Render_StateZWriteEnable(1);
    if (g_CurrentViews != 1) {
        DisplayList_Init();
    }
    g_debugTriCount = 0;
    Render_StateFogEnable(1);
    m_vertexBuffer = 0;
    m_VtxDescFlag = 0;
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(0);
}

void Render_FlipBuffers(void) {
    PrintDVDMessage();
    GXSetColorUpdate(1);
    GXCopyDisp(g_currentBuffer, 1);
    GXDrawDone();
    VISetNextFrameBuffer(g_currentBuffer);
    if (m_firstFrame) {
        VISetBlack(0);
        m_firstFrame = 0;
    }
    VIFlush();
    VIWaitForRetrace();
    if (g_currentBuffer == g_frameBuffer1) {
        g_currentBuffer = g_frameBuffer2;
    } else {
        g_currentBuffer = g_frameBuffer1;
    }
}

void DisplayList_Init(void);
s32 DisplayList_Create(void) {
    g_pDL = Allocate(0x125DC, 0, "");
    if (g_pDL != NULL) {
        DisplayList_Init();
        return 1;
    }
    return 0;
}

void DisplayList_Init(void) {
    memset(g_pDL, 0, 0x18);
}

void Render_StateBlendMode(u32 mode) {
    switch (mode) {
    case 0:
    case 1:
        m_srcBlend = 1;
        m_dstBlend = 0;
        break;
    case 2:
        m_srcBlend = 1;
        m_dstBlend = 1;
        break;
    case 3:
    case 8:
    case 10:
        m_srcBlend = 0;
        m_dstBlend = 3;
        break;
    case 5:
    case 6:
    case 7:
        m_srcBlend = 4;
        m_dstBlend = 5;
        break;
    default:
        m_srcBlend = 1;
        m_dstBlend = 0;
        break;
    }
    GXSetBlendMode(1, m_srcBlend, m_dstBlend, 5);
}

void Gfx_StateTextureBlendMode(s32 mode) {
    m_blendMode = mode;
    switch (mode) {
    case 1:
        GXSetTevOp(0, 1);
        break;
    case 2:
        GXSetTevOp(0, 0);
        break;
    case 3:
        GXSetTevOp(0, 3);
        break;
    case 4:
        GXSetTevOp(0, 2);
        break;
    }
}

void Render_StateAlphaBlendEnable(s32 enable) {
    s32 fogMode = m_fogMode;

    if (enable == 1) {
        GXSetBlendMode(1, m_srcBlend, m_srcBlend, 5);
        GXSetFog(0, 1.0f, 10.0f, 1.0f, 10.0f, m_fogColor);
    } else {
        GXSetBlendMode(0, 4, 5, 5);
        m_fogMode = !m_fogMode;
        Render_StateFogEnable(fogMode);
    }
}

void Render_StateCullMode(s32 mode) {
    switch (mode) {
    case 0:
        GXSetCullMode(0);
        break;
    case 1:
        GXSetCullMode(1);
        break;
    case 2:
        GXSetCullMode(2);
        break;
    }
    m_cullMode = mode;
}

void Render_StateZEnable(s32 enable) {
    GXSetZMode(enable, m_zmode, m_zwrite);
    m_zstate = enable;
}

void Render_StateZWriteEnable(s32 enable) {
    GXSetZMode(m_zstate, m_zmode, enable);
    m_zwrite = enable;
}

void Render_StateFogEnable(s32 enable) {
    if (m_fogMode != enable) {
        if (enable) {
            GXSetFog(m_fogType, m_fogRange1, m_fogRange2, 1.0f, 15000.0f, m_fogColor);
        } else {
            GXSetFog(0, 1.0f, 10.0f, 1.0f, 10.0f, m_fogColor);
        }
        m_fogMode = enable;
    }
}

void Gfx_GetRenderStats(RenderStats *stats) {
    g_renderStats.numTris = g_pGDL->numTris;
    g_renderStats.debugTriCount = g_debugTriCount;
    *stats = g_renderStats;
}

void Render_ClearScreen(void) {

}

void Set2DRectVtxDesc(void) {
    if (m_VtxDescFlag != 2) {
        m_VtxDescFlag = 2;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void Set2DRectFlatVtxDesc(void) {
    if (m_VtxDescFlag != 0x80) {
        m_VtxDescFlag = 0x80;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    }
}

void Set3DRectVtxDesc(void) {
    if (m_VtxDescFlag != 0x40) {
        m_VtxDescFlag = 0x40;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriFlatVtxDesc(void) {
    if (m_VtxDescFlag != 0x20) {
        m_VtxDescFlag = 0x20;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    }
}

void Gfx_SetFog(GXColor *color, u8 type, f32 start, f32 end) {
    extern GXColor m_fogColor_803C76C8;
    extern u8 m_fogType_803C76C4;
    extern f32 m_fogRange1_803C76C0;
    extern f32 m_fogRange2_803C76BC;
    u8 r, g, b, a;
    f32 scale;
    switch (type) {
    case 0:
        m_fogType_803C76C4 = 2;
        break;
    case 1:
        m_fogType_803C76C4 = 4;
        break;
    case 2:
        m_fogType_803C76C4 = 5;
        break;
    }
    a = color->a;
    b = color->b;
    g = color->g;
    r = color->r;
    m_fogColor_803C76C8.g = g;
    m_fogColor_803C76C8.r = r;
    m_fogColor_803C76C8.b = b;
    m_fogColor_803C76C8.a = 0xFF;
    if (a != 0) {
        scale = a / 255.0f;
        m_fogRange2_803C76BC = end / scale;
        m_fogRange1_803C76C0 = start / scale;
    } else {
        m_fogRange1_803C76C0 = 1.0f;
        m_fogRange2_803C76BC = 1.0f;
    }
}

void Render_Destroy(void) {

}

void Gfx_GetViewport(s32 *x, s32 *y, s32 *width, s32 *height) {
    *x = 0;
    *y = 0;
    *width = 640;
    *height = 480;
}

void CreateSilhouette(SilhouetteEdge *edges, u32 count, f32 *dir) {
    SilhouetteEdge *e;
    u32 i;
    f32 d0;
    f32 d1;

    e = edges;
    for (i = 0; i < count; i++) {
        d0 = PSVECDotProduct(e->normal0, dir);
        d1 = PSVECDotProduct(e->normal1, dir);
        if (d0 < 0.0f && d1 > 0.0f) {
            e->type = 2;
        } else if (d0 > 0.0f && d1 < 0.0f) {
            e->type = 1;
        } else {
            e->type = 0;
        }
        e++;
    }
}

void DrawVolume(f32 (*verts)[3], SilhouetteEdge *edges, u32 numEdges, f32 dx, f32 dy, f32 dz) {
    SilhouetteEdge *e;
    u32 i;

    e = edges;
    for (i = 0; i < numEdges; i++) {
        if (e->type & 2) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(verts[e->v0][0], verts[e->v0][1], verts[e->v0][2]);
            GXColor1u32(0xFF0004);
            GXPosition3f32(verts[e->v1][0], verts[e->v1][1], verts[e->v1][2]);
            GXColor1u32(0xFF0004);
            GXPosition3f32(dx + verts[e->v1][0], dy + verts[e->v1][1], dz + verts[e->v1][2]);
            GXColor1u32(0xFF0004);
            GXPosition3f32(dx + verts[e->v0][0], dy + verts[e->v0][1], dz + verts[e->v0][2]);
            GXColor1u32(0xFF0004);
        }
        if (e->type & 1) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(verts[e->v1][0], verts[e->v1][1], verts[e->v1][2]);
            GXColor1u32(0xFF0004);
            GXPosition3f32(verts[e->v0][0], verts[e->v0][1], verts[e->v0][2]);
            GXColor1u32(0xFF0004);
            GXPosition3f32(dx + verts[e->v0][0], dy + verts[e->v0][1], dz + verts[e->v0][2]);
            GXColor1u32(0xFF0004);
            GXPosition3f32(dx + verts[e->v1][0], dy + verts[e->v1][1], dz + verts[e->v1][2]);
            GXColor1u32(0xFF0004);
        }
        e++;
    }
}

