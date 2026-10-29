#include "common.h"

typedef struct OSContext OSContext;
typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext *context, ...);

extern OSErrorHandler __OSErrorTable[16];

#pragma peephole on
OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler)
{
    OSErrorHandler *slot;
    OSErrorHandler oldHandler;

    slot = __OSErrorTable + error;
    oldHandler = *slot;
    *slot = handler;
    return oldHandler;
}
