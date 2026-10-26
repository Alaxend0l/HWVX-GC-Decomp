#include "common.h"


void ShowStats_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void ShowStats_Release(void) {
    M2C_UNK Gfx_FontManager_ReleaseFont(s32);
    extern s32 debugFont;
    Gfx_FontManager_ReleaseFont(debugFont);
}

void ShowStats_StartFrame(void) {
    M2C_UNK Performance_Start();
    M2C_UNK Release_Performance_Get(s32);
    extern u8 g_timeBar[0x20];
    extern s32 g_timeBarOn;
    extern s32 g_pActiveTimeBar;

    g_timeBarOn = !g_timeBarOn;
    g_pActiveTimeBar = (s32)(g_timeBar + g_timeBarOn * 0x10);
    Performance_Start();
    Release_Performance_Get(g_pActiveTimeBar);
}

void ShowStats_BeginScene(void) {
    M2C_UNK Gfx_BeginScene();
    M2C_UNK Release_Performance_Diff(s32);
    M2C_UNK Release_Performance_Get(s32);
    extern s32 g_pActiveTimeBar;
    Release_Performance_Diff(g_pActiveTimeBar);
    Release_Performance_Get(g_pActiveTimeBar + 4);
    Gfx_BeginScene();
}

