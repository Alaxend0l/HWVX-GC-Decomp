#include "common.h"


typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDCreate(s32 chan, const char *fileName, u32 size, void *fileInfo)
{
    s32 CARDCreateAsync(s32 chan, const char *fileName, u32 size, void *fileInfo, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDCreateAsync(chan, fileName, size, fileInfo, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}
