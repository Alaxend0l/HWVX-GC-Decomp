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
volatile u16 __MEMRegs[64] : 0xCC004000;
volatile u16 __DSPRegs[32] : 0xCC005000;
volatile u32 __AIRegs[8] : 0xCC006C00;
volatile u32 __EXIRegs[16] : 0xCC006800;
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

u32 SetInterruptMask(u32 mask, u32 current) {
    u32 reg;

    switch (__cntlzw(mask)) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        reg = 0;
        if (!(current & 0x80000000)) reg |= 0x1;
        if (!(current & 0x40000000)) reg |= 0x2;
        if (!(current & 0x20000000)) reg |= 0x4;
        if (!(current & 0x10000000)) reg |= 0x8;
        if (!(current & 0x08000000)) reg |= 0x10;
        __MEMRegs[0xE] = (u16)reg;
        mask &= ~0xF8000000;
        break;
    case 5:
    case 6:
    case 7:
        reg = __DSPRegs[5];
        reg &= ~0x1F8;
        if (!(current & 0x04000000)) reg |= 0x10;
        if (!(current & 0x02000000)) reg |= 0x40;
        if (!(current & 0x01000000)) reg |= 0x100;
        __DSPRegs[5] = (u16)reg;
        mask &= ~0x07000000;
        break;
    case 8:
        reg = __AIRegs[0];
        reg &= ~0x2C;
        if (!(current & 0x00800000)) reg |= 0x4;
        __AIRegs[0] = reg;
        mask &= ~0x00800000;
        break;
    case 9:
    case 10:
    case 11:
        reg = __EXIRegs[0];
        reg &= ~0x2C0F;
        if (!(current & 0x00400000)) reg |= 0x1;
        if (!(current & 0x00200000)) reg |= 0x4;
        if (!(current & 0x00100000)) reg |= 0x400;
        __EXIRegs[0] = reg;
        mask &= ~0x00700000;
        break;
    case 12:
    case 13:
    case 14:
        reg = __EXIRegs[5];
        reg &= ~0xC0F;
        if (!(current & 0x00080000)) reg |= 0x1;
        if (!(current & 0x00040000)) reg |= 0x4;
        if (!(current & 0x00020000)) reg |= 0x400;
        __EXIRegs[5] = reg;
        mask &= ~0x000E0000;
        break;
    case 15:
    case 16:
        reg = __EXIRegs[10];
        reg &= ~0xF;
        if (!(current & 0x00010000)) reg |= 0x1;
        if (!(current & 0x00008000)) reg |= 0x4;
        __EXIRegs[10] = reg;
        mask &= ~0x00018000;
        break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
        reg = 0xF0;
        if (!(current & 0x00004000)) reg |= 0x800;
        if (!(current & 0x00000800)) reg |= 0x8;
        if (!(current & 0x00000400)) reg |= 0x4;
        if (!(current & 0x00000200)) reg |= 0x2;
        if (!(current & 0x00000100)) reg |= 0x1;
        if (!(current & 0x00000080)) reg |= 0x100;
        if (!(current & 0x00000040)) reg |= 0x1000;
        if (!(current & 0x00002000)) reg |= 0x200;
        if (!(current & 0x00001000)) reg |= 0x400;
        if (!(current & 0x00000020)) reg |= 0x2000;
        __PIRegs[1] = reg;
        mask &= ~0x00007FE0;
        break;
    default:
        break;
    }
    return mask;
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

#pragma peephole off
extern u32 InterruptPrioTable[];
extern __OSInterrupt __OSLastInterrupt;
extern s64 __OSLastInterruptTime;
extern u32 __OSLastInterruptSrr0;
void OSLoadContext(void *context);
s64 OSGetTime(void);
s32 OSDisableScheduler(void);
s32 OSEnableScheduler(void);
void __OSReschedule(void);
#pragma peephole off
void __OSDispatchInterrupt(__OSInterrupt exception, void *context) {
    u32 intsr;
    u32 reg;
    u32 cause;
    u32 unmasked;
    u32 *prio;
    __OSInterrupt interrupt;
    __OSInterruptHandler handler;

    intsr = __PIRegs[0];
    intsr &= ~0x10000;
    if (intsr == 0 || (intsr & __PIRegs[1]) == 0) {
        OSLoadContext(context);
    }

    cause = 0;

    if (intsr & 0x80) {
        reg = __MEMRegs[15];
        if (reg & 0x1) cause |= 0x80000000;
        if (reg & 0x2) cause |= 0x40000000;
        if (reg & 0x4) cause |= 0x20000000;
        if (reg & 0x8) cause |= 0x10000000;
        if (reg & 0x10) cause |= 0x08000000;
    }
    if (intsr & 0x40) {
        reg = __DSPRegs[5];
        if (reg & 0x8) cause |= 0x04000000;
        if (reg & 0x20) cause |= 0x02000000;
        if (reg & 0x80) cause |= 0x01000000;
    }
    if (intsr & 0x20) {
        reg = __AIRegs[0];
        if (reg & 0x8) cause |= 0x00800000;
    }
    if (intsr & 0x10) {
        reg = __EXIRegs[0];
        if (reg & 0x2) cause |= 0x00400000;
        if (reg & 0x8) cause |= 0x00200000;
        if (reg & 0x800) cause |= 0x00100000;
        reg = __EXIRegs[5];
        if (reg & 0x2) cause |= 0x00080000;
        if (reg & 0x8) cause |= 0x00040000;
        if (reg & 0x800) cause |= 0x00020000;
        reg = __EXIRegs[10];
        if (reg & 0x2) cause |= 0x00010000;
        if (reg & 0x8) cause |= 0x00008000;
    }
    if (intsr & 0x2000) cause |= 0x20;
    if (intsr & 0x1000) cause |= 0x40;
    if (intsr & 0x400) cause |= 0x1000;
    if (intsr & 0x200) cause |= 0x2000;
    if (intsr & 0x100) cause |= 0x80;
    if (intsr & 0x8) cause |= 0x800;
    if (intsr & 0x4) cause |= 0x400;
    if (intsr & 0x2) cause |= 0x200;
    if (intsr & 0x800) cause |= 0x4000;
    if (intsr & 0x1) cause |= 0x100;

    unmasked = cause & ~(*(u32 *)0x800000C4 | *(u32 *)0x800000C8);
    if (unmasked) {
        for (prio = InterruptPrioTable;; ++prio) {
            if (unmasked & *prio) {
                interrupt = (__OSInterrupt)__cntlzw(unmasked & *prio);
                break;
            }
        }
        handler = __OSGetInterruptHandler(interrupt);
        if (handler) {
            if (4 < interrupt) {
                __OSLastInterrupt = interrupt;
                __OSLastInterruptTime = OSGetTime();
                __OSLastInterruptSrr0 = *(u32 *)((u8 *)context + 0x198);
            }
            OSDisableScheduler();
            handler(interrupt, context);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }
    }
    OSLoadContext(context);
}
