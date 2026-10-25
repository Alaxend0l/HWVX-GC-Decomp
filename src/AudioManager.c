#include "common.h"


void SoundManager_Destroy(void) {
    M2C_UNK Audio_Destroy();
    extern s32 g_sources_803C6F38;
    extern s32 g_tracks_803C6F30;
    Audio_Destroy();
    Deallocate(g_sources_803C6F38);
    Deallocate(g_tracks_803C6F30);
}

void SoundManager_Heartbeat(void) {
    M2C_UNK MusicHearbeat();
    s32 OSDisableInterrupts();
    M2C_UNK OSRestoreInterrupts(s32);
    M2C_UNK SFX_Heartbeat();
    s32 temp_r31;

    temp_r31 = OSDisableInterrupts();
    SFX_Heartbeat();
    MusicHearbeat();
    OSRestoreInterrupts(temp_r31);
}

