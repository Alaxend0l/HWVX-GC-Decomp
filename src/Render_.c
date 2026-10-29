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

void Render_ClearScreen(void) {

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

