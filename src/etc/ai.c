#include "common.h"


void AIReset(void) {
    extern s32 __AI_init_flag;
    __AI_init_flag = 0;
}
