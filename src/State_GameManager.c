#include "common.h"


void StateGameManager_RefreshMCPTextures(void) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    void Gfx_UnLoadTexture(u16 id);
    void DataPack_RefreshTextures(void *pack);
    void *pack;
    u32 i;

    pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
    if (pack != NULL) {
        for (i = 0; i < M2C_FIELD(pack, u32 *, 0x2C); i++) {
            Gfx_UnLoadTexture(M2C_FIELD(pack, u16 **, 0x30)[i]);
        }
        DataPack_RefreshTextures(pack);
    }
}

s32 StateGameManager_Init(void) {
    u8 *GameSettings_GetMCP(void);
    extern struct { u8 *mcp; s32 unk4; } g_GameManagerState;
    extern s32 g_gameDifficulty;
    void ShowStats_Init(void);
    void ProcessManager_Init(void);
    void TimeHandler_Heartbeat(void);
    void Input_Init(void);
    void Input_Heartbeat(void);
    void SoundManager_Init(void);
    void SoundManager_Heartbeat(void);
    void InterfaceManager_Init(void);
    void InterfaceManager_Heartbeat(void);
    void InterfaceManager_Render(void);
    void ThingManager_DARYL_Init(void);
    void ThingManager_DARYL_Heartbeat(void);
    void ThingManager_DARYL_Render(void);
    void LidHandler_Init(void);
    void LidHandler_Heartbeat(void);
    void CameraManager_Init(void);
    void PlayerManager_Init(void);
    void Grid_Init(Vector4 *min, Vector4 *max, s32 x, s32 z);
    void *CameraManager_GetNewCamera(void);
    void GameSetting_Init(void);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void *camera = NULL;
    Vector4 min;
    Vector4 max;

    ShowStats_Init();
    ProcessManager_Init();
    ProcessManager_Add("TimeHandler", NULL, TimeHandler_Heartbeat, 0, 0);
    ProcessManager_Add("Input", Input_Init, Input_Heartbeat, 0, 1);
    ProcessManager_Add("SoundManager", SoundManager_Init, SoundManager_Heartbeat, 0, 1);
    ProcessManager_Add("Interface", InterfaceManager_Init, InterfaceManager_Heartbeat, (s32)InterfaceManager_Render, 1);
    ProcessManager_Add("ThingManager", ThingManager_DARYL_Init, ThingManager_DARYL_Heartbeat, (s32)ThingManager_DARYL_Render, 0);
    ProcessManager_Add("LidHandler", LidHandler_Init, LidHandler_Heartbeat, 0, 1);
    CameraManager_Init();
    PlayerManager_Init();
    min.x = min.y = min.z = -500.0f;
    max.x = max.y = max.z = 500.0f;
    Grid_Init(&min, &max, 0x10, 0x10);
    if (M2C_FIELD(GetCameraManager(), u8 *, 0x640) == 0) {
        camera = CameraManager_GetNewCamera();
    }
    ViewManager_SetCamera(camera, 0);
    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        GameSettings_SetCodeVariable(0xC8, 0, g_gameDifficulty);
        GameSettings_SetCodeVariable(0x198, 0, 0);
        g_GameManagerState.unk4 = M2C_FIELD(g_GameManagerState.mcp, s32 *, 0x2C) - 1;
    }
    if (g_GameManagerState.mcp == NULL) {
        GameSetting_Init();
        if ((g_GameManagerState.mcp = GameSettings_GetMCP()) == NULL) {
            StateManager_QueueChange(0xB);
            return 1;
        }
        if (*g_GameManagerState.mcp != 0) {
            StateManager_QueueChange(0xB);
            return 1;
        }
        g_GameManagerState.unk4 = 0;
    }
    return 1;
}

