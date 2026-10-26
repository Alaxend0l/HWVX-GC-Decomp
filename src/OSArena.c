#include "common.h"

s32 __OSArenaLo = -1;
s32 __OSArenaHi;


s32 OSGetArenaHi(void) {
    return __OSArenaHi;
}

s32 OSGetArenaLo(void) {
    extern s32 __OSArenaLo;
    return __OSArenaLo;
}

void OSSetArenaHi(s32 arg0) {
    __OSArenaHi = arg0;
}

void OSSetArenaLo(s32 arg0) {
    __OSArenaLo = arg0;
}

