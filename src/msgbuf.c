#include "common.h"

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

void *TRK_memset(void *dst, s32 val, u32 n);
s32 TRKAcquireMutex(void *mutex);
s32 TRKReleaseMutex(void *mutex);

extern TRKBuffer gTRKMsgBufs[3];
__declspec(section ".data") extern s32 gTRKBigEndian;

void *TRK_memcpy(void *dst, const void *src, u32 n);
s32 TRKAppendBuffer(TRKBuffer *b, const void *data, u32 length);

s32 TRKAppendBuffer1_ui32(TRKBuffer *b, u32 data) {
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)&data;
    } else {
        byteData = (u8 *)&data;
        bigEndianData = swapBuffer;
        bigEndianData[0] = byteData[3];
        bigEndianData[1] = byteData[2];
        bigEndianData[2] = byteData[1];
        bigEndianData[3] = byteData[0];
    }
    return TRKAppendBuffer(b, bigEndianData, sizeof(data));
}

s32 TRKAppendBuffer1_ui16(TRKBuffer *b, u16 data) {
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)&data;
    } else {
        byteData = (u8 *)&data;
        bigEndianData = swapBuffer;
        bigEndianData[0] = byteData[1];
        bigEndianData[1] = byteData[0];
    }
    return TRKAppendBuffer(b, bigEndianData, sizeof(data));
}

s32 TRKReadBuffer(TRKBuffer *b, void *data, u32 length) {
    s32 error = 0;
    u32 bytesLeft;

    if (length == 0) {
        return 0;
    }
    bytesLeft = b->length - b->position;
    if (length > bytesLeft) {
        error = 0x302;
        length = bytesLeft;
    }
    TRK_memcpy(data, b->data + b->position, length);
    b->position += length;
    return error;
}

s32 TRKSetBufferPosition(TRKBuffer *b, u32 position) {
    s32 err = 0;

    if (position > 0x880) {
        err = 0x301;
    } else {
        b->position = position;
        if (position > b->length) {
            b->length = position;
        }
    }
    return err;
}

void TRKResetBuffer(TRKBuffer *b, u8 keepData) {
    b->length = 0;
    b->position = 0;
    if (!keepData) {
        TRK_memset(b->data, 0, 0x880);
    }
}

void TRKReleaseBuffer(s32 idx) {
    TRKBuffer *b;

    if (idx == -1) {
        return;
    }
    if (idx >= 0 && idx < 3) {
        b = &gTRKMsgBufs[idx];
        TRKAcquireMutex(b);
        b->isInUse = 0;
        TRKReleaseMutex(b);
    }
}

TRKBuffer *TRKGetBuffer(s32 idx) {
    TRKBuffer *buf = NULL;

    if (idx >= 0 && idx < 3) {
        buf = &gTRKMsgBufs[idx];
    }
    return buf;
}

