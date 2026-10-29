#include "common.h"

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);

extern u32 m_VtxDescFlag;

void Render_ClearScreen(void) {

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

