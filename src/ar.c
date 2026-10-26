#include "common.h"


void ARReset(void) {
    extern s32 __AR_init_flag;
    __AR_init_flag = 0;
}
