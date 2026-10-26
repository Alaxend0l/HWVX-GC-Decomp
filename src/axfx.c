#include "common.h"


void __AXFXAllocFunction(s32 arg0)
{
    extern s32 __OSCurrHeap;
    void *OSAllocFromHeap(s32 heap, u32 size);

    OSAllocFromHeap(__OSCurrHeap, arg0);
}
