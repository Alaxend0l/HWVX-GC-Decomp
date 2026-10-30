#include "common.h"

typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;
typedef u16 OSError;

volatile u16 __DSPRegs[] : 0xCC005000;
volatile u32 __DIRegs[] : 0xCC006000;

extern s16 __OSLastInterrupt;
extern u32 __OSLastInterruptSrr0;
extern s64 __OSLastInterruptTime;
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
void OSReport(const char *msg, ...)
{
    va_list marker;

    __builtin_va_info(&marker);
    vprintf(msg, marker);
}

void OSReport(const char *msg, ...);
u32 *OSGetStackPointer(void);
BOOL OSDisableInterrupts(void);
void PPCHalt(void);
void OSPanic(const char *file, int line, const char *msg, ...)
{
    va_list marker;
    u32 i;
    u32 *p;

    OSDisableInterrupts();
    __builtin_va_info(&marker);
    vprintf(msg, marker);
    OSReport(" in \"%s\" on line %d.\n", file, line);

    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = OSGetStackPointer(); p && (u32)p != 0xFFFFFFFF && i++ < 16; p = (u32 *)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }

    PPCHalt();
}

OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler)
{
    OSErrorHandler *slot;
    OSErrorHandler oldHandler;

    slot = __OSErrorTable + error;
    oldHandler = *slot;
    *slot = handler;
    return oldHandler;
}
