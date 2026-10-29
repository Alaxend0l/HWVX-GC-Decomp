#include "common.h"


void __AXDSPInitCallback(void) {
    extern s32 __AXDSPInitFlag;
    __AXDSPInitFlag = 1;
}

void __AXDSPDoneCallback(void) {
    extern s32 Prepared_803C7260;
    Prepared_803C7260 = 1;
}

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void AIStopDMA(void);
void DSPHalt(void);
u32 DSPGetDMAStatus(void);
void DSPReset(void);
extern s32 __AXUserFrameCallback;
void __AXOutQuit(void) {
    s32 old;

    old = OSDisableInterrupts();
    __AXUserFrameCallback = 0;
    AIStopDMA();
    DSPHalt();
    while (DSPGetDMAStatus()) {
    }
    DSPReset();
    OSRestoreInterrupts(old);
}

s32 __AXUserFrameCallback;
void AXRegisterCallback(s32 arg0) {
    __AXUserFrameCallback = arg0;
}

