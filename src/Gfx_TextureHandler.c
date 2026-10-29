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

s32 Gfx_TextureHandler_Create(void) {
    m_totalTextures = 0x1FF;
    m_texture = Allocate(0x7FC0, 0, "");
    memset(m_texture, 0, m_totalTextures * sizeof(Texture));
    return 1;
}

void Gfx_TextureHandler_Destroy(void) {
    u32 i;

    for (i = 1; i < m_totalTextures; i++) {
        Render_DestroyTextureSlot(i);
        if (m_texture[i].data != NULL && m_texture[i].unk3A == 0) {
            Deallocate(m_texture[i].data);
        }
    }
    Deallocate(m_texture);
    m_texture = NULL;
}

void Gfx_FontManager_Init(void);
void Gfx_TextureManager_Init(void) {
    u32 i;

    for (i = 1; i < m_totalTextures; i++) {
        Render_DestroyTextureSlot(i);
        if (m_texture[i].data != NULL && m_texture[i].unk3A == 0) {
            Deallocate(m_texture[i].data);
        }
        memset(&m_texture[i], 0, 0x20);
        m_texture[i].refCount = 0;
        m_texture[i].unk30 = 0;
        m_texture[i].width = 0;
        m_texture[i].data = NULL;
    }
    Gfx_FontManager_Init();
}

void Gfx_SetTextureWrapMode(u16 id, u16 mode) {
    Texture *tex = &m_texture[id];

    tex->wrapMode = mode;
    Render_CreateTextureSlot(id, 1, tex);
}

void Gfx_LoadPalettes_RAM(void) {

}
