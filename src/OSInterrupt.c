#include "common.h"


typedef s16 __OSInterrupt;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, void *context);
__OSInterruptHandler *InterruptHandlerTable;
__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt)
{
	return InterruptHandlerTable[interrupt];
}
