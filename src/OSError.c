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

extern volatile s16 __OSLastInterrupt;
extern volatile u32 __OSLastInterruptSrr0;
extern volatile s64 __OSLastInterruptTime;
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

void __OSUnhandledException(u8 exception, OSContext *context, u32 dsisr, u32 dar) {
    void OSDisableScheduler(void);
    void OSEnableScheduler(void);
    void __OSReschedule(void);
    void OSLoadContext(OSContext *context);
    void OSDumpContext(OSContext *context);
    s64 OSGetTime(void);

    if (!(context->srr1 & 2)) {
        OSReport("Non-recoverable Exception %d", exception);
    } else {
        if (__OSErrorTable[exception]) {
            OSDisableScheduler();
            __OSErrorTable[exception](exception, context, dsisr, dar);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }
        if (exception == 8) {
            OSLoadContext(context);
        }
        OSReport("Unhandled Exception %d", exception);
    }
    OSReport("\n");
    OSDumpContext(context);
    OSReport("\nDSISR = 0x%08x                   DAR  = 0x%08x\n", dsisr, dar);
    OSReport("TB = 0x%016llx\n", OSGetTime());
    switch (exception) {
    case 2:
        OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access invalid address 0x%x (read from DAR)\n", context->srr0, dar);
        break;
    case 3:
        OSReport("\nAttempted to fetch instruction from invalid address 0x%x (read from SRR0)\n", context->srr0);
        break;
    case 5:
        OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access unaligned address 0x%x (read from DAR)\n", context->srr0, dar);
        break;
    case 6:
        OSReport("\nProgram exception : Possible illegal instruction/operation at or around 0x%x (read from SRR0)\n", context->srr0, dar);
        break;
    case 15:
        OSReport("\n");
        OSReport("AI DMA Address =   0x%04x%04x\n", __DSPRegs[0x18], __DSPRegs[0x19]);
        OSReport("ARAM DMA Address = 0x%04x%04x\n", __DSPRegs[0x10], __DSPRegs[0x11]);
        OSReport("DI DMA Address =   0x%08x\n", __DIRegs[5]);
        break;
    }
    OSReport("\nLast interrupt (%d): SRR0 = 0x%08x  TB = 0x%016llx\n", __OSLastInterrupt, __OSLastInterruptSrr0, __OSLastInterruptTime);
    PPCHalt();
}

