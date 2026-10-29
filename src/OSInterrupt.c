#include "common.h"


typedef s16 __OSInterrupt;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, void *context);
static __OSInterruptHandler *InterruptHandlerTable;
#pragma peephole off
__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler)
{
	__OSInterruptHandler oldHandler;

	oldHandler = InterruptHandlerTable[interrupt];
	InterruptHandlerTable[interrupt] = handler;
	return oldHandler;
}

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
	*(u32 *)((u32)0x00C4 + 0x80000000) = 0;
	*(u32 *)((u32)0x00C8 + 0x80000000) = 0;
	__PIRegs[1] = 0xF0;
	__OSMaskInterrupts(0xFFFFFFE0);
	__OSSetExceptionHandler(4, ExternalInterruptHandler);
}

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
u32 SetInterruptMask(u32 mask, u32 current);
u32 __OSMaskInterrupts(u32 global)
{
	u32 mask;
	u32 local;
	u32 prev;
	BOOL enabled;

	enabled = OSDisableInterrupts();
	prev = *(u32 *)((u32)0x00C4 + 0x80000000);
	local = *(u32 *)((u32)0x00C8 + 0x80000000);
	mask = ~(prev | local) & global;
	global |= prev;
	*(u32 *)((u32)0x00C4 + 0x80000000) = global;
	global |= local;
	goto check;
	do {
		mask = SetInterruptMask(mask, global);
check:;
	} while (mask);
	OSRestoreInterrupts(enabled);
	return prev;
}

#pragma peephole off
u32 __OSUnmaskInterrupts(u32 global)
{
	u32 prev;
	u32 local;
	u32 mask;
	BOOL enabled;

	enabled = OSDisableInterrupts();
	prev = *(u32 *)((u32)0x00C4 + 0x80000000);
	local = *(u32 *)((u32)0x00C8 + 0x80000000);
	mask = (prev | local) & global;
	global = prev & ~global;
	*(u32 *)((u32)0x00C4 + 0x80000000) = global;
	global |= local;
	while (mask) {
		mask = SetInterruptMask(mask, global);
	}
	OSRestoreInterrupts(enabled);
	return prev;
}
#pragma peephole reset
