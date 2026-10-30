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

typedef struct OSResetFunctionInfo {
    s32 (*func)(s32 final);
    u32 priority;
    struct OSResetFunctionInfo *next;
    struct OSResetFunctionInfo *prev;
} OSResetFunctionInfo;

static OSResetFunctionInfo ResetFunctionInfo = {
    OnReset,
    127,
};

static void MEMIntrruptHandler(__OSInterrupt interrupt, OSContext *context)
{
    u32 addr;
    u32 cause;

    cause = __MEMRegs[0xF];
    addr = (((u32)__MEMRegs[0x12] & 0x3FF) << 16) | __MEMRegs[0x11];
    __MEMRegs[0x10] = 0;

    if (__OSErrorTable[15]) {
        __OSErrorTable[15](15, context, cause, addr);
        return;
    }
    __OSUnhandledException(15, context, cause, addr);
}

typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext *context);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
u32 __OSUnmaskInterrupts(u32 mask);
__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler);
void OSRegisterResetFunction(OSResetFunctionInfo *info);
void RealMode(void *config);
void Config24MB(void);
void Config48MB(void);
void __OSInitMemoryProtection(void)
{
    u32 padding[12];
    u32 simulatedSize;
    BOOL enabled;

    simulatedSize = *(u32 *)0x800000F0;
    enabled = OSDisableInterrupts();

    if (simulatedSize <= 0x1800000) {
        RealMode(Config24MB);
    } else if (simulatedSize <= 0x3000000) {
        RealMode(Config48MB);
    }

    __MEMRegs[16] = 0;
    __MEMRegs[8] = 0xFF;

    __OSMaskInterrupts(0xF0000000);

    __OSSetInterruptHandler(0, MEMIntrruptHandler);
    __OSSetInterruptHandler(1, MEMIntrruptHandler);
    __OSSetInterruptHandler(2, MEMIntrruptHandler);
    __OSSetInterruptHandler(3, MEMIntrruptHandler);
    __OSSetInterruptHandler(4, MEMIntrruptHandler);
    OSRegisterResetFunction(&ResetFunctionInfo);

    if (*(u32 *)0x800000F0 < *(u32 *)0x80000028 && *(u32 *)0x800000F0 - 0x1800000 == 0) {
        __MEMRegs[20] = 2;
    }

    __OSUnmaskInterrupts(0x08000000);
    OSRestoreInterrupts(enabled);
}
