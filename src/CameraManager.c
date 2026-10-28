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

void GetTrimmedCameraPos(u8 *camera, Vector4 *from, Vector4 *dir, f32 *trim, f32 dist) {
    extern f32 g_fDeltaTimeSec;
    f32 PSVECDotProduct(void *a, void *b);
    f32 PSVECSquareMag(void *v);
    void PSVECNormalize(void *src, void *dst);
    struct {
        Vector4 point;
        Vector4 normal;
        u8 unk20[0x30];
    } info;
    Vector4 pos;
    Vector4 d;
    f32 t;
    f32 trimAmt;
    f32 travelled;
    f32 len;
    f32 dot;
    s8 hit;
    s8 trimmed;

    trimAmt = *trim;
    travelled = 0.0f;
    pos = *from;
    trimmed = 0;
    len = MagnitudeVector(dir);
    d.x = -dir->x;
    d.y = -dir->y;
    d.z = -dir->z;
    do {
        t = dist - travelled;
        hit = RayHitsTerrainAndObjects_Camera(&pos, &d, &t, &info);
        pos.x += d.x * t;
        pos.y += d.y * t;
        pos.z += d.z * t;
        if (hit) {
            dot = -PSVECDotProduct(&info.normal, &d);
            d.x += dot * info.normal.x;
            d.y += dot * info.normal.y;
            d.z += dot * info.normal.z;
            if (dot < 0.0f) {
                pos.x = from->x - dir->x * dist;
                pos.y = from->y - dir->y * dist;
                pos.z = from->z - dir->z * dist;
                break;
            }
            if (PSVECSquareMag(&d) < 0.1f) {
                d.y += 1.0f;
            }
            if (PSVECSquareMag(&d)) {
                PSVECNormalize(&d, &d);
            }
            trimmed = 1;
            d.x *= len;
            d.y *= len;
            d.z *= len;
            pos.x += info.normal.x * trimAmt;
            pos.y += info.normal.y * trimAmt;
            pos.z += info.normal.z * trimAmt;
        }
        travelled += t;
        if (t < 1e-7f) {
            break;
        }
    } while (travelled < dist);
    if (trimmed) {
        *trim += 3.0f * g_fDeltaTimeSec;
        if (*trim > 1.8f) {
            *trim = 1.8f;
        }
    } else {
        *trim = 0.0f;
    }
    *(Vector4 *)(camera + 0x60) = pos;
}

void GetTrimmedCameraPos2(Vector4 *from, Vector4 *to, f32 *trim) {
    extern f32 g_fDeltaTimeSec;
    f32 PSVECDotProduct(void *a, void *b);
    f32 PSVECSquareDistance(void *a, void *b);
    void PSVECSubtract(void *a, void *b, void *out);
    f32 Sqrt(f32 x);
    struct {
        Vector4 point;
        Vector4 normal;
        u8 unk20[0x30];
    } info;
    Vector4 pos;
    Vector4 target;
    Vector4 d;
    f32 t;
    f32 trimAmt;
    f32 travelled;
    f32 dist;
    f32 len;
    f32 dot;
    s8 hit;
    s8 trimmed;

    trimAmt = *trim;
    travelled = 0.0f;
    pos = *from;
    target = *to;
    dist = Sqrt(PSVECSquareDistance(from, to));
    trimmed = 0;
    PSVECSubtract(&target, &pos, &d);
    do {
        t = 1.0f - travelled;
        hit = RayHitsTerrainAndObjects_Camera(&pos, &d, &t, &info);
        if (t == 0.0f && !trimmed) {
            hit = 0;
            t += 0.2f;
        }
        pos.x += d.x * t;
        pos.y += d.y * t;
        pos.z += d.z * t;
        if (hit) {
            dot = -PSVECDotProduct(&info.normal, &d);
            if (dot < 0.0f) {
                return;
            }
            d.x += dot * info.normal.x;
            d.y += dot * info.normal.y;
            d.z += dot * info.normal.z;
            len = MagnitudeVector(&d);
            if (len < 0.01f) {
                trimmed = 1;
                break;
            }
            len = dist / len;
            trimmed = 1;
            d.x *= len;
            d.y *= len;
            d.z *= len;
            pos.x += info.normal.x * trimAmt;
            pos.y += info.normal.y * trimAmt;
            pos.z += info.normal.z * trimAmt;
        }
        travelled += t;
        if (t < 1e-6f) {
            break;
        }
    } while (travelled < 1.0f);
    *to = pos;
    if (trimmed) {
        *trim += g_fDeltaTimeSec;
        if (*trim > 1.8f) {
            *trim = 1.8f;
        }
    } else {
        *trim -= g_fDeltaTimeSec;
        if (*trim <= 0.0f) {
            *trim = 0.0f;
        }
    }
}

void Camera_UpdatePosition_Zoom_Generic(u8 *cam, u8 *mode, Vector4 *target, Vector4 *eye, f32 zoom, f32 shake) {
    extern f32 g_fDeltaTimeSec;
    double sin(double x);
    double atan2(double y, double x);
    f32 MagnitudeVector(void *v);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    void Gfx_SetProjectionMatrix(f32 fov);
    Vector4 pos;

    pos = *target;
    *(f32 *)(cam + 0xB8) += 0.2f * g_fDeltaTimeSec;
    pos.x = shake * (*(f32 *)(cam + 0xA0) * (f32)sin(*(f32 *)(cam + 0xB8))) + target->x;
    pos.y = shake * (*(f32 *)(cam + 0xA4) * (f32)sin(*(f32 *)(cam + 0xB8))) + target->y;
    pos.z = shake * (*(f32 *)(cam + 0xA8) * (f32)sin(*(f32 *)(cam + 0xB8))) + target->z;
    {
        Vector4 up = {0.0f, 1.0f, 0.0f, 0.0f};
        Vector4 dir;
        f32 mag;
        f32 inv;
        f32 fov;

        PSVECSubtract(eye, &pos, &dir);
        mag = MagnitudeVector(&dir);
        inv = 1.0f / mag;
        fov = *(f32 *)(mode + 0x10) * (zoom * (zoom * (f32)atan2(40.0, mag)));
        *(f32 *)(cam + 0x80) = dir.x * inv;
        *(f32 *)(cam + 0x84) = dir.y * inv;
        *(f32 *)(cam + 0x88) = dir.z * inv;
        PSVECCrossProduct(&up, (Vector4 *)(cam + 0x80), (Vector4 *)(cam + 0xA0));
        PSVECCrossProduct((Vector4 *)(cam + 0x80), (Vector4 *)(cam + 0xA0), (Vector4 *)(cam + 0x90));
        inv = 1.0f / MagnitudeVector(cam + 0xA0);
        *(f32 *)(cam + 0xA0) *= inv;
        *(f32 *)(cam + 0xA4) *= inv;
        *(f32 *)(cam + 0xA8) *= inv;
        inv = 1.0f / MagnitudeVector(cam + 0x90);
        *(f32 *)(cam + 0x90) *= inv;
        *(f32 *)(cam + 0x94) *= inv;
        *(f32 *)(cam + 0x98) *= inv;
        if (fov > 150.0f) {
            fov = 150.0f;
        }
        if (fov < 6.0f) {
            fov = 6.0f;
        }
        Gfx_SetProjectionMatrix(fov);
    }
    *(Vector4 *)(cam + 0x60) = pos;
    PSVECSubtract((Vector4 *)(cam + 0x60), &pos, (Vector4 *)(cam + 0x70));
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
