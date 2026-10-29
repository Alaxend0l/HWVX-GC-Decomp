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

