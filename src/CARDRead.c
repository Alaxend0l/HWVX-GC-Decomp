#include "common.h"
typedef void (*CARDCallback)(s32 chan, s32 result);



typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDRead(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset)
{
    s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDReadAsync(fileInfo, buf, length, offset, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(fileInfo->chan);
}
