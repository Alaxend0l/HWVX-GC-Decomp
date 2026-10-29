#include "common.h"

void TRKTargetSetStopped(s32 stopped);
void UnreserveEXI2Port(void);
void TRKSwapAndGo(void);
void ReserveEXI2Port(void);

s32 TRKTargetContinue(void) {
    TRKTargetSetStopped(0);
    UnreserveEXI2Port();
    TRKSwapAndGo();
    ReserveEXI2Port();
    return 0;
}
