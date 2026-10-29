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

extern Texture *m_texture;
extern u32 m_totalTextures;

void *Allocate(u32 size, s32 flags, const char *name);
void Deallocate(void *p);
void *memset(void *dst, int c, u32 n);
void Render_CreateTextureSlot(u32 id, s32 arg1, Texture *texture);
void Render_DestroyTextureSlot(u32 id);

void Gfx_LoadPalettes_RAM(void) {

}
