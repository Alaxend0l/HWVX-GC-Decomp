#include "common.h"

typedef struct GfxWindow {
    f32 unk0[4][4];
    f32 unk40[4][4];
    f32 unk80[4][4];
    f32 unkC0[4][4];
    f32 unk100[4][4];
    u8 unk140[0x180 - 0x140];
    Vector4 cameraPos;
    Vector4 cameraRight;
    Vector4 cameraUp;
    Vector4 cameraAt;
    u8 unk1C0[0x270 - 0x1C0];
    f32 fov;
    u8 unk274[0x280 - 0x274];
    u32 width;
    u32 height;
    u8 unk288[0x2B0 - 0x288];
    u32 unk2B0;
    u32 unk2B4;
    u32 unk2B8;
    u32 unk2BC;
    u8 unk2C0[0x2D0 - 0x2C0];
} GfxWindow;

typedef struct GfxState {
    u8 unk0[0x93];
    u8 unk93;
    u8 unk94[0xA0 - 0x94];
} GfxState;

extern GfxState g_state;
extern f32 g_identity[4][4];
extern f32 GXCameraSpace[3][4];

GfxWindow *GetRenderWindow(void);
void *memcpy(void *dst, const void *src, u32 n);
void MultiplyMatrix4x4(f32 a[4][4], f32 b[4][4], f32 out[4][4]);
void C_MTXOrtho(f32 m[3][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void GXSetProjection(f32 m[3][4], s32 type);
void PSMTXIdentity(f32 m[3][4]);
void GXLoadPosMtxImm(f32 m[3][4], u32 id);
void GXSetCurrentMtx(u32 id);

void MatrixToMtxTranspose(f32 dst[3][4], f32 src[4][4]);
void Gfx_SetWorldMatrix(f32 (*matrix)[4]) {
    if (matrix == NULL) {
        if (g_state.unk93) {
            return;
        }
        g_state.unk93 = 1;
        matrix = g_identity;
    } else {
        g_state.unk93 = 0;
    }
    memcpy(GetRenderWindow()->unk0, matrix, 0x40);
    MultiplyMatrix4x4(GetRenderWindow()->unk0, GetRenderWindow()->unk40, GetRenderWindow()->unk80);
    MultiplyMatrix4x4(GetRenderWindow()->unk80, GetRenderWindow()->unkC0, GetRenderWindow()->unk100);
    MatrixToMtxTranspose(GXCameraSpace, GetRenderWindow()->unk80);
    GXLoadPosMtxImm(GXCameraSpace, 0);
    GXSetCurrentMtx(0);
}

void Render_SetClipPlanes(f32 fov);
void Gfx_SetCameraMatrix(Vector4 *pos, Vector4 *right, Vector4 *up, Vector4 *at) {
    f32 m[4][4] __attribute__((aligned(16)));
    Vector4 defPos __attribute__((aligned(16)));
    Vector4 defRight __attribute__((aligned(16)));
    Vector4 defUp __attribute__((aligned(16)));
    Vector4 defAt __attribute__((aligned(16)));

    if (pos == NULL) {
        pos = &defPos;
        defPos.x = 0.0f;
        defPos.y = 0.0f;
        defPos.z = 0.0f;
    }
    if (right == NULL) {
        right = &defRight;
        up = &defUp;
        at = &defAt;
        defRight.x = 0.0f;
        defRight.y = 0.0f;
        defRight.z = -1.0f;
        defUp.x = 0.0f;
        defUp.y = 1.0f;
        defUp.z = 0.0f;
        defAt.x = -1.0f;
        defAt.y = 0.0f;
        defAt.z = 0.0f;
    }
    GetRenderWindow()->cameraPos = *pos;
    GetRenderWindow()->cameraRight = *right;
    GetRenderWindow()->cameraUp = *up;
    GetRenderWindow()->cameraAt = *at;
    m[0][0] = -at->x;
    m[1][0] = -at->y;
    m[2][0] = -at->z;
    m[0][1] = up->x;
    m[1][1] = up->y;
    m[2][1] = up->z;
    m[0][2] = -right->x;
    m[1][2] = -right->y;
    m[2][2] = -right->z;
    m[3][0] = -(m[0][0] * pos->x + m[1][0] * pos->y + m[2][0] * pos->z);
    m[3][1] = -(m[0][1] * pos->x + m[1][1] * pos->y + m[2][1] * pos->z);
    m[3][2] = -(m[0][2] * pos->x + m[1][2] * pos->y + m[2][2] * pos->z);
    m[2][3] = 0.0f;
    m[1][3] = 0.0f;
    m[0][3] = 0.0f;
    m[3][3] = 1.0f;
    memcpy(GetRenderWindow()->unk40, m, 0x40);
    MultiplyMatrix4x4(GetRenderWindow()->unk0, GetRenderWindow()->unk40, GetRenderWindow()->unk80);
    MultiplyMatrix4x4(GetRenderWindow()->unk80, GetRenderWindow()->unkC0, GetRenderWindow()->unk100);
    MatrixToMtxTranspose(GXCameraSpace, GetRenderWindow()->unk80);
    GXLoadPosMtxImm(GXCameraSpace, 0);
    GXSetCurrentMtx(0);
    Render_SetClipPlanes(GetRenderWindow()->fov);
    g_state.unk93 = 0;
}

f32 Gfx_GetProjectionMatrix(void) {
    return GetRenderWindow()->fov;
}

void Gfx_SetOrthoMatrix(void) {
    f32 mtx[3][4];
    f32 proj[3][4];
    f32 width;

    width = GetRenderWindow()->width;
    C_MTXOrtho(proj, 0.0f, GetRenderWindow()->height, 0.0f, width, 1.0f, -1.0f);
    GXSetProjection(proj, 1);
    PSMTXIdentity(mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
}

void MultiplyMatrix4x4(f32 a[4][4], f32 b[4][4], f32 out[4][4]) {
    out[0][0] = b[0][0] * a[0][0] + b[1][0] * a[0][1] + b[2][0] * a[0][2] + b[3][0] * a[0][3];
    out[0][1] = b[0][1] * a[0][0] + b[1][1] * a[0][1] + b[2][1] * a[0][2] + b[3][1] * a[0][3];
    out[0][2] = b[0][2] * a[0][0] + b[1][2] * a[0][1] + b[2][2] * a[0][2] + b[3][2] * a[0][3];
    out[0][3] = b[0][3] * a[0][0] + b[1][3] * a[0][1] + b[2][3] * a[0][2] + b[3][3] * a[0][3];
    out[1][0] = b[0][0] * a[1][0] + b[1][0] * a[1][1] + b[2][0] * a[1][2] + b[3][0] * a[1][3];
    out[1][1] = b[0][1] * a[1][0] + b[1][1] * a[1][1] + b[2][1] * a[1][2] + b[3][1] * a[1][3];
    out[1][2] = b[0][2] * a[1][0] + b[1][2] * a[1][1] + b[2][2] * a[1][2] + b[3][2] * a[1][3];
    out[1][3] = b[0][3] * a[1][0] + b[1][3] * a[1][1] + b[2][3] * a[1][2] + b[3][3] * a[1][3];
    out[2][0] = b[0][0] * a[2][0] + b[1][0] * a[2][1] + b[2][0] * a[2][2] + b[3][0] * a[2][3];
    out[2][1] = b[0][1] * a[2][0] + b[1][1] * a[2][1] + b[2][1] * a[2][2] + b[3][1] * a[2][3];
    out[2][2] = b[0][2] * a[2][0] + b[1][2] * a[2][1] + b[2][2] * a[2][2] + b[3][2] * a[2][3];
    out[2][3] = b[0][3] * a[2][0] + b[1][3] * a[2][1] + b[2][3] * a[2][2] + b[3][3] * a[2][3];
    out[3][0] = b[0][0] * a[3][0] + b[1][0] * a[3][1] + b[2][0] * a[3][2] + b[3][0] * a[3][3];
    out[3][1] = b[0][1] * a[3][0] + b[1][1] * a[3][1] + b[2][1] * a[3][2] + b[3][1] * a[3][3];
    out[3][2] = b[0][2] * a[3][0] + b[1][2] * a[3][1] + b[2][2] * a[3][2] + b[3][2] * a[3][3];
    out[3][3] = b[0][3] * a[3][0] + b[1][3] * a[3][1] + b[2][3] * a[3][2] + b[3][3] * a[3][3];
}

void MatrixToMtxTranspose(f32 dst[3][4], f32 src[4][4]) {
    dst[0][0] = src[0][0];
    dst[0][1] = src[1][0];
    dst[0][2] = src[2][0];
    dst[0][3] = src[3][0];
    dst[1][0] = src[0][1];
    dst[1][1] = src[1][1];
    dst[1][2] = src[2][1];
    dst[1][3] = src[3][1];
    dst[2][0] = src[0][2];
    dst[2][1] = src[1][2];
    dst[2][2] = src[2][2];
    dst[2][3] = src[3][2];
}

s32 isInWindow(f32 *pos) {
    if (GetRenderWindow()->unk2B0 > pos[0] || GetRenderWindow()->unk2B4 < pos[0]) {
        return 0;
    }
    if (GetRenderWindow()->unk2B8 > pos[1] || GetRenderWindow()->unk2BC < pos[1]) {
        return 0;
    }
    pos[0] -= GetRenderWindow()->unk2B0;
    pos[1] -= GetRenderWindow()->unk2B8;
    return 1;
}

void Render_StateSetScissor(void);
void GXGetViewportv(f32 *vp);
void GXGetProjectionv(f32 *pm);
void GXProject(f32 x, f32 y, f32 z, f32 mtx[3][4], f32 *pm, f32 *vp, f32 *sx, f32 *sy, f32 *sz);
extern f32 GXCameraSpace[3][4];
void Gfx_TransformVector(f32 *in, f32 *out) {
    f32 pm[7];
    f32 vp[6];

    Render_StateSetScissor();
    GXGetViewportv(vp);
    GXGetProjectionv(pm);
    vp[3] *= 1.0714f;
    GXProject(in[0], in[1], in[2], GXCameraSpace, pm, vp, &out[0], &out[1], &out[2]);
    out[3] = -(GetRenderWindow()->unk100[3][3] +
               (in[2] * GetRenderWindow()->unk100[2][3] +
                (in[0] * GetRenderWindow()->unk100[0][3] +
                 in[1] * GetRenderWindow()->unk100[1][3])));
}

