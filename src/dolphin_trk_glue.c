#include "common.h"

extern void (*gDBCommTable[7])(void);

static inline s32 TRKWriteUARTN(const void *bytes, u32 length) {
    return ((s32 (*)(const void *, u32))gDBCommTable[4])(bytes, length) == 0 ? 0 : -1;
}


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

s32 TRKReadUARTPoll(u8 *c) {
    typedef struct {
        s32 writePos;
        s32 readPos;
        s32 readCount;
        s32 unkC;
        u8 readBuf[0x110A];
    } TRKUARTBuffer;
    extern s32 gWritePos[];
    extern void (*gDBCommTable[7])(void);
    TRKUARTBuffer *buf = (TRKUARTBuffer *)gWritePos;
    s32 readErr = 4;

    if (buf->readPos >= buf->readCount) {
        buf->readPos = 0;
        buf->readCount = ((s32 (*)(void))gDBCommTable[2])();
        if (buf->readCount > 0) {
            if (buf->readCount > 0x110A) {
                buf->readCount = 0x110A;
            }
            readErr = ((s32 (*)(u8 *, s32))gDBCommTable[3])(buf->readBuf, buf->readCount) == 0 ? 0 : -1;
            if (readErr) {
                buf->readCount = 0;
            }
        }
    }
    if (buf->readPos < buf->readCount) {
        *c = buf->readBuf[buf->readPos++];
        readErr = 0;
    }
    return readErr;
}

s32 WriteUART1(u8 c) {
    extern u8 gWriteBuf[];
    extern s32 gWritePos[];
    gWriteBuf[gWritePos[0]++] = c;
    return 0;
}

s32 WriteUARTFlush(void) {
    unsigned long result = 0;
    extern u8 gWriteBuf_8030CAF4[];
    extern s32 gWritePos_8030B9D8[];
    extern void (*gDBCommTable[7])(void);

    while (gWritePos_8030B9D8[0] < 0x800) {
        gWriteBuf_8030CAF4[gWritePos_8030B9D8[0]] = 0;
        gWritePos_8030B9D8[0]++;
    }
    if (gWritePos_8030B9D8[0] != 0) {
        result = ((s32 (*)(u8 *, s32))gDBCommTable[4])(gWriteBuf_8030CAF4, gWritePos_8030B9D8[0]);
        result = result != 0 ? -1 : 0;
        gWritePos_8030B9D8[0] = 0;
    }
    return result;
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

s32 InitMetroTRKCommTable(s32 hwId) {
    void OSReport(const char *msg, ...);
    s32 Hu_IsStub(void);
    s32 AMC_IsStub(void);
    void DBInitComm(void);
    void DBInitInterrupts(void);
    void DBQueryData(void);
    void DBRead(void);
    void DBWrite(void);
    void DBOpen(void);
    void DBClose(void);
    void EXI2_Init(void);
    void EXI2_EnableInterrupts(void);
    void EXI2_Poll(void);
    void EXI2_ReadN(void);
    void EXI2_WriteN(void);
    void EXI2_Reserve(void);
    void EXI2_Unreserve(void);
    extern void (*gDBCommTable[7])(void);
    s32 result;

    if (hwId == 1) {
        OSReport("MetroTRK : Set to GDEV hardware\n");
        result = Hu_IsStub();
        gDBCommTable[0] = DBInitComm;
        gDBCommTable[1] = DBInitInterrupts;
        gDBCommTable[2] = DBQueryData;
        gDBCommTable[3] = DBRead;
        gDBCommTable[4] = DBWrite;
        gDBCommTable[5] = DBOpen;
        gDBCommTable[6] = DBClose;
    } else {
        OSReport("MetroTRK : Set to AMC DDH hardware\n");
        result = AMC_IsStub();
        gDBCommTable[0] = EXI2_Init;
        gDBCommTable[1] = EXI2_EnableInterrupts;
        gDBCommTable[2] = EXI2_Poll;
        gDBCommTable[3] = EXI2_ReadN;
        gDBCommTable[4] = EXI2_WriteN;
        gDBCommTable[5] = EXI2_Reserve;
        gDBCommTable[6] = EXI2_Unreserve;
    }
    return result;
}

void TRKEXICallBack(s16 chan, void *ctx) {
    OSEnableScheduler();
    TRKLoadContext(ctx, 0x500);
}

