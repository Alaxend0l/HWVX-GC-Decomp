#include "common.h"


typedef s16 __OSInterrupt;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, void *context);
static __OSInterruptHandler *InterruptHandlerTable;
__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt)
{
	return InterruptHandlerTable[interrupt];
}

void *memset(void *dst, int c, unsigned long n);
u32 __OSMaskInterrupts(u32 mask);
void __OSSetExceptionHandler(u8 exception, void *handler);
void ExternalInterruptHandler(void);
volatile u32 __PIRegs[12] : 0xCC003000;
void __OSInterruptInit(void)
{
	InterruptHandlerTable = (__OSInterruptHandler *)((u32)0x3040 + 0x80000000);
	memset(InterruptHandlerTable, 0, 32 * sizeof(__OSInterruptHandler));
	*(volatile u32 *)((u32)0x00C4 + 0x80000000) = 0;
	*(volatile u32 *)((u32)0x00C8 + 0x80000000) = 0;
	__PIRegs[1] = 0xF0;
	__OSMaskInterrupts(0xFFFFFFE0);
	__OSSetExceptionHandler(4, ExternalInterruptHandler);
}

