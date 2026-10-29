#include "common.h"


void DCFlushRangeNoSync(void *addr, u32 nBytes) {
    if (nBytes > 0) {
        if ((u32)addr & 0x1F) {
            nBytes += 0x20;
        }
        nBytes = (nBytes + 0x1F) >> 5;
        do {
            __dcbf(addr, 0);
            addr = (u8 *)addr + 0x20;
        } while (--nBytes);
    }
}

void DCZeroRange(void *addr, u32 nBytes) {
    if (nBytes > 0) {
        if ((u32)addr & 0x1F) {
            nBytes += 0x20;
        }
        nBytes = (nBytes + 0x1F) >> 5;
        do {
            __dcbz(addr, 0);
            addr = (u8 *)addr + 0x20;
        } while (--nBytes);
    }
}

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void __LCEnable(void);
void LCEnable(void) {
    s32 enabled;

    enabled = OSDisableInterrupts();
    __LCEnable();
    OSRestoreInterrupts(enabled);
}

#pragma peephole off
u32 PPCMfl2cr(void);
void PPCMtl2cr(u32 val);
void DBPrintf(const char *fmt, ...);
void L2GlobalInvalidate(void) {
    __sync();
    PPCMtl2cr(PPCMfl2cr() & ~0x80000000);
    __sync();
    PPCMtl2cr(PPCMfl2cr() | 0x00200000);
    while (PPCMfl2cr() & 0x00000001) {
    }
    PPCMtl2cr(PPCMfl2cr() & ~0x00200000);
    while (PPCMfl2cr() & 0x00000001) {
        DBPrintf(">>> L2 INVALIDATE : SHOULD NEVER HAPPEN\n");
    }
}

