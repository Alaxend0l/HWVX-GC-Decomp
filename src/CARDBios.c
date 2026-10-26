
#include "common.h"

void __CARDDefaultApiCallback(void)
{
}

extern int CARDUnmount(int);
s32 OnReset(s32 arg0)
{
    if ((arg0 == 0) && ((CARDUnmount(0) == -1) || (CARDUnmount(1) == -1)))
    {
        return 0;
    }
    return 1;
}

