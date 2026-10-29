#include "common.h"


void __ARQCallbackHack(void) {
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

