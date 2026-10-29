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
    extern s32 gWritePos;
    gWriteBuf[gWritePos++] = c;
    return 0;
}

