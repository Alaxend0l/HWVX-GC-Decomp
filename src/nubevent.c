#include "common.h"

s32 TRKInitializeMutex(void *mutex);
s32 TRKAcquireMutex(void *mutex);
s32 TRKReleaseMutex(void *mutex);
void *TRK_memcpy(void *dst, const void *src, u32 n);
s32 TRKReleaseBuffer(s32 idx);

TRKEventQueue gTRKEventQueue;
