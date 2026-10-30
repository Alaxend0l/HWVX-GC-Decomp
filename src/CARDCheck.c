#include "common.h"

typedef struct CARDControl {
    s32 attached;              // 0x00
    s32 result;                // 0x04
    u16 size;                  // 0x08
    u8  unkA[0xC - 0xA];
    s32 sectorSize;            // 0x0C
    u16 cBlock;                // 0x10
    u8  unk12[0x80 - 0x12];
    void *workArea;            // 0x80
    u8 *currentDir;            // 0x84
    void *buffer;              // 0x88
} CARDControl;

typedef struct CARDDirCheck {
    u8 padding0[0x3A];
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

static inline CARDDirCheck *__CARDGetDirCheck(u8 *dir) {
    return (CARDDirCheck *)(dir + 0x1FC0);
}

void *memcpy(void *dst, const void *src, u32 n);
s32 CARDCheckExAsync(s32 chan, s32 *xferBytes, void *callback);
void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDSync(s32 chan);
s32 VerifyID(void *card);
s32 VerifyDir(CARDControl *card, s32 *outCurrent);
s32 VerifyFAT(void *card, s32 *outCurrent);
void __CARDCheckSum(void *ptr, int length, u16 *checksum, u16 *checksumInv) {
    u16 *p;
    int i;

    length /= sizeof(u16);
    *checksum = *checksumInv = 0;
    for (i = 0, p = ptr; i < length; i++, p++) {
        *checksum += *p;
        *checksumInv += ~*p;
    }
    if (*checksum == 0xFFFF) {
        *checksum = 0;
    }
    if (*checksumInv == 0xFFFF) {
        *checksumInv = 0;
    }
}

s32 __CARDVerify(void *card) {
    s32 result;
    s32 errors;

    result = VerifyID(card);
    if (result < 0) {
        return result;
    }
    errors = VerifyDir(card, NULL);
    errors += VerifyFAT(card, NULL);
    switch (errors) {
    case 0:
        return 0;
    case 1:
        return -6;
    default:
        return -6;
    }
}

s32 CARDCheck(s32 chan) {
    s32 xferBytes;
    s32 result;

    result = CARDCheckExAsync(chan, &xferBytes, __CARDSyncCallback);
    if (result < 0 || &xferBytes == NULL) {
        return result;
    }
    return __CARDSync(chan);
}
