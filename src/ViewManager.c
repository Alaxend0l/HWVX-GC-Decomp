#include "common.h"


void ViewManager_SetSinglePlayer(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    Gfx_SetRenderWindow(0);
    g_CurrentViews |= 1 << 0;
    g_viewFormat = 1;
    g_CurrentViews &= ~(1 << 1);
    g_CurrentViews &= ~(1 << 2);
    g_CurrentViews &= ~(1 << 3);
    g_CurrentViews &= ~(1 << 4);
}
