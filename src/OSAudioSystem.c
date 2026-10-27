#include "common.h"

typedef struct DSPRegs {
    volatile u16 regs[32];
} DSPRegs;

void __OSInitAudioSystem(void);

#pragma push
#pragma set use_lmw_stmw on
void __OSInitAudioSystem(void)
{
    extern u8 DSPInitCode[];
    extern void memcpy();
    extern void DCFlushRange();
    extern u32 OSGetArenaHi(void);
    extern u32 OSGetTick(void);
    volatile u16 *dsp;
    volatile u16 *status;
    volatile u32 *regA;
    volatile u32 *regB;
    volatile u32 *regC;
    u32 start;
    u16 temp;
    u32 elapsed;

    memcpy((void *)(OSGetArenaHi() - 0x80), (void *)0x81000000, 0x80);
    memcpy((void *)0x81000000, DSPInitCode, 0x80);
    DCFlushRange((void *)0x81000000, 0x80);

    dsp = (volatile u16 *)0xCC005000;
    dsp[9] = 0x43;
    dsp[5] = 0x8AC;
    dsp[5] |= 1;
    while (dsp[5] & 1)
    {
    }
    dsp[0] = 0;
    while ((((u32)dsp[2] << 16) | dsp[3]) & 0x80000000)
    {
    }
    *((volatile u32 *)0xCC005020) = 0x0100;
    *((volatile u32 *)0xCC005024) = 0;
    *((volatile u32 *)0xCC005028) = 0x20;
    temp = dsp[5];
    while ((temp & 0x20) == 0)
    {
        temp = dsp[5];
    }
    dsp[5] = temp;
    status = dsp + 5;
    start = OSGetTick();
    while ((s32)(OSGetTick() - start) < 0x892)
    {
    }
    *((volatile u32 *)0xCC005020) = 0x0100;
    *((volatile u32 *)0xCC005024) = 0;
    *((volatile u32 *)0xCC005028) = 0x20;
    temp = dsp[5];
    while ((temp & 0x20) == 0)
    {
        temp = dsp[5];
    }
    dsp[5] = temp;
    dsp[5] &= ~0x800;
    while (dsp[5] & 0x800)
    {
    }
    dsp[5] &= ~0x4;
    while (dsp[2] & 0x8000)
    {
    }
    while (dsp[3] & 0x8000)
    {
    }
    dsp[5] |= 4;
    dsp[5] = 0x8AC;
    dsp[5] |= 1;
    while ((dsp[5] & 1) == 0)
    {
    }
    memcpy((void *)0x81000000, (void *)(OSGetArenaHi() - 0x80), 0x80);
}
#pragma pop