s32 StateGameManager_Heartbeat(void) {
    typedef struct { s32 name; s32 script; } PackRef;
    typedef struct { u8 type; void *movie; PackRef *pack; } Step;
    typedef struct { u8 pad0[4]; s32 menuName; u8 pad8[0x20]; s32 menuFile; u32 numSteps; Step *steps; } MCP;
    extern struct { MCP *mcp; u32 step; } g_GameManagerState;
    extern s8 g_bInterfacePlaying;
    extern s32 g_interfacePackID;
    void StateManager_GotoState(s32);
    void StateMovie_SetMovieToPlay(void *);
    s32 FileStreaming_LoadDataPack(s32, s32, s32, s32);
    void *FileStreaming_InitializeDataPack(s32, s32, s32);
    void *DataPack_GetScript(void *, s32);
    void InterfaceManager_Create(void *, s32, void (*)(void *));
    void StateGameManager_InterfaceHandler(void *);
    void ShowStats_StartFrame(void);
    void ViewManager_PrepForRender(void);
    void ProcessManager_Heartbeat(s32);
    void ShowStats_BeginScene(void);
    void ProcessManager_Render(s32);
    void ShowStats_EndScene(void);
    u32 InterfaceManager_GetTotalActiveInterfaces(void);
    void FileStreaming_ReleaseDataPack(s32, s32);
    void GameSettings_LoadDefaultMemoryCardData(void);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void StateTitleScreen_SetMenuFile(s32, s32);
    s8 loadDefaults = 0;
    Step *step;
    void *pack;

    if (g_GameManagerState.mcp == NULL) {
        StateManager_GotoState(0xB);
        return 1;
    }
    if (g_GameManagerState.step < g_GameManagerState.mcp->numSteps) {
        step = &g_GameManagerState.mcp->steps[g_GameManagerState.step];
        switch (step->type) {
        case 0:
            if (step->movie != NULL) {
                StateMovie_SetMovieToPlay(step->movie);
                StateManager_QueueChange(3);
            }
            break;
        case 1:
            loadDefaults = 1;
        case 4:
            if (!g_bInterfacePlaying) {
                g_interfacePackID = FileStreaming_LoadDataPack(step->pack->name, 1, step->pack->script, 0);
                if ((pack = FileStreaming_InitializeDataPack(g_interfacePackID, 1, 1)) != NULL) {
                    InterfaceManager_Create(DataPack_GetScript(pack, step->pack->script), 0, StateGameManager_InterfaceHandler);
                    g_bInterfacePlaying = 1;
                }
                if (g_GameManagerState.step == g_GameManagerState.mcp->numSteps - 1) {
                    FileStreaming_LoadDataPack(g_GameManagerState.mcp->menuName, 1, g_GameManagerState.mcp->menuFile, 0);
                }
            }
            ShowStats_StartFrame();
            ViewManager_PrepForRender();
            ProcessManager_Heartbeat(0);
            ShowStats_BeginScene();
            ProcessManager_Render(1);
            ShowStats_EndScene();
            if (!g_bInterfacePlaying && InterfaceManager_GetTotalActiveInterfaces() == 0) {
                FileStreaming_ReleaseDataPack(g_interfacePackID, 1);
                g_interfacePackID = -1;
                if (loadDefaults) {
                    GameSettings_LoadDefaultMemoryCardData();
                }
            } else {
                g_bInterfacePlaying = 1;
            }
            break;
        }
        if (!g_bInterfacePlaying && GameSettings_GetCodeVariable(0x198, 0) == 0) {
            if (loadDefaults) {
                GameSettings_LoadDefaultMemoryCardData();
            }
            g_GameManagerState.step++;
        }
    } else {
        StateTitleScreen_SetMenuFile(g_GameManagerState.mcp->menuName, g_GameManagerState.mcp->menuFile);
        StateManager_QueueChange(2);
    }
    return 1;
}

s32 StateGameManager_Shutdown(void) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    void FileStreaming_HeartBeat(void);
    void ShowStats_Release(void);
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);

    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        FileStreaming_ReleaseAllDataPacks(1, 1);
    }
    FileStreaming_HeartBeat();
    ShowStats_Release();
    return 1;
}

void StateGameManager_Release(void) {
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);
    FileStreaming_ReleaseAllDataPacks(3, 1);
}

s32 StateGameManager_GetProgScript(u32 index) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    extern struct { u8 *mcp; s32 unk4; } g_GameManagerState;

    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        if (g_GameManagerState.mcp != NULL) {
            return M2C_FIELD(M2C_FIELD(g_GameManagerState.mcp, u8 **, 0x24), s32 *, 8);
        }
        return 0;
    }
    if (g_GameManagerState.mcp == NULL || index >= M2C_FIELD(g_GameManagerState.mcp, u32 *, 0x34)) {
        return 0;
    }
    return M2C_FIELD(g_GameManagerState.mcp, s32 **, 0x38)[index];
}

void StateGameManager_InterfaceHandler(void *event) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    extern u8 g_bInterfacePlaying;
    switch (M2C_FIELD(event, u16 *, 4)) {
    case 5:
        if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
            StateManager_QueueChange(0xB);
        }
        g_bInterfacePlaying = 0;
        break;
    }
}

