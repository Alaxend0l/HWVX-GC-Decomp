#include "common.h"

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

typedef struct TRKFramingState {
    s32 msgBufID;
    TRKBuffer *buffer;
    u8 receiveState;
    s32 isEscape;
    u8 fcs;
} TRKFramingState;

void TRKConstructEvent(TRKEvent *event, s32 eventType);
s32 TRKPostEvent(TRKEvent *event);
s32 TRKTestForPacket(void);
void *TRKGetBuffer(s32 idx);
s32 TRKSetBufferPosition(void *buffer, u32 pos);
s32 TRKReadBuffer1_ui8(void *buffer, u8 *data);
s32 TRKReleaseBuffer(s32 idx);

void *gTRKInputPendingPtr;
TRKFramingState gTRKFramingState;

s32 TRKReadUARTPoll(u8 *c);
s32 TRKGetFreeBuffer(s32 *msgID, TRKBuffer **outMsg);
s32 TRKStandardACK(TRKBuffer *b, u8 commandID, u8 replyError);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

static inline void TRKDiscardFrame(TRKFramingState *state) {
    if (gTRKFramingState.msgBufID != -1) {
        TRKReleaseBuffer(gTRKFramingState.msgBufID);
        gTRKFramingState.msgBufID = -1;
    }
    state->buffer = NULL;
    state->receiveState = 0;
}

static inline s32 TRKCheckFrame(TRKFramingState *state) {
    TRKBuffer *b = state->buffer;

    if (b->length < 2) {
        TRKStandardACK(b, 0xFF, 2);
        TRKDiscardFrame(state);
        return 0;
    }
    b->position = 0;
    b->length--;
    return 1;
}

s32 TRKTerminateSerialHandler(void) {
    return 0;
}

s32 TRKInitializeSerialHandler(void) {
    gTRKFramingState.msgBufID = -1;
    gTRKFramingState.receiveState = 0;
    gTRKFramingState.isEscape = 0;
    return 0;
}

void TRKProcessInput(s32 bufferIdx) {
    TRKEvent event;

    TRKConstructEvent(&event, 2);
    gTRKFramingState.msgBufID = -1;
    event.msgBufID = bufferIdx;
    TRKPostEvent(&event);
}

void TRKGetInput(void) {
    void *buf;
    s32 id;
    u8 command;

    id = TRKTestForPacket();
    if (id != -1) {
        buf = TRKGetBuffer(id);
        TRKSetBufferPosition(buf, 0);
        TRKReadBuffer1_ui8(buf, &command);
        if (command < 0x80) {
            TRKProcessInput(id);
        } else {
            TRKReleaseBuffer(id);
        }
    }
}

