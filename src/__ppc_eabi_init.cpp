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
    void *         aligned;
    u32            addr;

    addr = (u32)address;
    aligned = (void *)(addr & 0xfffffff1);
    size += addr - (u32)aligned;

    do
    {
        __dcbst(aligned, 0);
        __sync();
        __icbi(aligned, 0);

        aligned = (void *)((u32)aligned + 8);
        size -= 8;
    } while ((s32)size >= 0);
    __isync();
}
extern "C" void __init_hardware(void)
{
    __OSPSInit();
    __OSCacheInit();
}

extern "C" void __init_user(void)
{
    __init_cpp();
}

#pragma push
#pragma peephole off
void __init_cpp(void) {
    void (**constructor)(void);

    for (constructor = _ctors; *constructor; constructor++) {
        (*constructor)();
    }
}
#pragma pop
extern "C" void _ExitProcess(void)
{
    PPCHalt();
}
