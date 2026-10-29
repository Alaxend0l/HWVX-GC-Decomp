#include "common.h"


typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

void TRKResetBuffer(TRKBuffer *b, u8 keepData);
s32 TRKMessageSend(TRKBuffer *b);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

__declspec(section ".data") extern s32 IsTRKConnected;
void __TRK_reset(void);
s32 TRKDoReset(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    __TRK_reset();
    return 0;
}

void TRKConstructEvent(TRKEvent *event, s32 eventType);
s32 TRKPostEvent(TRKEvent *event);
s32 TRKDoDisconnect(TRKBuffer *b) {
    TRKEvent event;
    s32 ackTries;
    s32 err;

    IsTRKConnected = 0;
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    if (err == 0) {
        TRKConstructEvent(&event, 1);
        TRKPostEvent(&event);
    }
    return err;
}

s32 TRKDoConnect(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    IsTRKConnected = 1;
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKDoUnsupported(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0x10);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKStandardACK(TRKBuffer *b, u8 commandID, u8 replyError) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, commandID);
    TRKAppendBuffer1_ui8(b, replyError);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

void SetTRKConnected(s32 connected) {
    IsTRKConnected = connected;
}

s32 GetTRKConnected(void) {
    return IsTRKConnected;
}
