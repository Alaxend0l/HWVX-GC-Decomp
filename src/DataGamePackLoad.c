#include "common.h"

typedef struct FileTOCEntry {
    u32 version;
    u32 type;
    u32 id;
    u8 unkC[0x14 - 0xC];
    u32 offset;
} FileTOCEntry;

typedef struct FileTOCCacheEntry {
    FileTOCEntry entries[50];
    u32 count;
    char name[0x4C];
} FileTOCCacheEntry;

extern FileTOCCacheEntry g_fileTOCCache[11];
extern s8 g_bSoundLoadingEnabled;
extern s32 g_fileID;
extern s32 g_openFileDataPackType;
void File_Close(s32 file);

typedef struct GXObject {
    u8 pad0[0x1C];
    u32 count;          /* 0x1C */
    u8 pad1F[0x4];
    s32 *shaderList;    /* 0x20 */
} GXObject;

typedef struct QuadPack {
    u8 pad00[0x10];
    void *data;         /* 0x10 */
    void *vertices;     /* 0x14 */
    u8 pad18[0x10];
    GXObject *gxObject; /* 0x28 */
    s32 gxBase;         /* 0x2C */
    u8 pad30[0x4];
    u8 flag34;          /* 0x34 */
    u8 pad35[0xC];
    void *alloc;        /* 0x44 */
} QuadPack;

void ShaderHolder_RemoveGX(s32 gx);
void VertexColorSeqManager_RemoveVertices(void *vertices);
void Deallocate(void *alloc);

void DataPack_EnableLoadingSounds(s8 enabled)
{
    g_bSoundLoadingEnabled = enabled;
}

void DataPack_Initialize(void)
{
    u32 i;

    for (i = 0; i < 21; i++) {
        g_fileTOCCache[i].name[0] = 0;
    }
}

void DataPack_CloseFiles(void)
{
    if (g_fileID > 0) {
        File_Close(g_fileID);
        g_fileID = -1;
        g_openFileDataPackType = 0xB;
    }
}

