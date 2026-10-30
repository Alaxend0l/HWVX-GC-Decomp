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

OSContext *__OSCurrentContext : 0x800000D4;
OSContext *__OSFPUContext : 0x800000D8;

OSContext *OSGetCurrentContext(void)
{
    return __OSCurrentContext;
}

#pragma peephole off
void OSClearContext(OSContext *context)
{
    context->mode = 0;
    context->state = 0;
    if (context == *(OSContext **)0x800000D8) {
        *(OSContext **)0x800000D8 = NULL;
    }
}

void OSReport(const char *fmt, ...);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
void OSSetCurrentContext(OSContext *context);
#pragma push
#pragma use_lmw_stmw on
#pragma peephole off
void OSDumpContext(OSContext *context)
{
    u32 i;
    u32 *p;

    OSReport("------------------------- Context 0x%08x -------------------------\n", context);
    for (i = 0; i < 16; ++i) {
        OSReport("r%-2d  = 0x%08x (%14d)  r%-2d  = 0x%08x (%14d)\n", i, context->gpr[i], context->gpr[i], i + 16, context->gpr[i + 16], context->gpr[i + 16]);
    }
    OSReport("LR   = 0x%08x                   CR   = 0x%08x\n", context->lr, context->cr);
    OSReport("SRR0 = 0x%08x                   SRR1 = 0x%08x\n", context->srr0, context->srr1);
    OSReport("\nGQRs----------\n");
    for (i = 0; i < 4; ++i) {
        OSReport("gqr%d = 0x%08x \t gqr%d = 0x%08x\n", i, context->gqr[i], i + 4, context->gqr[i + 4]);
    }
    if (context->state & 1u) {
        OSContext *currentContext;
        OSContext fpuContext;
        BOOL enabled;

        enabled = OSDisableInterrupts();
        currentContext = OSGetCurrentContext();
        OSClearContext(&fpuContext);
        OSSetCurrentContext(&fpuContext);
        OSReport("\n\nFPRs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("fr%d \t= %d \t fr%d \t= %d\n", i, (u32)context->fpr[i], i + 1, (u32)context->fpr[i + 1]);
        }
        OSReport("\n\nPSFs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("ps%d \t= 0x%x \t ps%d \t= 0x%x\n", i, (u32)context->psf[i], i + 1, (u32)context->psf[i + 1]);
        }
        OSClearContext(&fpuContext);
        OSSetCurrentContext(currentContext);
        OSRestoreInterrupts(enabled);
    }
    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = (u32 *)context->gpr[1]; p && (u32)p != 0xffffffff && i++ < 16; p = (u32 *)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }
}
#pragma pop

#pragma peephole on
void OSSwitchFPUContext(void);
void *__OSSetExceptionHandler(u8 exception, void *handler);
void DBPrintf(const char *fmt, ...);
#pragma peephole off
void __OSContextInit(void)
{
    __OSSetExceptionHandler(7, OSSwitchFPUContext);
    *(OSContext **)0x800000D8 = NULL;
    DBPrintf("FPU-unavailable handler installed\n");
}
