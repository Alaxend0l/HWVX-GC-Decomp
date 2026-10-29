#include "common.h"


u8 ErrorCode2Num(u32);
u8 *__OSLockSramEx(void);
BOOL __OSUnlockSramEx(BOOL);
void __DVDStoreErrorCode(u32 error) {
    u8 *sram;
    u32 num;
    u8 code;

    if (error == 0x01234567) {
        num = 0xFF;
    } else if (error == 0x01234568) {
        num = 0xFE;
    } else {
        num = error >> 24;
        code = ErrorCode2Num(error & 0xFFFFFF);
        if (num >= 6) {
            num = 6;
        }
        num = code + num * 30;
    }
    sram = __OSLockSramEx();
    sram[0x24] = num;
    __OSUnlockSramEx(TRUE);
}
