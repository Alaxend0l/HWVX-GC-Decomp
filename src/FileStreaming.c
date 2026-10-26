#include "common.h"

extern u8 *g_StreamingDataPacks[11];
extern u32 g_MaxStreamingDataPacks[11];
extern s32 g_numQuadPacksInMemory;
extern s32 g_numQuadPacksToLoad;
extern s32 g_itemTimer;
extern void *g_pDataPackToLoad;
void *Allocate(u32 size, s32 type, const char *name);
void memset(void *dst, s32 c, u32 n);
void Deallocate(void *alloc);
s32 FileStreaming_ReleaseAllDataPacks(s32 type, s8 wait);
extern u8 g_bLoadingThreadRunning;
extern s8 g_bLoadQuadrants;
extern s8 g_bLoadDataPack;
extern s32 g_numDataPacksToRelease;
extern s32 g_numQuadPacksToRelease;
extern void *g_pPacksToBeReleased[10];
extern void *g_pQuadPackToBeReleased[11];
extern s32 g_dataPackToLoadType;
s32 FileStreaming_GetPackID(char *name, s32 type, u32 id);
void FileStreaming_HeartBeat(void);
void LoadDataPack(void);
void FileStreaming_LoadingThread(void);
void DataPack_LoadFile(s32 file, void *pack, s32 type, s32 size, s32 *out);
void DataPack_ReleaseDataPack(void *pack);
void DataPack_ReleaseQuadPack(void *pack);
void LoadQuadrants(s32 arg0);
void EndThread(void);

#pragma push
#pragma use_lmw_stmw on

void FileStreaming_Initialize(void)
{
    u32 i;
    u32 size;

    g_numQuadPacksInMemory = 0;
    g_numQuadPacksToLoad = 0;
    g_itemTimer = 0;
    for (i = 0; i < 11; i++) {
        if (i != 7 && g_MaxStreamingDataPacks[i] != 0) {
            size = g_MaxStreamingDataPacks[i] * 0xA8;
            g_StreamingDataPacks[i] = (u8 *) Allocate(size, 0, "");
            memset(g_StreamingDataPacks[i], 0, size);
        } else {
            g_StreamingDataPacks[i] = NULL;
        }
    }
    g_pDataPackToLoad = NULL;
}

void FileStreaming_Destroy(void)
{
    u32 i;

    for (i = 0; i < 11; i++) {
        FileStreaming_ReleaseAllDataPacks(i, 1);
        Deallocate(g_StreamingDataPacks[i]);
    }
}

u8 IsLoadingThreadRunning(void)
{
    return g_bLoadingThreadRunning;
}

s32 FileStreaming_GetNumAvailableQuadPacks(void)
{
    return 0xB;
}

void FileStreaming_LoadingThread(void)
{
    g_bLoadingThreadRunning = 1;
    if (g_bLoadQuadrants)
    {
        LoadQuadrants(0);
    }
    if (g_bLoadDataPack)
    {
        LoadDataPack();
    }
    g_bLoadQuadrants = 0;
    g_bLoadDataPack = 0;
    g_bLoadingThreadRunning = 0;
    EndThread();
}

s32 FileStreaming_GetPackID(char *name, s32 type, u32 id)
{
    u8 *pack;
    u32 i;

    if (type >= 11)
    {
        return -1;
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++)
    {
        pack = g_StreamingDataPacks[type];
        if ((s8)pack[i * 0xA8] != 0 && id == M2C_FIELD(pack + i * 0xA8, u32 *, 8))
        {
            if (name == M2C_FIELD(pack + i * 0xA8, char **, 4) || strcmp(name, M2C_FIELD(pack + i * 0xA8, char **, 4)) == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

#pragma push
#pragma use_lmw_stmw on
s32 FileStreaming_ReleaseAllDataPacks(s32 type, s8 wait)
{
    u8 *pack;
    u32 i;

    if (type >= 11)
    {
        return 0;
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++)
    {
        pack = g_StreamingDataPacks[type];
        if (pack != NULL && (s8)pack[i * 0xA8 + 1] != 0)
        {
            M2C_FIELD(pack + i * 0xA8, s32 *, 0xC) = 1;
            FileStreaming_ReleaseDataPack(i, type);
        }
    }
    if (wait)
    {
        FileStreaming_HeartBeat();
        FileStreaming_HeartBeat();
    }
    return 1;
}

#pragma push
#pragma use_lmw_stmw off
void FileStreaming_HeartBeat(void)
{
    u32 i;

    for (i = 0; i < g_numDataPacksToRelease; i++)
    {
        DataPack_ReleaseDataPack((u8 *)g_pPacksToBeReleased[i] + 0x14);
        ((u8 *)g_pPacksToBeReleased[i])[0] = 0;
    }
    g_numDataPacksToRelease = 0;
    for (i = 0; i < g_numQuadPacksToRelease; i++)
    {
        DataPack_ReleaseQuadPack(g_pQuadPackToBeReleased[i]);
    }
    g_numQuadPacksToRelease = 0;
}

#pragma push
#pragma use_lmw_stmw off
void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh)
{
    if (type >= 11 || index >= g_MaxStreamingDataPacks[type])
    {
        return NULL;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8] == 0 || (s8)g_StreamingDataPacks[type][index * 0xA8 + 1] == 0)
    {
        return NULL;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8 + 2] == 0 && refresh)
    {
        DataPack_RefreshPtrs(g_StreamingDataPacks[type] + index * 0xA8 + 0x14);
    }
    g_StreamingDataPacks[type][index * 0xA8 + 2] = 1;
    return g_StreamingDataPacks[type] + index * 0xA8 + 0x14;
}
#pragma pop

#pragma pop

#pragma push
#pragma use_lmw_stmw off
void LoadDataPack(void)
{
    s32 result;
    u8 *pack;

    pack = (u8 *)g_pDataPackToLoad;
    if (pack != NULL)
    {
        g_pDataPackToLoad = NULL;
        pack[1] = 1;
        DataPack_LoadFile(M2C_FIELD(pack, s32 *, 4), pack + 0x14, g_dataPackToLoadType, M2C_FIELD(pack, s32 *, 8), &result);
    }
}

