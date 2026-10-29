#include "common.h"

typedef struct THPReadBuffer {
    u8 *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct THPPlayer {
    u8 fileInfo[0x50];
    u32 numFrames;
    u8 pad54[0x64 - 0x54];
    s32 movieDataOffsets;
    u8 pad68[0x9E - 0x68];
    u8 playFlag;
    u8 pad9F;
    s32 dvdError;
    u8 padA4[0xB0 - 0xA4];
    s32 initOffset;
    s32 initReadSize;
    s32 initReadFrame;
} THPPlayer;

extern THPPlayer ActivePlayer;


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

void *PopFreeReadBuffer(void);
void PushReadedBuffer(void *arg0);
#pragma use_lmw_stmw on
void Reader(void) {
    s32 DVDReadPrio(void *fileInfo, void *addr, s32 length, s32 offset, s32 prio);
    s32 OSSuspendThread(void *thread);
    void PrepareReady(s32 flag);
    extern u8 ReadThread[];
    THPReadBuffer *readBuffer;
    s32 offset;
    s32 size;
    s32 readFrame;
    s32 result;
    s32 frameNumber;

    readFrame = 0;
    offset = ActivePlayer.initOffset;
    size = ActivePlayer.initReadSize;
    while (1) {
        readBuffer = PopFreeReadBuffer();
        result = DVDReadPrio(&ActivePlayer, readBuffer->ptr, size, offset, 2);
        if (result != size) {
            if (result == -1) {
                ActivePlayer.dvdError = -1;
            }
            if (readFrame == 0) {
                PrepareReady(0);
            }
            OSSuspendThread(ReadThread);
        }
        readBuffer->frameNumber = readFrame;
        PushReadedBuffer(readBuffer);
        offset += size;
        size = *(s32 *)readBuffer->ptr;
        frameNumber = (readFrame + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
        if (frameNumber == ActivePlayer.numFrames - 1) {
            if (ActivePlayer.playFlag & 1) {
                offset = ActivePlayer.movieDataOffsets;
            } else {
                OSSuspendThread(ReadThread);
            }
        }
        readFrame++;
    }
}
void *PopReadedBuffer(void) {
    extern u8 ReadedBufferQueue[0x20];
    void *msg;

    OSReceiveMessage(ReadedBufferQueue, &msg, 1);
    return msg;
}

void PushReadedBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}

void *PopFreeReadBuffer(void) {
    extern u8 FreeReadBufferQueue[0x20];
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
