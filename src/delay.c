#include "common.h"

typedef struct AXFX_DELAY {
    u32 currentSize[3];
    u32 currentPos[3];
    u32 currentFeedback[3];
    u32 currentOutput[3];
    s32 *left;
    s32 *right;
    s32 *sur;
    u32 delay[3];
    u32 feedback[3];
    u32 output[3];
} AXFX_DELAY;

typedef struct AXFX_BUFFERUPDATE {
    s32 *left;
    s32 *right;
    s32 *surround;
} AXFX_BUFFERUPDATE;

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
BOOL AXFXDelaySettings(AXFX_DELAY *delay);

extern void *(*__AXFXAlloc)(u32);
BOOL AXFXDelayShutdown(AXFX_DELAY *delay);
void AXFXDelayCallback(AXFX_BUFFERUPDATE *bufferUpdate, AXFX_DELAY *delay)
{
    s32 l;
    s32 r;
    s32 s;
    s32 *leftD;
    s32 *rightD;
    s32 *surD;
    s32 *lBusData;
    s32 *rBusData;
    s32 *sBusData;
    u32 i;

    lBusData = bufferUpdate->left;
    rBusData = bufferUpdate->right;
    sBusData = bufferUpdate->surround;
    leftD = delay->left + (delay->currentPos[0] * 160);
    rightD = delay->right + (delay->currentPos[1] * 160);
    surD = delay->sur + (delay->currentPos[2] * 160);

    for (i = 0; i < 160; i++) {
        l = *leftD;
        r = *rightD;
        s = *surD;
        *leftD = *lBusData + ((s32)(l * delay->currentFeedback[0]) >> 7);
        *rightD = *rBusData + ((s32)(r * delay->currentFeedback[1]) >> 7);
        *surD = *sBusData + ((s32)(s * delay->currentFeedback[2]) >> 7);
        *lBusData++ = (s32)(l * delay->currentOutput[0]) >> 7;
        *rBusData++ = (s32)(r * delay->currentOutput[1]) >> 7;
        *sBusData++ = (s32)(s * delay->currentOutput[2]) >> 7;
        leftD++;
        rightD++;
        surD++;
    }

    delay->currentPos[0] = (delay->currentPos[0] + 1) % delay->currentSize[0];
    delay->currentPos[1] = (delay->currentPos[1] + 1) % delay->currentSize[1];
    delay->currentPos[2] = (delay->currentPos[2] + 1) % delay->currentSize[2];
}

BOOL AXFXDelaySettings(AXFX_DELAY *delay)
{
    u32 i;
    s32 *l;
    s32 *r;
    s32 *s;
    BOOL old;

    AXFXDelayShutdown(delay);
    old = OSDisableInterrupts();

    for (i = 0; i < 3; i++) {
        delay->currentSize[i] = ((delay->delay[i] - 5) * 32 + 159) / 160;
        delay->currentPos[i] = 0;
        delay->currentFeedback[i] = (delay->feedback[i] * 128) / 100;
        delay->currentOutput[i] = (delay->output[i] * 128) / 100;
    }

    delay->left = __AXFXAlloc(delay->currentSize[0] * 0x280);
    delay->right = __AXFXAlloc(delay->currentSize[1] * 0x280);
    delay->sur = __AXFXAlloc(delay->currentSize[2] * 0x280);

    l = delay->left;
    r = delay->right;
    s = delay->sur;

    for (i = 0; i < delay->currentSize[0] * 160; i++) {
        *l++ = 0;
    }
    for (i = 0; i < delay->currentSize[1] * 160; i++) {
        *r++ = 0;
    }
    for (i = 0; i < delay->currentSize[2] * 160; i++) {
        *s++ = 0;
    }

    OSRestoreInterrupts(old);
    return TRUE;
}

BOOL AXFXDelayInit(AXFX_DELAY *delay)
{
    BOOL old;

    old = OSDisableInterrupts();
    delay->left = NULL;
    delay->right = NULL;
    delay->sur = NULL;
    OSRestoreInterrupts(old);
    return AXFXDelaySettings(delay);
}

extern void (*__AXFXFree)(void *);
BOOL AXFXDelayShutdown(AXFX_DELAY *delay)
{
    BOOL old = OSDisableInterrupts();

    if (delay->left != NULL)
        __AXFXFree(delay->left);
    if (delay->right != NULL)
        __AXFXFree(delay->right);
    if (delay->sur != NULL)
        __AXFXFree(delay->sur);

    OSRestoreInterrupts(old);
    return TRUE;
}

