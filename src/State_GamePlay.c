#include "common.h"
#include "CarA.h"

typedef struct GPAIScripts {
    u8 pad0[0x90];
    u32 scripts[4];         /* 0x90 one per difficulty */
} GPAIScripts;

typedef struct GPItem {
    u8 pad0[0x30];
    struct {
        u8 pad0[0x20];
        s32 count;          /* 0x20 */
    } *info;                /* 0x30 */
} GPItem;

/* A car placed in the world (World_GetTileCar), 0x60 bytes. */
typedef struct GPTileCar {
    Vector4 pos;            /* 0x00 */
    f32 rotX;               /* 0x10 */
    f32 rotY;               /* 0x14 */
    f32 rotZ;               /* 0x18 */
    u8 pad1C[8];
    GPAIScripts *aiScripts; /* 0x24 */
    CarA *creature;         /* 0x28 */
    u32 mode;               /* 0x2C */
    u8 pad30[4];
    u8 unk34;               /* 0x34 */
    u8 port;                /* 0x35 1-4: human player slot */
    u8 pad36[2];
    f32 nearDist;           /* 0x38 */
    f32 farDist;            /* 0x3C */
    u32 scripts[4];         /* 0x40 one per difficulty */
    u8 pad50[4];
    GPItem *item;           /* 0x54 */
    u8 pad58[7];
    u8 unk5F;               /* 0x5F */
} GPTileCar;

typedef struct GPCreatureParams {
    Vector4 pos;            /* 0x00 */
    Vector4 quat;           /* 0x10 */
    Vector4 vel;            /* 0x20 */
    GPAIScripts *aiScripts; /* 0x30 */
    u8 pad34[8];
    void *player;           /* 0x3C */
} GPCreatureParams;

typedef struct GPAIPlayerDesc {
    u32 script;
    f32 nearSq;
    f32 farSq;
    u8 unkC;
    u8 unkD;
} GPAIPlayerDesc;

typedef struct GPSpeakerSound {
    u8 type;                /* 0x00 */
    u8 pad1[7];
    u8 unk8[4];             /* 0x08 */
    u8 unkC[8];             /* 0x0C */
    s32 unk14;              /* 0x14 */
    u8 *unk18;              /* 0x18 */
} GPSpeakerSound;

typedef struct GPTileSpeaker {
    Vector4 pos;            /* 0x00 */
    u8 pad10[4];
    GPSpeakerSound *sound;  /* 0x14 */
    u8 pad18[4];
    u32 mode;               /* 0x1C */
    u8 pad20[0xC];
    u32 emitter;            /* 0x2C */
} GPTileSpeaker;

typedef struct GPTilePath {
    u8 pad0[4];
    u32 unk4;               /* 0x04 */
    u32 unk8;               /* 0x08 */
    u32 unkC;               /* 0x0C */
    CarA *thing;            /* 0x10 */
    u32 unk14;              /* 0x14 */
    u8 pad18;
    u8 hasSpeed;            /* 0x19 */
    u8 pad1A[2];
    f32 speed;              /* 0x1C */
} GPTilePath;


void ExitStateGamePlay(s32 arg0) {
    extern s8 g_bExitStateGamePlay;
    extern s32 g_targetGameState;
    g_targetGameState = arg0;
    g_bExitStateGamePlay = 1;
}

