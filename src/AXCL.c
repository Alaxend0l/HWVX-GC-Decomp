#include "common.h"


s32 __AXGetCommandListCycles(void) {
    extern s32 __AXCommandListCycles;
    return __AXCommandListCycles;
}

void __AXClQuit() {}

