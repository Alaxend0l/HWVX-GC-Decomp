#include "common.h"

typedef struct ARQRequest {
    struct ARQRequest *next;
    u32 owner;
    u32 type;
    u32 priority;
    u32 source;
    u32 dest;
    u32 length;
    void (*callback)(u32);
} ARQRequest;


static ARQRequest *__ARQRequestQueueHi;
static ARQRequest *__ARQRequestTailHi;
static ARQRequest *__ARQRequestQueueLo;
static ARQRequest *__ARQRequestTailLo;
static ARQRequest *__ARQRequestPendingHi;
static ARQRequest *__ARQRequestPendingLo;
static void (*__ARQCallbackHi)(u32);
static void (*__ARQCallbackLo)(u32);
static u32 __ARQChunkSize;
static s32 __ARQ_init_flag;

void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);
void *ARRegisterDMACallback(void *callback);
s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32);

void __ARQServiceQueueLo(void) {
    if (__ARQRequestPendingLo == NULL && __ARQRequestQueueLo != NULL) {
        __ARQRequestPendingLo = __ARQRequestQueueLo;
        __ARQRequestQueueLo = __ARQRequestQueueLo->next;
    }
    if (__ARQRequestPendingLo) {
        if (__ARQRequestPendingLo->length <= __ARQChunkSize) {
            if (__ARQRequestPendingLo->type == 0) {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->source, __ARQRequestPendingLo->dest,
                           __ARQRequestPendingLo->length);
            } else {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->dest, __ARQRequestPendingLo->source,
                           __ARQRequestPendingLo->length);
            }
            __ARQCallbackLo = __ARQRequestPendingLo->callback;
        } else {
            if (__ARQRequestPendingLo->type == 0) {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->source, __ARQRequestPendingLo->dest,
                           __ARQChunkSize);
            } else {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->dest, __ARQRequestPendingLo->source,
                           __ARQChunkSize);
            }
        }
        __ARQRequestPendingLo->length -= __ARQChunkSize;
        __ARQRequestPendingLo->source += __ARQChunkSize;
        __ARQRequestPendingLo->dest += __ARQChunkSize;
    }
}

void __ARQCallbackHack(void) {
}

void __ARQInterruptServiceRoutine(void) {
    if (__ARQCallbackHi) {
        __ARQCallbackHi((u32)__ARQRequestPendingHi);
        __ARQRequestPendingHi = NULL;
        __ARQCallbackHi = NULL;
    } else if (__ARQCallbackLo) {
        __ARQCallbackLo((u32)__ARQRequestPendingLo);
        __ARQRequestPendingLo = NULL;
        __ARQCallbackLo = NULL;
    }
    if (__ARQRequestQueueHi) {
        if (__ARQRequestQueueHi->type == 0) {
            ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->source, __ARQRequestQueueHi->dest,
                       __ARQRequestQueueHi->length);
        } else {
            ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->dest, __ARQRequestQueueHi->source,
                       __ARQRequestQueueHi->length);
        }
        __ARQCallbackHi = __ARQRequestQueueHi->callback;
        __ARQRequestPendingHi = __ARQRequestQueueHi;
        __ARQRequestQueueHi = __ARQRequestQueueHi->next;
    }
    if (__ARQRequestPendingHi == NULL) {
        __ARQServiceQueueLo();
    }
}

void ARQInit(void)
{
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
    __ARQ_init_flag = 0;
}

void ARQPostRequest(ARQRequest *request, u32 owner, u32 type, u32 priority, u32 source, u32 dest, u32 length, void (*callback)(u32)) {
    s32 level;

    request->next = NULL;
    request->owner = owner;
    request->type = type;
    request->source = source;
    request->dest = dest;
    request->length = length;
    if (callback) {
        request->callback = callback;
    } else {
        request->callback = (void (*)(u32))__ARQCallbackHack;
    }
    level = OSDisableInterrupts();
    switch (priority) {
    case 0:
        if (__ARQRequestQueueLo) {
            __ARQRequestTailLo->next = request;
        } else {
            __ARQRequestQueueLo = request;
        }
        __ARQRequestTailLo = request;
        break;
    case 1:
        if (__ARQRequestQueueHi) {
            __ARQRequestTailHi->next = request;
        } else {
            __ARQRequestQueueHi = request;
        }
        __ARQRequestTailHi = request;
        break;
    }
    if (__ARQRequestPendingHi == NULL && __ARQRequestPendingLo == NULL) {
        if (__ARQRequestQueueHi) {
            if (__ARQRequestQueueHi->type == 0) {
                ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->source, __ARQRequestQueueHi->dest,
                           __ARQRequestQueueHi->length);
            } else {
                ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->dest, __ARQRequestQueueHi->source,
                           __ARQRequestQueueHi->length);
            }
            __ARQCallbackHi = __ARQRequestQueueHi->callback;
            __ARQRequestPendingHi = __ARQRequestQueueHi;
            __ARQRequestQueueHi = __ARQRequestQueueHi->next;
        }
        if (__ARQRequestPendingHi == NULL) {
            __ARQServiceQueueLo();
        }
    }
    OSRestoreInterrupts(level);
}

void ARQFlushQueue(void)
{
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

