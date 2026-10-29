#include "common.h"

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s16 s16;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct {
    u32 dummy[8];
} GXTexObj;

void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrap_s, s32 wrap_t, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
void GXLoadTexObj(GXTexObj *obj, s32 id);
void GXBegin(s32 type, s32 vtxfmt, u16 nverts);

void GXSetZMode(int, int, int);
void GXSetBlendMode(int, int, int, int);
void GXSetNumTexGens(int);
void GXSetNumChans(int);
void GXSetNumTevStages(int);
void GXSetTevOrder(int, int, int, int);
void GXSetTevOp(int, int);
void GXSetTevSwapMode(int, int, int);
void GXSetTevSwapModeTable(int, int, int, int, int);
void THPGXRestore(void) {
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0xF);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
}
