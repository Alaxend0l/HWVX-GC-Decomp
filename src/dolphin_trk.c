#include "common.h"

u32 TRK_ISR_OFFSETS[15] = {
    0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700,
    0x800, 0x900, 0xC00, 0xD00, 0xF00, 0x1300, 0x1400, 0x1700
};

u32 lc_base;
u32 gap_08_8030B9CC;

extern u8 gTRKCPUState[];
extern u8 gTRKInterruptVectorTable[];

void *TRK_memcpy(void *dst, const void *src, u32 n);
void TRK_flush_cache(u32 start, u32 length);

void __TRK_reset(void)
{
    u32 *src;
    u32 *offsets;
    u32 mask;
    u32 offset;
    u32 dest;
    s32 i;

    if ((lc_base <= 0x44) && ((lc_base + 0x4000) > 0x44) && (*(u32 *)(gTRKCPUState + 0x238) & 3))
    {
        src = (u32 *)0x44;
    }
    else
    {
        src = (u32 *)0x80000044;
    }
    mask = *src;
    offsets = TRK_ISR_OFFSETS;
    for (i = 0; i <= 0xE; i++)
    {
        if (mask & (1 << i))
        {
            offset = *offsets;
            offsets = offsets + 1;
            if ((offset >= lc_base) && (offset < (lc_base + 0x4000)) && (*(u32 *)(gTRKCPUState + 0x238) & 3))
            {
                dest = offset;
            }
            else
            {
                dest = (offset & ~3) | 0x8000;
            }
            TRK_memcpy((void *)dest, gTRKInterruptVectorTable + offset, 0x100);
            TRK_flush_cache(dest, 0x100);
        }
        offsets = offsets + 1;
    }
}
