#include "common.h"

typedef void (*EXICallback)(s32 chan, void *context);

typedef struct EXIControl {
    EXICallback exiCallback;
    EXICallback tcCallback;
    EXICallback extCallback;
    volatile u32 state;
    s32 immLen;
    u8 *immBuf;
    u32 dev;
    u32 id;
    s32 idTime;
    s32 items;
    struct {
        u32 dev;
        EXICallback callback;
    } queue[3];
} EXIControl;

static EXIControl Ecb[3];

volatile u32 __EXIRegs[16] : 0xCC006800;

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
s64 OSGetTime(void);

u32 EXIGetState(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    return exi->state;
}
