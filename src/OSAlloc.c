#include "common.h"

s32 OSSetCurrentHeap(s32 arg0) {
    extern s32 __OSCurrHeap;
    s32 temp_r0;

    temp_r0 = __OSCurrHeap;
    __OSCurrHeap = arg0;
    return temp_r0;
}
