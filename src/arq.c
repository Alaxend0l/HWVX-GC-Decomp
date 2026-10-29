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


void __ARQServiceQueueLo(void) {
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
    extern void *__ARQRequestQueueLo;
    extern void *__ARQRequestPendingLo;
    extern u32 __ARQChunkSize;
    extern void *__ARQCallbackLo;
    void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);

    if (__ARQRequestPendingLo == NULL && __ARQRequestQueueLo != NULL) {
        __ARQRequestPendingLo = __ARQRequestQueueLo;
        __ARQRequestQueueLo = ((ARQRequest *)__ARQRequestQueueLo)->next;
    }
    if (__ARQRequestPendingLo) {
        if (((ARQRequest *)__ARQRequestPendingLo)->length <= __ARQChunkSize) {
            if (((ARQRequest *)__ARQRequestPendingLo)->type == 0) {
                ARStartDMA(((ARQRequest *)__ARQRequestPendingLo)->type, ((ARQRequest *)__ARQRequestPendingLo)->source, ((ARQRequest *)__ARQRequestPendingLo)->dest, ((ARQRequest *)__ARQRequestPendingLo)->length);
            } else {
                ARStartDMA(((ARQRequest *)__ARQRequestPendingLo)->type, ((ARQRequest *)__ARQRequestPendingLo)->dest, ((ARQRequest *)__ARQRequestPendingLo)->source, ((ARQRequest *)__ARQRequestPendingLo)->length);
            }
            __ARQCallbackLo = ((ARQRequest *)__ARQRequestPendingLo)->callback;
        } else {
            if (((ARQRequest *)__ARQRequestPendingLo)->type == 0) {
                ARStartDMA(((ARQRequest *)__ARQRequestPendingLo)->type, ((ARQRequest *)__ARQRequestPendingLo)->source, ((ARQRequest *)__ARQRequestPendingLo)->dest, __ARQChunkSize);
            } else {
                ARStartDMA(((ARQRequest *)__ARQRequestPendingLo)->type, ((ARQRequest *)__ARQRequestPendingLo)->dest, ((ARQRequest *)__ARQRequestPendingLo)->source, __ARQChunkSize);
            }
        }
        ((ARQRequest *)__ARQRequestPendingLo)->length -= __ARQChunkSize;
        ((ARQRequest *)__ARQRequestPendingLo)->source += __ARQChunkSize;
        ((ARQRequest *)__ARQRequestPendingLo)->dest += __ARQChunkSize;
    }
}

void __ARQCallbackHack(void) {
}

void __ARQInterruptServiceRoutine(void) {
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
    extern void *__ARQRequestQueueHi;
    extern void *__ARQRequestPendingHi;
    extern void *__ARQRequestPendingLo;
    extern void *__ARQCallbackHi;
    extern void *__ARQCallbackLo;
    void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);
    void __ARQServiceQueueLo(void);

    if (__ARQCallbackHi) {
        ((void (*)(u32))__ARQCallbackHi)((u32)__ARQRequestPendingHi);
        __ARQRequestPendingHi = NULL;
        __ARQCallbackHi = NULL;
    } else if (__ARQCallbackLo) {
        ((void (*)(u32))__ARQCallbackLo)((u32)__ARQRequestPendingLo);
        __ARQRequestPendingLo = NULL;
        __ARQCallbackLo = NULL;
    }
    if (__ARQRequestQueueHi) {
        if (((ARQRequest *)__ARQRequestQueueHi)->type == 0) {
            ARStartDMA(((ARQRequest *)__ARQRequestQueueHi)->type, ((ARQRequest *)__ARQRequestQueueHi)->source, ((ARQRequest *)__ARQRequestQueueHi)->dest, ((ARQRequest *)__ARQRequestQueueHi)->length);
        } else {
            ARStartDMA(((ARQRequest *)__ARQRequestQueueHi)->type, ((ARQRequest *)__ARQRequestQueueHi)->dest, ((ARQRequest *)__ARQRequestQueueHi)->source, ((ARQRequest *)__ARQRequestQueueHi)->length);
        }
        __ARQCallbackHi = ((ARQRequest *)__ARQRequestQueueHi)->callback;
        __ARQRequestPendingHi = __ARQRequestQueueHi;
        __ARQRequestQueueHi = ((ARQRequest *)__ARQRequestQueueHi)->next;
    }
    if (__ARQRequestPendingHi == NULL) {
        __ARQServiceQueueLo();
    }
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

void ARQPostRequest(ARQRequest *request, u32 owner, u32 type, u32 priority, u32 source, u32 dest, u32 length, void (*callback)(u32)) {
    extern ARQRequest *__ARQRequestQueueHi;
    extern ARQRequest *__ARQRequestTailHi;
    extern ARQRequest *__ARQRequestQueueLo;
    extern ARQRequest *__ARQRequestTailLo;
    extern ARQRequest *__ARQRequestPendingHi;
    extern ARQRequest *__ARQRequestPendingLo;
    extern void (*__ARQCallbackHi)(u32);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);
    void __ARQServiceQueueLo(void);
    void __ARQCallbackHack(void);
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

