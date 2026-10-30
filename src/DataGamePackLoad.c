#include "common.h"

extern FileTOCCacheEntry g_fileTOCCache[11];
extern s8 g_bSoundLoadingEnabled;
extern s32 g_fileID;
extern s32 g_openFileDataPackType;
void File_Close(s32 file);



void ShaderHolder_RemoveGX(s32 gx);



void VertexColorSeqManager_RemoveVertices(void *vertices);
void Deallocate(void *alloc);
s32 FileStreaming_LoadDataPack(void *name, s32 type, u32 id, s8 threaded);
void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
void ShaderHolder_AddGX(void *gx, u32 index);
void VertexColorSeqManager_AddVertices(void *vertices);

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

#pragma push
#pragma use_lmw_stmw on
void DataPack_LoadQuadPack(s32 file, QuadPack *quad) {
    s32 File_Seek(s32 handle, s32 offset, s8 mode);
    u32 File_Read(s32 handle, u32 size, void *buf);
    void *Allocate(u32 size, u32 flags, const char *name);
    void UpdatePointers(u32 *offsets, u32 count, u8 *base);
    s32 *reloc;
    u8 *data;
    u32 *pointers;
    u32 i;
    s32 *texRelocs = NULL;
    QuadPackHeader *header;
    u32 offset;
    u8 *base;
    u32 packID;
    u32 n;
    void *alloc;
    u32 idx;

    if (quad->enabled != 0) {
        if ((u32)quad->fileOffset == 0) {
            quad->enabled = 0;
            return;
        }
        File_Seek(file, quad->fileOffset, 0);
        alloc = Allocate(quad->size + 0x40, 0, "");
        quad->alloc = alloc;
        if (alloc != NULL) {
            data = (u8 *)(((u32)alloc + 0x3F) & ~0x3F);
            File_Read(file, quad->size, data);
            quad->header = (QuadPackHeader *)data;
            offset = 0x54;
            header = quad->header;
            if (header->valid == 0) {
                quad->data = NULL;
                return;
            }
            n = header->numRelocs;
            if (n != 0) {
                header->relocs = (s32 *)(data + offset);
                offset += n * 8;
            }
            header = quad->header;
            n = header->numShaders;
            if (n != 0) {
                header->shaderList = (s32 *)(data + offset);
                offset += n * 8;
            }
            header = quad->header;
            n = header->numTexRelocs;
            if (n != 0) {
                texRelocs = (s32 *)(data + offset);
                offset += n * 8;
            }
            pointers = (u32 *)(data + offset);
            offset += header->numPointers * 4;
            offset += header->dataSize;
            quad->gxBase = (s32)(data + offset);
            reloc = texRelocs;
            for (i = 0; i < quad->header->numTexRelocs; i++) {
                base = (u8 *)quad->gxBase;
                packID = reloc[1] & 0xFFFF0000;
                if (packID != 0) {
                    packID >>= 16;
                    header = quad->header;
                    idx = header->refs[0].id != packID;
                    if (header->refs[idx].pack == NULL) {
                        header->refs[idx].pack = FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack(&header->refs[idx], 8, packID, 0), 8, 1);
                    }
                    *(u32 *)(base + reloc[0]) = ((u32 *)header->refs[idx].pack->base)[reloc[1] & 0xFFFF];
                } else {
                    *(u32 *)(base + reloc[0]) += (u32)base;
                }
                reloc += 2;
            }
            UpdatePointers(pointers, quad->header->numPointers, (u8 *)quad->gxBase);
            quad->data = ((void **)quad->gxBase)[0];
            quad->vertices = ((void **)quad->gxBase)[1];
        }
    }
}

void DataPack_RefreshQuadPackPtrs(DataPack *pack, void *quad)
{
    s32 *reloc;
    s32 *shader;
    u32 id;
    QuadPackHeader *header;
    u8 *base;
    u32 i;
    u32 idx;
    u32 texID;

    header = *(QuadPackHeader **)((u8 *)quad + 0x28);
    base = *(u8 **)((u8 *)quad + 0x2C);
    reloc = header != NULL ? header->relocs : NULL;
    shader = header != NULL ? header->shaderList : NULL;
    if (header != NULL)
    {
        for (i = 0; i < header->numRelocs; i++)
        {
            id = reloc[1];
            if (id & 0xFFFF0000)
            {
                u32 packID = id >> 16;
                idx = header->refs[0].id != packID;
                if (header->refs[idx].pack == NULL)
                {
                    header->refs[idx].pack =
                        FileStreaming_InitializeDataPack(
                            FileStreaming_LoadDataPack(&header->refs[idx], 8, packID, 0), 8, 1);
                }
                texID = header->refs[idx].pack->textureIDs[id & 0xFFFF];
            }
            else
            {
                texID = pack->textureIDs[id];
            }
            *(u32 *)(base + reloc[0]) = texID;
            reloc += 2;
        }
        for (i = 0; i < header->numShaders; i++)
        {
            ShaderHolder_AddGX(base + shader[0], pack->shaderIDs[shader[1]]);
            shader += 2;
        }
        if (*(void **)((u8 *)quad + 0x14) != NULL)
        {
            VertexColorSeqManager_AddVertices(*(void **)((u8 *)quad + 0x14));
        }
    }
}

