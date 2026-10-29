#include "common.h"

typedef struct DSPRegs {
    volatile u16 regs[32];
} DSPRegs;

u8 DSPInitCode[128] = {
    0x02, 0x9F, 0x00, 0x10, 0x02, 0x9F, 0x00, 0x33, 0x02, 0x9F, 0x00, 0x34, 0x02, 0x9F, 0x00, 0x35,
    0x02, 0x9F, 0x00, 0x36, 0x02, 0x9F, 0x00, 0x37, 0x02, 0x9F, 0x00, 0x38, 0x02, 0x9F, 0x00, 0x39,
    0x12, 0x06, 0x12, 0x03, 0x12, 0x04, 0x12, 0x05, 0x00, 0x80, 0x80, 0x00, 0x00, 0x88, 0xFF, 0xFF,
    0x00, 0x84, 0x10, 0x00, 0x00, 0x64, 0x00, 0x1D, 0x02, 0x18, 0x00, 0x00, 0x81, 0x00, 0x1C, 0x1E,
    0x00, 0x44, 0x1B, 0x1E, 0x00, 0x84, 0x08, 0x00, 0x00, 0x64, 0x00, 0x27, 0x19, 0x1E, 0x00, 0x00,
    0x00, 0xDE, 0xFF, 0xFC, 0x02, 0xA0, 0x80, 0x00, 0x02, 0x9C, 0x00, 0x28, 0x16, 0xFC, 0x00, 0x54,
    0x16, 0xFD, 0x43, 0x48, 0x00, 0x21, 0x02, 0xFF, 0x02, 0xFF, 0x02, 0xFF, 0x02, 0xFF, 0x02, 0xFF,
    0x02, 0xFF, 0x02, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void __OSInitAudioSystem(void);

#pragma push
#pragma set use_lmw_stmw on
volatile u16 __DSPRegs[32] : 0xCC005000;
void __OSInitAudioSystem(void)
{
    extern u8 DSPInitCode[];
    extern void memcpy();
    extern void DCFlushRange();
    extern u32 OSGetArenaHi(void);
    extern u32 OSGetTick(void);
    u8 errFlag;
    u16 reg16;
    u32 start_tick;

    memcpy((void *)(OSGetArenaHi() - 0x80), (void *)0x81000000, 0x80);
    memcpy((void *)0x81000000, DSPInitCode, 0x80);
    DCFlushRange((void *)0x81000000, 0x80);

    __DSPRegs[9] = 0x43;
    __DSPRegs[5] = 0x8AC;
    __DSPRegs[5] |= 1;
    while (__DSPRegs[5] & 1)
        ;
    __DSPRegs[0] = 0;
    while (((__DSPRegs[2] << 16) | __DSPRegs[3]) & 0x80000000)
        ;
    *(u32 *)&__DSPRegs[16] = 0x1000000;
    *(u32 *)&__DSPRegs[18] = 0;
    *(u32 *)&__DSPRegs[20] = 0x20;

    reg16 = __DSPRegs[5];
    while (!(reg16 & 0x20))
        reg16 = __DSPRegs[5];
    __DSPRegs[5] = reg16;

    start_tick = OSGetTick();
    while ((s32)(OSGetTick() - start_tick) < 0x892)
        ;

    *(u32 *)&__DSPRegs[16] = 0x1000000;
    *(u32 *)&__DSPRegs[18] = 0;
    *(u32 *)&__DSPRegs[20] = 0x20;

    reg16 = __DSPRegs[5];
    while (!(reg16 & 0x20))
        reg16 = __DSPRegs[5];
    __DSPRegs[5] = reg16;

    __DSPRegs[5] &= ~0x800;
    while (__DSPRegs[5] & 0x400)
        ;
    __DSPRegs[5] &= ~4;

    errFlag = 0;
    reg16 = __DSPRegs[2];
    while (!(reg16 & 0x8000))
        reg16 = __DSPRegs[2];
    (void)__DSPRegs[3];
    reg16 != 42069;

    __DSPRegs[5] |= 4;
    __DSPRegs[5] = 0x8AC;
    __DSPRegs[5] |= 1;
    while (__DSPRegs[5] & 1)
        ;
    memcpy((void *)0x81000000, (void *)(OSGetArenaHi() - 0x80), 0x80);
}

#pragma use_lmw_stmw on
void __OSStopAudioSystem(void)
{
    extern u32 OSGetTick(void);
    u32 reg32;
    u16 reg16;
    u32 start_tick;

    __DSPRegs[5] = 0x804;
    reg16 = __DSPRegs[27];
    __DSPRegs[27] = reg16 & ~0x8000;
    reg16 = __DSPRegs[5];
    while (reg16 & 0x400)
        reg16 = __DSPRegs[5];
    reg16 = __DSPRegs[5];
    while (reg16 & 0x200)
        reg16 = __DSPRegs[5];
    __DSPRegs[5] = 0x8AC;
    __DSPRegs[0] = 0;
    while (((__DSPRegs[2] << 16) | __DSPRegs[3]) & 0x80000000)
        ;
    start_tick = OSGetTick();
    while ((s32)(OSGetTick() - start_tick) < 0x2C)
        ;
    reg32 = __DSPRegs[5];
    __DSPRegs[5] = reg32 | 1;
    reg16 = __DSPRegs[5];
    while (reg16 & 1)
        reg16 = __DSPRegs[5];
}
