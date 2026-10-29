#include "common.h"

typedef void (*AXVoiceCallback)(void *);

typedef struct AXVPB {
    void *next;
    void *prev;
    void *next1;
    u32 priority;
    AXVoiceCallback callback;
    u32 userContext;
    u32 index;
    u32 sync;
    u32 depop;
    u32 updateMS;
    u32 updateCounter;
    u32 updateTotal;
    u16 *updateWrite;
    u16 updateData[128];
    void *itdBuffer;
    u8 pb_unk138[0xE];
    u16 state;
} AXVPB;

void __AXSetPBDefault(AXVPB *p);

static AXVPB * __AXStackHead[32];
static AXVPB * __AXStackTail[32];
static void * __AXCallbackStack;

void *__AXPopCallbackStack(void);
void __AXRemoveFromStack(AXVPB *p);
void __AXPushFreeStack(AXVPB *p);

#pragma peephole on
void *__AXGetStackHead(u32 priority)
{
    return __AXStackHead[priority];
}

static AXVPB *__AXPopFreeStack(void)
{
    AXVPB *p = __AXStackHead[0];

    if (p != NULL) {
        __AXStackHead[0] = p->next;
    }
    return p;
}

static AXVPB *__AXPopStackFromBottom(u32 priority)
{
    AXVPB *p = NULL;

    if (__AXStackHead[priority] != NULL) {
        if (__AXStackHead[priority] == __AXStackTail[priority]) {
            p = __AXStackHead[priority];
            __AXStackHead[priority] = __AXStackTail[priority] = NULL;
        } else if (__AXStackTail[priority] != NULL) {
            p = __AXStackTail[priority];
            __AXStackTail[priority] = p->prev;
            __AXStackTail[priority]->next = NULL;
        }
    }
    return p;
}

void __AXServiceCallbackStack(void)
{
    AXVPB *pvpb;

    for (pvpb = __AXPopCallbackStack(); pvpb != NULL; pvpb = __AXPopCallbackStack()) {
        if (pvpb->priority != 0) {
            if (pvpb->callback != NULL) {
                pvpb->callback(pvpb);
            }
            __AXRemoveFromStack(pvpb);
            __AXPushFreeStack(pvpb);
        }
    }
}

void __AXRemoveFromStack(AXVPB *p)
{
    u32 i = p->priority;
    AXVPB *head = __AXStackHead[i];
    AXVPB *tail = __AXStackTail[i];

    if (head == tail) {
        __AXStackHead[i] = __AXStackTail[i] = NULL;
        return;
    }
    if (p == head) {
        __AXStackHead[i] = p->next;
        ((AXVPB *)__AXStackHead[i])->prev = NULL;
        return;
    }
    if (p == tail) {
        __AXStackTail[i] = p->prev;
        ((AXVPB *)__AXStackTail[i])->next = NULL;
        return;
    }
    head = p->prev;
    tail = p->next;
    head->next = tail;
    tail->prev = head;
}

void __AXAllocInit(void)
{
    u32 i;

    __AXCallbackStack = NULL;
    for (i = 0; i < 32; i++) {
        __AXStackTail[i] = NULL;
        __AXStackHead[i] = NULL;
    }
}

void __AXAllocQuit(void)
{
    u32 i;

    __AXCallbackStack = NULL;
    for (i = 0; i < 32; i++) {
        __AXStackTail[i] = NULL;
        __AXStackHead[i] = NULL;
    }
}

void __AXPushFreeStack(AXVPB *p)
{
    p->next = __AXStackHead[0];
    __AXStackHead[0] = p;
    p->priority = 0;
}

void __AXPushCallbackStack(void * arg0)
{
    ((void **)arg0)[2] = __AXCallbackStack;
    __AXCallbackStack = arg0;
}

void *__AXPopCallbackStack(void)
{
    void *pvpb = __AXCallbackStack;

    if (pvpb != NULL) {
        __AXCallbackStack = ((void **)pvpb)[2];
    }
    return pvpb;
}
void AXFreeVoice(AXVPB *p)
{
    BOOL old;

    old = OSDisableInterrupts();
    __AXRemoveFromStack(p);
    if (p->state == 1) {
        p->depop = 1;
    }
    __AXSetPBDefault(p);
    p->next = __AXStackHead[0];
    __AXStackHead[0] = p;
    p->priority = 0;
    OSRestoreInterrupts(old);
}

AXVPB *AXAcquireVoice(u32 priority, AXVoiceCallback callback, u32 userContext)
{
    BOOL old;
    AXVPB *pvpb;
    u32 i;

    old = OSDisableInterrupts();
    pvpb = __AXStackHead[0];
    if (pvpb != NULL) {
        __AXStackHead[0] = pvpb->next;
    }
    if (pvpb == NULL) {
        for (i = 1; i < priority; i++) {
            AXVPB *p = NULL;

            if (__AXStackHead[i] != NULL) {
                if (__AXStackHead[i] == __AXStackTail[i]) {
                    p = __AXStackHead[i];
                    __AXStackHead[i] = __AXStackTail[i] = NULL;
                } else if (__AXStackTail[i] != NULL) {
                    p = __AXStackTail[i];
                    __AXStackTail[i] = p->prev;
                    __AXStackTail[i]->next = NULL;
                }
            }
            pvpb = p;
            if (pvpb != NULL) {
                if (pvpb->state == 1) {
                    pvpb->depop = 1;
                }
                if (pvpb->callback != NULL) {
                    pvpb->callback(pvpb);
                }
                break;
            }
        }
    }
    if (pvpb != NULL) {
        pvpb->next = __AXStackHead[priority];
        pvpb->prev = NULL;
        if (pvpb->next != NULL) {
            __AXStackHead[priority]->prev = pvpb;
            __AXStackHead[priority] = pvpb;
        } else {
            __AXStackTail[priority] = pvpb;
            __AXStackHead[priority] = pvpb;
        }
        pvpb->priority = priority;
        pvpb->callback = callback;
        pvpb->userContext = userContext;
        __AXSetPBDefault(pvpb);
    }
    OSRestoreInterrupts(old);
    return pvpb;
}

