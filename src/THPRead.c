#include "common.h"


void ReadThreadStart(void) {
    extern s32 ReadThreadCreated;
    extern u8 ReadThread[];

    if (ReadThreadCreated) {
        OSResumeThread(ReadThread);
    }
}

void ReadThreadCancel(void) {
    extern s32 ReadThreadCreated;
    extern u8 ReadThread[];

    if (ReadThreadCreated) {
        OSCancelThread(ReadThread);
        ReadThreadCreated = 0;
    }
}

void *PopReadedBuffer(void) {
    extern u8 ReadedBufferQueue[];
    void *msg;

    OSReceiveMessage(ReadedBufferQueue, &msg, 1);
    return msg;
}

void PushReadedBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}

void *PopFreeReadBuffer(void) {
    extern u8 FreeReadBufferQueue[];
    void *msg;

    OSReceiveMessage(FreeReadBufferQueue, &msg, 1);
    return msg;
}

void PushFreeReadBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}

void *PopReadedBuffer2(void) {
    extern u8 ReadedBufferQueue2[];
    void *msg;

    OSReceiveMessage(ReadedBufferQueue2, &msg, 1);
    return msg;
}

void PushReadedBuffer2(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
