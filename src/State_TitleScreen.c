#include "common.h"

typedef struct DemoSettings {
    u8 bRandom;
    u8 pad1[7];
    u32 *demoList;
} DemoSettings;

typedef struct MCPSettings {
    u8 pad0[0x24];
    DemoSettings *demos;
} MCPSettings;

MCPSettings *GameSettings_GetMCP(void);
void GameSettings_SetCodeVariable(s32 code, s32 arg, s32 value);
s32 GameSettings_GetDifficulty(void);
u32 RandomNumberGenerator_GetRandomInteger(u32 min, u32 max);
void StateProgressionManager_SetProgressionID(s32 id, s32 subID);
extern s32 g_gameDifficulty;

void StateTitleScreen_SetMenuFile(s32 arg0, s32 arg1) {
    extern s32 g_mainMenuFile;
    extern s32 g_mainMenuFileID;
    g_mainMenuFile = arg0;
    g_mainMenuFileID = arg1;
}

s32 StateTitleScreen_Shutdown(void) {
    M2C_UNK Gfx_EndScene();
    M2C_UNK Gfx_FlipBuffers();
    M2C_UNK InterfaceManager_ReleaseAllInterfaces();
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);
    extern u8 g_bPlayingFMV;
    extern s32 g_pTitleDataPack;
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    ShowStats_Release();
    if ((s8) g_bPlayingFMV == 0) {
        InterfaceManager_ReleaseAllInterfaces();
        FileStreaming_ReleaseAllDataPacks(1, 1);
        g_pTitleDataPack = 0;
    }
    return 1;
}

s32 StateTitleScreen_Heartbeat(void) {
    M2C_UNK ProcessManager_Heartbeat(M2C_UNK);
    M2C_UNK ProcessManager_Render(M2C_UNK);
    M2C_UNK ShowStats_EndScene();
    M2C_UNK TextureManager_ReleaseCache();
    M2C_UNK ViewManager_PrepForRender();
    M2C_UNK Wheel_PlayAllAirborne();
    extern u8 g_clearTextureCache;
    ShowStats_StartFrame();
    ViewManager_PrepForRender();
    ProcessManager_Heartbeat(0);
    ShowStats_BeginScene();
    ProcessManager_Render(1);
    Wheel_PlayAllAirborne();
    ShowStats_EndScene();
    if ((s8) g_clearTextureCache == 0) {
        TextureManager_ReleaseCache();
        g_clearTextureCache = 1;
    }
    return 1;
}

void TitleScreen_Interface(InterfaceMsg *msg) {
    extern s32 g_pInterfaceScript;
    extern u8 g_bPlayingFMV;
    s32 StateManager_QueueChange(s32 state);
    u32 GameSettings_GetCodeVariable(s32 code, s32 arg);
    s8 IsLoadingThreadRunning(void);
    switch (msg->type) {
    case 0:
        break;
    case 6:
        msg->result = 4;
        break;
    case 5:
        if (GameSettings_GetCodeVariable(0x198, 0)) {
            StateManager_QueueChange(0xB);
        }
        break;
    case 15:
        if (IsLoadingThreadRunning()) {
            msg->result = 0xB;
        }
        break;
    case 9:
        StateManager_QueueChange(2);
        break;
    case 17:
        g_pInterfaceScript = msg->value;
        g_bPlayingFMV = 1;
        msg->result = 4;
        break;
    }
}

void TitleScreen_RunDemoLoop(void) {
    static u32 progressionIndex;
    MCPSettings *mcp;
    u32 index;

    mcp = GameSettings_GetMCP();
    if (mcp != NULL) {
        if (mcp->demos->demoList == NULL) {
            index = 0;
            return;
        }
        if (mcp->demos->bRandom) {
            index = RandomNumberGenerator_GetRandomInteger(0, *mcp->demos->demoList);
        } else {
            index = progressionIndex++;
        }
        if (index >= *mcp->demos->demoList) {
            index = 0;
            progressionIndex = 1;
        }
        if (index < 6) {
            GameSettings_SetCodeVariable(0x12D, 0, 3);
        } else {
            GameSettings_SetCodeVariable(0x12D, 0, 2);
        }
        g_gameDifficulty = GameSettings_GetDifficulty();
        GameSettings_SetCodeVariable(0x198, 0, 1);
        GameSettings_SetCodeVariable(0x133, 0, 3);
        GameSettings_SetCodeVariable(0x131, 0, 1);
        GameSettings_SetCodeVariable(0xC8, 0, 3);
        StateProgressionManager_SetProgressionID(0, index);
    }
}

