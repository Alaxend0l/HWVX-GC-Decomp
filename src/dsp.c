#include "common.h"


s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag;
    return __DSP_init_flag;
}
