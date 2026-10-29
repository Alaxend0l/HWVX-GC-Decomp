#include "common.h"
#include "CarA.h"

/* A camera sub-mode a vehicle hands out (CameraInfo.subMode). */
typedef struct CameraSubMode {
    u8 type;                /* 0x00 0: default, 2: zoom, 3: orbit, 4: bone */
    u8 pad1[3];
    void *params;           /* 0x04 */
} CameraSubMode;

typedef struct CameraOrbitParams {
    u8 pad0[0x10];
    f32 fov;                /* 0x10 */
    f32 radius;             /* 0x14 */
    f32 height;             /* 0x18 */
    f32 speed;              /* 0x1C */
} CameraOrbitParams;

typedef struct CameraBoneParams {
    Vector4 offset;         /* 0x00 bone-local camera position */
    u8 pad10[4];
    u16 bone;               /* 0x14 */
} CameraBoneParams;

typedef struct CameraZoomParams {
    u8 pad0[0x10];
    f32 zoom;               /* 0x10 */
    u8 pad14[0xC];
} CameraZoomParams;

/* Filled by the vehicle's getCameraInfo. */
typedef struct CameraInfo {
    u8 pad0[0x14];
    f32 fov;                /* 0x14 */
    u8 pad18[8];
    Matrix4 mtx;            /* 0x20 */
    u8 pad60[0x24];
    CameraSubMode *subMode; /* 0x84 */
    u32 cameraCount;        /* 0x88 */
    u8 pad8C[4];
} CameraInfo;

typedef struct CameraWorldScale {
    u8 pad0[0x3C];
    f32 scale;              /* 0x3C */
} CameraWorldScale;

typedef struct CameraWorld {
    u8 pad0[0x9C];
    CameraWorldScale *unk9C; /* 0x9C */
} CameraWorld;


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

static inline void Camera_Orbit(Camera *cam, Vector4 *center, Vector4 *target, f32 radius, f32 height, f32 speed, f32 fov) {
    extern f32 g_fDeltaTimeSec;
    double sin(double x);
    double cos(double x);
    f32 PSVECSquareMag(void *v);
    void PSVECNormalize(void *src, void *dst);
    void PSVECSubtract(void *a, void *b, void *out);
    void PSVECCrossProduct(void *a, void *b, void *out);
    void Gfx_SetProjectionMatrix(f32 fov);
    Vector4 eye;
    Vector4 dir;
    Vector4 up = {0.0f, 1.0f, 0.0f, 0.0f};
    f32 s;
    f32 c;

    cam->modeTime += speed * g_fDeltaTimeSec;
    s = sin(cam->modeTime);
    c = cos(cam->modeTime);
    eye.x = radius * s + center->x;
    eye.y = center->y + height;
    eye.z = radius * c + center->z;
    PSVECSubtract(target, &eye, &dir);
    GetTrimmedCameraPos((u8 *)cam, target, &dir, &cam->trim, 1.0f);
    PSVECSubtract(target, &cam->pos, &cam->dir);
    if (PSVECSquareMag(&cam->dir)) {
        PSVECNormalize(&cam->dir, &cam->dir);
    }
    PSVECCrossProduct(&up, &cam->dir, &cam->right);
    PSVECCrossProduct(&cam->dir, &cam->right, &cam->up);
    if (PSVECSquareMag(&cam->right)) {
        PSVECNormalize(&cam->right, &cam->right);
    }
    if (PSVECSquareMag(&cam->up)) {
        PSVECNormalize(&cam->up, &cam->up);
    }
    Gfx_SetProjectionMatrix(fov);
}

static inline void Camera_LookAt(Camera *cam, Vector4 *pos, Vector4 *target) {
    f32 PSVECSquareMag(void *v);
    void PSVECNormalize(void *src, void *dst);
    void PSVECSubtract(void *a, void *b, void *out);
    void PSVECCrossProduct(void *a, void *b, void *out);
    Vector4 up = {0.0f, 1.0f, 0.0f, 0.0f};

    cam->pos = *pos;
    PSVECSubtract(target, &cam->pos, &cam->dir);
    if (PSVECSquareMag(&cam->dir)) {
        PSVECNormalize(&cam->dir, &cam->dir);
    }
    PSVECCrossProduct(&up, &cam->dir, &cam->right);
    PSVECCrossProduct(&cam->dir, &cam->right, &cam->up);
    if (PSVECSquareMag(&cam->right)) {
        PSVECNormalize(&cam->right, &cam->right);
    }
    if (PSVECSquareMag(&cam->up)) {
        PSVECNormalize(&cam->up, &cam->up);
    }
}

