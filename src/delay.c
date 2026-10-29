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

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
BOOL AXFXDelaySettings(AXFX_DELAY *delay);

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

