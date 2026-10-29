#include "common.h"

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

s32 TRKGetFreeBuffer(s32 *msgID, TRKBuffer **outMsg);
void TRKReleaseBuffer(s32 idx);
TRKBuffer *TRKGetBuffer(s32 idx);
s32 TRKSetBufferPosition(TRKBuffer *b, u32 pos);
s32 TRKRequestSend(TRKBuffer *msgBuf, s32 *bufferId, u32 p1, u32 p2, s32 p3);
s32 TRKAppendBuffer1_ui32(TRKBuffer *b, u32 data);
s32 TRKReadBuffer1_ui8(TRKBuffer *b, u8 *data);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

s32 HandleCloseFileSupportRequest(u32 fileHandle, u8 *ioResult) {
    s32 error;
    s32 replyBufferId;
    TRKBuffer *replyBuffer;
    s32 bufferId;
    TRKBuffer *buffer;

    error = TRKGetFreeBuffer(&bufferId, &buffer);
    if (error == 0) {
        error = TRKAppendBuffer1_ui8(buffer, 0xD3);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui32(buffer, fileHandle);
    }
    if (error == 0) {
        *ioResult = 0;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }
        if (error == 0) {
            error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
        }
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}
