#include "common.h"

Vector4 g_carPos[80];
f32 g_carRot[80];
Vector4 g_carCamPos;

#pragma push
#pragma use_lmw_stmw on
s32 StateRenderTest_Init(void) {
    extern s32 g_pCamera;
    extern s8 g_cameraManagerPause;
    extern s32 pCar;
    extern s16 g_dirt;
    extern s32 g_debugDataFlag;
    void Geometry_Init(s8 clear);
    void Gfx_SetEnviroMapMode(s32 mode);
    void CameraManager_Init(void);
    s32 CameraManager_GetNewCamera(void);
    void Gfx_SetProjectionMatrix(f32 fov);
    void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
    void Gfx_SetWorldMatrix(void *mtx);
    void Gfx_StateTextureBlendMode(s32 mode);
    s32 Geometry_Get(char *name);
    void ProcessManager_Init(void);
    void ProcessManager_Add(char *name, void (*init)(void), void (*update)(void), s32 arg, u32 flags);
    void TimeHandler_Heartbeat(void);
    void Input_Init(void);
    void Input_Heartbeat(void);
    s32 Gfx_LoadTexture(char *name);
    s32 Gfx_FontManager_AddFont(s32, s32, f32, f32, f32);
    void Gfx_SetDebugFont(s32 font);
    f32 RandomNumberGenerator_GetRandomFloat(f32 min, f32 max);
    s32 j;
    s32 i;

    Geometry_Init(1);
    Gfx_SetEnviroMapMode(1);
    CameraManager_Init();
    g_pCamera = CameraManager_GetNewCamera();
    g_cameraManagerPause = 1;
    Gfx_SetProjectionMatrix(90.0f);
    Gfx_SetCameraMatrix(NULL, NULL, NULL, NULL);
    Gfx_SetWorldMatrix(NULL);
    Gfx_StateTextureBlendMode(2);
    pCar = Geometry_Get("car");
    ProcessManager_Init();
    ProcessManager_Add("TimeHandler", NULL, TimeHandler_Heartbeat, 0, 0);
    ProcessManager_Add("Input", Input_Init, Input_Heartbeat, 0, 0);
    Gfx_SetDebugFont(Gfx_FontManager_AddFont(Gfx_LoadTexture("font36"), 1, 18.285f, 18.285f, 18.285f));
    g_dirt = Gfx_LoadTexture("dirt");
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 10; i++) {
            g_carPos[j * 10 + i].x = i * 15 - 25.0f;
            g_carPos[j * 10 + i].z = 60.0f + j * 15;
            g_carRot[j * 10 + i] = RandomNumberGenerator_GetRandomFloat(0.0f, 6.2831855f);
        }
    }
    g_carCamPos.x = 50.0f;
    g_carCamPos.y = -20.0f;
    g_carCamPos.z = 120.0f;
    g_debugDataFlag = 100;
    return 1;
}
#pragma pop

s32 StateRenderTest_Shutdown(void) {
    return 1;
}
