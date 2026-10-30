#include "common.h"

typedef struct THPVideoInfo {
    u32 xSize;
    u32 ySize;
} THPVideoInfo;

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
    extern s32 m_rendermode;
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
    m_rendermode = rmode;
}

s32 Mpeg_StartMovie(char *name, f32 volume) {
    typedef struct {
        u32 viTVmode;
        u16 fbWidth;
        u16 efbHeight;
    } RenderMode;
    typedef struct THPVideoInfo {
        u32 xSize;
        u32 ySize;
    } THPVideoInfo;
    BOOL THPPlayerOpen(char *fileName, BOOL onMemory);
    BOOL THPPlayerSetBuffer(void *buffer);
    BOOL THPPlayerPrepare(s32 frameNum, s32 playFlag);
    BOOL THPPlayerGetVideoInfo(THPVideoInfo *info);
    u32 THPPlayerCalcNeedMemory(void);
    BOOL THPPlayerPlay(void);
    void *OSAllocFromHeap(s32 heap, u32 size);
    void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
    void OSPanic(const char *file, int line, const char *msg, ...);
    extern u8 badMojo;
    extern s32 m_rendermode;
    extern THPVideoInfo m_videoInfo;
    extern s32 m_movie_x;
    extern s32 m_movie_y;
    extern void *m_movie_buffer;
    extern s32 m_movieVolume;
    extern s32 __OSCurrHeap;
    char *p = name;
    u32 size;

    if (badMojo) {
        return 0;
    }
    for (; *p; p++) {
        *p = tolower(*p);
        if (*p == '\\') {
            *p = '/';
        }
    }
    if (!THPPlayerOpen(name, FALSE)) {
        OSPanic("THPPlay.c", 93, "Fail to open the thp file\n");
    }
    GXSetViewport(0.0f, 0.0f, ((RenderMode *)m_rendermode)->fbWidth, ((RenderMode *)m_rendermode)->efbHeight, 0.0f, 1.0f);
    THPPlayerGetVideoInfo(&m_videoInfo);
    m_movie_x = (((RenderMode *)m_rendermode)->fbWidth - m_videoInfo.xSize) / 2;
    m_movie_y = (((RenderMode *)m_rendermode)->efbHeight - m_videoInfo.ySize) / 2;
    size = THPPlayerCalcNeedMemory();
    m_movie_buffer = OSAllocFromHeap(__OSCurrHeap, size);
    if (m_movie_buffer == NULL) {
        OSPanic("THPPlay.c", 113, "Can't allocate the memory\n");
    }
    THPPlayerSetBuffer(m_movie_buffer);
    if (!THPPlayerPrepare(0, 0)) {
        OSPanic("THPPlay.c", 122, "Fail to prepare\n");
    }
    THPPlayerPlay();
    m_movieVolume = (u32)(46.0f * volume);
}

void Mpeg_StopMovie(void) {
    void *AIRegisterDMACallback(void *callback);
    void GXSetDispCopyGamma(s32 gamma);
    void GXSetCopyFilter(u8 aa, void *pattern, u8 vf, void *vfilter);
    void OSFreeToHeap(s32 heap, void *ptr);
    void THPPlayerStop(void);
    void THPPlayerClose(void);
    void THPPlayerQuit(void);
    extern u8 badMojo;
    extern s32 m_rendermode;
    extern void *audio_restore_cb;
    extern void *m_movie_buffer;
    extern s32 __OSCurrHeap;
    u8 *rmode;

    if (badMojo == 0) {
        THPPlayerStop();
        THPPlayerClose();
        GXSetNumChans(1);
        GXSetNumTexGens(1);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevOp(0, 0);
        GXSetDispCopyGamma(1);
        rmode = (u8 *)m_rendermode;
        GXSetCopyFilter(rmode[0x19], rmode + 0x1A, 1, rmode + 0x32);
        AIRegisterDMACallback(audio_restore_cb);
        THPPlayerQuit();
        VIWaitForRetrace();
        Render_FlipBuffers();
        GXSetPixelFmt(1, 0);
        OSFreeToHeap(__OSCurrHeap, m_movie_buffer);
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
        Gfx_BeginScene();
        Gfx_EndScene();
        Render_FlipBuffers();
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
    }
}

u32 Mpeg_Heartbeat(void) {
    void GXInvalidateVtxCache(void);
    void THPPlayerDrawCurrentFrame(s32 rmode, s32 x, s32 y, u32 polyW, u32 polyH);
    void THPPlayerDrawDone(void);
    s32 THPPlayerGetState(void);
    void THPPlayerStop(void);
    void THPPlayerClose(void);
    extern u8 badMojo;
    extern s32 m_movie_x;
    extern s32 m_movie_y;
    extern s32 m_rendermode;
    extern THPVideoInfo m_videoInfo;

    if (badMojo) {
        return 0;
    }
    GXInvalidateVtxCache();
    THPPlayerDrawCurrentFrame(m_rendermode, m_movie_x, m_movie_y, m_videoInfo.xSize, m_videoInfo.ySize);
    THPPlayerDrawDone();
    if (THPPlayerGetState() == 5) {
        THPPlayerStop();
        THPPlayerClose();
        badMojo = 1;
        return 0;
    }
    return THPPlayerGetState() != 3;
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
    extern s32 SoundBufferNumber;
    extern u8 SoundBuffer[][0x3C0];
    extern void *audio_restore_cb;

    audio_restore_cb = AIRegisterDMACallback(AudioCallback);
    memset(SoundBuffer[SoundBufferNumber], 0, 0x3C0);
    DCFlushRange(SoundBuffer[SoundBufferNumber], 0x3C0);
    AIInitDMA(SoundBuffer[SoundBufferNumber], 0x3C0);
    AIStartDMA();
}

void AudioCallback(void) {
    s32 OSEnableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    void THPPlayerAudioOutput(void *buf, s32 samples, s32 vol);
    extern s32 SoundBufferNumber;
    extern u8 SoundBuffer[][0x3C0];
    extern s32 m_movieVolume;
    s32 old;

    old = OSEnableInterrupts();
    SoundBufferNumber ^= 1;
    memset(SoundBuffer[SoundBufferNumber], 0, 0x3C0);
    THPPlayerAudioOutput(SoundBuffer[SoundBufferNumber], 0xF0, m_movieVolume);
    DCFlushRange(SoundBuffer[SoundBufferNumber], 0x3C0);
    AIInitDMA(SoundBuffer[SoundBufferNumber], 0x3C0);
    OSRestoreInterrupts(old);
}
