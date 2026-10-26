#include "common.h"

void OSClearContext(void *);
void OSSetCurrentContext(void *);

void (*SamplingCallback)(void);
s32 __PADSpec;
s32 Spec;
void *MakeStatus;

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
