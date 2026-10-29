#include "common.h"


typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDDelete(s32 chan, const char *fileName)
{
    s32 CARDDeleteAsync(s32 chan, const char *fileName, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDDeleteAsync(chan, fileName, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}
