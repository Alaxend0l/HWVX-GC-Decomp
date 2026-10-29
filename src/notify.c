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
s32 TRKRequestSend(TRKBuffer *msgBuf, s32 *bufferId, u32 p1, u32 p2, s32 p3);
void TRKTargetAddStopInfo(TRKBuffer *b);
void TRKTargetAddExceptionInfo(TRKBuffer *b);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

s32 TRKDoNotifyStopped(u8 cmd) {
    s32 err;
    s32 reply;
    s32 bufIdx;
    TRKBuffer *buffer;

    err = TRKGetFreeBuffer(&bufIdx, &buffer);
    if (err == 0) {
        if (TRKAppendBuffer1_ui8(buffer, cmd) == 0) {
            if (cmd == 0x90) {
                TRKTargetAddStopInfo(buffer);
            } else {
                TRKTargetAddExceptionInfo(buffer);
            }
        }
        err = TRKRequestSend(buffer, &reply, 2, 3, 1);
        if (err == 0) {
            TRKReleaseBuffer(reply);
        }
        TRKReleaseBuffer(bufIdx);
    }
    return err;
}
