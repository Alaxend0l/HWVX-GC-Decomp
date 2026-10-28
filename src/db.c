#include "common.h"


s32 __DBIsExceptionMarked(u8 whichOne)
{
    extern u32 *__DBInterface;

    return __DBInterface[1] & (1 << whichOne);
}

void DBPrintf(char *str, ...)
{
}
