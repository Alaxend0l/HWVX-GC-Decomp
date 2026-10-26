#include "common.h"


void ARQReset(void) {
    extern s32 __ARQ_init_flag;
    __ARQ_init_flag = 0;
}
