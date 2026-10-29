#include "common.h"

typedef struct THPReadBuffer {
    u8 *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct THPPlayer {
    u8 pad0[0x50];
    u32 numFrames;
    u8 pad54[0x9E - 0x54];
    u8 playFlag;
    u8 audioExist;
    u8 padA0[0xB8 - 0xA0];
    s32 initReadFrame;
    u8 padBC[0xD0 - 0xBC];
    s32 videoDecodeCount;
} THPPlayer;

extern THPPlayer ActivePlayer;

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
