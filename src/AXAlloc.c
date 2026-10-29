#include "common.h"

extern void * __AXCallbackStack;
extern void * __AXStackHead[32];
extern void * __AXStackTail[32];

#pragma peephole on
void *__AXGetStackHead(u32 priority)
{
    return __AXStackHead[priority];
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

