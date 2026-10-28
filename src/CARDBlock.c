#include "common.h"

s32 __CARDGetFatBlock(void * arg0)
{
    return ((s32 *)arg0)[0x22];
}
