#include "common.h"

s32 TRKInitializeEventQueue(void);
s32 TRKInitializeMessageBuffers(void);
s32 TRKInitializeDispatcher(void);
s32 TRKInitializeSerialHandler(void);
s32 TRKInitializeTarget(void);
void usr_put_initialize(void);
void TRKTargetSetInputPendingPtr(void *ptr);
s32 TRKInitializeIntDrivenUART(u32 param1, u32 param2, u32 param3, volatile u8 **inputPendingPtrRef);

extern u8 *gTRKInputPendingPtr[];
__declspec(section ".data") s32 gTRKBigEndian;

static BOOL TRK_InitializeEndian(void) {
    u8 bendian[4];
    BOOL result = FALSE;

    gTRKBigEndian = TRUE;
    bendian[0] = 0x12;
    bendian[1] = 0x34;
    bendian[2] = 0x56;
    bendian[3] = 0x78;
    if (*(u32 *)bendian == 0x12345678) {
        gTRKBigEndian = TRUE;
    } else if (*(u32 *)bendian == 0x78563412) {
        gTRKBigEndian = FALSE;
    } else {
        return TRUE;
    }
    return result;
}

void TRKNubWelcome(void)
{
    void TRK_board_display(char *str);

    static const char name[] = "MetroTRK for GAMECUBE v0.10";
    TRK_board_display((char *)name);
}

s32 TRKTerminateNub(void)
{
    s32 TRKTerminateSerialHandler(void);

    TRKTerminateSerialHandler();
    return 0;
}

s32 TRKInitializeNub(void) {
    s32 result;
    s32 resultTemp;
    u8 bendian[4];
    s32 one;

    result = 0;
    one = 1;
    gTRKBigEndian = one;
    bendian[0] = 0x12;
    bendian[1] = 0x34;
    bendian[2] = 0x56;
    bendian[3] = 0x78;
    if (*(u32 *)bendian == 0x12345678) {
        gTRKBigEndian = one;
    } else if (*(u32 *)bendian == 0x78563412) {
        gTRKBigEndian = result;
    } else {
        result = one;
    }

    if (result == 0) {
        usr_put_initialize();
    }
    if (result == 0) {
        result = TRKInitializeEventQueue();
    }
    if (result == 0) {
        result = TRKInitializeMessageBuffers();
    }
    if (result == 0) {
        result = TRKInitializeDispatcher();
    }
    if (result == 0) {
        resultTemp = TRKInitializeIntDrivenUART(0xE100, 1, 0, (volatile u8 **)gTRKInputPendingPtr);
        TRKTargetSetInputPendingPtr(gTRKInputPendingPtr[0]);
        if (resultTemp != 0) {
            result = resultTemp;
        }
    }
    if (result == 0) {
        result = TRKInitializeSerialHandler();
    }
    if (result == 0) {
        result = TRKInitializeTarget();
    }
    return result;
}

