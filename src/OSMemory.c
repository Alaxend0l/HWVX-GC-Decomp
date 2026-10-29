#include "common.h"


typedef struct OSContext OSContext;
typedef s16 __OSInterrupt;
typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext *context, ...);

extern OSErrorHandler __OSErrorTable[16];
void __OSUnhandledException(u8 exception, OSContext *context, u32 dsisr, u32 dar);

u32 __OSMaskInterrupts(u32 mask);
volatile u16 __MEMRegs[64] : 0xCC004000;
static s32 OnReset(s32 final)
{
    if (final != 0) {
        __MEMRegs[8] = 0xFF;
        __OSMaskInterrupts(0xF0000000);
    }
    return 1;
}
