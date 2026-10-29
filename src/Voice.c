

#include "common.h"

typedef struct SFX {
    void *data;
    u8 unk4[0x1C];
} SFX;

typedef struct Voice {
    u32 id;
    u8 unk4[0x11];
    u8 state;
    u8 unk16[0x1E];
    s32 pitch;
    s32 vol;
    u8 unk3C[4];
    s32 posX;
    s32 posY;
    u8 unk48[0x28];
} Voice;

extern Voice g_voice[0x20];
extern SFX g_sound[0xFE];
extern u8 g_soundLst[0xFE];
extern u32 g_freeSound;
extern s32 g_sfxvol;
extern s8 g_speakers;
extern void (*VoiceUpdate)(void);

void *memcpy(void *dst, const void *src, u32 n);
void *memset(void *dst, int c, u32 n);
void ProcessVoice_(void);
u32 CreateSFX_(SFX *sfx);
void AudioVoice_Create_(void *voices);
void AudioVoice_Destroy_(void);

void AudioVoice_Create(void) {
    memset(g_voice, 0, sizeof(g_voice));
    g_sfxvol = 0x7FFF;
    g_speakers = 1;
    AudioVoice_Create_(g_voice);
}

void AudioVoice_Destroy(void) {
    VoiceUpdate = 0;
    AudioVoice_Destroy_();
}

void SetSpeakerMode(s8 mode) {
    g_speakers = mode;
}

void ProcessVoice(void)
{
    ProcessVoice_();
}

#pragma optimization_level 1
u32 CreateSFX(void *desc) {
    u32 idx;
    SFX *sfx;

    if (g_freeSound == 0xFFFFFFFF) {
        return 0;
    }
    sfx = &g_sound[g_freeSound];
    memcpy(sfx, desc, sizeof(SFX));
    if (CreateSFX_(sfx) == 0) {
        return 0;
    }
    idx = g_freeSound;
    g_freeSound = g_soundLst[g_freeSound];
    g_soundLst[idx++] = 0xFF;
    return idx;
}

void SetVoiceState_(Voice *voice, s32 state);
void SetVoiceVol_(Voice *voice, s32 vol);
s8 SetVoiceRate_(Voice *voice);
void ReleaseVoice(u32 id) {
    if (id == g_voice[(u8)id].id) {
        SetVoiceState_(&g_voice[(u8)id], 3);
    }
}

s32 SetVoiceVol(u32 id, s32 vol) {
    Voice *voice = &g_voice[(u8)id];

    if (voice->id == id) {
        voice->vol = vol;
        if (voice->state == 1) {
            SetVoiceVol_(voice, g_sfxvol);
        }
        return 1;
    }
    return 0;
}

