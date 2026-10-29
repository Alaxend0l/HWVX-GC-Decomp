#include "common.h"


void __ARQCallbackHack(void) {
}

void ARQInit(void)
{
    extern s32 __ARQ_init_flag;
    extern void *__ARQRequestQueueHi;
    extern void *__ARQRequestQueueLo;
    extern u32 __ARQChunkSize;
    extern void *__ARQRequestPendingHi;
    extern void *__ARQRequestPendingLo;
    extern void *__ARQCallbackHi;
    extern void *__ARQCallbackLo;
    void __ARQInterruptServiceRoutine(void);
    void *ARRegisterDMACallback(void *callback);

    if (__ARQ_init_flag == 1) {
        return;
    }
    __ARQRequestQueueHi = __ARQRequestQueueLo = NULL;
    __ARQChunkSize = 0x1000;
    ARRegisterDMACallback(__ARQInterruptServiceRoutine);
    __ARQRequestPendingHi = NULL;
    __ARQRequestPendingLo = NULL;
    __ARQCallbackHi = NULL;
    __ARQCallbackLo = NULL;
    __ARQ_init_flag = 1;
}

void ARQReset(void) {
    extern s32 __ARQ_init_flag;
    __ARQ_init_flag = 0;
}

void ARQFlushQueue(void)
{
    extern void *__ARQCallbackLo;
    extern void *__ARQRequestPendingLo;
    extern void *__ARQRequestQueueHi;
    extern void *__ARQRequestQueueLo;
    extern void *__ARQRequestTailHi;
    extern void *__ARQRequestTailLo;
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    s32 level;

    level = OSDisableInterrupts();
    __ARQRequestQueueHi = NULL;
    __ARQRequestTailHi = NULL;
    __ARQRequestQueueLo = NULL;
    __ARQRequestTailLo = NULL;
    if (__ARQCallbackLo == NULL) {
        __ARQRequestPendingLo = NULL;
    }
    OSRestoreInterrupts(level);
}

