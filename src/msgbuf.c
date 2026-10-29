#include "common.h"

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

void *TRK_memset(void *dst, s32 val, u32 n);
s32 TRKAcquireMutex(void *mutex);
s32 TRKReleaseMutex(void *mutex);

extern TRKBuffer gTRKMsgBufs[3];
