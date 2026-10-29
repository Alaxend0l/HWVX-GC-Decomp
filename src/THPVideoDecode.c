#include "common.h"

#pragma use_lmw_stmw on

typedef struct THPReadBuffer {
    u8 *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct THPPlayer {
    u8 pad0[0x50];
    u32 numFrames;
    u8 pad54[0x6C - 0x54];
    u32 numComponents;
    u8 components[16];
    u8 pad80[0x94 - 0x80];
    void *thpWork;
    u8 pad98[0x9E - 0x98];
    u8 playFlag;
    u8 audioExist;
    u8 padA0[0xA4 - 0xA0];
    s32 videoError;
    u8 padA8[0xB8 - 0xA8];
    s32 initReadFrame;
    u8 padBC[0xD0 - 0xBC];
    s32 videoDecodeCount;
} THPPlayer;

extern THPPlayer ActivePlayer;

typedef struct THPTextureSet {
    u8 *ytexture;
    u8 *utexture;
    u8 *vtexture;
    s32 frameNumber;
} THPTextureSet;

extern s32 First;
extern u8 VideoDecodeThread[];
extern u8 FreeTextureSetQueue[];
extern u8 DecodedTextureSetQueue[];
extern s32 VideoDecodeThreadCreated;

void VideoDecodeThreadStart(void) {
    extern s32 VideoDecodeThreadCreated;
    extern u8 VideoDecodeThread[];

    if (VideoDecodeThreadCreated) {
        OSResumeThread(VideoDecodeThread);
    }
}

void VideoDecodeThreadCancel(void) {
    extern s32 VideoDecodeThreadCreated;
    extern u8 VideoDecodeThread[];

    if (VideoDecodeThreadCreated) {
        OSCancelThread(VideoDecodeThread);
        VideoDecodeThreadCreated = 0;
    }
}

void *VideoDecoder(void *param) {
    void VideoDecode(THPReadBuffer *readBuffer);
    void *PopReadedBuffer(void);
    void *PopReadedBuffer2(void);
    void PushFreeReadBuffer(void *arg0);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    THPReadBuffer *readBuffer;
    s32 old;
    u32 curFrame;

    while (TRUE) {
        if (ActivePlayer.audioExist) {
            while (ActivePlayer.videoDecodeCount < 0) {
                readBuffer = PopReadedBuffer2();
                curFrame = (readBuffer->frameNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
                if (curFrame == ActivePlayer.numFrames - 1 && !(ActivePlayer.playFlag & 1)) {
                    VideoDecode(readBuffer);
                }
                PushFreeReadBuffer(readBuffer);
                old = OSDisableInterrupts();
                ActivePlayer.videoDecodeCount++;
                OSRestoreInterrupts(old);
            }
        }
        if (ActivePlayer.audioExist) {
            readBuffer = PopReadedBuffer2();
        } else {
            readBuffer = PopReadedBuffer();
        }
        VideoDecode(readBuffer);
        PushFreeReadBuffer(readBuffer);
    }
}

void *PopFreeTextureSet(void);
void PushDecodedTextureSet(void *arg0);
void PrepareReady(s32 arg0);
s32 THPVideoDecode(void *file, void *tileY, void *tileU, void *tileV, void *work);
void VideoDecode(THPReadBuffer *readBuffer) {
    THPTextureSet *textureSet;
    s32 old;
    u32 i;
    u32 *compSizePtr;
    u8 *ptr;

    ptr = readBuffer->ptr + ActivePlayer.numComponents * 4 + 8;
    compSizePtr = (u32 *)(readBuffer->ptr + 8);
    textureSet = PopFreeTextureSet();
    for (i = 0; i < ActivePlayer.numComponents; i++) {
        switch (ActivePlayer.components[i]) {
        case 0:
            ActivePlayer.videoError = THPVideoDecode(ptr, textureSet->ytexture, textureSet->utexture, textureSet->vtexture, ActivePlayer.thpWork);
            if (ActivePlayer.videoError) {
                if (First) {
                    PrepareReady(0);
                    First = 0;
                }
                OSSuspendThread(VideoDecodeThread);
            }
            textureSet->frameNumber = readBuffer->frameNumber;
            PushDecodedTextureSet(textureSet);
            old = OSDisableInterrupts();
            ActivePlayer.videoDecodeCount++;
            OSRestoreInterrupts(old);
            break;
        }
        ptr += *compSizePtr;
        compSizePtr++;
    }
    if (First) {
        PrepareReady(1);
        First = 0;
    }
}

void *PopFreeTextureSet(void) {
    extern u8 FreeTextureSetQueue[];
    void *msg;

    OSReceiveMessage(FreeTextureSetQueue, &msg, 1);
    return msg;
}

void PushFreeTextureSet(void *arg0)
{
    extern u8 UsedTextureSetQueue_8036EC78[];
    OSSendMessage(UsedTextureSetQueue_8036EC78, arg0, 0);
}

void *PopDecodedTextureSet(s32 flags) {
    extern u8 DecodedTextureSetQueue[];
    void *msg;

    if (OSReceiveMessage(DecodedTextureSetQueue, &msg, flags) == 1) {
        return msg;
    }
    return NULL;
}

void PushDecodedTextureSet(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
