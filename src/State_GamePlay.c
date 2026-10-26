#include "common.h"


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

