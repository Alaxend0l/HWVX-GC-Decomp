#include "common.h"


void __AXDSPDoneCallback(void) {
    extern s32 Prepared_803C7260;
    Prepared_803C7260 = 1;
}

s32 __AXUserFrameCallback;
void AXRegisterCallback(s32 arg0) {
    __AXUserFrameCallback = arg0;
}

