#include "common.h"


void ARReset(void) {
    extern s32 __AR_init_flag;
    __AR_init_flag = 0;
}

s32 ARGetSize(void) {
    extern s32 __AR_Size;
    return __AR_Size;
}

