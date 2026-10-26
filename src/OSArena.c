#include "common.h"


s32 OSGetArenaLo(void) {
    extern s32 __OSArenaLo;
    return __OSArenaLo;
}
