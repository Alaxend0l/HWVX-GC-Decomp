#include "common.h"

s32 TRKInitializeNub(void);
void TRKNubWelcome(void);
void TRKNubMainLoop(void);
s32 TRKTerminateNub(void);

__declspec(section ".data") static s32 TRK_mainError;

void TRK_main(void) {
    TRK_mainError = TRKInitializeNub();
    if (TRK_mainError == 0) {
        TRKNubWelcome();
        TRKNubMainLoop();
    }
    TRK_mainError = TRKTerminateNub();
}
