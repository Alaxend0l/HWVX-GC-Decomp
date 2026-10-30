#include "common.h"


static u32 ErrorTable[] = {
    0x00000000, 0x00023A00, 0x00062800, 0x00030200, 0x00031100, 0x00052000,
    0x00052001, 0x00052100, 0x00052400, 0x00052401, 0x00052402, 0x000B5A01,
    0x00056300, 0x00020401, 0x00020400, 0x00040800, 0x00100007, 0x00000000,
};

u8 ErrorCode2Num(u32);
u8 *__OSLockSramEx(void);
BOOL __OSUnlockSramEx(BOOL);
#pragma push
#pragma dont_inline on
u8 ErrorCode2Num(u32 errorCode) {
    u32 i;

    for (i = 0; i < sizeof(ErrorTable) / sizeof(ErrorTable[0]); i++) {
        if (ErrorTable[i] == errorCode) {
            return (u8)i;
        }
    }

    if ((errorCode >= 0x00100000) && (errorCode <= 0x00100008)) {
        return 17;
    }

    return 29;
}
#pragma pop

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
