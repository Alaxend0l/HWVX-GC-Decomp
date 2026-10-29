#include "common.h"


s32 CARDCheckExAsync(s32 chan, s32 *xferBytes, void *callback);
void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDSync(s32 chan);
s32 VerifyID(void *card);
s32 VerifyDir(void *card, s32 *outCurrent);
s32 VerifyFAT(void *card, s32 *outCurrent);
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
