#include "common.h"

extern void * __AXCallbackStack;

void __AXPushCallbackStack(void * arg0)
{
    ((void **)arg0)[2] = __AXCallbackStack;
    __AXCallbackStack = arg0;
}
