#include "common.h"

typedef void (*SISteeringCallback)(s32 chan, s32 ret);
typedef void (*SISteeringResetCallback)(s32 chan);

typedef struct SISteeringControl {
    u8 output[3];
    u8 input[9];
    u32 outputBytes;
    u32 inputBytes;
    SISteeringCallback callback;
    s32 ret;
    u32 unk1C;
    u32 unk20;
    SISteeringResetCallback resetCallback;
} SISteeringControl;

SISteeringControl __SISteering[4];
s32 __SIResetSteering;

void __SISteeringEnable(s32 chan);
s32 __SISteeringTransfer(s32 chan, u32 outputBytes, u32 inputBytes, void *proc);
s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);

void DefaultCallback(void) {

}

static void ResetProc(s32 chan) {
    SISteeringControl *steering = &__SISteering[chan];

    if (steering->ret == 0 && __SIResetSteering == 0) {
        __SISteeringEnable(chan);
    }
}

