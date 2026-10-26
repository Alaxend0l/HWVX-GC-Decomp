#include "common.h"

extern s32 PPCMfhid0(void);
extern void PPCMthid0(s32);
void PPCDisableSpeculation(void) {
    s32 PPCMfhid0();
    M2C_UNK PPCMthid0(s32);
    PPCMthid0(PPCMfhid0() | 0x200);
}
