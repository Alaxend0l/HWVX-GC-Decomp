#include "common.h"

extern u8 *gTRKInputPendingPtr[];

s32 TRKGetNextEvent(TRKEvent *event);
void TRKDestructEvent(TRKEvent *event);
void *TRKGetBuffer(s32 idx);
s32 TRKDispatchMessage(void *buffer);
s32 TRKTargetInterrupt(TRKEvent *event);
s32 TRKTargetSupportRequest(void);
void TRKGetInput(void);
s32 TRKTargetStopped(void);
void TRKTargetContinue(void);

void TRKNubMainLoop(void) {
    TRKEvent event;
    BOOL isShutdownRequested;
    BOOL isNewInput;

    isShutdownRequested = FALSE;
    isNewInput = FALSE;
    while (isShutdownRequested == FALSE) {
        if (TRKGetNextEvent(&event) != FALSE) {
            isNewInput = FALSE;
            switch (event.eventType) {
            case 0:
                break;
            case 2:
                TRKDispatchMessage(TRKGetBuffer(event.msgBufID));
                break;
            case 1:
                isShutdownRequested = TRUE;
                break;
            case 3:
            case 4:
                TRKTargetInterrupt(&event);
                break;
            case 5:
                TRKTargetSupportRequest();
                break;
            }
            TRKDestructEvent(&event);
        } else if (isNewInput == FALSE || *gTRKInputPendingPtr[0] != 0) {
            isNewInput = TRUE;
            TRKGetInput();
        } else {
            if (TRKTargetStopped() == FALSE) {
                TRKTargetContinue();
            }
            isNewInput = FALSE;
        }
    }
}
