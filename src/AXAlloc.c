#include "common.h"

extern void * __AXCallbackStack;
extern void * __AXStackHead[32];
extern void * __AXStackTail[32];

void __AXPushCallbackStack(void * arg0)
{
    ((void **)arg0)[2] = __AXCallbackStack;
    __AXCallbackStack = arg0;
}
