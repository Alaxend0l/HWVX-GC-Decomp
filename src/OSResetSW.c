#include "common.h"

typedef void (*OSResetCallback)(void);

s64 __OSGetSystemTime(void);
u32 __OSMaskInterrupts(u32 mask);

volatile u32 __PIRegs[12] : 0xCC003000;

static OSResetCallback ResetCallback;
static BOOL Down;
static BOOL LastState;
static s64 HoldUp;
static s64 HoldDown;
