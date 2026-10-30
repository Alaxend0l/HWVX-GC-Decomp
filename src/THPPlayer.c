#include "common.h"

typedef struct THPAudioBuffer {
    s16 *buffer;
    s16 *curPtr;
    u32 validSample;
    s32 frameNumber;
} THPAudioBuffer;

typedef struct THPVideoInfo {
    u32 xSize;
    u32 ySize;
} THPVideoInfo;

typedef struct THPTextureSet {
    u8 *ytexture;
    u8 *utexture;
    u8 *vtexture;
    s32 frameNumber;
} THPTextureSet;

typedef struct THPReadBuffer {
    u8 *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct THPPlayer {
    u8 fileInfo[0x3C];
    u8 tag[4];
    u32 version;
    u32 unk44;
    u32 unk48;
    f32 frameRate;
    u32 numFrames;      /* 0x50: compared with the frame Prepare wants */
    u32 unk54;
    u32 unk58;
    u32 dataOffset;
    u32 frameOffsets;   /* 0x60: start of the per-frame offsets, 4 bytes each */
    u32 unk64;
    u32 unk68;
    u32 numComponents;
    u8 components[0x80 - 0x70];
    THPVideoInfo videoInfo;
    u8 audioInfo[0xC];
    u8 *bufferEnd;
    BOOL open;
    u8 state;
    u8 unk9D;
    u8 playFlag;
    u8 audioExist;
    s32 dvdError;
    s32 videoError;
    s32 onMemory;
    u8 *movieData;
    s32 initOffset;
    s32 initReadSize;
    s32 initReadFrame;
    u8 padBC[4];
    u32 unkC0;
    u32 unkC4;
    s32 unkC8; /* prevCount */
    s32 unkCC; /* curCount */
    s32 videoDecodeCount;
    s32 curVideoNumber;
    s32 audioFrameNumber;
    THPTextureSet *dispTextureSet;
    THPAudioBuffer *audioBuffer;
    THPReadBuffer readBuffer[10];
    THPTextureSet textureSet[3];
    THPAudioBuffer audioBuffers[3];
    u8 pad194[0x198 - 0x194];
} THPPlayer;

THPPlayer ActivePlayer;
static u8 UsedTextureSetQueue_8036EC78[0x20];
static void *UsedTextureSetMessage[3];
static s32 WorkBuffer[16] __attribute__((aligned(32)));
static u8 PrepareReadyQueue[0x20];
static void *PrepareReadyMessage;
static s32 Initialized;
static s32 m_cd_drive_status;
static s32 m_reset;
extern s64 g_resetTicks;

s32 DVDOpen(char *fileName, void *fileInfo);
s32 DVDClose(void *fileInfo);
s32 DVDReadPrio(void *fileInfo, void *addr, s32 length, s32 offset, s32 prio);
void DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void *callback, s32 prio);
s32 DVDGetDriveStatus(void);
void Render_BeginScene(void);
void Render_EndScene(void);
void Render_FlipBuffers(void);
s64 OSGetTime(void);
s32 OSGetResetButtonState(void);
void VISetBlack(s32 black);
void VIWaitForRetrace(void);
void VIFlush(void);
void OSResetSystem(s32 reset, u32 resetCode, s32 forceMenu);
void *memset(void *dst, s32 c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);
s32 strcmp(const char *a, const char *b);
extern u16 VolumeTable[];
void *PopDecodedAudioBuffer(s32 arg0);
void PushFreeAudioBuffer(void *arg0);

#pragma push
#pragma use_lmw_stmw on

s32 THPPlayerInit(void)
{
    void LCEnable(void);
    void OSInitMessageQueue(void *, void *, s32);
    s32 THPInit(void);

    memset(&ActivePlayer, 0, sizeof(ActivePlayer));
    LCEnable();
    OSInitMessageQueue(UsedTextureSetQueue_8036EC78, UsedTextureSetMessage, 3);
    if (THPInit())
    {
        Initialized = 1;
        return TRUE;
    }
    return FALSE;
}

void THPPlayerQuit(void) {
    M2C_UNK LCDisable();
    LCDisable();
    Initialized = 0;
}

