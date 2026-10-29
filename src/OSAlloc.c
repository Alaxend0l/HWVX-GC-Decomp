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

