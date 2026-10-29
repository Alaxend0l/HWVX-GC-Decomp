#include "common.h"


typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDFormat(s32 chan)
{
    s32 __CARDFormatRegionAsync(s32 chan, u16 encode, CARDCallback callback);
    u16 OSGetFontEncode(void);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = __CARDFormatRegionAsync(chan, OSGetFontEncode(), __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}