s32 StateGamePlay_Heartbeat(void) {
    extern Vector4 g_lastCamPos;
    extern u32 g_pauseState;
    extern u32 g_StopInput;
    extern u8 g_bFreezeWorld;
    extern u32 g_DemoModeCounter;
    extern u32 g_iDeltaTimeMS;
    extern s32 g_DemoModeSwitchVehicleTime;
    extern u32 g_DemoModeCurrentPlayer;
    extern s32 g_DemoModeSwitchCameraTime;
    extern s8 g_bExitStateGamePlay;
    extern s32 g_targetGameState;
    extern s32 g_gamePlayCallback;
    extern s8 g_bRenderWorld;
    void StateGamePlay_Pause(s32 pause);
    void ShowStats_StartFrame(void);
    void ShowStats_BeginScene(void);
    void ShowStats_EndScene(void);
    void Gfx_SetCameraMatrix(Vector4 *pos, void *a, void *b, void *c);
    void Gfx_GetCameraPosition(Vector4 *pos);
    void Gfx_ScreenOverlay(void *colour, s32 arg);
    void ProcessManager_Heartbeat(u32 paused);
    void ProcessManager_Render(s32 renderWorld);
    void ViewManager_PrepForRender(void);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    u8 PlayerManager_PlayerHumanCount(void);
    u8 PlayerManager_PlayerCount(void);
    void *PlayerManager_GetHumanPlayerByIndex(u8 index);
    void *PlayerManager_GetPlayerByIndex(u8 index);
    s8 Input_KeyPress(u32 chan, u8 key);
    u32 InterfaceManager_Create(void *arg0, s32 arg1, void (*handler)(void *msg));
    void InterfaceManager_SetInput(u32 id, s32 input);
    void InterfaceManager_ReleaseAllUnpausedInterfaces(void);
    void InterfaceManager_ReleaseInterface(u32 id);
    void Wheel_PlayAllAirborne(void);
    u32 CameraManager_GetActiveCameras(void);
    void **CameraManager_GetCameraByIndex(s32 index);
    void Camera_SetPlayer(void **camera, void *player);
    void StateManager_QueueChange(s32 state);
    s8 IsLoadingThreadRunning(void);
    void GamePlay_Interface(void *msg);
    static s32 cameraIndex;
    static u32 pauseInterfaceID = 0xFFFFFFFF;
    static s8 bControllerRemoved;
    s8 removed;
    s32 input;
    u32 key;
    s8 pressed;
    u32 colour[2];

    pressed = 0;
    removed = 0;
    ShowStats_StartFrame();
    Gfx_SetCameraMatrix(&g_lastCamPos, NULL, NULL, NULL);
    ProcessManager_Heartbeat(g_pauseState);
    ViewManager_PrepForRender();
    Gfx_GetCameraPosition(&g_lastCamPos);
    if (GameSettings_GetCodeVariable(0x198, 0) == 0) {
        u32 code;
        {
            u32 i;
            u32 count;
            void *player;

            count = PlayerManager_PlayerHumanCount();
            for (i = 0; i < count; i++) {
                player = PlayerManager_GetHumanPlayerByIndex(i);
                if (Input_KeyPress(M2C_FIELD(player, u32 *, 0xC8), 0)) {
                    input = M2C_FIELD(player, u32 *, 0xC8);
                    pressed = 1;
                    break;
                }
            }
        }
        if (GameSettings_GetCodeVariable(0x259, 0) == 0) {
            input = 0;
            pressed = 1;
        }
        if (g_StopInput != 0) {
            input = 0;
            pressed = 1;
        }
        if ((code = GameSettings_GetCodeVariable(0xD4, 0)) != 0) {
            input = code - 1;
            pressed = 1;
            removed = 1;
        }
        if (pressed && GameSettings_GetCodeVariable(0x19B, 0)) {
            if (g_pauseState == 0 && M2C_FIELD(M2C_FIELD(g_worldData.world, u8 **, 0x9C), void **, 0xCC) != NULL) {
                g_pauseState = 1;
                StateGamePlay_Pause(1);
                g_bFreezeWorld = 1;
                bControllerRemoved = removed;
                pauseInterfaceID = InterfaceManager_Create(M2C_FIELD(M2C_FIELD(g_worldData.world, u8 **, 0x9C), void **, 0xCC), 0, GamePlay_Interface);
                InterfaceManager_SetInput(pauseInterfaceID, input);
                GameSettings_SetCodeVariable(0xD5, 0, input + 1);
            } else if (M2C_FIELD(M2C_FIELD(g_worldData.world, u8 **, 0x9C), void **, 0xCC) != NULL && removed && !bControllerRemoved) {
                s32 oldInput = GameSettings_GetCodeVariable(0xD5, 0) - 1;
                bControllerRemoved = removed;
                InterfaceManager_ReleaseAllUnpausedInterfaces();
                pauseInterfaceID = InterfaceManager_Create(M2C_FIELD(M2C_FIELD(g_worldData.world, u8 **, 0x9C), void **, 0xCC), 0, GamePlay_Interface);
                InterfaceManager_SetInput(pauseInterfaceID, oldInput);
            }
            Wheel_PlayAllAirborne();
        } else if (!removed) {
            bControllerRemoved = 0;
        }
        if (g_pauseState != 0) {
            if (removed) {
                InterfaceManager_SetInput(pauseInterfaceID, -2);
            } else {
                s32 oldInput = GameSettings_GetCodeVariable(0xD5, 0) - 1;
                InterfaceManager_SetInput(pauseInterfaceID, oldInput);
            }
        }
    } else {
        if (g_DemoModeCounter > g_iDeltaTimeMS) {
            g_DemoModeCounter -= g_iDeltaTimeMS;
        } else {
            g_DemoModeCounter = 0;
        }
        if (g_DemoModeCounter < g_DemoModeSwitchVehicleTime && CameraManager_GetActiveCameras() == 1) {
            void *player;
            void **camera;

            g_DemoModeSwitchVehicleTime = g_DemoModeCounter - 8000;
            if (g_DemoModeSwitchVehicleTime < 0) {
                g_DemoModeSwitchVehicleTime = 0;
            }
            g_DemoModeCurrentPlayer++;
            if (g_DemoModeCurrentPlayer >= PlayerManager_PlayerCount()) {
                g_DemoModeCurrentPlayer = 0;
            }
            player = PlayerManager_GetPlayerByIndex(g_DemoModeCurrentPlayer);
            camera = CameraManager_GetCameraByIndex(0);
            if (*camera != NULL && M2C_FIELD(*camera, u32 *, 0x68) < 0xFFFFFFFF) {
                InterfaceManager_ReleaseInterface(M2C_FIELD(*camera, u32 *, 0x68));
                M2C_FIELD(*camera, u32 *, 0x68) = 0xFFFFFFFF;
            }
            Camera_SetPlayer(camera, player);
        }
        if (g_DemoModeCounter < g_DemoModeSwitchCameraTime) {
            u32 i;
            u32 count;
            u8 *player;

            g_DemoModeSwitchCameraTime = g_DemoModeCounter - 10000;
            if (g_DemoModeSwitchCameraTime < 0) {
                g_DemoModeSwitchCameraTime = 0;
            }
            count = PlayerManager_PlayerCount();
            for (i = 0; i < count; i++) {
                player = PlayerManager_GetPlayerByIndex(i);
                player[0x31] = cameraIndex++;
            }
        }
        if (g_DemoModeCounter == 0) {
            g_targetGameState = 12;
            g_bExitStateGamePlay = 1;
        } else {
            for (key = 0; key < 20; key++) {
                if (Input_KeyPress(0xFFFFFFFF, key)) {
                    g_targetGameState = 12;
                    g_bExitStateGamePlay = 1;
                }
            }
        }
    }
    ShowStats_BeginScene();
    ProcessManager_Render(g_bRenderWorld);
    if (g_pauseState != 0) {
        colour[1] = 0;
        colour[0] = 0;
        Gfx_ScreenOverlay(colour, 0);
    }
    ShowStats_EndScene();
    if (g_bExitStateGamePlay && !IsLoadingThreadRunning()) {
        StateManager_QueueChange(g_targetGameState);
    }
    switch (g_gamePlayCallback) {
    case 0:
        break;
    case 1:
        g_targetGameState = 2;
        g_bExitStateGamePlay = 1;
        break;
    case 2:
        g_targetGameState = 5;
        g_bExitStateGamePlay = 1;
        break;
    case 3:
        g_gamePlayCallback = 0;
        g_pauseState = 0;
        StateGamePlay_Pause(0);
        if (pauseInterfaceID < 0xFFFFFFFF) {
            InterfaceManager_ReleaseInterface(pauseInterfaceID);
        }
        GameSettings_SetCodeVariable(0xD5, 0, 0);
        g_bFreezeWorld = 0;
        break;
    }
    return 1;
}

