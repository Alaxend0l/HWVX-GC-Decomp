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
    u8 pad0[0x6C];
    u32 numComponents;
    u8 components[0x10];
} THPPlayer;

extern THPPlayer ActivePlayer;

void *PopReadedBuffer(void);
void PushReadedBuffer2(void *arg0);
u32 THPAudioDecode(s16 *buffer, u8 *audioFrame, s32 flag);
void AudioDecode(THPReadBuffer *readBuffer);
void *PopFreeAudioBuffer(void);
void PushDecodedAudioBuffer(void *arg0);


void AudioDecodeThreadStart(void) {
    extern s32 AudioDecodeThreadCreated;
    extern u8 AudioDecodeThread[];

    if (AudioDecodeThreadCreated) {
        OSResumeThread(AudioDecodeThread);
    }
}

void AudioDecodeThreadCancel(void) {
    extern s32 AudioDecodeThreadCreated;
    extern u8 AudioDecodeThread[];

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
    extern u8 FreeAudioBufferQueue[];
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
    extern u8 DecodedAudioBufferQueue[];
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
