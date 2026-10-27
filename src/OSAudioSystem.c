#include "common.h"

typedef struct DSPRegs {
    volatile u16 regs[32];
} DSPRegs;

void __OSInitAudioSystem(void);

#pragma push
#pragma set use_lmw_stmw on
void __OSInitAudioSystem(void)
{
    extern volatile u16 __DSPRegs[32];
    extern u8 DSPInitCode[];
    extern void memcpy();
    extern void DCFlushRange();
    extern u32 OSGetArenaHi(void);
    extern u32 OSGetTick(void);
    volatile u16 *status;
    volatile u16 *lo;
    volatile u16 *hi;
    volatile u32 *regA;
    volatile u32 *regB;
    volatile u32 *regC;
    u32 start;
    u16 tmp;

    memcpy((void *)(OSGetArenaHi() - 0x80), (void *)0x81000000, 0x80);
    memcpy((void *)0x81000000, DSPInitCode, 0x80);
    DCFlushRange((void *)0x81000000, 0x80);

    __DSPRegs[9] = 0x43;
    status = __DSPRegs + 5;
    __DSPRegs[5] = 0x8AC;
    __DSPRegs[5] |= 1;
    while (*status & 1)
    {
    }
    __DSPRegs[0] = 0;
    lo = __DSPRegs + 2;
    hi = __DSPRegs + 3;
    while ((((u32)*lo << 16) | *hi) & 0x80000000)
    {
    }
    regA = (volatile u32 *)(__DSPRegs + 0x10);
    regB = (volatile u32 *)(__DSPRegs + 0x12);
    regC = (volatile u32 *)(__DSPRegs + 0x14);
    *regA = 0x01000000;
    *regB = 0;
    *regC = 0x20;
    tmp = *status;
    while ((tmp & 0x20) == 0)
    {
        tmp = *status;
    }
    *status = tmp;
    start = OSGetTick();
    while ((s32)(OSGetTick() - start) < 0x892)
    {
    }
    *regA = 0x01000000;
    *regB = 0;
    *regC = 0x20;
    tmp = *status;
    while ((tmp & 0x20) == 0)
    {
        tmp = *status;
    }
    *status = tmp;
    *status &= ~0x800;
    while (*status & 0x400)
    {
    }
    *status &= ~0x4;
    tmp = *lo;
    while ((tmp & 0x8000) == 0)
    {
        tmp = *lo;
    }
    tmp = *hi;
    while ((tmp & 0x8000) == 0)
    {
        tmp = *hi;
    }
    *status |= 4;
    *status = 0x8AC;
    *status |= 1;
    while ((*status & 1) == 0)
    {
    }
    memcpy((void *)0x81000000, (void *)(OSGetArenaHi() - 0x80), 0x80);
}
#pragma pop
