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

s32 OSGetCurrentContext(void)
{
    return *(s32 *)0x800000d4;
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

