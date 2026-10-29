#include "common.h"

void Mpeg_Init(void);


void Mpeg_Create(void)
{
    Mpeg_Init();
}

void Mpeg_Init(void) {
    void GXSetDispCopyGamma(s32 gamma);
    void GXSetCopyFilter(u8 aa, void *pattern, u8 vf, void *vfilter);
    void GXSetCopyClear(GXColor color, u32 z);
    void GXSetCullMode(s32 mode);
    s32 THPPlayerInit(void);
    void AudioInit(void);
    extern s32 m_rendermode_803C7754;
    extern s32 rmode;
    GXColor black;

    black.r = 0;
    black.g = 0;
    black.b = 0;
    black.a = 0xFF;
    GXSetDispCopyGamma(0);
    GXSetCopyFilter(0, 0, 0, 0);
    GXSetCopyClear(black, 0xFFFFFF);
    GXSetCullMode(0);
    THPPlayerInit();
    AudioInit();
    m_rendermode_803C7754 = rmode;
}

u32 Mpeg_Heartbeat(void) {
    typedef struct THPVideoInfo {
        u32 xSize;
        u32 ySize;
    } THPVideoInfo;
    void GXInvalidateVtxCache(void);
    void THPPlayerDrawCurrentFrame(s32 rmode, s32 x, s32 y, u32 polyW, u32 polyH);
    void THPPlayerDrawDone(void);
    s32 THPPlayerGetState(void);
    void THPPlayerStop(void);
    void THPPlayerClose(void);
    extern u8 badMojo_803C7734;
    extern s32 m_movie_x_803C7740;
    extern s32 m_movie_y_803C773C;
    extern s32 m_rendermode_803C7754;
    extern THPVideoInfo m_videoInfo_803C7744;

    if (badMojo_803C7734) {
        return 0;
    }
    GXInvalidateVtxCache();
    THPPlayerDrawCurrentFrame(m_rendermode_803C7754, m_movie_x_803C7740, m_movie_y_803C773C, m_videoInfo_803C7744.xSize, m_videoInfo_803C7744.ySize);
    THPPlayerDrawDone();
    if (THPPlayerGetState() == 5) {
        THPPlayerStop();
        THPPlayerClose();
        badMojo_803C7734 = 1;
        return 0;
    }
    return THPPlayerGetState() == 3;
}

void Mpeg_Render(void)
{
    Render_FlipBuffers();
}

void Mpeg_Destroy(void) {

}

void AudioInit(void) {
    void AudioCallback(void);
    void *AIRegisterDMACallback(void *callback);
    void AIStartDMA(void);
    extern s32 SoundBufferNumber_803C7730;
    extern u8 SoundBuffer_8036E360[][0x3C0];
    extern void *audio_restore_cb;

    audio_restore_cb = AIRegisterDMACallback(AudioCallback);
    memset(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0, 0x3C0);
    DCFlushRange(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    AIInitDMA(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    AIStartDMA();
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
