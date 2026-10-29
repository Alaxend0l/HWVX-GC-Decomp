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
    u8 pad94[4];
    BOOL open;
    u8 state;
    u8 unk9D;
    u8 playFlag;
    u8 audioExist;
    u8 padA0[5];
    s32 onMemory;
    u8 padAC[0xC];
    s32 initReadFrame;
    u8 padBC[4];
    u32 unkC0;
    u32 unkC4;
    s32 unkC8; /* prevCount */
    s32 unkCC; /* curCount */
    u32 unkD0;
    u8 padD4[4];
    s32 audioFrameNumber;
    THPTextureSet *dispTextureSet;
    THPAudioBuffer *audioBuffer;
} THPPlayer;

extern THPPlayer ActivePlayer;
extern u8 WorkBuffer[0x40];
extern u8 UsedTextureSetQueue_8036EC78[];
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
    void OSInitMessageQueue(u8 *, u8 *, s32);
    s32 THPInit(void);
    u8 *player;

    player = (u8 *)&ActivePlayer;
    memset(player, 0, 0x198);
    LCEnable();
    OSInitMessageQueue(player + 0x198, player + 0x1B8, 3);
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

void PrepareReady(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
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
    extern u8 UsedTextureSetQueue_8036EC78[];
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

