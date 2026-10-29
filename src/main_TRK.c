#include "common.h"

s32 TRKInitializeNub(void);
void TRKNubWelcome(void);
void TRKNubMainLoop(void);
s32 TRKTerminateNub(void);

__declspec(section ".data") static s32 TRK_mainError;