void StateGamePlay_Pause(s32 pause) {
    extern u8 g_bFreezeWorld;
    void SoundManager_Pause(u8 pause);
    void Input_ForceFeedbackPause(u8 pause);
    void InterfaceManager_PauseAllInterface(u8 pause);
    u8 bPause;

    CameraManager_Pause(pause);
    bPause = pause;
    SoundManager_Pause(bPause);
    Input_ForceFeedbackPause(bPause);
    InterfaceManager_PauseAllInterface(bPause);
    g_bFreezeWorld = 1;
}

s32 StateGamePlay_Shutdown(void) {
    extern s8 g_iTimeShift;
    extern u8 g_bFreezeWorld;
    extern void (*DestroyMusic)(void);
    void Trigger_PauseTACSystem(s8 arg0);
    void Gfx_BeginScene(void);
    void Gfx_EndScene(void);
    void Gfx_FlipBuffers(void);
    void NavHolder_Release(void);
    void CameraManager_ReleaseAllEmitters(void);
    void ThingManager_DARYL_Release(void);
    void MatrixCache_Init(void);
    void Grid_Release(void);
    void SongManager_StopBKSong(void);
    void MovieManager_Destroy(void);
    void ShowStats_Release(void);
    void PlayerManager_Release(void);
    void World_Unload(void);
    void ViewManager_SetSinglePlayer(void);
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    void Gfx_ClearScreen(void);

    Trigger_PauseTACSystem(1);
    while ((s8)IsLoadingThreadRunning()) {
    }
    g_iTimeShift = 0;
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    NavHolder_Release();
    CameraManager_ReleaseAllEmitters();
    ThingManager_DARYL_Release();
    MatrixCache_Init();
    Grid_Release();
    DestroyMusic();
    SongManager_StopBKSong();
    MovieManager_Destroy();
    ShowStats_Release();
    PlayerManager_Release();
    Trigger_PauseTACSystem(0);
    World_Unload();
    ViewManager_SetSinglePlayer();
    GameSettings_SetCodeVariable(0x19B, 0, 1);
    g_bFreezeWorld = 0;
    Gfx_ClearScreen();
    return 1;
}

