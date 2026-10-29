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
    u8 unk140[0x270 - 0x140];
    f32 fov;
    u8 unk274[0x2D0 - 0x274];
} GfxWindow;

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
extern void *FontData;
extern s32 __OSCurrHeap;

GfxWindow *GetRenderWindow(void);
void *memcpy(void *dst, const void *src, u32 n);
void *OSAllocFromHeap(s32 heap, u32 size);
BOOL OSInitFont(void *fontData);

void GXSetTevOp(s32 id, s32 mode);
void Render_StateFogEnable(s32 enable);

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
    f32 normal0[4];
    f32 normal1[3];
    s32 type;
} SilhouetteEdge;

f32 PSVECDotProduct(const f32 *a, const f32 *b);

extern GDL *g_pGDL;
extern RenderStats g_renderStats;
extern s32 g_debugTriCount;

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

