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

void DataPack_EnableLoadingSounds(s8 enabled)
{
    g_bSoundLoadingEnabled = enabled;
}
