#include "common.h"

extern s32 PPCMfhid0(void);
extern void PPCMthid0(s32);
void PPCHalt(void) {
    M2C_SYNC();
loop_1:
    goto loop_1;
}

void PPCDisableSpeculation(void) {
    s32 PPCMfhid0();
    M2C_UNK PPCMthid0(s32);
    PPCMthid0(PPCMfhid0() | 0x200);
}

void PPCSetFpNonIEEEMode(void) {
    M2C_ERROR(/* unknown instruction: mtfsb1 cr7gt */);
}

