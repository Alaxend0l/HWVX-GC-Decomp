#include "common.h"

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);
void GXSetArray(s32 attr, void *data, u8 stride);
void GXSetChanCtrl(s32 chan, u8 enable, s32 amb_src, s32 mat_src, u32 light_mask, s32 diff_fn, s32 attn_fn);
void DCFlushRange(void *addr, u32 nBytes);
void GXSetChanMatColor(s32 chan, GXColor mat_color);
void GXSetChanAmbColor(s32 chan, GXColor amb_color);

extern GXColor white;
extern GXColor black;

extern u32 m_VtxDescFlag;
extern u32 *m_vertexBuffer;
extern u32 m_pointLightMask;

void Render_UseLighting(u8 enable) {
    if (enable) {
        GXSetChanCtrl(4, 1, 0, 0, m_pointLightMask | 1, 2, 2);
    } else {
        GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
    }
}

void SetVertexBuffer(u32 *vb) {
    u8 *data;

    if (vb != m_vertexBuffer) {
        data = (u8 *)(vb + 4);
        m_vertexBuffer = vb;
        GXSetArray(9, data, 0xC);
        GXSetArray(0xD, data + vb[0], 8);
        if (vb[1] != 0) {
            GXSetArray(0xB, data + vb[1], 4);
        }
        if (vb[2] != 0) {
            GXSetArray(0xA, data + vb[2], 0xC);
        }
        DCFlushRange(vb, vb[3]);
    }
}

void SetTriVtxDescIdx(void) {
    if (m_VtxDescFlag != 0x100) {
        m_VtxDescFlag = 0x100;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriVtxDescIdxAlpha(void) {
    if (m_VtxDescFlag != 0x400) {
        m_VtxDescFlag = 0x400;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

