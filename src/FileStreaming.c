#include "common.h"

extern u8 *g_StreamingDataPacks[11];
extern u32 g_MaxStreamingDataPacks[11];
extern s32 g_numQuadPacksInMemory;
extern s32 g_numQuadPacksToLoad;
extern s32 g_itemTimer;
extern void *g_pDataPackToLoad;
void *Allocate(u32 size, s32 type, const char *name);
void memset(void *dest, s32 value, u32 count);

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

s32 FileStreaming_GetNumAvailableQuadPacks(void)
{
    return 0xB;
}

