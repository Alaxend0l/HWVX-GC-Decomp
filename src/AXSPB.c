#include "common.h"

extern s32 __AXSpbAL;
extern s32 __AXSpbAR;
extern s32 __AXSpbAS;
extern s32 __AXSpbAAL;
extern s32 __AXSpbAAR;
extern s32 __AXSpbAAS;
extern s32 __AXSpbABL;
extern s32 __AXSpbABR;
extern s32 __AXSpbABS;

typedef struct {
    u8 pad[0x52];
    s16 dL;
    s16 dAL;
    s16 dBL;
    s16 dR;
    s16 dAR;
    s16 dBR;
    s16 dS;
    s16 dAS;
    s16 dBS;
} tVoicePacket;

extern u8 __AXStudio[];

void DCFlushRange(void *addr, u32 nBytes);

typedef struct {
    u16 hi;
    u16 lo;
    s16 delta;
} tDepop;

static void __AXDepopFade(s32 *hist, s32 *samp, s16 *delta) {
    s32 step;

    step = *hist / 160;
    if (step) {
        if (step > 20) {
            step = 20;
        }
        if (step < -20) {
            step = -20;
        }
        *samp = *hist;
        *hist -= step * 160;
        *delta = -step;
    } else {
        *hist = 0;
        *samp = 0;
        *delta = 0;
    }
}

static void __AXDepopFadeMain(s32 *hist, tDepop *d) {
    s32 step;

    step = *hist / 160;
    if (step) {
        if (step > 20) {
            step = 20;
        }
        if (step < -20) {
            step = -20;
        }
        *(s32 *)&d->hi = *hist;
        *hist -= step * 160;
        d->delta = -step;
    } else {
        *hist = 0;
        *(s32 *)&d->hi = 0;
        d->delta = 0;
    }
}

void *__AXGetStudio(void) {
    return __AXStudio;
}

void __AXPrintStudio(void) {
    u8 *studio = __AXStudio;

    __AXDepopFade(&__AXSpbAL, (s32 *)(studio + 0x0), (s16 *)(studio + 0x4));
    __AXDepopFade(&__AXSpbAR, (s32 *)(studio + 0x6), (s16 *)(studio + 0xA));
    __AXDepopFade(&__AXSpbAS, (s32 *)(studio + 0xC), (s16 *)(studio + 0x10));
    __AXDepopFade(&__AXSpbAAL, (s32 *)(studio + 0x12), (s16 *)(studio + 0x16));
    __AXDepopFade(&__AXSpbAAR, (s32 *)(studio + 0x18), (s16 *)(studio + 0x1C));
    __AXDepopFade(&__AXSpbAAS, (s32 *)(studio + 0x1E), (s16 *)(studio + 0x22));
    __AXDepopFade(&__AXSpbABL, (s32 *)(studio + 0x24), (s16 *)(studio + 0x28));
    __AXDepopFade(&__AXSpbABR, (s32 *)(studio + 0x2A), (s16 *)(studio + 0x2E));
    __AXDepopFade(&__AXSpbABS, (s32 *)(studio + 0x30), (s16 *)(studio + 0x34));
    DCFlushRange(studio, 0x36);
}

void __AXSPBInit(void) {
    __AXSpbABS = 0;
    __AXSpbABR = 0;
    __AXSpbABL = 0;
    __AXSpbAAS = 0;
    __AXSpbAAR = 0;
    __AXSpbAAL = 0;
    __AXSpbAS = 0;
    __AXSpbAR = 0;
    __AXSpbAL = 0;
}

void __AXSPBQuit(void) {
}

void __AXDepopVoice(tVoicePacket *pvpb) {
    __AXSpbAL += pvpb->dL;
    __AXSpbAAL += pvpb->dAL;
    __AXSpbABL += pvpb->dBL;
    __AXSpbAR += pvpb->dR;
    __AXSpbAAR += pvpb->dAR;
    __AXSpbABR += pvpb->dBR;
    __AXSpbAS += pvpb->dS;
    __AXSpbAAS += pvpb->dAS;
    __AXSpbABS += pvpb->dBS;
}

