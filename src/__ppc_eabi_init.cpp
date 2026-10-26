#include "common.h"


extern "C" void __OSPSInit(void);
extern "C" void __OSCacheInit(void);
extern "C" void PPCHalt(void);
extern "C" void __init_cpp(void);
extern "C" u32 __mfmsr(void);
extern "C" void __mtmsr(u32);
extern "C" void __dcbst(void *, int);
extern "C" void __icbi(void *, int);
extern "C" void (*_ctors[])(void);

extern "C" void __flush_cache(void * address, u32 size)
{
    void * aligned;
    u32 addr;

    addr = (u32)address;
    aligned = (void *)(addr & 0xfffffff1);
    size += addr - (u32)aligned;
    do
    {
        __dcbst((void *)0, (int)(u32)aligned);
        __sync();
        __icbi((void *)0, (int)(u32)aligned);
        aligned = (void *)((u32)aligned + 8);
        size -= 8;
    } while ((s32)size >= 0);
    __isync();
}
