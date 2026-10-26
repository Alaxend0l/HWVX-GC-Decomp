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

s32 Camera_TriggerActionGetLocation(u8 *cam, u32 *data, Vector4 *out) {
    extern u8 *g_pNoRayCollideThing;
    extern s32 g_triggerAction_params_default;
    void XformVec(void *mtx, Vector4 *in, Vector4 *out);
    Vector4 offset;
    Vector4 local;
    u8 *thing;
    s32 i = 0;

    switch (data[i++]) {
    case 5:
        *out = *(Vector4 *)(cam + 0x60);
        break;
    case 6:
        *out = *(Vector4 *)data[i++];
        out->x += *(f32 *)&data[i++];
        out->y += *(f32 *)&data[i++];
        out->z += *(f32 *)&data[i++];
        break;
    case 4:
        thing = *(u8 **)(data[i++] + 0x10) + 0x10;
        offset.x = *(f32 *)&data[i++];
        offset.y = *(f32 *)&data[i++];
        offset.z = *(f32 *)&data[i++];
        XformVec(thing, &offset, out);
        break;
    case 3:
        out->x = ((Vector4 *)data[i])->x;
        out->y = ((Vector4 *)data[i])->y;
        out->z = ((Vector4 *)data[i++])->z;
        out->x += *(f32 *)&data[i++];
        out->y += *(f32 *)&data[i++];
        out->z += *(f32 *)&data[i++];
        break;
    case 2:
        thing = *(u8 **)(data[i++] + 0x2C);
        if (thing == NULL) {
            *out = *(Vector4 *)data[i - 1];
        } else {
            *out = *(Vector4 *)(thing + 0x40);
        }
        out->x += *(f32 *)&data[i++];
        out->y += *(f32 *)&data[i++];
        out->z += *(f32 *)&data[i++];
        break;
    case 1:
        thing = *(u8 **)(data[i++] + 0x28);
        local.x = *(f32 *)&data[i++];
        local.y = *(f32 *)&data[i++];
        local.z = *(f32 *)&data[i++];
        g_pNoRayCollideThing = thing;
        if (*(u8 **)(thing + 0x94) != NULL) {
            *(u32 *)(*(u8 **)(thing + 0x94) + 0x6C) |= 0x80;
        }
        if (*(u32 *)(thing + 0x74) & 0x201000) {
            *(s32 **)(cam + 0xB4) = &g_triggerAction_params_default;
        }
        if (thing != NULL) {
            XformVec(thing + 0x10, &local, out);
        }
        break;
    }
    return i;
}

void CameraManager_ReturnAllToPlayer(void) {
    extern u8 g_cameraManager[];
    extern u32 g_triggerAction_params_default;
    u32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        M2C_FIELD(g_cameraManager + i * 0x190, u32 **, 0xB4) = &g_triggerAction_params_default;
    }
}

void CameraManager_SetCameraGlobalMode(u32 *mode) {
   
    extern u8 g_cameraManager[];
    u32 i;

    if (mode[0] >= 1 && mode[0] <= 4) {
        ((Camera *)g_cameraManager)[mode[0] - 1].mode = mode + 1;
        ((Camera *)g_cameraManager)[mode[0] - 1].modeTime = 0.0f;
        return;
    }
    for (i = 0; i < g_cameraManager[0x640]; i++) {
        ((Camera *)g_cameraManager)[i].mode = mode + (mode[0] == 0);
        ((Camera *)g_cameraManager)[i].modeTime = 0.0f;
    }
}

void *CameraManager_GetCameraFromPlayer(void *player) {
    extern u8 g_cameraManager[];
    u32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        if (M2C_FIELD(g_cameraManager + i * 0x190, void **, 0) == player) {
            return g_cameraManager + i * 0x190;
        }
    }
    return NULL;
}
