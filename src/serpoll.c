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
