

#include "common.h"

typedef struct SFX {
    void *data;
    u8 unk4[0x1C];
} SFX;

extern u8 g_voice[0x20][0x70];
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
