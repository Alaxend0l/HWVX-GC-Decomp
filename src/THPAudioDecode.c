#include "common.h"

#pragma use_lmw_stmw on

typedef struct THPAudioBuffer {
    s16 *buffer;
    s16 *curPtr;
    u32 validSample;
    s32 frameNumber;
} THPAudioBuffer;

typedef struct THPReadBuffer {
    u8 *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct THPPlayer {
    u8 pad0[0x50];
    u32 numFrames;
    u8 pad54[0x6C - 0x54];
    u32 numComponents;
    u8 components[0x10];
    u8 pad80[0x9E - 0x80];
    u8 playFlag;
    u8 pad9F[0xAC - 0x9F];
    u8 *movieData;
    u8 padB0[4];
    s32 initReadSize;
    s32 initReadFrame;
} THPPlayer;

extern THPPlayer ActivePlayer;
extern u8 AudioDecodeThread[];
extern s32 AudioDecodeThreadCreated;
extern u8 FreeAudioBufferQueue[];
extern u8 DecodedAudioBufferQueue[];
s32 OSSuspendThread(void *thread);

void *PopReadedBuffer(void);
void PushReadedBuffer2(void *arg0);
u32 THPAudioDecode(s16 *buffer, u8 *audioFrame, s32 flag);
void AudioDecode(THPReadBuffer *readBuffer);
void *PopFreeAudioBuffer(void);
void PushDecodedAudioBuffer(void *arg0);


void AudioDecodeThreadStart(void) {
    if (AudioDecodeThreadCreated) {
        OSResumeThread(AudioDecodeThread);
    }
}

void AudioDecodeThreadCancel(void) {
    if (AudioDecodeThreadCreated) {
        OSCancelThread(AudioDecodeThread);
        AudioDecodeThreadCreated = 0;
    }
}

static void AudioDecoder(void) {
    void *buf;

    while (TRUE) {
        buf = PopReadedBuffer();
        AudioDecode(buf);
        PushReadedBuffer2(buf);
    }
}

static void AudioDecoderForOnMemory(void *bufAddr) {
    THPReadBuffer readBuffer;
    s32 tmp;
    s32 size;
    s32 i;

    i = 0;
    size = ActivePlayer.initReadSize;
    readBuffer.ptr = bufAddr;
    while (1) {
        readBuffer.frameNumber = i;
        AudioDecode(&readBuffer);
        tmp = (i + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
        if (tmp == ActivePlayer.numFrames - 1) {
            if (ActivePlayer.playFlag & 1) {
                size = *(s32 *)readBuffer.ptr;
                readBuffer.ptr = ActivePlayer.movieData;
            } else {
                OSSuspendThread(AudioDecodeThread);
            }
        } else {
            tmp = *(s32 *)readBuffer.ptr;
            readBuffer.ptr += size;
            size = tmp;
        }
        i++;
    }
}

void AudioDecode(THPReadBuffer *readBuffer) {
    THPAudioBuffer *audioBuf;
    u32 i;
    u32 *compSizePtr;
    u8 *ptr;

    ptr = readBuffer->ptr + ActivePlayer.numComponents * 4 + 8;
    compSizePtr = (u32 *)(readBuffer->ptr + 8);
    audioBuf = PopFreeAudioBuffer();
    for (i = 0; i < ActivePlayer.numComponents; i++) {
        switch (ActivePlayer.components[i]) {
        case 1:
            audioBuf->validSample = THPAudioDecode(audioBuf->buffer, ptr, 0);
            audioBuf->curPtr = audioBuf->buffer;
            audioBuf->frameNumber = readBuffer->frameNumber;
            PushDecodedAudioBuffer(audioBuf);
            break;
        }
        ptr += *compSizePtr;
        compSizePtr++;
    }
}

void *PopFreeAudioBuffer(void) {
    void *msg;

    OSReceiveMessage(FreeAudioBufferQueue, &msg, 1);
    return msg;
}

void PushFreeAudioBuffer(void *arg0)
{
    extern u8 UsedTextureSetQueue_8036EC78[];
    OSSendMessage(UsedTextureSetQueue_8036EC78, arg0, 0);
}

void *PopDecodedAudioBuffer(s32 flags) {
    void *msg;

    if (OSReceiveMessage(DecodedAudioBufferQueue, &msg, flags) == 1) {
        return msg;
    }
    return NULL;
}

void PushDecodedAudioBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
