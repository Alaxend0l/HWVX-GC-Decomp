#include "common.h"


void TRKUARTInterruptHandler(void) {

}

void TRK_board_display(char *str) {
    void OSReport(const char *, ...);
    OSReport(str);
}

void UnreserveEXI2Port(void) {
    extern void (*gDBCommTable[7])(void);
    gDBCommTable[6]();
}

void ReserveEXI2Port(void) {
    extern void (*gDBCommTable[7])(void);
    gDBCommTable[5]();
}

s32 WriteUART1(u8 c) {
    extern u8 gWriteBuf[];
    extern s32 gWritePos[];
    gWriteBuf[gWritePos[0]++] = c;
    return 0;
}

void TRKEXICallBack(s16 chan, void *ctx);
void TRKLoadContext(void *ctx, u32 value);
void OSEnableScheduler(void);
void EnableEXI2Interrupts(void) {
    extern void (*gDBCommTable[7])(void);
    gDBCommTable[1]();
}

s32 TRKInitializeIntDrivenUART(u32 param1, u32 param2, u32 param3, volatile u8 **inputPendingPtrRef) {
    extern void (*gDBCommTable[7])(void);
    ((void (*)(volatile u8 **, void *))gDBCommTable[0])(inputPendingPtrRef, TRKEXICallBack);
    return 0;
}

