#include "common.h"

u32 TRK_ISR_OFFSETS[15] = {
    0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700,
    0x800, 0x900, 0xC00, 0xD00, 0xF00, 0x1300, 0x1400, 0x1700
};

extern u32 lc_base[];

extern u8 gTRKCPUState[];
extern u8 gTRKState[];
s32 __TRK_get_MSR(void);
extern u8 gTRKInterruptVectorTable[];

void *TRK_memcpy(void *dst, const void *src, u32 n);
void TRK_flush_cache(u32 start, u32 length);

static inline void TRK_copy_vector(u32 offset)
{
    u32 dest;

    if ((offset >= lc_base[0]) && (offset < (lc_base[0] + 0x4000)) && (*(u32 *)(gTRKCPUState + 0x238) & 3))
    {
        dest = offset;
    }
    else
    {
        dest = (offset & 0x3FFFFFFF) | 0x80000000;
    }
    TRK_memcpy((void *)dest, gTRKInterruptVectorTable + offset, 0x100);
    TRK_flush_cache(dest, 0x100);
}

#pragma push
#pragma use_lmw_stmw on
__declspec(section ".init") void __TRK_reset(void)
{
    u32 *src;
    u32 mask;
    s32 i;

    if ((lc_base[0] <= 0x44) && ((lc_base[0] + 0x4000) > 0x44) && (*(u32 *)(gTRKCPUState + 0x238) & 3))
    {
        src = (u32 *)0x44;
    }
    else
    {
        src = (u32 *)0x80000044;
    }
    mask = *src;
    for (i = 0; i <= 0xE; i++)
    {
        if (mask & (1 << i))
        {
            u32 offset = TRK_ISR_OFFSETS[i];
            u32 dest;

            if ((offset >= lc_base[0]) && (offset < (lc_base[0] + 0x4000)) && (*(u32 *)(gTRKCPUState + 0x238) & 3))
            {
                dest = offset;
            }
            else
            {
                dest = (offset & 0x3FFFFFFF) | 0x80000000;
            }
            TRK_memcpy((void *)dest, gTRKInterruptVectorTable + offset, 0x100);
            TRK_flush_cache(dest, 0x100);
        }
    }
}

s32 TRKInitializeTarget(void)
{
    *(s32 *)(gTRKState + 0x98) = 1;
    *(s32 *)(gTRKState + 0x8C) = __TRK_get_MSR();
    lc_base[0] = 0xE0000000;
    return 0;
}

u32 TRKTargetTranslate(u32 addr)
{
    u32 base;

    base = lc_base[0];
    if ((addr < base) || (addr >= (base + 0x4000)) || !(*(u32 *)(gTRKCPUState + 0x238) & 3))
    {
        return (addr & 0x3FFFFFFF) | 0x80000000;
    }
    return addr;
}

void EnableMetroTRKInterrupts(void)
{
    EnableEXI2Interrupts();
}
