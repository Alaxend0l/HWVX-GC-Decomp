#include "common.h"

typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;


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
void LCStoreBlocks(void *destTag, void *srcAddr, u32 numBlocks);
#pragma peephole off
u32 LCStoreData(void *destAddr, void *srcAddr, u32 nBytes) {
    u32 numBlocks = (nBytes + 31) / 32;
    u32 numTransactions = (numBlocks + 128 - 1) / 128;

    while (numBlocks > 0) {
        if (numBlocks < 128) {
            LCStoreBlocks(destAddr, srcAddr, numBlocks);
            numBlocks = 0;
        } else {
            LCStoreBlocks(destAddr, srcAddr, 0);
            numBlocks -= 128;
            destAddr = (void *)((u32)destAddr + 4096);
            srcAddr = (void *)((u32)srcAddr + 4096);
        }
    }
    return numTransactions;
}
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

#pragma dont_inline on
u32 PPCMfhid0(void);
u32 PPCMfmsr(void);
void PPCMtmsr(u32 val);
void ICEnable(void);
void DCEnable(void);
void DMAErrorHandler(u16 error, OSContext *context, ...);
void *OSSetErrorHandler(u16 error, void *handler);
void __OSCacheInit(void) {
    u32 oldMSR;

    if (!(PPCMfhid0() & 0x8000)) {
        ICEnable();
        DBPrintf("L1 i-caches initialized\n");
    }
    if (!(PPCMfhid0() & 0x4000)) {
        DCEnable();
        DBPrintf("L1 d-caches initialized\n");
    }
    if (!(PPCMfl2cr() & 0x80000000)) {
        oldMSR = PPCMfmsr();
        __sync();
        PPCMtmsr(0x30);
        __sync();
        __sync();
        PPCMtl2cr(PPCMfl2cr() & ~0x80000000);
        __sync();
        L2GlobalInvalidate();
        PPCMtmsr(oldMSR);
        PPCMtl2cr((PPCMfl2cr() | 0x80000000) & ~0x00200000);
        DBPrintf("L2 cache initialized\n");
    }
    OSSetErrorHandler(1, DMAErrorHandler);
    DBPrintf("Locked cache machine check handler installed\n");
}

