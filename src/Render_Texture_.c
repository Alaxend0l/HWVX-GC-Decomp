#include "common.h"

typedef struct Texture {
    u8 unk0[0x24];
    void *data;
    u32 unk28;
    f32 unk2C;
    u16 unk30;
    u16 width;
    u16 height;
    s16 refCount;
    u16 unk38;
    u16 unk3A;
    u16 view;
    u16 wrapMode;
} Texture;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

void *GXGetTexObjData(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrapS, s32 wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
void GXLoadTexObj(GXTexObj *obj, s32 id);
void GXSetNumTexGens(u8 nTexGens);
void GXSetTevOp(s32 id, s32 mode);
void GXSetTevOrder(s32 stage, s32 coord, s32 map, s32 color);
void DCFlushRange(void *addr, u32 len);

extern Texture *m_texture;

GXTexObj m_gxTexObjArray[511];
u32 m_curTexture;

#pragma use_lmw_stmw on

void Render_DestroyTextureSlot(u32 id) {
    GXInitTexObj(&m_gxTexObjArray[id], NULL, 0x20, 0x20, 6, 0, 0, 0);
}

void Render_SetTextureHandle(u16 id) {
    if (id == 0xFFFF) {
        GXSetNumTexGens(0);
        GXSetTevOp(0, 4);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        m_curTexture = 0xFFFF;
    } else if (m_curTexture != id) {
        GXSetNumTexGens(1);
        GXSetTevOp(0, 0);
        GXSetTevOrder(0, 0, 0, 4);
        GXLoadTexObj(&m_gxTexObjArray[id], 0);
        m_curTexture = id;
    }
}

