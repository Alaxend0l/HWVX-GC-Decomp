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

void *__AXGetStudio(void) {
    return __AXStudio;
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

