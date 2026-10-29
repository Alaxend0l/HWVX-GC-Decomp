#include "common.h"

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);
void GXSetArray(s32 attr, void *data, u8 stride);
void GXSetChanCtrl(s32 chan, u8 enable, s32 amb_src, s32 mat_src, u32 light_mask, s32 diff_fn, s32 attn_fn);
void DCFlushRange(void *addr, u32 nBytes);

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

