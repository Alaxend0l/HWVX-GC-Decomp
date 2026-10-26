#include "common.h"


void __ARQCallbackHack(void) {
}

void ARQReset(void) {
    extern s32 __ARQ_init_flag;
    __ARQ_init_flag = 0;
}
