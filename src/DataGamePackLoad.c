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
#pragma readonly_strings on
s32 DataPack_LoadFile(char *name, s32 arg1, u32 type, u32 id) {
    extern u32 g_buffer[];
    char *strcpy(char *dst, const char *src);
    char *strcat(char *dst, const char *src);
    int strcmp(const char *a, const char *b);
    char *strncpy(char *dst, const char *src, u32 n);
    u32 strlen(const char *s);
    void *memcpy(void *dst, const void *src, u32 n);
    int sprintf(char *buf, const char *fmt, ...);
    void Dprintf(const char *fmt, ...);
    s32 File_Open(u32 dev, const char *name, u32 flags, s32 *size);
    u32 File_Read(s32 handle, u32 size, void *buf);
    s32 File_Seek(s32 handle, s32 offset, s8 mode);
    void DataPack_LoadDataPack(s32 file, s32 arg1);
    FileTOCCacheEntry *toc;
    char *tocName;
    u32 i;
    s8 found;
    char path[0x80];

    found = 0;
    strcpy(path, name);
    strcat(path, ".xgc");
    if ((g_fileID != -1 && g_fileTOCCache[type].name == NULL) || strcmp(g_fileTOCCache[type].name, name) != 0) {
        for (i = 0; i < 11; i++) {
            if (i != type && g_fileTOCCache[i].name != NULL && strcmp(g_fileTOCCache[i].name, name) == 0) {
                memcpy(&g_fileTOCCache[type], &g_fileTOCCache[i], sizeof(FileTOCCacheEntry));
                if (g_fileID && g_openFileDataPackType == i) {
                    g_openFileDataPackType = type;
                } else if (g_openFileDataPackType == type) {
                    File_Close(g_fileID);
                    g_fileID = -1;
                    g_openFileDataPackType = 0xB;
                }
                break;
            }
        }
    }
    if (g_fileID == -1 || (tocName = g_fileTOCCache[type].name) == NULL || strcmp(tocName, name) != 0 ||
        (g_openFileDataPackType != type && strcmp(tocName, g_fileTOCCache[g_openFileDataPackType].name) != 0)) {
        if (g_fileID != -1) {
            File_Close(g_fileID);
            g_openFileDataPackType = 0xB;
        }
        g_fileID = File_Open(0, path, 0, 0);
        if (g_fileID == -1) {
            if (strcmp(name, "mcp") != 0) {
                char msg[0x80];
                sprintf(msg, "Could not open the data pack: %s\n", path);
                Dprintf(msg);
            }
            return 0;
        }
        g_openFileDataPackType = type;
    }
    toc = &g_fileTOCCache[type];
    tocName = toc->name;
    if (tocName == NULL || strcmp(tocName, name) != 0) {
        File_Read(g_fileID, 0x800, g_buffer);
        if (g_buffer[0] != 0x020100A0 || g_buffer[1] != 0x040100AE) {
            File_Close(g_fileID);
            g_fileID = -1;
            g_openFileDataPackType = 0xB;
            return 0;
        }
        if (g_buffer[2] != 0xB7) {
            char msg[0x80];
            File_Close(g_fileID);
            g_fileID = -1;
            g_openFileDataPackType = 0xB;
            sprintf(msg, "The data pack is out of date: %s. Packfile:%d,  Code:%d\nYou need to repack the data using the latest BrainCore\n", path, g_buffer[2], 0xB7);
            Dprintf(msg);
            return 0;
        }
        g_fileTOCCache[type].count = g_buffer[3];
        memcpy(toc, &g_buffer[4], g_fileTOCCache[type].count * sizeof(FileTOCEntry));
        if (strlen(name) >= 0x4C) {
            strncpy(tocName, name, 0x4C);
            g_fileTOCCache[type].name[0x4B] = 0;
        } else {
            strcpy(tocName, name);
        }
    }
    for (i = 0; i < toc->count; i++) {
        if (type == toc->entries[i].type && id == toc->entries[i].id) {
            if (toc->entries[i].version != 0xB7) {
                char msg[0x80];
                sprintf(msg, "The data pack is out of date: %s. Packfile:%d,  Code:%d\nYou need to repack the data using the latest BrainCore\n", path, toc->entries[i].version, 0xB7);
                Dprintf(msg);
                File_Close(g_fileID);
                g_fileID = -1;
                g_openFileDataPackType = 0xB;
                return 0;
            }
            File_Seek(g_fileID, g_fileTOCCache[type].entries[i].offset + 0x10 + g_fileTOCCache[type].count * sizeof(FileTOCEntry), 0);
            found = 1;
            break;
        }
    }
    if (!found) {
        char msg[0x80];
        sprintf(msg, "Could not load the data pack with Pack ID: %d. Within the file: %s\n.Check the PackID and repack using the latest BrainCore.\n", id, path);
        Dprintf(msg);
        File_Close(g_fileID);
        g_fileID = -1;
        g_openFileDataPackType = 0xB;
        return 0;
    }
    DataPack_LoadDataPack(g_fileID, arg1);
    return 1;
}