void GamePlay_Interface(void *msg) {
    void GamePlay_Startup(void);
    extern struct {
        void (*handleInput)(void *msg);
        void (*update)(void *msg);
    } g_handleInterfaceGP;

    switch (M2C_FIELD(msg, u16 *, 4)) {
    case 3:
        GamePlay_Startup();
        if (g_handleInterfaceGP.handleInput != NULL) {
            g_handleInterfaceGP.handleInput(msg);
        }
        break;
    case 1:
        if (g_handleInterfaceGP.update != NULL) {
            g_handleInterfaceGP.update(msg);
        }
        break;
    case 2:
    default:
        if (g_handleInterfaceGP.handleInput != NULL) {
            g_handleInterfaceGP.handleInput(msg);
        }
        break;
    }
}

void GamePlay_Startup(void) {
    extern void GamePlay_PauseOptions_HandleInput(void *msg);
    extern struct {
        void (*handleInput)(void *msg);
        void (*update)(void);
    } g_handleInterfaceGP;

    g_handleInterfaceGP.handleInput = GamePlay_PauseOptions_HandleInput;
    g_handleInterfaceGP.update = NULL;
}

void GamePlay_PauseOptions_HandleInput(void *msg) {
    typedef struct {
        u8 unk0[4];
        u16 command;
        u16 value;
        u8 unk8[0x18];
        s32 result;
    } InterfaceMsg;
    extern s8 g_bExitStateGamePlay;
    extern s32 g_targetGameState;
    extern s32 g_gamePlayCallback;
    s32 ActionEndLevel(s32 *arg0);
    void StateProgressionManager_RestartCurrentLevel(void);
    static u16 endCommand;
    static s32 params[2];
    InterfaceMsg *m = (InterfaceMsg *)msg;

    switch (m->command) {
    case 3:
        endCommand = 0xFFFF;
        break;
    case 7:
        params[0] = 4;
        params[1] = m->value;
        m->result = 4;
        endCommand = 7;
        break;
    case 8:
        params[0] = 5;
        params[1] = m->value;
        m->result = 4;
        endCommand = 8;
        break;
    case 9:
        m->result = 4;
        endCommand = 9;
        break;
    case 10:
        m->result = 4;
        endCommand = 10;
        break;
    case 16:
        m->result = 4;
        endCommand = 16;
        break;
    case 5:
        switch (endCommand) {
        case 7:
        case 8:
            ActionEndLevel(params);
            break;
        case 9:
            g_targetGameState = 2;
            g_bExitStateGamePlay = 1;
            break;
        case 10:
            StateProgressionManager_RestartCurrentLevel();
            break;
        case 16:
            g_gamePlayCallback = 3;
            break;
        }
        break;
    }
}

