#include "common.h"

typedef struct OSContext OSContext;
typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext *context, ...);

extern OSErrorHandler __OSErrorTable[16];

typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];

int vprintf(const char *format, va_list arg);

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
