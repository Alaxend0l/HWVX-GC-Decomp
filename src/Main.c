#include "common.h"

s32 Main_Init(void) {
    
    void TimeHandler_Init(void);
    void Init_Math3D(void);
    void Gfx_Create(void);
    s8 Init_Display(void);
    void Gfx_Destroy(void);
    void Input_Create(void);
    void Input_Init(void);
    void SoundManager_Create(void);
    void FileManager_Create(void);
    void DataPack_Initialize(void);
    void StateManager_Create(void);
    void CameraManager_Create(void);
    void ThingManager_DARYL_Create(void);
    s8 Geometry_Create(void);
    void RandomNumberGenerator_Init(void);
    void ParticleManager_Init(void);
    void Trail_Create(void);
    void World_Create(void);
    void ShaderHolder_Create(void);
    void MatrixCache_Create(void);
    void ProcessManager_Create(void);
    void ShowStats_Create(void);
    void Trigger_Create(void);
    void Action_Create(void);
    void VertexColorSeqManager_Create(void);
    void FileStreaming_Initialize(void);
    void VolumeManager_Create(void);
    void GSI_Init(void);
    void Gfx_TextureManager_Init(void);
    void InterfaceManager_Init(void);
    void GameSettings_GetGameInfo(GameInfo *info);
    void FileManager_SetAppInfo(const char *a, const char *b, const char *c, const char *d, const char *e);
    s32 StateManager_GotoState(s32 state);
    GameInfo info;

    TimeHandler_Init();
    Init_Math3D();
    Gfx_Create();
    if (!Init_Display()) {
        Gfx_Destroy();
        return 0;
    }
    Input_Create();
    Input_Init();
    SoundManager_Create();
    FileManager_Create();
    LogHandler_Create("MM_Log.txt", 1);
    DataPack_Initialize();
    StateManager_Create();
    CameraManager_Create();
    ThingManager_DARYL_Create();
    if (!Geometry_Create()) {
        return 0;
    }
    RandomNumberGenerator_Init();
    ParticleManager_Init();
    Trail_Create();
    World_Create();
    ShaderHolder_Create();
    MatrixCache_Create();
    ProcessManager_Create();
    ShowStats_Create();
    Trigger_Create();
    Action_Create();
    VertexColorSeqManager_Create();
    FileStreaming_Initialize();
    VolumeManager_Create();
    GSI_Init();
    Gfx_TextureManager_Init();
    InterfaceManager_Init();
    GameSettings_GetGameInfo(&info);
    FileManager_SetAppInfo(((const char **)&info)[0], ((const char **)&info)[1], ((const char **)&info)[2], ((const char **)&info)[3], ((const char **)&info)[4]);
    return StateManager_GotoState(12) != -1;
}


s32 Main_Loop(void) {
    extern s32 (*Heartbeat_Process)(void);
    s32 StateManager_QueuedState(void);
    s32 StateManager_GotoState(s32 state);
    s32 queued;

    queued = StateManager_QueuedState();
    if (queued != -1 && queued != StateManager_GotoState(queued)) {
        return 0;
    }
    return Heartbeat_Process();
}

s32 Main_Shutdown(void) {
    void PlayerManager_Destroy(void);
    void ThingManager_DARYL_Destroy(void);
    void Input_Destroy(void);
    void ShaderHolder_Destroy(void);
    void VertexColorSeqManager_Destroy(void);
    void MatrixCache_Destroy(void);
    void World_Destroy(void);
    void Geometry_Destroy(void);
    void StateManager_Destroy(void);
    void CameraManager_Destroy(void);
    void Trigger_Destroy(void);
    void Action_Destroy(void);
    void LogHandler_Destroy(void);
    void FileManager_Destroy(void);
    void Gfx_Destroy(void);
    void StateGameManager_Release(void);
    void FileStreaming_Destroy(void);
    void SoundManager_Destroy(void);
    void HeapDump(void);

    PlayerManager_Destroy();
    ThingManager_DARYL_Destroy();
    Input_Destroy();
    ShaderHolder_Destroy();
    VertexColorSeqManager_Destroy();
    MatrixCache_Destroy();
    World_Destroy();
    Geometry_Destroy();
    StateManager_Destroy();
    CameraManager_Destroy();
    Trigger_Destroy();
    Action_Destroy();
    LogHandler_Destroy();
    FileManager_Destroy();
    Gfx_Destroy();
    StateGameManager_Release();
    FileStreaming_Destroy();
    SoundManager_Destroy();
    HeapDump();
    return 1;
}