typedef struct DataPackFileHeader {
    u32 dataSize;
    u32 soundDataSize;
    u32 numSoundPointers;
    u32 numPointers;
    u32 numRelocs;
    u32 numTextures;
    u32 textureDataOffset;
    u32 numSoundRelocs;
    u32 numSounds;
    u32 sfxOffset;
    u32 palettes;
    u32 paletteOffset;
    u32 numShaders;
    u32 numGX;
    u32 numTexRelocs;
    u32 shaderOffset;
    u32 numTriggers;
    u32 triggerOffset;
    u32 songOffset;
    u32 songBankOffset;
    DataPackRef refs[2];
} DataPackFileHeader;

void DataPack_LoadDataPack(s32 file, DataPack *pack) {
    void *Allocate(u32 size, u32 flags, const char *name);
    extern s8 g_bSoundLoadingEnabled;
    void *memset(void *dst, s32 c, u32 n);
    void *memcpy(void *dst, const void *src, u32 n);
    int sprintf(char *buf, const char *fmt, ...);
    void Dprintf(const char *fmt, ...);
    u32 File_Read(s32 handle, u32 size, void *buf);
    void UpdatePointers(u32 *offsets, u32 count, u8 *base);
    void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
    void Deallocate(void *p);
    void SongManager_LoadSong(void *song, void *bank);
    u16 CreateSFX(void *sfx);
    static DataPackFileHeader header;
    char msg[0x68];
    u32 relocSize;
    s8 freeData;
    u32 soundIDsSize;
    u8 *alloc;
    u32 i;
    u32 readSize;
    u32 size;
    u32 pad;
    u8 *data;
    u8 *sfx;
    u32 offset;
    u32 *reloc;
    u32 *pointers;
    u32 packID;
    u32 idx;

    memset(pack, 0, sizeof(DataPack));
    File_Read(file, sizeof(DataPackFileHeader), &header);
    pack->numRelocs = header.numRelocs;
    pack->numSoundRelocs = header.numSoundRelocs;
    pack->numTextures = header.numTextures;
    pack->numSounds = header.numSounds;
    pack->palettes = (void *)header.palettes;
    pack->numShaders = header.numShaders;
    pack->numGX = header.numGX;
    pack->numTriggers = header.numTriggers;
    pack->songLoaded = 0;
    pack->dataSize = header.dataSize;
    memcpy(pack->refs, header.refs, sizeof(header.refs));
    size = header.numRelocs * 8 + ((pack->numSounds + pack->numTextures) * 2 + (s32)pack->numShaders * 2 + header.numGX * 8 + header.numSoundRelocs * 8 + header.dataSize + 0x60) + (relocSize = header.numTexRelocs * 8 + header.numPointers * 4);
    alloc = Allocate(size, 0, "");
    if (alloc == NULL) {
        return;
    }
    pack->soundIDs = (u16 *)alloc;
    soundIDsSize = pack->numSounds * 2;
    if (header.soundDataSize != 0) {
        freeData = 0;
        pad = (header.numSoundPointers * 4) & 0x1F;
        readSize = header.numSoundPointers * 4;
        readSize += pad == 0 ? 0 : 0x20 - pad;
        readSize += header.soundDataSize;
        if (readSize + 0x60 <= size - soundIDsSize) {
            pack->soundData = alloc + soundIDsSize;
        } else {
            pack->soundData = Allocate(readSize + 0x60, 0, "");
            freeData = 1;
        }
        sfx = (u8 *)(((u32)pack->soundData + 0x3F) & ~0x3F);
        File_Read(file, readSize, sfx);
        data = sfx;
        sfx += header.numSoundPointers * 4;
        pack->soundBase = (u8 *)(((u32)sfx + 0x1F) & ~0x1F);
        UpdatePointers((u32 *)data, header.numSoundPointers, pack->soundBase);
        pack->sfxData = (u8 *)(header.sfxOffset + (u32)pack->soundBase);
        if (header.songOffset != 0 || (header.soundDataSize != 0 && header.numSoundRelocs == 0)) {
            pack->song = (void *)(header.songOffset + (u32)pack->soundBase);
        }
        if (header.songBankOffset != 0) {
            pack->songBank = (void *)(header.songBankOffset + (u32)pack->soundBase);
        }
        pack->soundDataSize = header.soundDataSize;
        if (pack->song != NULL && pack->songBank != NULL && g_bSoundLoadingEnabled) {
            SongManager_LoadSong(pack->song, pack->songBank);
            pack->songLoaded = 1;
        }
        sfx = pack->sfxData;
        for (i = 0; i < pack->numSounds; i++) {
            if (g_bSoundLoadingEnabled) {
                pack->soundIDs[i] = CreateSFX(sfx);
            } else {
                pack->soundIDs[i] = 0;
            }
            if (pack->soundIDs[i] == 0 && g_bSoundLoadingEnabled) {
                sprintf(msg, "Could not load the SFX, out of sound space.  SFX size = %d\n", *(u32 *)(sfx + 4));
                Dprintf(msg);
            }
            sfx += 0x20;
        }
        if (freeData) {
            Deallocate(pack->soundData);
            pack->soundData = NULL;
            pack->soundBase = NULL;
            pack->sfxData = NULL;
            pack->song = NULL;
            pack->songBank = NULL;
        }
        pack->soundData = NULL;
        pack->soundBase = NULL;
        pack->sfxData = NULL;
        pack->song = NULL;
        pack->songBank = NULL;
    }
    pack->textureIDs = (u16 *)(alloc + soundIDsSize);
    offset = soundIDsSize + pack->numTextures * 2;
    pack->shaderIDs = (u16 *)(alloc + offset);
    readSize = header.numRelocs * 8 + header.numGX * 8 + header.numSoundRelocs * 8;
    pad = readSize & 0x1F;
    pad = pad == 0 ? 0 : 0x20 - pad;
    offset += pack->numShaders * 2;
    data = alloc + offset;
    data = (u8 *)(((u32)data + 0x3F) & ~0x3F);
    readSize += pad;
    readSize += header.dataSize;
    readSize += relocSize;
    File_Read(file, readSize, data);
    pack->relocs = (s32 *)data;
    offset = header.numRelocs * 8;
    pack->gxList = (u32 *)(data + offset);
    offset += header.numGX * 8;
    pack->soundRelocs = (u32 *)(data + offset);
    offset += header.numSoundRelocs * 8;
    pack->base = data + offset;
    pack->base = (u8 *)(((u32)pack->base + 0x1F) & ~0x1F);
    offset = header.dataSize + offset;
    offset = pad + offset;
    reloc = (u32 *)(data + offset);
    pointers = reloc + header.numTexRelocs * 2;
    for (i = 0; i < header.numTexRelocs; i++) {
        packID = reloc[1] & 0xFFFF0000;
        if (packID != 0) {
            packID >>= 16;
            idx = pack->refs[0].id != packID;
            if (pack->refs[idx].pack == NULL) {
                pack->refs[idx].pack = FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack(&pack->refs[idx], 8, packID, 0), 8, 1);
            }
            *(u32 *)(pack->base + reloc[0]) = ((u32 *)pack->refs[idx].pack->base)[reloc[1] & 0xFFFF];
        } else {
            *(u32 *)(pack->base + reloc[0]) += (u32)pack->base;
        }
        reloc += 2;
    }
    UpdatePointers(pointers, header.numPointers, pack->base);
    pack->paletteData = (void *)(header.paletteOffset + (u32)pack->base);
    pack->shaders = (DataPackShader *)(header.shaderOffset + (u32)pack->base);
    pack->textureData = header.textureDataOffset + (u32)pack->base;
    pack->triggers = (u8 *)(header.triggerOffset + (u32)pack->base);
}
#pragma pop

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
    u16 Gfx_LoadTexture_RAM(u32 *data);
    void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
    u8 *base;
    u32 texData;
    u32 id;
    u32 i;
    u32 idx;
    u32 texID;

    base = pack->base;
    Gfx_LoadPalettes_RAM(pack->paletteData, pack->palettes);
    texData = pack->textureData;
    for (i = 0; i < pack->numTextures; i++) {
        pack->textureIDs[i] = Gfx_LoadTexture_RAM(&texData);
        if (texData & 0x1F) {
            texData += 0x20 - (texData & 0x1F);
        }
    }
    for (i = 0; i < pack->numRelocs; i++) {
        id = pack->relocs[i * 2 + 1];
        if (id & 0xFFFF0000) {
            u32 packID = id >> 16;
            idx = pack->refs[0].id != packID;
            if (pack->refs[idx].pack == NULL) {
                pack->refs[idx].pack = FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack(&pack->refs[idx], 8, packID, 0), 8, 1);
            }
            texID = pack->refs[idx].pack->textureIDs[id & 0xFFFF];
        } else {
            texID = pack->textureIDs[id];
        }
        *(u32 *)(base + pack->relocs[i * 2]) = texID;
    }
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
