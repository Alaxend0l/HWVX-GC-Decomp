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