BOOL THPPlayerOpen(char *fileName, BOOL onMemory) {
    s32 offset;
    u32 i;

    if (!Initialized) {
        return FALSE;
    }
    if (ActivePlayer.open) {
        return FALSE;
    }
    memset(&ActivePlayer.videoInfo, 0, 8);
    memset(&ActivePlayer.audioInfo, 0, 0xC);
    if (!DVDOpen(fileName, &ActivePlayer)) {
        return FALSE;
    }
    DVDReadAsyncPrio(&ActivePlayer, WorkBuffer, 0x40, 0, NULL, 2);
    do {
        m_cd_drive_status = DVDGetDriveStatus();
        switch (m_cd_drive_status) {
        case -1:
        case 4:
        case 5:
        case 6:
        case 11:
            Render_BeginScene();
            Render_EndScene();
            Render_FlipBuffers();
            break;
        }
        if (OSGetResetButtonState()) {
            m_reset = 1;
        }
        if (m_reset && !OSGetResetButtonState()) {
            VISetBlack(1);
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VISetBlack(1);
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            if ((OSGetTime() - g_resetTicks) / (*(u32 *)0x800000F8 / 4) < 8) {
                OSResetSystem(1, 0, 0);
            } else {
                OSResetSystem(0, 0, 0);
            }
        }
    } while (m_cd_drive_status != 0);
    memcpy(ActivePlayer.tag, WorkBuffer, 0x30);
    if (strcmp((char *)ActivePlayer.tag, "THP") != 0) {
        DVDClose(&ActivePlayer);
        return FALSE;
    }
    if (ActivePlayer.version != 0x10000) {
        DVDClose(&ActivePlayer);
        return FALSE;
    }
    offset = ActivePlayer.dataOffset;
    if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
        DVDClose(&ActivePlayer);
        return FALSE;
    }
    memcpy(&ActivePlayer.numComponents, WorkBuffer, 0x14);
    offset += 0x14;
    ActivePlayer.audioExist = 0;
    for (i = 0; i < ActivePlayer.numComponents; i++) {
        switch (ActivePlayer.components[i]) {
        case 0:
            if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
                DVDClose(&ActivePlayer);
                return FALSE;
            }
            memcpy(&ActivePlayer.videoInfo, WorkBuffer, 8);
            offset += 8;
            break;
        case 1:
            if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
                DVDClose(&ActivePlayer);
                return FALSE;
            }
            memcpy(&ActivePlayer.audioInfo, WorkBuffer, 0xC);
            ActivePlayer.audioExist = 1;
            offset += 0xC;
            break;
        default:
            return FALSE;
        }
    }
    ActivePlayer.unk9D = 0;
    ActivePlayer.state = 0;
    ActivePlayer.playFlag = 0;
    ActivePlayer.onMemory = onMemory;
    ActivePlayer.open = TRUE;
    return TRUE;
}

BOOL THPPlayerClose(void) {
    s32 DVDClose(void *fileInfo);
    if (ActivePlayer.open && ActivePlayer.state == 0) {
        ActivePlayer.open = FALSE;
        DVDClose(ActivePlayer.fileInfo);
        return TRUE;
    }
    return FALSE;
}

s32 THPPlayerCalcNeedMemory(void)
{
    u32 pixels;
    u32 need;

    if (ActivePlayer.open)
    {
        if (ActivePlayer.onMemory)
        {
            need = (ActivePlayer.unk58 + 0x1F) & ~0x1F;
        }
        else
        {
            need = ((ActivePlayer.unk44 + 0x1F) & ~0x1F) * 10;
        }
        pixels = ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize;
        need += ((pixels + 0x1F) & ~0x1F) * 3;
        need += (((pixels >> 2) + 0x1F) & ~0x1F) * 3;
        need += (((pixels >> 2) + 0x1F) & ~0x1F) * 3;
        if (ActivePlayer.audioExist)
        {
            need += (((ActivePlayer.unk48 << 2) + 0x1F) & ~0x1F) * 3;
        }
        return need + 0x1000;
    }
    return 0;
}

