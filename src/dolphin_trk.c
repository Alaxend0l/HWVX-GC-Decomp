#include "common.h"

u32 TRK_ISR_OFFSETS[15] = {
    0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700,
    0x800, 0x900, 0xC00, 0xD00, 0xF00, 0x1300, 0x1400, 0x1700
};

static u32 lc_base;
static u32 gap_08_8030B9CC;

extern u8 gTRKCPUState[];
extern u8 gTRKInterruptVectorTable[];

void *TRK_memcpy(void *dst, const void *src, u32 n);
void TRK_flush_cache(u32 start, u32 length);

void __TRK_reset(void)
{
    TRK_memcpy((void *)0, (const void *)0, 0);
}
