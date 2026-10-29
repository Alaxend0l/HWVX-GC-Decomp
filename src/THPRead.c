#include "common.h"


void ReadThreadStart(void) {
    extern s32 ReadThreadCreated;
    extern u8 ReadThread[];

    if (ReadThreadCreated) {
        OSResumeThread(ReadThread);
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

void PushFreeReadBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}

void PushReadedBuffer2(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
