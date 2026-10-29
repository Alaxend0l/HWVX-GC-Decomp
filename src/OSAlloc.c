#include "common.h"

typedef struct Cell {
    struct Cell *prev;
    struct Cell *next;
    s32 size;
} Cell;

typedef struct HeapDesc {
    s32 size;
    Cell *free;
    Cell *allocated;
} HeapDesc;

extern HeapDesc *HeapArray;
extern s32 NumHeaps;
extern void *ArenaStart;
extern void *ArenaEnd;
extern s32 __OSCurrHeap;

s32 OSSetCurrentHeap(s32 arg0) {
    extern s32 __OSCurrHeap;
    s32 temp_r0;

    temp_r0 = __OSCurrHeap;
    __OSCurrHeap = arg0;
    return temp_r0;
}

void *OSInitAlloc(void *arenaStart, void *arenaEnd, s32 maxHeaps)
{
    u32 arraySize;
    s32 i;

    arraySize = sizeof(HeapDesc) * maxHeaps;
    HeapArray = arenaStart;
    NumHeaps = maxHeaps;
    for (i = 0; i < NumHeaps; i++) {
        HeapDesc *hd = &HeapArray[i];
        hd->size = -1;
        hd->free = hd->allocated = NULL;
    }
    __OSCurrHeap = -1;
    arenaStart = (void *)((u8 *)HeapArray + arraySize);
    arenaStart = (void *)(((u32)arenaStart + 0x1F) & ~0x1F);
    ArenaStart = arenaStart;
    ArenaEnd = (void *)((u32)arenaEnd & ~0x1F);
    return arenaStart;
}

s32 OSCreateHeap(void *start, void *end)
{
    s32 heap;
    HeapDesc *hd;
    Cell *cell;

    start = (void *)(((u32)start + 0x1F) & ~0x1F);
    end = (void *)((u32)end & ~0x1F);
    for (heap = 0; heap < NumHeaps; heap++) {
        hd = &HeapArray[heap];
        if (hd->size < 0) {
            hd->size = (u8 *)end - (u8 *)start;
            cell = (Cell *)start;
            cell->prev = NULL;
            cell->next = NULL;
            cell->size = hd->size;
            hd->free = cell;
            hd->allocated = NULL;
            return heap;
        }
    }
    return -1;
}