void DCInvalidateRange(void *addr, u32 nBytes);
BOOL THPPlayerSetBuffer(u8 *buffer) {
    u32 i;
    u8 *ptr;
    u32 ysize;
    u32 uvsize;

    if (ActivePlayer.open && ActivePlayer.state == 0) {
        ptr = buffer;
        if (ActivePlayer.onMemory) {
            ActivePlayer.movieData = ptr;
            ptr += ActivePlayer.unk58;
        } else {
            for (i = 0; i < 10; i++) {
                ActivePlayer.readBuffer[i].ptr = ptr;
                ptr += (ActivePlayer.unk44 + 0x1F) & ~0x1F;
            }
        }

        ysize = (ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize + 0x1F) & ~0x1F;
        uvsize = (ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4 + 0x1F) & ~0x1F;
        for (i = 0; i < 3; i++) {
            ActivePlayer.textureSet[i].ytexture = ptr;
            DCInvalidateRange(ptr, ysize);
            ptr += ysize;
            ActivePlayer.textureSet[i].utexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
            ActivePlayer.textureSet[i].vtexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
        }

        if (ActivePlayer.audioExist) {
            for (i = 0; i < 3; i++) {
                ActivePlayer.audioBuffers[i].buffer = (s16 *)ptr;
                ActivePlayer.audioBuffers[i].curPtr = (s16 *)ptr;
                ActivePlayer.audioBuffers[i].validSample = 0;
                ptr += (ActivePlayer.unk48 * 4 + 0x1F) & ~0x1F;
            }
        }

        ActivePlayer.bufferEnd = ptr;
        return TRUE;
    }
    return FALSE;
}

void PrepareReady(void *arg0) {
    OSSendMessage(PrepareReadyQueue, arg0, 1);
}

BOOL THPPlayerPrepare(s32 frameNum, u8 playFlag) {
    void PlayControl(u32 retraceCount);
    void CreateVideoDecodeThread(s32 priority, u8 *ptr);
    void CreateAudioDecodeThread(s32 priority, u8 *ptr);
    void CreateReadThread(s32 priority);
    void PushFreeReadBuffer(void *buffer);
    void PushFreeTextureSet(void *buffer);
    void VideoDecodeThreadStart(void);
    void AudioDecodeThreadStart(void);
    void ReadThreadStart(void);
    void OSInitMessageQueue(void *, void *, s32);
    s32 OSReceiveMessage(void *messageQueue, void **message, s32 block);
    void *VISetPostRetraceCallback(void *callback);
    extern void *OldVIPostCallback;
    s32 offset;
    u8 *ptr;
    s32 i;
    void *msg;

    if (ActivePlayer.open && ActivePlayer.state == 0) {
        if (frameNum > 0) {
            if (!ActivePlayer.frameOffsets) {
                return FALSE;
            }
            if (ActivePlayer.numFrames > frameNum) {
                offset = ActivePlayer.frameOffsets + (frameNum - 1) * 4;
                if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
                    return FALSE;
                }
                ActivePlayer.initOffset = ActivePlayer.unk64 + ((u32 *)WorkBuffer)[0];
                ActivePlayer.initReadSize = ((u32 *)WorkBuffer)[1] - ((u32 *)WorkBuffer)[0];
                ActivePlayer.initReadFrame = frameNum;
            } else {
                return FALSE;
            }
        } else {
            ActivePlayer.initOffset = ActivePlayer.unk64;
            ActivePlayer.initReadSize = ActivePlayer.unk54;
            ActivePlayer.initReadFrame = frameNum;
        }

        ActivePlayer.playFlag = playFlag;
        ActivePlayer.videoDecodeCount = 0;

        if (ActivePlayer.onMemory) {
            if (DVDReadPrio(&ActivePlayer, ActivePlayer.movieData, ActivePlayer.unk58, ActivePlayer.unk64, 2) < 0) {
                return FALSE;
            }
            ptr = ActivePlayer.movieData + ActivePlayer.initOffset - ActivePlayer.unk64;
            CreateVideoDecodeThread(20, ptr);
            if (ActivePlayer.audioExist) {
                CreateAudioDecodeThread(12, ptr);
            }
        } else {
            CreateVideoDecodeThread(20, NULL);
            if (ActivePlayer.audioExist) {
                CreateAudioDecodeThread(12, NULL);
            }
            CreateReadThread(8);
        }

        if (!ActivePlayer.onMemory) {
            for (i = 0; i < 10; i++) {
                PushFreeReadBuffer(&ActivePlayer.readBuffer[i]);
            }
        }
        for (i = 0; i < 3; i++) {
            PushFreeTextureSet(&ActivePlayer.textureSet[i]);
        }
        if (ActivePlayer.audioExist) {
            for (i = 0; i < 3; i++) {
                PushFreeAudioBuffer(&ActivePlayer.audioBuffers[i]);
            }
        }

        OSInitMessageQueue(PrepareReadyQueue, &PrepareReadyMessage, 1);
        VideoDecodeThreadStart();
        if (ActivePlayer.audioExist) {
            AudioDecodeThreadStart();
        }
        if (!ActivePlayer.onMemory) {
            ReadThreadStart();
        }
        OSReceiveMessage(PrepareReadyQueue, &msg, 1);
        if ((BOOL)msg == FALSE) {
            return FALSE;
        }
        ActivePlayer.state = 1;
        ActivePlayer.unk9D = 0;
        ActivePlayer.dispTextureSet = NULL;
        ActivePlayer.audioBuffer = NULL;
        ActivePlayer.curVideoNumber = 0;
        ActivePlayer.audioFrameNumber = 0;
        OldVIPostCallback = VISetPostRetraceCallback(PlayControl);
        return TRUE;
    }
    return FALSE;
}

