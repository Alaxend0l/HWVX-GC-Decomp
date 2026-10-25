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

void ViewManager_SetTwoPlayerHorizontal(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4);
    g_CurrentViews &= ~(1 << 0);
    Gfx_SetRenderWindow(1);
    Gfx_SetRenderWindowSize(0, 0, 0x27F, 0xEF, 0);
    Gfx_SetRenderWindow(2);
    Gfx_SetRenderWindowSize(0, 0xF0, 0x27F, 0x1DF, 0);
    g_CurrentViews &= ~(1 << 3);
    g_viewFormat = 2;
    g_CurrentViews &= ~(1 << 4);
}

void ViewManager_SetThreePlayer(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4);
    g_CurrentViews &= ~(1 << 0);
    Gfx_SetRenderWindow(1);
    Gfx_SetRenderWindowSize(0, 0, 0x13F, 0xEF, 0);
    Gfx_SetRenderWindow(2);
    Gfx_SetRenderWindowSize(0x140, 0, 0x27F, 0xEF, 0);
    Gfx_SetRenderWindow(3);
    Gfx_SetRenderWindowSize(0, 0xF0, 0x13F, 0x1DF, 0);
    g_CurrentViews &= ~(1 << 4);
    g_viewFormat = 4;
}

void ViewManager_SetFourPlayer(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4);
    g_CurrentViews &= ~(1 << 0);
    Gfx_SetRenderWindow(1);
    Gfx_SetRenderWindowSize(0, 0, 0x13F, 0xEF, 0);
    Gfx_SetRenderWindow(2);
    Gfx_SetRenderWindowSize(0x140, 0, 0x27F, 0xEF, 0);
    Gfx_SetRenderWindow(3);
    Gfx_SetRenderWindowSize(0, 0xF0, 0x13F, 0x1DF, 0);
    Gfx_SetRenderWindow(4);
    Gfx_SetRenderWindowSize(0x140, 0xF0, 0x27F, 0x1DF, 0);
    g_viewFormat = 5;
}

