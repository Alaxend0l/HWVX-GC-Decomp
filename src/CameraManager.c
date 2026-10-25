#include "common.h"


void *GetCameraManager(void) {
    extern u8 g_cameraManager[];
    return g_cameraManager;
}

s32 CameraManager_Create(void) {
    return 1;
}

void CameraManager_Destroy(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

u8 CameraManager_GetActiveCameras(void) {
    extern u8 g_cameraManager[];
    return g_cameraManager[0x640];
}

void *CameraManager_GetCameraByIndex(u32 arg0) {
    extern u8 g_cameraManager[];
    if (arg0 >= g_cameraManager[0x640] || arg0 >= 4U) {
        return NULL;
    }
    return g_cameraManager + arg0 * 0x190;
}

void CameraManager_Init(void) {
    extern u8 g_cameraManager[];
    extern s8 g_cameraManagerPause;
    extern u8 g_springProxy[];
    g_cameraManagerPause = 0;
    g_cameraManager[0x640] = 0;
    SoundListener_SetTotalListeners(0);
    memset(g_springProxy + 0x60, 0, 0x20);
    memset(g_springProxy + 0x40, 0, 0x20);
    M2C_FIELD(g_springProxy, s32 *, 0x1C0) = 0;
    memset(g_springProxy + 0x140, 0, 0x20);
    memset(g_springProxy + 0xC0, 0, 0x20);
    M2C_FIELD(g_springProxy, s32 *, 0x1C4) = 0;
}

void CameraManager_ResetAllCameras(void) {
    extern u8 g_cameraManager[];
    u32 i;

    for (i = 0; i < 4; i++) {
        u8 *cam = g_cameraManager + i * 0x190;
        M2C_FIELD(cam, f32 *, 0x60) = 0.0f;
        M2C_FIELD(cam, f32 *, 0x64) = 0.0f;
        M2C_FIELD(cam, f32 *, 0x68) = 0.0f;
        M2C_FIELD(cam, f32 *, 0x80) = 0.0f;
        M2C_FIELD(cam, f32 *, 0x84) = 0.0f;
        M2C_FIELD(cam, f32 *, 0x88) = 1.0f;
        M2C_FIELD(cam, f32 *, 0x90) = 0.0f;
        M2C_FIELD(cam, f32 *, 0x94) = 1.0f;
        M2C_FIELD(cam, f32 *, 0x98) = 0.0f;
        M2C_FIELD(cam, f32 *, 0xA0) = 1.0f;
        M2C_FIELD(cam, f32 *, 0xA4) = 0.0f;
        M2C_FIELD(cam, f32 *, 0xA8) = 0.0f;
    }
}

void CameraManager_Pause(s8 arg0) {
    extern s8 g_cameraManagerPause;
    g_cameraManagerPause = arg0;
}

