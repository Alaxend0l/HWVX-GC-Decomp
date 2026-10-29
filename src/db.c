#include "common.h"


void __DBExceptionDestinationAux(void);
void DBInit(void) {
    extern u32 *__DBInterface;
    extern BOOL DBVerbose;
    extern void __DBExceptionDestination(void);
    __DBInterface = (u32 *)(0x40 + 0x80000000);
    __DBInterface[2] = (u32)__DBExceptionDestination - 0x80000000;
    DBVerbose = TRUE;
}

void __DBExceptionDestination(void) {
    __mtmsr(__mfmsr() | 0x30);
    __DBExceptionDestinationAux();
}

s32 __DBIsExceptionMarked(u8 whichOne)
{
    extern u32 *__DBInterface;

    return __DBInterface[1] & (1 << whichOne);
}

void DBPrintf(char *str, ...)
{
}
