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
} AXVPB;

extern void * __AXCallbackStack;
extern void * __AXStackHead[32];
extern void * __AXStackTail[32];

void *__AXPopCallbackStack(void);
void __AXRemoveFromStack(AXVPB *p);
void __AXPushFreeStack(AXVPB *p);

#pragma peephole on
void *__AXGetStackHead(u32 priority)
{
    return __AXStackHead[priority];
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

