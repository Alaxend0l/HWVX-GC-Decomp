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

/* In DataGamePackLoad.c the quad pack's shader object is reached through a
   plain u32 pointer, so this file's view of a QuadPack is just its header. */
typedef struct QuadPackRef {
    void *pack;                            /* 0x00 */
    u32 id;                                /* 0x04 */
} QuadPackRef;

typedef struct QuadPackHeader {
    u32 numRelocs;    /* 0x00 */
    u32 numShaders;   /* 0x04 */
    u32 numRefs;      /* 0x08 */
    void *refs;       /* 0x0C */
    u32 *relocs;      /* 0x10 */
    u32 *shaders;     /* 0x14 */
    u32 unk18[4];     /* 0x18 */
    u32 texCount;     /* 0x28 */
    u16 *texIDs;      /* 0x2C */
} QuadPackHeader;
void VertexColorSeqManager_RemoveVertices(void *vertices);
void Deallocate(void *alloc);

void DataPack_EnableLoadingSounds(s8 enabled)
{
    g_bSoundLoadingEnabled = enabled;
}

void DataPack_Initialize(void)
{
    u32 i;

    for (i = 0; i < 11; i++) {
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

void DataPack_ReleaseQuadPack(QuadPack *pack)
{
    s32 *list;
    s32 base;
    u32 i;

    if (pack->gxObject != NULL) {
        base = pack->gxBase;
        list = pack->gxObject->shaderList;
        for (i = 0; i < pack->gxObject->count; i++) {
            ShaderHolder_RemoveGX(base + *list);
            list += 2;
        }
        if (pack->vertices != NULL) {
            VertexColorSeqManager_RemoveVertices(pack->vertices);
        }
        Deallocate(pack->alloc);
        pack->alloc = NULL;
        pack->data = NULL;
        pack->vertices = NULL;
        pack->gxBase = 0;
        pack->gxObject = NULL;
        pack->flag34 = 0;
    }
}

u8 *DataPack_GetScript(void *pack, s32 id)
{
    u32 *entries;
    u32 i;
    u8 *table = *(u8 **)pack;

    if (table == NULL)
    {
        return NULL;
    }
    for (i = 0; i < M2C_FIELD(table, u32 *, 0); i++)
    {
        entries = M2C_FIELD(table, u32 **, 4);
        if (id == entries[i * 2 + 1])
        {
            return (u8 *)entries[i * 2];
        }
    }
    return NULL;
}

