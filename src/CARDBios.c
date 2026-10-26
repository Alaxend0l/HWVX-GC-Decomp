
#include "common.h"

void __CARDDefaultApiCallback(void)
{
}

extern int CARDUnmount(int);
int OnReset(int arg0)
{
    if (arg0 != 0)
    {
        return 1;
    }
    if (CARDUnmount(0) == -1)
    {
        return 0;
    }
    if (CARDUnmount(1) == -1)
    {
        return 0;
    }
    return 1;
}

