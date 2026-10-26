#include "common.h"


s32 __AXGetNumVoices(void) {
    extern s32 __AXNumVoices;
    return __AXNumVoices;
}

void *__AXGetPBs(void) {
    extern u8 __AXPB[];
    return __AXPB;
}

