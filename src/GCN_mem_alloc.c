#include "common.h"

#pragma readonly_strings on

extern s32 __OSCurrHeap;
void OSReport(const char *msg, ...);
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void OSSetArenaLo(void *newLo);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, s32 maxHeaps);
s32 OSCreateHeap(void *start, void *end);
s32 OSSetCurrentHeap(s32 heap);
s32 OSFreeToHeap(s32 heap, s32 ptr);
s32 OSAllocFromHeap(s32 heap, s32 size);

static inline void InitDefaultHeap(void) {
    void *arenaLo;
    void *arenaHi;

    OSReport("GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n");
    OSReport("Metrowerks CW runtime library initializing default heap\n");
    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
    OSSetArenaLo(arenaLo);
    arenaLo = (void *)(((u32)arenaLo + 0x1F) & ~0x1F);
    arenaHi = (void *)((u32)arenaHi & ~0x1F);
    OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
    OSSetArenaLo(arenaLo = arenaHi);
}

void __sys_free(void *ptr) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    OSFreeToHeap(__OSCurrHeap, (s32)ptr);
}

void *__sys_alloc(u32 size) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    return (void *)OSAllocFromHeap(__OSCurrHeap, size);
}

