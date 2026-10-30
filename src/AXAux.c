#include "common.h"

u32 __AXBufferAuxA[3][480];
u32 __AXBufferAuxB[3][480];

void * __AXCallbackAuxA;
void * __AXCallbackAuxB;
void * __AXContextAuxA;
void * __AXContextAuxB;
void * __AXAuxADspWrite;
void * __AXAuxADspRead;
void * __AXAuxBDspWrite;
void * __AXAuxBDspRead;
u32 __AXAuxDspWritePosition;
u32 __AXAuxDspReadPosition;
u32 __AXAuxDspWritePositionDpl2;
u32 __AXAuxDspReadPositionDpl2;
u32 __AXAuxCpuReadWritePosition;
u32 __AXClMode;


void __AXAuxInit(void) {
    u32 i;
    u32 *pA;
    u32 *pB;

    __AXCallbackAuxA = NULL;
    __AXCallbackAuxB = NULL;
    __AXContextAuxA = NULL;
    __AXContextAuxB = NULL;
    __AXAuxDspWritePosition = 0;
    __AXAuxDspReadPosition = 1;
    __AXAuxDspWritePositionDpl2 = 0;
    __AXAuxDspReadPositionDpl2 = 1;
    __AXAuxCpuReadWritePosition = 2;
    pA = (u32 *)__AXBufferAuxA;
    pB = (u32 *)__AXBufferAuxB;
    for (i = 0; i < 480; i++) {
        *pA = 0;
        pA++;
        *pB = 0;
        pB++;
    }
}

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

void __AXProcessAux(void)
{
    void DCInvalidateRange(void *addr, u32 size);
    void DCFlushRangeNoSync(void *addr, u32 size);
    u32 *base;

    __AXAuxADspWrite = __AXBufferAuxA[__AXAuxDspWritePosition];
    __AXAuxADspRead = __AXBufferAuxA[__AXAuxDspReadPosition];
    __AXAuxBDspWrite = __AXBufferAuxB[__AXAuxDspWritePosition];
    __AXAuxBDspRead = __AXBufferAuxB[__AXAuxDspReadPosition];

    if (__AXCallbackAuxA)
    {
        if (__AXClMode == 2)
        {
            u32 *data[4];

            base = __AXBufferAuxA[__AXAuxCpuReadWritePosition];
            data[0] = base;
            data[1] = base + 160;
            data[2] = base + 320;
            data[3] = base + 1760;
            DCInvalidateRange(data[0], 0x780);
            DCInvalidateRange(data[3], 0x280);
            ((void (*)(void *, void *))__AXCallbackAuxA)(data, __AXContextAuxA);
            DCFlushRangeNoSync(data[0], 0x780);
            DCFlushRangeNoSync(data[3], 0x280);
        }
        else
        {
            u32 *data[3];

            base = __AXBufferAuxA[__AXAuxCpuReadWritePosition];
            data[0] = base;
            data[1] = base + 160;
            data[2] = base + 320;
            DCInvalidateRange(data[0], 0x780);
            ((void (*)(void *, void *))__AXCallbackAuxA)(data, __AXContextAuxA);
            DCFlushRangeNoSync(data[0], 0x780);
        }
    }

    if (__AXCallbackAuxB && __AXClMode != 2)
    {
        u32 *data[3];

        base = __AXBufferAuxB[__AXAuxCpuReadWritePosition];
        data[0] = base;
        data[1] = base + 160;
        data[2] = base + 320;
        DCInvalidateRange(data[0], 0x780);
        ((void (*)(void *, void *))__AXCallbackAuxB)(data, __AXContextAuxB);
        DCFlushRangeNoSync(data[0], 0x780);
    }

    __AXAuxDspWritePosition++;
    __AXAuxDspReadPosition++;
    __AXAuxCpuReadWritePosition++;
    __AXAuxDspWritePositionDpl2++;
    __AXAuxDspReadPositionDpl2++;
    __AXAuxDspWritePosition %= 3;
    __AXAuxDspReadPosition %= 3;
    __AXAuxDspWritePositionDpl2 %= 2;
    __AXAuxDspReadPositionDpl2 %= 2;
    __AXAuxCpuReadWritePosition %= 3;
}

void AXRegisterAuxACallback(void *callback, void *context)
{
    __AXCallbackAuxA = callback;
    __AXContextAuxA = context;
}

void AXRegisterAuxBCallback(void *callback, void *context)
{
    __AXCallbackAuxB = callback;
    __AXContextAuxB = context;
}

