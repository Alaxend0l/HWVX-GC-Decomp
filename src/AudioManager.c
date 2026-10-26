#include "common.h"


void SoundManager_Create(void) {
    M2C_UNK8 Audio_Create();
    void Dprintf(const char *, ...);
    int printf(const char *, ...);
    extern s8 g_songLoaded_803C6F2C;
    extern s32 g_sources_803C6F38;
    extern s32 g_tracks_803C6F30;

    Dprintf("SoundManager_Create()");
    if (!Audio_Create()) {
        printf("AudioManager :: Audio_Create Failed\n");
    }
    Dprintf("SoundManager_Create() 1");
    g_songLoaded_803C6F2C = 0;
    g_sources_803C6F38 = Allocate(0x1F40, 0, "");
    g_tracks_803C6F30 = Allocate(0x500, 0, "");
}

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

void SFX_Heartbeat(void) {
    M2C_UNK SFX_ProcessSource(u32);
    extern s32 g_sscount;
    u32 var_r31;

    var_r31 = 0U;
    g_sscount = 0;
    do {
        SFX_ProcessSource(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x64U);
}

void MusicHearbeat(void) {
    typedef struct MusicFade {
        f32 cur;
        f32 target;
        f32 time;
    } MusicFade;
    typedef struct MusicTrack {
        u32 active;
        f32 vol;
        f32 target;
        f32 time;
        u8 pad10[0x28 - 0x10];
    } MusicTrack;
    extern s8 g_paused_803C6F2D;
    extern MusicFade g_globalMusic_800DBE30[3];
    extern MusicTrack *g_tracks_803C6F30;
    extern f32 g_fDeltaTimeMS;
    extern void (*SetMusicPitch)(u32);
    extern void (*SetMusicTempo)(u32);
    extern void (*SetMusicTrackVol)(u32, u32);
    void SetMusicVol(u32 vol);
    u32 i;

    if (g_paused_803C6F2D) {
        return;
    }
    if (g_globalMusic_800DBE30[0].time > 0.0f && g_globalMusic_800DBE30[0].cur != g_globalMusic_800DBE30[0].target) {
        g_globalMusic_800DBE30[0].cur = (g_globalMusic_800DBE30[0].target - g_globalMusic_800DBE30[0].cur) * (g_fDeltaTimeMS / g_globalMusic_800DBE30[0].time) + g_globalMusic_800DBE30[0].cur;
        g_globalMusic_800DBE30[0].time -= g_fDeltaTimeMS;
        if (g_globalMusic_800DBE30[0].cur < 0.0f) {
            g_globalMusic_800DBE30[0].cur = 0.0f;
        }
        SetMusicVol(32767.0f * g_globalMusic_800DBE30[0].cur);
    }
    if (g_globalMusic_800DBE30[1].time > 0.0f && g_globalMusic_800DBE30[1].cur != g_globalMusic_800DBE30[1].target) {
        g_globalMusic_800DBE30[1].cur = (g_globalMusic_800DBE30[1].target - g_globalMusic_800DBE30[1].cur) * (g_fDeltaTimeMS / g_globalMusic_800DBE30[1].time) + g_globalMusic_800DBE30[1].cur;
        g_globalMusic_800DBE30[1].time -= g_fDeltaTimeMS;
        if (g_globalMusic_800DBE30[1].cur < 0.0f) {
            g_globalMusic_800DBE30[1].cur = 0.0f;
        }
        SetMusicPitch(65536.0f * g_globalMusic_800DBE30[1].cur);
    }
    if (g_globalMusic_800DBE30[2].time > 0.0f && g_globalMusic_800DBE30[2].cur != g_globalMusic_800DBE30[2].target) {
        g_globalMusic_800DBE30[2].cur = (g_globalMusic_800DBE30[2].target - g_globalMusic_800DBE30[2].cur) * (g_fDeltaTimeMS / g_globalMusic_800DBE30[2].time) + g_globalMusic_800DBE30[2].cur;
        g_globalMusic_800DBE30[2].time -= g_fDeltaTimeMS;
        if (g_globalMusic_800DBE30[2].cur < 0.0f) {
            g_globalMusic_800DBE30[2].cur = 0.0f;
        }
        SetMusicTempo(65536.0f * g_globalMusic_800DBE30[2].cur);
    }
    for (i = 0; i < 32; i++) {
        if (g_tracks_803C6F30[i].active && g_tracks_803C6F30[i].time > 0.0f && g_tracks_803C6F30[i].vol != g_tracks_803C6F30[i].target) {
            g_tracks_803C6F30[i].vol = (g_fDeltaTimeMS / g_tracks_803C6F30[i].time) * (g_tracks_803C6F30[i].target - g_tracks_803C6F30[i].vol) + g_tracks_803C6F30[i].vol;
            g_tracks_803C6F30[i].time -= g_fDeltaTimeMS;
            if (g_tracks_803C6F30[i].vol < 0.0f) {
                g_tracks_803C6F30[i].vol = 0.0f;
            }
            SetMusicTrackVol(i, 32767.0f * g_tracks_803C6F30[i].vol);
        }
    }
}

void SoundListener_Set(s32 listener, s32 param, void *value) {
    typedef struct SoundListener {
        Vector4 pos;
        Vector4 vel;
        u8 unk20[0x30];
        u8 mtx[0x40];
    } SoundListener;
    extern SoundListener g_listener[4];
    void *memcpy(void *dst, const void *src, u32 n);

    switch (param) {
    case 5:
        g_listener[listener].pos = *(Vector4 *)value;
        break;
    case 6:
        g_listener[listener].vel = *(Vector4 *)value;
        break;
    case 7:
        memcpy(g_listener[listener].mtx, value, 0x40);
        break;
    }
}

void SoundListener_SetTotalListeners(u32 arg0) {
    extern s32 g_numListeners;
    if (arg0 != 0U) {
        g_numListeners = arg0 - 1;
    }
}

void *SoundAudioSample_Play(void *sample) {
    void *voice;

    if (sample == NULL) {
        return NULL;
    }
    voice = (void *)GetVoice(sample, (u8)(rand() % 10 + 5));
    if (voice != NULL) {
        SetVoiceState(voice, 1);
        return voice;
    }
    return NULL;
}

void SoundAudioSample_Set(void *voice, s32 type, s32 value) {
    switch (type) {
    case 0:
        SetVoiceVol(voice, (s16)value);
        break;
    case 1:
        SetVoicePitch(voice, value);
        break;
    case 4:
        SetVoicePos(voice, (s16)value, -0x7FFF);
        break;
    case 9:
        break;
    }
}

void SongManager_StopBKSong(void) {

}

void SongManager_LoadSong(void) {
    M2C_UNK CreateMusic();
    extern s8 g_songLoaded_803C6F2C;
    CreateMusic();
    g_songLoaded_803C6F2C = 1;
}

void SongManager_PlaySong(void) {
    extern s8 g_songLoaded_803C6F2C;
    extern void (*SetMusicState)(s32);
    extern f32 g_globalMusic_800DBE30[];
    void SetMusicVol(u32 vol);

    if (g_songLoaded_803C6F2C) {
        SetMusicState(1);
        SetMusicVol(32767.0 * g_globalMusic_800DBE30[0]);
    }
}

s32 ActionMusicSetSong(void) {
    return 1;
}
