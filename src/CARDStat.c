#include "common.h"


typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDSetStatus(s32 chan, s32 fileNo, u8 *stat)
{
    s32 CARDSetStatusAsync(s32 chan, s32 fileNo, u8 *stat, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDSetStatusAsync(chan, fileNo, stat, __CARDSyncCallback);
    if (result >= 0) {
        result = __CARDSync(chan);
    } else {
        return result;
    }
    return result;
}
