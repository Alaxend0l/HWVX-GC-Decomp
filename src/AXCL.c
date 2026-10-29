#include "common.h"


s32 __AXGetCommandListCycles(void) {
    extern s32 __AXCommandListCycles;
    return __AXCommandListCycles;
}

void __AXClInit(void) {
    extern u32 __AXClMode;
    extern u16 *__AXClWrite;
    extern u32 __AXCommandListPosition;
    extern u16 __AXCommandList[2][384];
    extern u32 __AXCompressor;
    __AXClMode = 0;
    __AXCommandListPosition = 0;
    __AXClWrite = (u16 *)&__AXCommandList;
    __AXCompressor = 1;
}

void __AXClQuit() {}

