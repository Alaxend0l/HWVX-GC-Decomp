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

