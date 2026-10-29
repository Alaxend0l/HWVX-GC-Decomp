#include "common.h"

s64 OSGetTime(void);
void __AXOutNewFrame(u32 lessDspCycles);
void *DSPAssertTask(void *task);
extern u32 __AXOutDspReady;
extern s64 __AXOsTime;

typedef struct DSPTaskInfo {
    u32 state;
    u32 priority;
    u32 flags;
    void *iram_mmem_addr;
    u32 iram_length;
    u32 iram_addr;
    void *dram_mmem_addr;
    u32 dram_length;
    u32 dram_addr;
    u16 dsp_init_vector;
    u16 dsp_resume_vector;
    void (*init_cb)(void);
    void (*res_cb)(void);
    void (*done_cb)(void);
    void (*req_cb)(void);
    struct DSPTaskInfo *next;
    struct DSPTaskInfo *prev;
    s64 t_context;
    s64 t_task;
} DSPTaskInfo;

typedef struct AXPROFILE {
    s64 axFrameStart;
    s64 auxProcessingStart;
    s64 auxProcessingEnd;
    s64 userCallbackStart;
    s64 userCallbackEnd;
    s64 axFrameEnd;
    u32 axNumVoices;
    u32 pad;
} AXPROFILE;

static u8 __AXOutBuffer[2][0x280] __attribute__((aligned(32)));
static u32 __AXOutSBuffer[160] __attribute__((aligned(32)));
static AXPROFILE __AXLocalProfile;
static DSPTaskInfo task;
static u16 ax_dram_image[8192];

void __AXOutAiCallback(void) {
    if (__AXOutDspReady == 0) {
        __AXOsTime = OSGetTime();
    }
    if (__AXOutDspReady == 1) {
        __AXOutDspReady = 0;
        __AXOutNewFrame(0);
    } else {
        __AXOutDspReady = 2;
        DSPAssertTask(&task);
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

