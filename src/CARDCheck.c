#include "common.h"


s32 CARDCheckExAsync(s32 chan, s32 *xferBytes, void *callback);
void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDSync(s32 chan);
s32 CARDCheck(s32 chan) {
    s32 xferBytes;
    s32 result;

    result = CARDCheckExAsync(chan, &xferBytes, __CARDSyncCallback);
    if (result < 0 || &xferBytes == NULL) {
        return result;
    }
    return __CARDSync(chan);
}
