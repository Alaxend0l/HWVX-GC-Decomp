#include "common.h"

extern u32 Scb[21];

s32 __OSSyncSram(void)
{
    return Scb[0x13];
}
