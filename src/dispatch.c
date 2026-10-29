#include "common.h"

__declspec(section ".data") u32 gTRKDispatchTableSize;

s32 TRKInitializeDispatcher(void) {
    gTRKDispatchTableSize = 32;
    return 0;
}
