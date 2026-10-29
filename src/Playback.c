#include "common.h"


s32 File_Write(u32 id, s32 size, void *data);
extern u32 g_fileIOHandle;
extern s32 g_playbackCache[0x1000];
extern u32 g_playbackPos;
void Playback_PutElement(s32 element) {
    g_playbackCache[g_playbackPos++] = element;
    if (g_playbackPos == 0x1000) {
        File_Write(g_fileIOHandle, 0x4000, g_playbackCache);
        g_playbackPos = 0;
    }
}
