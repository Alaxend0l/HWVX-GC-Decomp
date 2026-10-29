#include "common.h"

extern u8 *gTRKInputPendingPtr[];

s32 TRKGetNextEvent(TRKEvent *event);
void TRKDestructEvent(TRKEvent *event);
void *TRKGetBuffer(s32 idx);
s32 TRKDispatchMessage(void *buffer);
s32 TRKTargetInterrupt(TRKEvent *event);
s32 TRKTargetSupportRequest(void);
void TRKGetInput(void);
s32 TRKTargetStopped(void);
void TRKTargetContinue(void);
