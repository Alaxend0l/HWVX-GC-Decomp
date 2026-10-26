#include "common.h"

extern u32 lc_base;
extern u8 gTRKCPUState[];
extern u32 TRK_ISR_OFFSETS[];
extern u8 gTRKInterruptVectorTable[];

void *TRK_memcpy(void *dst, const void *src, u32 n);
void TRK_flush_cache(u32 start, u32 length);