s32 THPPlayerPlay(void)
{
    if (ActivePlayer.open && (ActivePlayer.state == 1 || ActivePlayer.state == 4))
    {
        ActivePlayer.state = 2;
        ActivePlayer.unkC8 = 0;
        ActivePlayer.unkCC = 0;
        ActivePlayer.unkC4 = -1;
        ActivePlayer.unkC0 = -1;
        return 1;
    }
    return 0;
}

void THPPlayerStop(void) {
    M2C_UNK AudioDecodeThreadCancel();
    M2C_UNK DVDCancel();
    u32 PopUsedTextureSet();
    M2C_UNK ReadThreadCancel();
    M2C_UNK VISetPostRetraceCallback(s32);
    M2C_UNK VideoDecodeThreadCancel();
    extern s32 OldVIPostCallback;
    if (((s32) M2C_FIELD(&ActivePlayer, s32 *, 0x98) != 0) && ((u8) M2C_FIELD(&ActivePlayer, u8 *, 0x9C) != 0)) {
        M2C_FIELD(&ActivePlayer, s8 *, 0x9D) = 0;
        M2C_FIELD(&ActivePlayer, u8 *, 0x9C) = 0U;
        VISetPostRetraceCallback(OldVIPostCallback);
        if ((s32) M2C_FIELD(&ActivePlayer, s32 *, 0xA8) == 0) {
            DVDCancel();
            ReadThreadCancel();
        }
        VideoDecodeThreadCancel();
        if ((u8) M2C_FIELD(&ActivePlayer, u8 *, 0x9F) != 0) {
            AudioDecodeThreadCancel();
        }
        do {

        } while (PopUsedTextureSet() != 0U);
        M2C_FIELD(&ActivePlayer, s32 *, 0xA0) = 0;
        M2C_FIELD(&ActivePlayer, s32 *, 0xA4) = 0;
    }
}

