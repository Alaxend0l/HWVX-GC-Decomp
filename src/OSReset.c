#include "common.h"


typedef struct OSResetFunctionInfo {
    BOOL (*func)(BOOL final);
    u32 priority;
    struct OSResetFunctionInfo *next;
    struct OSResetFunctionInfo *prev;
} OSResetFunctionInfo;

extern OSResetFunctionInfo *ResetFunctionQueue;

#pragma peephole off
volatile u32 __PIRegs[12] : 0xCC003000;
volatile u16 __VIRegs[59] : 0xCC002000;
void __OSDoHotReset(u32 resetCode) {
    s32 OSDisableInterrupts(void);
    void ICFlashInvalidate(void);
    void Reset(u32);
    OSDisableInterrupts();
    __VIRegs[1] = 0;
    ICFlashInvalidate();
    Reset(resetCode * 8);
}

u32 OSGetResetCode(void) {
    if (*(u8 *)0x800030E2 != 0) {
        return 0x80000000;
    }
    return (__PIRegs[9] & ~7) >> 3;
}
