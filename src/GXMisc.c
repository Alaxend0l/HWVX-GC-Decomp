#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u16 vNum;
    u8 pad6[0x1DC - 0x6];
    u32 peCtrl;
    u8 pad1E0[0x4F1 - 0x1E0];
    u8 dlSaveContext;
    u8 pad4F2[2];
    u32 dirtyState;
} GXData;

void __GXSetDirtyState(void);
void PPCSync(void);

typedef struct OSThread OSThread;
typedef struct OSThreadQueue { OSThread *head; OSThread *tail; } OSThreadQueue;

void *__OSSetInterruptHandler(s16 interrupt, void *handler);
void __OSUnmaskInterrupts(u32 mask);
void OSInitThreadQueue(OSThreadQueue *queue);
void GXTokenInterruptHandler(s16 interrupt, void *context);
void GXFinishInterruptHandler(s16 interrupt, void *context);

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void OSSleepThread(OSThreadQueue *queue);

typedef struct OSContext { u8 data[0x2C8]; } OSContext;
void OSClearContext(OSContext *context);
void OSSetCurrentContext(OSContext *context);
void OSWakeupThread(OSThreadQueue *queue);

static void (*DrawDoneCB)(void);
static u8 DrawDone;
static OSThreadQueue FinishQueue;

void GXFlush(void);

static inline void GXSetDrawDone(void) {
    u32 reg;
    s32 enabled;

    enabled = OSDisableInterrupts();
    reg = 0x45000002;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    GXFlush();
    DrawDone = 0;
    OSRestoreInterrupts(enabled);
}

extern GXData *gx;


extern volatile u16 *__peReg;
void GXSetMisc(s32 token, u32 val) {
    switch (token) {
    case 1:
        gx->vNum = val;
        gx->vNumNot = !gx->vNum;
        gx->bpSentNot = 1;
        if (gx->vNum) {
            gx->dirtyState |= 8;
        }
        break;
    case 2:
        gx->dlSaveContext = (val > 0);
        break;
    case 0:
    default:
        break;
    }
}

void GXFlush(void) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    PPCSync();
}

#pragma dont_inline off
void GXDrawDone(void) {
    s32 enabled;

    GXSetDrawDone();
    enabled = OSDisableInterrupts();
    while (!DrawDone) {
        OSSleepThread(&FinishQueue);
    }
    OSRestoreInterrupts(enabled);
}

void GXPixModeSync(void) {
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->peCtrl;
    gx->bpSentNot = 0;
}

void GXPokeAlphaMode(s32 func, u8 threshold) {
    u32 reg;

    reg = (func << 8) | threshold;
    __peReg[3] = reg;
}

void GXPokeAlphaRead(s32 mode) {
    u32 reg;

    reg = mode;
    reg = (reg & ~4) | 4;
    __peReg[4] = reg;
}

void GXPokeAlphaUpdate(u8 update_enable) {
    u32 reg;

    reg = __peReg[1];
    reg = (reg & ~0x10) | (update_enable << 4);
    __peReg[1] = reg;
}

void GXPokeColorUpdate(u8 update_enable) {
    u32 reg;

    reg = __peReg[1];
    reg = (reg & ~0x8) | (update_enable << 3);
    __peReg[1] = reg;
}

void GXPokeDstAlpha(u8 enable, u8 alpha) {
    u32 reg;

    reg = alpha | (enable << 8);
    __peReg[2] = reg;
}

void GXPokeDither(u8 dither) {
    u32 reg;

    reg = __peReg[1];
    reg = (reg & ~0x4) | (dither << 2);
    __peReg[1] = reg;
}

void GXPokeZMode(u8 compare_enable, s32 func, u8 update_enable) {
    u32 reg;

    reg = compare_enable;
    reg = (reg & ~0xE) | (func << 1);
    reg = (reg & ~0x10) | (update_enable << 4);
    __peReg[0] = reg;
}

void __GXPEInit(void) {
    u32 reg;

    __OSSetInterruptHandler(0x12, GXTokenInterruptHandler);
    __OSSetInterruptHandler(0x13, GXFinishInterruptHandler);
    OSInitThreadQueue(&FinishQueue);
    __OSUnmaskInterrupts(0x2000);
    __OSUnmaskInterrupts(0x1000);
    reg = __peReg[5];
    reg = (reg & ~4) | 4;
    reg = (reg & ~8) | 8;
    reg = (reg & ~1) | 1;
    reg = (reg & ~2) | 2;
    __peReg[5] = reg;
}

