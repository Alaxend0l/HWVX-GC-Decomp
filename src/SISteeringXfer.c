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

extern SISteeringControl __SISteering[4];
extern s32 __SIResetSteering;

void __SISteeringHandler(s32 chan, u32 status, void *context);
s32 SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback, s64 delay);
void *OSGetCurrentContext(void);
void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
void __OSReschedule(void);

static void TypeAndStatusCallback(s32 chan, u32 type) {
    SISteeringControl *steering = &__SISteering[chan];
    SISteeringResetCallback resetCallback;
    SISteeringCallback callback;
    u8 exceptionContext[0x2C8];
    void *context;

    if (__SIResetSteering != 0) {
        return;
    }
    if ((type & 0xFFFF0000) == 0x08000000) {
        if (SITransfer(chan, steering, steering->outputBytes, steering->input, steering->inputBytes,
                       __SISteeringHandler, 0) != 0) {
            return;
        }
        steering->ret = -2;
    } else {
        steering->ret = -1;
    }
    if (steering->resetCallback != NULL) {
        resetCallback = steering->resetCallback;
        steering->resetCallback = NULL;
        resetCallback(chan);
    }
    if (steering->callback != NULL) {
        context = OSGetCurrentContext();
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        callback = steering->callback;
        steering->callback = NULL;
        callback(chan, steering->ret);
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
        __OSReschedule();
    }
}
