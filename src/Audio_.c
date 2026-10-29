#include "common.h"


s32 Audio_Create_(void) {
    void Audio_Update_(void);
    void ARInit(u32 *, u32);
    void ARQInit(void);
    void AIInit(u8 *);
    void AXInit(void);
    void AISetDSPSampleRate(u32);
    void AXRegisterCallback(void (*)(void));
    ARInit(0, 0);
    ARQInit();
    AIInit(0);
    AXInit();
    AISetDSPSampleRate(1);
    AXRegisterCallback(Audio_Update_);
    return 1;
}

void Audio_Destroy_(void) {
    M2C_UNK AIReset();
    M2C_UNK ARQReset();
    M2C_UNK ARReset();
    M2C_UNK AXQuit();
    AXQuit();
    AIReset();
    ARQReset();
    ARReset();
}

void Audio_Update_(void) {
    void Audio_Update(s32);
    Audio_Update(0xD05);
}

u32 HAllocA_(s32 size) {
    typedef struct MemBlk {
        u32 addr;
        s32 size;
        u32 unk8;
        u32 unkC;
    } MemBlk;
    extern MemBlk g_memblk[];
    extern volatile s32 g_last;
    extern u32 g_heap;

    size = (size + 0x1F) & ~0x1F;
    if (g_memblk[g_last].size < 0 || g_heap != g_memblk[g_last].addr || g_memblk[g_last].size < size) {
        return 0;
    }
    g_memblk[g_last].addr += size;
    g_memblk[g_last].size -= size;
    return g_heap;
}

void MemCpyDone(void) {
    extern s32 g_arqWait;
    g_arqWait = 0;
}

u32 MemCpyA_(u32 dst, u32 src, u32 len) {
    extern u32 g_arqWait;
    void ARQPostRequest(void *req, u32 owner, u32 type, u32 prio, u32 src, u32 dst, u32 len, void (*cb)(void));
    void PPCSync(void);
    u8 req[0x20];

    g_arqWait = 1;
    if (src & 0x80000000) {
        ARQPostRequest(req, 0, 0, 1, src, dst, len, MemCpyDone);
    } else {
        ARQPostRequest(req, 0, 1, 1, src, dst, len, MemCpyDone);
    }
    while (g_arqWait != 0) {
        PPCSync();
    }
    return dst + len;
}

