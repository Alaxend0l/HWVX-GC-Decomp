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
void Render_CreateTextureSlot();
int sprintf(char *s, const char *format, ...);
s32 File_Open(u32 dev, const char *name, u32 flags, s32 *size);
s32 File_Read(s32 handle, s32 size, void *data);
void File_Close(s32 handle);
void Render_DestroyTextureSlot(u32 id);
s32 File_Seek(s32 handle, s32 offset, s8 mode);

typedef struct BTFHeader {
    u8 version;
    u8 unk1;
    s16 width;
    s16 height;
    s16 bpp;
    s16 numColors;
    s16 numMips;
    s32 unkC;
} BTFHeader;

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

#pragma push
#pragma use_lmw_stmw on
s32 LoadTGA(const char *name, u32 id) {
    u8 header[18];
    char path[56];
    s32 size;
    s32 handle;
    u8 *dst;
    u8 *src;
    u8 *buf;
    s16 width;
    s16 height;
    s32 bpp;
    s32 i;

    sprintf(path, "%s.tga", name);
    handle = File_Open(0, path, 0, 0);
    if (handle == -1) {
        return 0;
    }
    File_Read(handle, 18, header);
    m_texture[id].width = height = header[14] + header[15] * 256;
    m_texture[id].unk30 = width = header[12] + header[13] * 256;
    if (height > 0x400 || width > 0x400) {
        return 0;
    }
    if (height == 0 || width == 0) {
        return 0;
    }
    i = width * height;
    m_texture[id].height = 0;
    bpp = 3;
    if (header[16] == 32) {
        bpp = 4;
    }
    dst = Allocate(i * bpp, 0, "");
    m_texture[id].data = dst;
    size = (header[16] >> 3) * (m_texture[id].unk30 * m_texture[id].width);
    src = buf = Allocate(size, 0, "");
    switch (header[16]) {
    case 8:
        break;
    case 32:
        m_texture[id].unk38 = 1;
        File_Read(handle, size, src);
        for (i = 0; i < size; i += 4) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            dst[3] = src[3];
            src += 4;
            dst += 4;
        }
        break;
    case 24:
        m_texture[id].unk38 = 0;
        File_Read(handle, size, src);
        for (i = 0; i < size; i += 3) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            src += 3;
            dst += 3;
        }
        break;
    case 16:
        m_texture[id].unk38 = 0;
        File_Read(handle, size, src);
        for (i = 0; i < size; i += 2) {
            dst[0] = (src[0] & 0x7C00) >> 7;
            dst[1] = (src[0] & 0x3E0) >> 2;
            dst[2] = (src[0] & 0x1F) << 3;
            src += 2;
            dst += 3;
        }
        break;
    }
    File_Close(handle);
    Deallocate(buf);
    Render_CreateTextureSlot(id, 1);
    return 1;
}
#pragma pop
void Gfx_SetTextureWrapMode(u16 id, u16 mode) {
    Texture *tex = &m_texture[id];

    tex->wrapMode = mode;
    Render_CreateTextureSlot(id, 1, tex);
}

void Gfx_LoadPalettes_RAM(void) {

}
