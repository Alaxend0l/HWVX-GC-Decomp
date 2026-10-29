#include "common.h"

typedef struct SISteeringStatus {
    u16 button;
    u8 misc;
    s8 steering;
    u8 gas;
    u8 brake;
    u8 left;
    u8 right;
    s8 err;
} SISteeringStatus;

typedef struct SISteeringControl {
    u8 unk0[0x18];
    s32 ret;
    u8 unk1C[0x28 - 0x1C];
} SISteeringControl;

extern SISteeringControl __SISteering[4];
extern u32 __SISteeringEnableBits;

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
BOOL SIIsChanBusy(s32 chan);
u32 SIGetStatus(s32 chan);
BOOL SIGetResponse(s32 chan, void *data);
void SIDisablePolling(u32 poll);