void PlayControl(u32 retraceCount) {
    s32 ProperTimingForStart(void);
    s32 ProperTimingForGettingNextFrame(void);
    void PushUsedTextureSet(void *buffer);
    void *PopDecodedTextureSet(s32 flag);
    extern void (*OldVIPostCallback)(u32);
    s32 diff;
    s32 frameNumber;
    THPTextureSet *textureSet;

    if (OldVIPostCallback) {
        OldVIPostCallback(retraceCount);
    }
    textureSet = (THPTextureSet *)-1;
    if (ActivePlayer.open && ActivePlayer.state == 2) {
        if (ActivePlayer.dvdError || ActivePlayer.videoError) {
            ActivePlayer.state = ActivePlayer.unk9D = 5;
            return;
        }
        if (*(s64 *)&ActivePlayer.unkC0 == 0 && ActivePlayer.unk9D == 0) {
            ActivePlayer.unk9D = 2;
        }
        (*(s64 *)&ActivePlayer.unkC0)++;
        if (ActivePlayer.unk9D == 0 || ActivePlayer.unk9D == 4) {
            if (ProperTimingForStart()) {
                if (ActivePlayer.audioExist) {
                    diff = ActivePlayer.curVideoNumber - ActivePlayer.audioFrameNumber;
                    if (diff <= 1) {
                        textureSet = PopDecodedTextureSet(0);
                        if (ActivePlayer.videoDecodeCount > diff) {
                            ActivePlayer.videoDecodeCount--;
                        }
                    } else {
                        ActivePlayer.unk9D = 2;
                    }
                } else {
                    textureSet = PopDecodedTextureSet(0);
                    ActivePlayer.unk9D = 2;
                }
            } else {
                *(s64 *)&ActivePlayer.unkC0 = -1;
            }
        } else if (ProperTimingForGettingNextFrame()) {
            if (ActivePlayer.audioExist) {
                diff = ActivePlayer.curVideoNumber - ActivePlayer.audioFrameNumber;
                if (diff <= 1) {
                    textureSet = PopDecodedTextureSet(0);
                    if (ActivePlayer.videoDecodeCount > diff) {
                        ActivePlayer.videoDecodeCount--;
                    }
                }
            } else {
                textureSet = PopDecodedTextureSet(0);
            }
        }
        if (textureSet && textureSet != (THPTextureSet *)-1) {
            ActivePlayer.curVideoNumber = textureSet->frameNumber;
            if (ActivePlayer.dispTextureSet) {
                PushUsedTextureSet(ActivePlayer.dispTextureSet);
            }
            ActivePlayer.dispTextureSet = textureSet;
        }
        if (!(ActivePlayer.playFlag & 1)) {
            if (ActivePlayer.audioExist) {
                frameNumber = (ActivePlayer.audioFrameNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
                if (frameNumber == ActivePlayer.numFrames - 1 && ActivePlayer.audioBuffer == NULL) {
                    frameNumber = (ActivePlayer.curVideoNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
                    if (frameNumber == ActivePlayer.numFrames - 1 && textureSet == NULL) {
                        ActivePlayer.state = ActivePlayer.unk9D = 3;
                    }
                }
            } else {
                frameNumber = (ActivePlayer.curVideoNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
                if (frameNumber == ActivePlayer.numFrames - 1 && textureSet == NULL) {
                    ActivePlayer.state = ActivePlayer.unk9D = 3;
                }
            }
        }
    }
}

s32 ProperTimingForStart(void)
{
    u32 VIGetNextField();

    if (ActivePlayer.playFlag & 2)
    {
        if (VIGetNextField() == 0)
        {
            return TRUE;
        }
        goto returnFalse;
    }
    if (ActivePlayer.playFlag & 4)
    {
        if (VIGetNextField() == 1)
        {
            return TRUE;
        }
        goto returnFalse;
    }
    return TRUE;

returnFalse:
    return FALSE;
}

s32 ProperTimingForGettingNextFrame(void)
{
    u32 VIGetNextField();
    s32 frameRate;

    if (ActivePlayer.playFlag & 2)
    {
        if (VIGetNextField() == 0)
        {
            return TRUE;
        }
    }
    else if (ActivePlayer.playFlag & 4)
    {
        if (VIGetNextField() == 1)
        {
            return TRUE;
        }
    }
    else
    {
        frameRate = ActivePlayer.frameRate * 100.0f;
        ActivePlayer.unkCC = (*(s64 *)&ActivePlayer.unkC0 * frameRate) / 5994;
        if (ActivePlayer.unkC8 != ActivePlayer.unkCC)
        {
            ActivePlayer.unkC8 = ActivePlayer.unkCC;
            return TRUE;
        }
    }
    return FALSE;
}

s32 THPPlayerDrawCurrentFrame(void *rmode, u32 x, u32 y, u32 polygonW, u32 polygonH)
{
    void THPGXYuv2RgbSetup(void *rmode);
    void THPGXYuv2RgbDraw(u8 *y_data, u8 *u_data, u8 *v_data, s16 x, s16 y, s16 textureWidth, s16 textureHeight, s16 polygonWidth, s16 polygonHeight);
    void THPGXRestore(void);
    s32 currFrame;

    if (ActivePlayer.open && ActivePlayer.state != 0 && ActivePlayer.dispTextureSet != NULL)
    {
        THPGXYuv2RgbSetup(rmode);
        THPGXYuv2RgbDraw(ActivePlayer.dispTextureSet->ytexture, ActivePlayer.dispTextureSet->utexture,
                         ActivePlayer.dispTextureSet->vtexture, x, y,
                         ActivePlayer.videoInfo.xSize, ActivePlayer.videoInfo.ySize, polygonW, polygonH);
        THPGXRestore();
        currFrame = (ActivePlayer.dispTextureSet->frameNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
        return currFrame;
    }
    return -1;
}

s32 THPPlayerAudioOutput(s16 *destination, u32 sample, s32 volume)
{
    s32 ret;
    u32 requestSample;
    s16 *dst;
    u16 vol;
    u32 sampleNum;
    s16 *src;
    s32 mix;
    u32 i;
    THPAudioBuffer *buf;

    if (volume < 0)
    {
        volume = 0;
    }
    if (volume > 0x7F)
    {
        volume = 0x7F;
    }
    vol = VolumeTable[volume];
    if (ActivePlayer.open && ActivePlayer.unk9D == 2 && ActivePlayer.audioExist)
    {
        requestSample = sample;
        dst = destination;
        ret = 0;
        while (TRUE)
        {
            if (ActivePlayer.audioBuffer == NULL)
            {
                buf = PopDecodedAudioBuffer(0);
                ActivePlayer.audioBuffer = buf;
                if (buf == NULL)
                {
                    break;
                }
                ActivePlayer.audioFrameNumber = buf->frameNumber;
            }
            sampleNum = ActivePlayer.audioBuffer->validSample;
            if (sampleNum)
            {
                if (sampleNum >= requestSample)
                {
                    sampleNum = requestSample;
                }
                src = ActivePlayer.audioBuffer->curPtr;
                for (i = 0; i < sampleNum * 2; i++)
                {
                    mix = *dst + ((vol * *src) >> 15);
                    if (mix < -32767)
                    {
                        mix = -32767;
                    }
                    if (mix > 32768)
                    {
                        mix = 32768;
                    }
                    *dst = mix;
                    dst++;
                    src++;
                }
                ret += sampleNum;
                requestSample -= sampleNum;
                ActivePlayer.audioBuffer->validSample -= sampleNum;
                ActivePlayer.audioBuffer->curPtr = src;
                if (ActivePlayer.audioBuffer->validSample == 0)
                {
                    PushFreeAudioBuffer(ActivePlayer.audioBuffer);
                    buf = PopDecodedAudioBuffer(0);
                    ActivePlayer.audioBuffer = buf;
                    if (buf != NULL)
                    {
                        ActivePlayer.audioFrameNumber = buf->frameNumber;
                    }
                }
                if (requestSample == 0)
                {
                    break;
                }
            }
        }
        return ret;
    }
    return 0;
}

BOOL THPPlayerGetVideoInfo(void *videoInfo)
{
    if (ActivePlayer.open)
    {
        memcpy(videoInfo, &ActivePlayer.videoInfo, 8);
        return TRUE;
    }
    return FALSE;
}

u8 THPPlayerGetState(void) {
    return M2C_FIELD(&ActivePlayer, u8 *, 0x9C);
}

void PushUsedTextureSet(void *arg0)
{
    OSSendMessage(UsedTextureSetQueue_8036EC78, arg0, 0);
}

void * PopUsedTextureSet(void)
{
    s32 OSReceiveMessage(void *messageQueue, void **message, s32 block);
    void *msg;

    if (OSReceiveMessage(UsedTextureSetQueue_8036EC78, &msg, 0) == 1)
    {
        return msg;
    }
    return NULL;
}

void THPPlayerDrawDone(void)
{
    void GXDrawDone(void);
    s32 OSReceiveMessage(void *messageQueue, void **message, s32 block);
    void PushFreeTextureSet(void *textureSet);
    void *msg;
    void *textureSet;

    GXDrawDone();
    if (Initialized)
    {
        while (TRUE)
        {
            if (OSReceiveMessage(UsedTextureSetQueue_8036EC78, &msg, 0) == 1)
            {
                textureSet = msg;
            }
            else
            {
                textureSet = NULL;
            }
            if (textureSet == NULL)
            {
                break;
            }
            PushFreeTextureSet(textureSet);
        }
    }
}

#pragma pop

