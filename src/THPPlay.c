#include "common.h"


void Mpeg_Destroy(void) {

}

void AudioCallback(void) {
    s32 OSEnableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    void THPPlayerAudioOutput(void *buf, s32 samples, s32 vol);
    extern s32 SoundBufferNumber_803C7730;
    extern u8 SoundBuffer_8036E360[][0x3C0];
    extern s32 m_movieVolume_803C7738;
    s32 old;

    old = OSEnableInterrupts();
    SoundBufferNumber_803C7730 ^= 1;
    memset(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0, 0x3C0);
    THPPlayerAudioOutput(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0xF0, m_movieVolume_803C7738);
    DCFlushRange(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    AIInitDMA(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    OSRestoreInterrupts(old);
}
