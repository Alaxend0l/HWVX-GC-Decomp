

s32 Main_Init(void) {
    typedef struct {
        const char *a;
        const char *b;
        const char *c;
        const char *d;
        const char *e;
        u8 unk14[0xC];
    } GameInfo;
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
    FileManager_SetAppInfo(info.a, info.b, info.c, info.d, info.e);
    return StateManager_GotoState(12) != -1;
}
