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

s32 TRKSetBufferPosition(TRKBuffer *b, u32 position) {
    s32 err = 0;

    if (position > 0x880) {
        err = 0x301;
    } else {
        b->position = position;
        if (position > b->length) {
            b->length = position;
        }
    }
    return err;
}