s32 ActionPlayInterfacePack(u32 *pack) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    u32 InterfaceManager_Create(u8 *, s32, void *, void *, void *);
    void InterfaceDataPackRouter(void);
    void *data;
    u8 *script;
    u32 id;
    s32 result;

    id = pack[0];
    data = FileStreaming_InitializeDataPack((void *)FileStreaming_GetPackID((char *)(pack + 1), 1, id), 1, 1);
    if (data == NULL) {
        result = 0;
    } else {
        script = DataPack_GetScript(data, id);
        if (script == NULL) {
            result = 0;
        } else {
            *(char **)((u8 *)g_InterfacePackData + g_NumInterfacesPlaying * sizeof(InterfacePackEntry)) = (char *)(pack + 1);
            g_InterfacePackData[g_NumInterfacesPlaying].id = id;
            g_InterfacePackData[g_NumInterfacesPlaying].unk8 =
                InterfaceManager_Create(script, 0, InterfaceDataPackRouter, g_InterfacePackData + g_NumInterfacesPlaying, pack + 1);
            g_NumInterfacesPlaying++;
            result = 1;
        }
    }
    return result;
}

s32 ActionEndInterfacePack(u32 *pack) {
    int strcmp(const char *, const char *);
    u32 i;

    for (i = 0; i < g_NumInterfacesPlaying; i++) {
        if (pack[0] == g_InterfacePackData[i].id && strcmp(g_InterfacePackData[i].name, (char *)(pack + 1)) == 0) {
            InterfaceManager_EndInterface(g_InterfacePackData[i].unk8);
            for (; i < g_NumInterfacesPlaying - 1; i++) {
                g_InterfacePackData[i] = g_InterfacePackData[i + 1];
            }
            g_NumInterfacesPlaying--;
            return 1;
        }
    }
    return 0;
}

void InterfaceHasEnded(u32 handle) {
    u32 i;

    for (i = 0; i < g_NumInterfacesPlaying; i++) {
        if (handle == g_InterfacePackData[i].unk8) {
            for (; i < g_NumInterfacesPlaying - 1; i++) {
                g_InterfacePackData[i] = g_InterfacePackData[i + 1];
            }
            g_NumInterfacesPlaying--;
        }
    }
}

void InterfaceDataPackRouter(void *arg0) {
    M2C_UNK ArmageddonDropShip(void *);
    M2C_UNK GSI_VehicleItemSelection(void *);
    M2C_UNK InterfaceManager_SetCallBackRoutine(s32, M2C_UNK (*)(void *));
    s32 GameSettings_GetGameType(void);

    switch (M2C_FIELD(arg0, u16 *, 6)) {
    case 0:
        InterfaceManager_SetCallBackRoutine(M2C_FIELD(arg0, s32 *, 0), NULL);
        break;
    case 1:
        break;
    case 2:
        if (GameSettings_GetGameType() == 0) {
            GSI_VehicleItemSelection(arg0);
            InterfaceManager_SetCallBackRoutine(M2C_FIELD(arg0, s32 *, 0), GSI_VehicleItemSelection);
        } else if (GameSettings_GetGameType() == 1) {
            ArmageddonDropShip(arg0);
            InterfaceManager_SetCallBackRoutine(M2C_FIELD(arg0, s32 *, 0), ArmageddonDropShip);
        } else {
            InterfaceManager_SetCallBackRoutine(M2C_FIELD(arg0, s32 *, 0), NULL);
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    default:
        InterfaceManager_SetCallBackRoutine(M2C_FIELD(arg0, s32 *, 0), NULL);
        break;
    }
}

void InterfaceInputAnimator_Release(void) {
    static const char name[] = "InterfaceInput";
    ProcessManager_Release((char *)name);
}

void InterfaceInputAnimator_SetCounter(s32 which, f32 value) {
    extern f32 g_LR_Counter;
    extern f32 g_UD_Counter;
    switch (which) {
    case 0:
        g_LR_Counter = value;
        break;
    case 1:
        g_UD_Counter = value;
        break;
    }
}

void InterfaceInputAnimator_Count(s32 arg0, s32 arg1, f32 farg0) {
    extern s32 g_LR_Direction;
    extern f32 g_LR_Max_Min;
    extern s32 g_UD_Direction;
    extern f32 g_UD_Max_Min;
    switch (arg0) {                                 /* irregular */
    case 0:
        g_LR_Max_Min = farg0;
        g_LR_Direction = arg1;
        return;
    case 1:
        g_UD_Max_Min = farg0;
        g_UD_Direction = arg1;
        return;
    }
}

f32 InterfaceInputAnimator_GetLRCounter(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

f32 InterfaceInputAnimator_GetUDCounter(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

