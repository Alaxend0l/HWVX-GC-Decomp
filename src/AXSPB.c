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
