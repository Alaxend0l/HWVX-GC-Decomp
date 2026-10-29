#include "common.h"


#pragma peephole off
volatile u32 __PIRegs[12] : 0xCC003000;
u32 OSGetResetCode(void) {
    if (*(u8 *)0x800030E2 != 0) {
        return 0x80000000;
    }
    return (__PIRegs[9] & ~7) >> 3;
}
