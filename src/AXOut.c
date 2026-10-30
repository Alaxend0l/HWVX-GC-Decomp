#include "common.h"

s64 OSGetTime(void);
void __AXOutNewFrame(u32 lessDspCycles);
void *DSPAssertTask(void *task);
volatile u32 __AXDebugSteppingMode;
volatile s32 __AXDSPDoneFlag;
volatile s32 __AXDSPInitFlag;
void (*__AXUserFrameCallback)(void);
volatile s64 __AXOsTime;
volatile u32 __AXOutDspReady;
volatile u32 __AXOutFrame;

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
u8 lbl_802F7848[24];

void __AXOutNewFrame(u32 lessDspCycles)
{
    void __AXSyncPBs(u32 lessDspCycles);
    void __AXPrintStudio(void);
    u32 __AXGetCommandListAddress(void);
    void DSPSendMailToDSP(u32 mail);
    u32 DSPCheckMailToDSP(void);
    void __AXServiceCallbackStack(void);
    void __AXProcessAux(void);
    void __AXNextFrame(void *sbuffer, void *buffer);
    void AIInitDMA(u32 start_addr, u32 length);
    u32 __AXGetNumVoices(void);
    AXPROFILE *__AXGetCurrentProfile(void);
    extern void (*__AXUserFrameCallback)(void);
    extern volatile u32 __AXOutFrame;
    u32 cl;
    AXPROFILE *profile;
    u8 *src;
    u8 *dst;
    int i;

    __AXLocalProfile.axFrameStart = OSGetTime();
    __AXSyncPBs(lessDspCycles);
    __AXPrintStudio();
    cl = __AXGetCommandListAddress();
    DSPSendMailToDSP(0xBABE0180);
    while (DSPCheckMailToDSP()) {
    }
    DSPSendMailToDSP(cl);
    while (DSPCheckMailToDSP()) {
    }
    __AXServiceCallbackStack();
    __AXLocalProfile.auxProcessingStart = OSGetTime();
    __AXProcessAux();
    __AXLocalProfile.auxProcessingEnd = OSGetTime();
    __AXLocalProfile.userCallbackStart = OSGetTime();
    if (__AXUserFrameCallback) {
        __AXUserFrameCallback();
    }
    __AXLocalProfile.userCallbackEnd = OSGetTime();
    __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[__AXOutFrame]);
    __AXOutFrame++;
    __AXOutFrame &= 1;
    AIInitDMA((u32)__AXOutBuffer[__AXOutFrame], sizeof(__AXOutBuffer[0]));
    __AXLocalProfile.axFrameEnd = OSGetTime();
    __AXLocalProfile.axNumVoices = __AXGetNumVoices();
    profile = __AXGetCurrentProfile();
    if (profile) {
        src = (u8 *)&__AXLocalProfile;
        dst = (u8 *)profile;
        for (i = 0; i < sizeof(AXPROFILE); i++) {
            *dst++ = *src++;
        }
    }
}

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
    extern volatile s32 __AXDSPInitFlag;
    __AXDSPInitFlag = 1;
}

void __AXDSPResumeCallback(void) {
    if (__AXOutDspReady == 2) {
        __AXOutDspReady = 0;
        __AXOutNewFrame((u32)(OSGetTime() - __AXOsTime) / 4);
    } else {
        __AXOutDspReady = 1;
    }
}

void __AXDSPDoneCallback(void) {
    __AXDSPDoneFlag = 1;
}

void __AXOutInitDSP(void)
{
    extern u8 axDspSlave[];
    extern u16 axDspSlaveLength;
    extern volatile s32 __AXDSPInitFlag;
    extern volatile s32 __AXDSPDoneFlag;
    s32 DSPCheckInit(void);
    void DSPInit(void);
    void *DSPAddTask(DSPTaskInfo *task);

    task.iram_mmem_addr = axDspSlave;
    task.iram_length = axDspSlaveLength;
    task.iram_addr = 0;
    task.dram_mmem_addr = ax_dram_image;
    task.dram_length = 0x2000;
    task.dram_addr = 0;
    task.dsp_init_vector = 0x10;
    task.dsp_resume_vector = 0x30;
    task.init_cb = __AXDSPInitCallback;
    task.res_cb = __AXDSPResumeCallback;
    task.done_cb = __AXDSPDoneCallback;
    task.req_cb = NULL;
    task.priority = 0;
    __AXDSPInitFlag = 0;
    __AXDSPDoneFlag = 0;
    if (!DSPCheckInit()) {
        DSPInit();
    }
    DSPAddTask(&task);
    while (!__AXDSPInitFlag) {
    }
}

void __AXOutInit(void)
{
    void __AXOutAiCallback(void);
    void __AXOutInitDSP(void);
    void DCFlushRange(void *addr, u32 nBytes);
    void *AIRegisterDMACallback(void (*callback)(void));
    void __AXNextFrame(void *sbuffer, void *buffer);
    void AIInitDMA(u32 start_addr, u32 length);
    void AIStartDMA(void);
    u32 *p;
    int i;

    __AXOutFrame = 0;
    __AXDebugSteppingMode = 0;
    p = (u32 *)__AXOutBuffer;
    for (i = 0; i < 320; i++) {
        p[i] = 0;
    }
    DCFlushRange(__AXOutBuffer, sizeof(__AXOutBuffer));
    p = __AXOutSBuffer;
    for (i = 0; i < 160; i++) {
        p[i] = 0;
    }
    DCFlushRange(__AXOutSBuffer, sizeof(__AXOutSBuffer));
    __AXOutInitDSP();
    AIRegisterDMACallback(__AXOutAiCallback);
    __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[1]);
    __AXOutDspReady = 1;
    __AXUserFrameCallback = NULL;
    AIInitDMA((u32)__AXOutBuffer[__AXOutFrame], sizeof(__AXOutBuffer[0]));
    AIStartDMA();
}

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void AIStopDMA(void);
void DSPHalt(void);
u32 DSPGetDMAStatus(void);
void DSPReset(void);
extern void (*__AXUserFrameCallback)(void);
void __AXOutQuit(void) {
    s32 old;

    old = OSDisableInterrupts();
    __AXUserFrameCallback = NULL;
    AIStopDMA();
    DSPHalt();
    while (DSPGetDMAStatus()) {
    }
    DSPReset();
    OSRestoreInterrupts(old);
}

void (*__AXUserFrameCallback)(void);
void AXRegisterCallback(void (*callback)(void)) {
    __AXUserFrameCallback = callback;
}

