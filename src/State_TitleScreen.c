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

void TitleScreen_Interface(InterfaceMsg *msg);
#pragma push
#pragma readonly_strings on
s32 StateTitleScreen_Init(s32 prevState) {
    extern void *g_pTitleDataPack;
    extern void *g_pInterfaceScript;
    extern s8 g_bPlayingFMV;
    extern void *g_mainMenuFile;
    extern s32 g_mainMenuFileID;
    extern s8 g_clearTextureCache;
    void Gfx_SetProjectionMatrix(f32 fov);
    void Gfx_StateTextureBlendMode(s32);
    void Gfx_BeginScene(void);
    void Gfx_EndScene(void);
    void Gfx_FlipBuffers(void);
    void GameSettings_ResetGame(void);
    void Gfx_TextureManager_Init(void);
    void StateGameManager_RefreshMCPTextures(void);
    void ShowStats_Init(void);
    void ProcessManager_Init(void);
    void ProcessManager_Add(char *name, void (*init)(void), void (*update)(void), s32 arg, u32 flags);
    void TimeHandler_Heartbeat(void);
    void Input_Init(void);
    void Input_Heartbeat(void);
    void SoundManager_Init(void);
    void SoundManager_Heartbeat(void);
    void InterfaceManager_Init(void);
    void InterfaceManager_Heartbeat(void);
    void InterfaceManager_Render(void);
    void ParticleManager_Init(void);
    void ParticleManager_Heartbeat(void);
    void ParticleManager_Render(void);
    void ShaderHolder_Heartbeat(void);
    void ThingManager_DARYL_Init(void);
    void ThingManager_DARYL_Heartbeat(void);
    void ThingManager_DARYL_Render(void);
    void LidHandler_Init(void);
    void LidHandler_Heartbeat(void);
    void DataPack_RefreshTextures(void *pack);
    void InterfaceManager_Create(void *, s32, void (*)(InterfaceMsg *));
    void *FileStreaming_LoadDataPack(void *, s32, s32, s32);
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    void *DataPack_GetScript(void *, s32);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void CameraManager_Init(void);
    void CameraManager_Pause(s32);
    void PlayerManager_Init(void);
    void PlayerManager_NewHumanPlayer(void);
    void Grid_Init(Vector4 *min, Vector4 *max, s32 x, s32 z);
    u8 *GetCameraManager(void);
    void *CameraManager_GetNewCamera(void);
    void ViewManager_SetCamera(void *camera, s32 view);
    void GameSettings_ResetPlayerVehicleItem(void);
    void *camera;
    void *script;
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));

    Gfx_SetProjectionMatrix(90.0f);
    Gfx_StateTextureBlendMode(2);
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    GameSettings_ResetGame();
    ShowStats_Init();
    Gfx_TextureManager_Init();
    StateGameManager_RefreshMCPTextures();
    ProcessManager_Init();
    ProcessManager_Add("TimeHandler", NULL, TimeHandler_Heartbeat, 0, 0);
    ProcessManager_Add("Input", Input_Init, Input_Heartbeat, 0, 1);
    if (prevState != 0xC && prevState != 3) {
        ProcessManager_Add("SoundManager", SoundManager_Init, SoundManager_Heartbeat, 0, 1);
    } else {
        ProcessManager_Add("SoundManager", NULL, SoundManager_Heartbeat, 0, 1);
    }
    ProcessManager_Add("Interface", InterfaceManager_Init, InterfaceManager_Heartbeat, (s32)InterfaceManager_Render, 1);
    ProcessManager_Add("Particle", ParticleManager_Init, ParticleManager_Heartbeat, (s32)ParticleManager_Render, 0);
    ProcessManager_Add("Shader", NULL, ShaderHolder_Heartbeat, 0, 0);
    ProcessManager_Add("ThingManager", ThingManager_DARYL_Init, ThingManager_DARYL_Heartbeat, (s32)ThingManager_DARYL_Render, 0);
    ProcessManager_Add("Lid Handler", LidHandler_Init, LidHandler_Heartbeat, 0, 1);
    switch (prevState) {
    case 3:
        DataPack_RefreshTextures(g_pTitleDataPack);
        InterfaceManager_Create(g_pInterfaceScript, 0, TitleScreen_Interface);
        g_bPlayingFMV = 0;
        g_pInterfaceScript = NULL;
        break;
    default:
        if (g_mainMenuFile == NULL) {
            return 0;
        }
        g_pTitleDataPack = FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack(g_mainMenuFile, 1, g_mainMenuFileID, 0), 1, 1);
        if (g_pTitleDataPack == NULL) {
            return 0;
        }
        script = DataPack_GetScript(g_pTitleDataPack, g_mainMenuFileID);
        if (script == NULL) {
            return 0;
        }
        InterfaceManager_Create(script, 0, TitleScreen_Interface);
        g_clearTextureCache = 0;
        if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
            GameSettings_SetCodeVariable(0xC8, 0, g_gameDifficulty);
            GameSettings_SetCodeVariable(0x198, 0, 0);
        }
        break;
    }
    CameraManager_Init();
    CameraManager_Pause(1);
    PlayerManager_Init();
    PlayerManager_NewHumanPlayer();
    min.x = min.y = min.z = -500.0f;
    max.x = max.y = max.z = 500.0f;
    Grid_Init(&min, &max, 0x10, 0x10);
    if (GetCameraManager()[0x640] == 0) {
        camera = CameraManager_GetNewCamera();
    }
    ViewManager_SetCamera(camera, 0);
    GameSettings_ResetPlayerVehicleItem();
    return 1;
}
#pragma pop

s32 StateTitleScreen_Shutdown(void) {
    M2C_UNK Gfx_EndScene();
    M2C_UNK Gfx_FlipBuffers();
    M2C_UNK InterfaceManager_ReleaseAllInterfaces();
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);
    extern s8 g_bPlayingFMV;
    extern void *g_pTitleDataPack;
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
    extern s8 g_clearTextureCache;
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
    if (mcp == NULL || mcp->demos->demoList == NULL) {
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

