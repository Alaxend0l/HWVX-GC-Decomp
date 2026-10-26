#include "common.h"


void __AXAuxQuit(void) {
    extern s32 __AXCallbackAuxA;
    extern s32 __AXCallbackAuxB;
    __AXCallbackAuxA = 0;
    __AXCallbackAuxB = 0;
}
