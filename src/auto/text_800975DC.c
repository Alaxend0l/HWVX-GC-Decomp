/* Functions from 0x800975DC to 0x80097824, found by decomp-toolkit. */
#include "common.h"
typedef struct ExceptionRecord {
    u16 flags;
} ExceptionRecord;

typedef struct MWExceptionInfo {
    ExceptionRecord *exception_record;
} MWExceptionInfo;

typedef struct FPRSave {
    f64 d;
    f32 ps[2];
} FPRSave;

typedef struct ThrowContext {
    FPRSave FPR[32];
    u32 GPR[32];
    u32 CR;
    char *SP;
} ThrowContext;

char *ExPPC_PopStackFrame__FP12ThrowContextP15MWExceptionInfo_800975DC(ThrowContext *context, MWExceptionInfo *info) {
    ExceptionRecord *exc = info->exception_record;
    char *callers_SP;
    char *PS_save_area;
    char *FPR_save_area;
    s32 saved_GPRs;
    s32 has_ps;
    s32 saved_FPRs;
    s32 i;
    s32 j;

    callers_SP = *(char **)context->SP;
    has_ps = (exc->flags >> 1) & 1;
    saved_FPRs = (exc->flags >> 6) & 0x1F;
    if (has_ps) {
        PS_save_area = callers_SP - saved_FPRs * 16;
        FPR_save_area = PS_save_area;
    } else {
        FPR_save_area = callers_SP - saved_FPRs * 8;
    }
    if (has_ps) {
        for (i = 32 - saved_FPRs, j = 0; i < 32; i++, j++) {
            context->FPR[i].ps[0] = ((FPRSave *)PS_save_area)[j].ps[0];
            context->FPR[i].ps[1] = ((FPRSave *)PS_save_area)[j].ps[1];
            context->FPR[i].d = ((FPRSave *)PS_save_area)[j].d;
        }
    } else {
        for (i = 32 - saved_FPRs, j = 0; i < 32; i++, j++) {
            context->FPR[i].d = ((f64 *)FPR_save_area)[j];
        }
    }
    saved_GPRs = info->exception_record->flags >> 11;
    for (i = 32 - saved_GPRs; i < 32; i++) {
        context->GPR[i] = ((u32 *)FPR_save_area)[i - 32];
    }
    context->SP = callers_SP;
    return *(char **)(callers_SP + 4);
}
