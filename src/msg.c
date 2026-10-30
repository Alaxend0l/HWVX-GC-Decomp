#include "common.h"

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

s32 WriteUART1(s8 c);
s32 WriteUARTFlush(void);

s32 TRKMessageSend(TRKBuffer *b) {
    u8 csum = 0;
    int j;
    u32 i;
    u8 c;
    s32 err;

    for (j = 0; j < b->length; j++) {
        csum = csum + b->data[j];
    }
    csum = csum ^ 0xff;

    err = WriteUART1(0x7e);
    if (err == 0) {
        for (i = 0; i < b->length; i++) {
            c = b->data[i];
            if (c == 0x7e || c == 0x7d) {
                err = WriteUART1(0x7d);
                c ^= 0x20;
                if (err != 0) {
                    break;
                }
            }
            err = WriteUART1(c);
            if (err != 0) {
                break;
            }
        }
    }
    if (err == 0) {
        u8 d = csum;
        for (i = 0; i < 1; i++) {
            if (d == 0x7e || d == 0x7d) {
                err = WriteUART1(0x7d);
                d ^= 0x20;
                if (err != 0) {
                    break;
                }
            }
            err = WriteUART1(d);
            if (err != 0) {
                break;
            }
        }
    }
    if (err == 0) {
        err = WriteUART1(0x7e);
    }
    if (err == 0) {
        err = WriteUARTFlush();
    }
    return err;
}
