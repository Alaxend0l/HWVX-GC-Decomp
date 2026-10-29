#include "common.h"

typedef struct GfxWindow {
    u8 unk0[0x270];
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
