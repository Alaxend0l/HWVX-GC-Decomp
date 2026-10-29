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

#pragma push
#pragma optimization_level 1
u32 HAllocA_(s32 size) {
    typedef struct MemBlk {
        u32 addr;
        s32 size;
        u32 unk8;
        u32 unkC;
    } MemBlk;
    extern MemBlk g_memblk[];
    extern s32 g_last;
    extern u32 g_heap;

    size = (size + 0x1F) & ~0x1F;
    if (g_memblk[g_last].size < 0 || g_heap != g_memblk[g_last].addr || g_memblk[g_last].size < size) {
        return 0;
    }
    g_memblk[g_last].addr += size;
    g_memblk[g_last].size -= size;
    return g_heap;
}

#pragma push
#pragma optimization_level 1
u32 MAllocA_(s32 size) {
    typedef struct MemBlk {
        u32 addr;
        s32 size;
        u32 prev;
        u32 next;
    } MemBlk;
    extern MemBlk g_memblk[];
    extern s32 g_last;
    extern u32 g_free;
    int printf();
    u32 i;
    u32 n;

    i = 0;
    size = (size + 0x1F) & ~0x1F;
    while (i != 0xFFFFFFFF) {
        if (g_memblk[i].size >= size) {
            if (g_memblk[i].size > size) {
                if (g_free == 0xFFFFFFFF) {
                    return 0;
                }
                n = g_free;
                g_free = g_memblk[n].next;
                g_memblk[n].addr = g_memblk[i].addr;
                g_memblk[n].size = g_memblk[i].size - size;
                g_memblk[n].prev = i;
                g_memblk[n].next = g_memblk[i].next;
                if (g_memblk[n].next == 0xFFFFFFFF) {
                    g_last = n;
                }
                g_memblk[i].next = n;
            }
            g_memblk[i].addr += g_memblk[i].size - size;
            g_memblk[i].size = -size;
            return g_memblk[i].addr;
        }
        i = g_memblk[i].next;
    }
    printf("Audio :: MAllocA_ - Insufficient ARAM  size: %i\n", size);
    return 0;
}
#pragma pop

#pragma pop
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

s32 Mul16_(u32 a, u32 b) {
    u32 hi = __mulhwu(a, b);
    u32 lo = a * b;
    return (lo >> 16) | (hi << 16);
}

