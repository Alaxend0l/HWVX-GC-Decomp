#include "common.h"

typedef struct TRKFramingState {
    s32 msgBufID;
    void *buffer;
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

