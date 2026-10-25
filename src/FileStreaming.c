#include "common.h"

extern u8 *g_StreamingDataPacks[11];
extern u32 g_MaxStreamingDataPacks[11];
extern s32 g_numQuadPacksInMemory;
extern s32 g_numQuadPacksToLoad;
extern s32 g_itemTimer;
extern void *g_pDataPackToLoad;
void *Allocate(u32 size, s32 type, const char *name);
void memset(void *dest, s32 value, u32 count);
