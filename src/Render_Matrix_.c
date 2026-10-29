#include "common.h"

typedef struct GfxWindow {
    u8 unk0[0x100];
    f32 unk100[4][4];
    u8 unk140[0x270 - 0x140];
    f32 fov;
    u8 unk274[0x280 - 0x274];
    u32 width;
    u32 height;
    u8 unk288[0x2D0 - 0x288];
} GfxWindow;

GfxWindow *GetRenderWindow(void);
void C_MTXOrtho(f32 m[3][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void GXSetProjection(f32 m[3][4], s32 type);
void PSMTXIdentity(f32 m[3][4]);
void GXLoadPosMtxImm(f32 m[3][4], u32 id);
void GXSetCurrentMtx(u32 id);

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