#pragma push
#pragma use_lmw_stmw on
void DataPack_RefreshTextures(DataPack *pack) {
    void Gfx_LoadPalettes_RAM(void *data, void *palettes);

    Gfx_LoadPalettes_RAM(pack->paletteData, pack->palettes);
}

void DataPack_RefreshPtrs(DataPack *pack) {
    void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
    u16 ShaderHolder_AddShader(s32 shader);
    void ShaderHolder_AddGX(void *gx, u32 index);
    s32 Trigger_AddTrigger(s32 *trigger);
    u8 *base;
    u32 i;
    u8 *trigger;
    u32 packID;
    u32 idx;
    u32 id;

    base = pack->base;
    if (base != NULL) {
        DataPack_RefreshTextures(pack);
        for (i = 0; i < pack->numSoundRelocs; i++) {
            id = pack->soundRelocs[i * 2 + 1];
            if (id & 0xFFFF0000) {
                packID = id >> 16;
                idx = pack->refs[0].id != packID;
                if (pack->refs[idx].pack == NULL) {
                    pack->refs[idx].pack = FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack(&pack->refs[idx], 8, packID, 0), 8, 1);
                }
                id = pack->refs[idx].pack->soundIDs[id & 0xFFFF];
            } else {
                id = pack->soundIDs[id];
            }
            *(u32 *)(base + pack->soundRelocs[i * 2]) = id;
        }
        for (i = 0; i < pack->numShaders; i++) {
            pack->shaderIDs[i] = ShaderHolder_AddShader(pack->shaders[i].shader);
        }
        for (i = 0; i < pack->numGX; i++) {
            ShaderHolder_AddGX(base + pack->gxList[i * 2], pack->shaderIDs[pack->gxList[i * 2 + 1]]);
        }
        trigger = pack->triggers;
        for (i = 0; i < pack->numTriggers; i++) {
            Trigger_AddTrigger((s32 *)trigger);
            trigger += 0x2C;
        }
    }
}

void DataPack_ReleaseDataPack(DataPack *pack) {
    void DestroySFX(u16 id);
    void ShaderHolder_RemoveShader(s32 shader);
    void Gfx_UnLoadTexture(u16 id);
    s32 Trigger_ReleaseTrigger(s32 *trigger);
    void Action_ReleaseAllActions(void);
    void *memset(void *dst, s32 c, u32 n);
    s8 releasedTriggers = 0;
    u32 *gx;
    u8 *trigger;
    u32 i;

    if (pack->soundIDs != NULL) {
        for (i = 0; i < pack->numSounds; i++) {
            if (pack->soundIDs[i] != 0) {
                DestroySFX(pack->soundIDs[i]);
            }
        }
        gx = pack->gxList;
        for (i = 0; i < pack->numGX; i++) {
            ShaderHolder_RemoveGX((s32)pack->base + *gx);
            gx += 2;
        }
        for (i = 0; i < pack->numShaders; i++) {
            ShaderHolder_RemoveShader(pack->shaders[i].shader);
        }
        for (i = 0; i < pack->numTextures; i++) {
            Gfx_UnLoadTexture(pack->textureIDs[i]);
        }
        trigger = pack->triggers;
        for (i = 0; i < pack->numTriggers; i++, trigger += 0x2C) {
            Trigger_ReleaseTrigger((s32 *)trigger);
            releasedTriggers = 1;
        }
        if (releasedTriggers) {
            Action_ReleaseAllActions();
        }
        Deallocate(pack->soundIDs);
        Deallocate(pack->soundData);
        pack->soundIDs = NULL;
        pack->soundData = NULL;
        memset(pack, 0, 0x94);
    }
}

#pragma pop

#pragma pop
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
