#include "common.h"


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

void PushDecodedAudioBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
