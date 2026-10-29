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
    u8 *steering = (u8 *)&__SISteering[chan];
    void *context;
    void (*callback)(s32);
    void (*cb)(s32, s32);
    u8 exceptionContext[0x2C8];

    if (__SIResetSteering != 0) {
        return;
    }
    if ((type & 0xFFFF0000) == 0x08000000) {
        if (SITransfer(chan, steering, *(u32 *)(steering + 0xC), steering + 3, *(u32 *)(steering + 0x10),
                       __SISteeringHandler, 0) != 0) {
            return;
        }
        *(s32 *)(steering + 0x18) = -2;
    } else {
        *(s32 *)(steering + 0x18) = -1;
    }
    callback = *(void (**)(s32))(steering + 0x24);
    if (callback != NULL) {
        *(void (**)(s32))(steering + 0x24) = NULL;
        callback(chan);
    }
    if (*(void **)(steering + 0x14) != NULL) {
        context = OSGetCurrentContext();
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        cb = *(void (**)(s32, s32))(steering + 0x14);
        *(void **)(steering + 0x14) = NULL;
        cb(chan, *(s32 *)(steering + 0x18));
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
        __OSReschedule();
    }
}