static inline void Camera_ZoomOnVehicle(Camera *cam, u8 *params) {
    s8 RayHitsTerrainAndObjects(Vector4 *from, Vector4 *dir, f32 *t, void *info);
    s32 NavHolder_FindIndexOfNearestNavPoint(Vector4 *pos, s32 hint);
    Vector4 *NavHolder_GetNavPoint(s32 index);
    void PSVECSubtract(void *a, void *b, void *out);
    Vector4 target;
    Vector4 delta;
    f32 t;
    Vector4 *nav;

    ((CarA *)cam->player->vehicle)->vtable->getTargetPos(cam->player->vehicle, &target);
    PSVECSubtract(&target, &cam->pos, &delta);
    t = 1.0f;
    if (RayHitsTerrainAndObjects(&cam->pos, &delta, &t, NULL)) {
        nav = NavHolder_GetNavPoint(NavHolder_FindIndexOfNearestNavPoint(&target, -1));
        if (nav != NULL) {
            cam->pos = *nav;
        }
    }
    Camera_UpdatePosition_Zoom_Generic((u8 *)cam, params, &cam->pos, &target, ((CarA *)cam->player->vehicle)->unk70, 0.0f);
}

static inline void Camera_OrbitWithParams(Camera *cam, Vector4 *center, CameraOrbitParams *params) {
    extern f32 g_fDeltaTimeSec;
    double sin(double x);
    double cos(double x);
    f32 PSVECSquareMag(void *v);
    void PSVECNormalize(void *src, void *dst);
    void PSVECSubtract(void *a, void *b, void *out);
    void PSVECCrossProduct(void *a, void *b, void *out);
    void Gfx_SetProjectionMatrix(f32 fov);
    {
        Vector4 eye;
        Vector4 dir;
        Vector4 up = {0.0f, 1.0f, 0.0f, 0.0f};
        f32 sinA;
        f32 cosA;

        cam->modeTime += params->speed * g_fDeltaTimeSec;
        sinA = sin(cam->modeTime);
        cosA = cos(cam->modeTime);
        eye.x = params->radius * sinA + center->x;
        eye.y = center->y + params->height;
        eye.z = params->radius * cosA + center->z;
        PSVECSubtract(center, &eye, &dir);
        GetTrimmedCameraPos((u8 *)cam, center, &dir, &cam->trim, 1.0f);
        PSVECSubtract(center, &cam->pos, &cam->dir);
        if (PSVECSquareMag(&cam->dir)) {
            PSVECNormalize(&cam->dir, &cam->dir);
        }
        PSVECCrossProduct(&up, &cam->dir, &cam->right);
        PSVECCrossProduct(&cam->dir, &cam->right, &cam->up);
        if (PSVECSquareMag(&cam->right)) {
            PSVECNormalize(&cam->right, &cam->right);
        }
        if (PSVECSquareMag(&cam->up)) {
            PSVECNormalize(&cam->up, &cam->up);
        }
        Gfx_SetProjectionMatrix(params->fov);
    }
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

#pragma push
#pragma use_lmw_stmw on
void Camera_UpdatePosition(Camera *cam) {
    extern CarA *g_pNoRayCollideThing;
    extern Matrix4 g_identity;
    extern Vector4 g_zeroVector;
    extern u8 g_lookerVTable[];
    extern u32 g_triggerAction_params_default;
    void PSVECSubtract(void *a, void *b, void *out);
    void Gfx_SetProjectionMatrix(f32 fov);
    void XformVec(void *mtx, Vector4 *in, Vector4 *out);
    void Camera_UpdatePosition_Default(Camera *cam, void *params);
    Vector4 oldPos __attribute__((aligned(16)));
    CameraZoomParams zoom __attribute__((aligned(16)));
    CameraInfo info;
    s32 i;
    u32 *mode;

    oldPos = cam->pos;
    info.subMode = NULL;
    info.fov = 60.0f;
    info.mtx = g_identity;
    if (cam->player != NULL) {
        cam->player->flags &= ~0x80;
    }
    mode = cam->mode;
    switch (mode[0]) {
    case 30:
        if (cam->player != NULL) {
            if (cam->player->vehicle != NULL) {
                g_pNoRayCollideThing = cam->player->vehicle;
                if (!(((CarA *)cam->player->vehicle)->collDirtyFlags & 0x201000)) {
                    ((CarA *)cam->player->vehicle)->vtable->getCameraInfo(cam->player->vehicle, cam->player->cameraIndex, &info);
                    if (cam->player->cameraIndex > info.cameraCount) {
                        cam->player->cameraIndex -= (u8)info.cameraCount;
                    }
                    cam->mtx = ((CarA *)cam->player->vehicle)->worldMatrix;
                } else {
                    info.mtx = cam->mtx;
                }
            } else {
                Camera_Orbit(cam, &cam->mtx.v[3], &cam->mtx.v[3], 15.0f, 5.0f, 0.3f, 60.0f);
                return;
            }
        }
        if (info.subMode == NULL) {
            g_pNoRayCollideThing = NULL;
            cam->right = info.mtx.v[0];
            cam->up = info.mtx.v[1];
            cam->dir = info.mtx.v[2];
            cam->pos = info.mtx.v[3];
            Gfx_SetProjectionMatrix(info.fov);
            if (cam->listener != NULL && cam->player != NULL && cam->player->vehicle != NULL) {
                f32 scale = ((CameraWorld *)g_worldData.world)->unk9C->scale;
                cam->listener->pos.x = ((CarA *)cam->player->vehicle)->velocity.x * scale + cam->pos.x;
                cam->listener->pos.y = ((CarA *)cam->player->vehicle)->velocity.y * scale + cam->pos.y;
                cam->listener->pos.z = ((CarA *)cam->player->vehicle)->velocity.z * scale + cam->pos.z;
            }
            return;
        }
        switch (info.subMode->type) {
        case 0:
            Camera_UpdatePosition_Default(cam, info.subMode->params);
            cam->vel = ((CarA *)cam->player->vehicle)->velocity;
            break;
        case 2:
            Camera_ZoomOnVehicle(cam, info.subMode->params);
            PSVECSubtract(&cam->pos, &oldPos, &cam->vel);
            break;
        case 3:
            Camera_OrbitVehicle(cam, info.subMode->params);
            cam->vel = g_zeroVector;
            break;
        case 4: {
            CameraBoneParams *params = info.subMode->params;
            CarA *car = cam->player->vehicle;

            if ((u8 *)car->vtable == g_carAVTable) {
                cam->right = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 0];
                cam->up = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 1];
                cam->dir = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 2];
                cam->pos = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 3];
                XformVec(&car->motionState.animMatrices[params->bone], &params->offset, &cam->pos);
            } else if ((u8 *)car->vtable == g_lookerVTable) {
                car = car->unkBC;
                if (car->motionState.animMatrices != NULL) {
                    cam->right = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 0];
                    cam->up = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 1];
                    cam->dir = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 2];
                    cam->pos = ((Vector4 *)car->motionState.animMatrices)[params->bone * 4 + 3];
                    XformVec(&car->motionState.animMatrices[params->bone], &params->offset, &cam->pos);
                }
            }
            Gfx_SetProjectionMatrix(60.0f);
            cam->vel = ((CarA *)cam->player->vehicle)->velocity;
            break;
        }
        }
        break;
    case 27: {
        Vector4 target __attribute__((aligned(16))) = {0.0f, 0.0f, 0.0f, 0.0f};
        Vector4 pos __attribute__((aligned(16))) = {1.0f, 1.0f, 1.0f, 0.0f};
        f32 fov;

        i = 1;
        fov = *(f32 *)&mode[i++];
        i += Camera_TriggerActionGetLocation((u8 *)cam, &mode[i], &pos);
        Camera_TriggerActionGetLocation((u8 *)cam, &mode[i], &target);
        Camera_LookAt(cam, &pos, &target);
        Gfx_SetProjectionMatrix(fov);
        break;
    }
    case 29: {
        Vector4 target __attribute__((aligned(16))) = {0.0f, 0.0f, 0.0f, 0.0f};
        Vector4 pos __attribute__((aligned(16))) = {1.0f, 1.0f, 1.0f, 0.0f};

        zoom.zoom = 0.014f;
        i = 1;
        i += Camera_TriggerActionGetLocation((u8 *)cam, &mode[i], &pos);
        Camera_TriggerActionGetLocation((u8 *)cam, &mode[i], &target);
        Camera_UpdatePosition_Zoom_Generic((u8 *)cam, (u8 *)&zoom, &pos, &target, 10.0f, 0.0f);
        break;
    }
    case 28: {
        Vector4 target __attribute__((aligned(16))) = {0.0f, 0.0f, 0.0f, 0.0f};
        Vector4 center __attribute__((aligned(16))) = {1.0f, 1.0f, 1.0f, 0.0f};
        f32 fov;
        f32 radius;
        f32 height;
        f32 speed;
        CarA *car;

        i = 1;
        fov = *(f32 *)&mode[i++];
        radius = *(f32 *)&mode[i++];
        height = *(f32 *)&mode[i++];
        speed = *(f32 *)&mode[i++];
        i += Camera_TriggerActionGetLocation((u8 *)cam, &mode[i], &center);
        Camera_TriggerActionGetLocation((u8 *)cam, &mode[i], &target);
        Camera_Orbit(cam, &center, &target, radius, height, speed, fov);
        car = cam->player->vehicle;
        if (car != NULL && (u8 *)car->vtable == g_carAVTable && (car->stuckCounter > 0 || car->unk460 <= 0) &&
            car->worldMatrix.v[1].y < 0.5f) {
            cam->mode = &g_triggerAction_params_default;
        }
        break;
    }
    }
    g_pNoRayCollideThing = NULL;
    if (cam->listener != NULL && cam->player != NULL && cam->player->vehicle != NULL) {
        f32 scale = ((CameraWorld *)g_worldData.world)->unk9C->scale;
        cam->listener->pos.x = ((CarA *)cam->player->vehicle)->velocity.x * scale + cam->pos.x;
        cam->listener->pos.y = ((CarA *)cam->player->vehicle)->velocity.y * scale + cam->pos.y;
        cam->listener->pos.z = ((CarA *)cam->player->vehicle)->velocity.z * scale + cam->pos.z;
    }
}
#pragma pop
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
