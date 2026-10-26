#include "common.h"

void OSClearContext(void *);
void OSSetCurrentContext(void *);

static u32 RecalibrateBits;
static u32 WaitingBits;
static u32 CheckingBits;
static u32 PendingBits;
void (*SamplingCallback)(void);
s32 __PADSpec;

u32 ResettingChan = 0x20;
u32 XPatchBits = 0xF0000000;
u32 AnalogMode = 0x300;
s32 Spec = 5;
void *MakeStatus = NULL;
f32 CmdReadOrigin = 8.0f;
f32 CmdCalibrate = 32.0f;

void SPEC0_MakeStatus(s32, void *, u32 *);
void SPEC1_MakeStatus(s32, void *, u32 *);
void SPEC2_MakeStatus_80076F6C(s32, void *, u32 *);

void PADSetSpec(s32 spec)
{
    __PADSpec = 0;
    switch (spec) {
    case 0:
        MakeStatus = SPEC0_MakeStatus;
        break;
    case 1:
        MakeStatus = SPEC1_MakeStatus;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        MakeStatus = SPEC2_MakeStatus_80076F6C;
        break;
    }
    Spec = spec;
}

static void SamplingHandler(s16 interrupt, void *context)
{
    u32 exceptionContext[0x2C8 / 4];

    if (SamplingCallback) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        SamplingCallback();
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

