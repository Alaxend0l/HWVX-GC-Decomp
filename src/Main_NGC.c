#include "common.h"


void ForceIPLReset(void) {
    extern s32 m_ilpreset;
    extern s32 m_reset;
    m_ilpreset = 1;
    m_reset = 1;
}

s32 Init_Display(void) {
    extern s32 g_viewID;
    s32 Gfx_CreateView(s32 a, s32 w, s32 h, s32 d, s32 e, s32 f);

    g_viewID = Gfx_CreateView(0, 640, 480, 16, 0, 0);
    return g_viewID >= 0;
}

void Main_Shutdown_NGC(void) {

}

void TextureManager_ReleaseCache(void) {

}

