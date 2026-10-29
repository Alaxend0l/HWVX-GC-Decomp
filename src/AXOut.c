#include "common.h"

s64 OSGetTime(void);
void __AXOutNewFrame(u32 lessDspCycles);
void *DSPAssertTask(void *task);
extern u32 __AXOutDspReady;
extern s64 __AXOsTime;
extern u8 task[0x50];

void __AXOutAiCallback(void) {
    if (__AXOutDspReady == 0) {
        __AXOsTime = OSGetTime();
    }
    if (__AXOutDspReady == 1) {
        __AXOutDspReady = 0;
        __AXOutNewFrame(0);
    } else {
        __AXOutDspReady = 2;
        DSPAssertTask(task);
    }
}

void __AXDSPInitCallback(void) {
    extern s32 __AXDSPInitFlag;
    __AXDSPInitFlag = 1;
}

s64 OSGetTime(void);
void __AXOutNewFrame(u32 lessDspCycles);
extern u32 __AXOutDspReady;
extern s64 __AXOsTime;
void __AXDSPResumeCallback(void) {
    if (__AXOutDspReady == 2) {
        __AXOutDspReady = 0;
        __AXOutNewFrame((u32)(OSGetTime() - __AXOsTime) >> 2);
    } else {
        __AXOutDspReady = 1;
    }
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

