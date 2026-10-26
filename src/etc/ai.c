#include "common.h"


s32 AIGetStreamPlayState(void)
{
    return *(s32 *)0xCC006C00 & 1;
}

void AIReset(void) {
    extern s32 __AI_init_flag;
    __AI_init_flag = 0;
}
