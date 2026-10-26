#include "common.h"


void MemCpyDone(void) {
    extern s32 g_arqWait;
    g_arqWait = 0;
}
