#include "common.h"

u32 __AXBufferAuxA[3][480];
u32 __AXBufferAuxB[3][480];

void * __AXCallbackAuxA;
void * __AXCallbackAuxB;
void * __AXContextAuxA;
void * __AXContextAuxB;
s32 __AXAuxADspWrite;
s32 __AXAuxADspRead;
s32 __AXAuxBDspWrite;
s32 __AXAuxBDspRead;
s32 __AXAuxDspWritePosition;
s32 __AXAuxDspReadPosition;
s32 __AXAuxDspWritePositionDpl2;
s32 __AXAuxDspReadPositionDpl2;
s32 __AXAuxCpuReadWritePosition;


void __AXAuxQuit(void) {
    extern void *__AXCallbackAuxA;
    extern void *__AXCallbackAuxB;
    __AXCallbackAuxA = 0;
    __AXCallbackAuxB = 0;
}

void __AXGetAuxAInput(void **out)
{
    if (__AXCallbackAuxA)
    {
        *out = __AXBufferAuxA[__AXAuxDspWritePosition];
    }
    else
    {
        *out = 0;
    }
}

void __AXGetAuxAInputDpl2(void **out)
{
    *out = &__AXBufferAuxB[__AXAuxDspWritePosition][320];
}

void __AXGetAuxAOutput(void **out)
{
    *out = __AXBufferAuxA[__AXAuxDspReadPosition];
}

void __AXGetAuxAOutputDpl2R(void **out)
{
    *out = &__AXBufferAuxA[__AXAuxDspReadPosition][160];
}

void __AXGetAuxAOutputDpl2Ls(void **out)
{
    *out = &__AXBufferAuxA[__AXAuxDspReadPosition][320];
}

void __AXGetAuxAOutputDpl2Rs(void **out)
{
    *out = &__AXBufferAuxB[__AXAuxDspReadPosition][320];
}

void __AXGetAuxBInput(void **out)
{
    if (__AXCallbackAuxB)
    {
        *out = __AXBufferAuxB[__AXAuxDspWritePosition];
    }
    else
    {
        *out = 0;
    }
}

void __AXGetAuxBOutput(void **out)
{
    *out = __AXBufferAuxB[__AXAuxDspReadPosition];
}

void __AXGetAuxBForDPL2(void **out)
{
    *out = __AXBufferAuxB[__AXAuxDspWritePositionDpl2];
}

void __AXGetAuxBOutputDPL2(void **out)
{
    *out = __AXBufferAuxB[__AXAuxDspReadPositionDpl2];
}

void AXRegisterAuxACallback(void *callback, void *context)
{
    __AXCallbackAuxA = callback;
    __AXContextAuxA = context;
}

