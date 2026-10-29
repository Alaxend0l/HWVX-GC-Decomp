#include "common.h"


void *PopFreeTextureSet(void) {
    extern u8 FreeTextureSetQueue[];
    void *msg;

    OSReceiveMessage(FreeTextureSetQueue, &msg, 1);
    return msg;
}

void PushFreeTextureSet(void *arg0)
{
    extern u8 UsedTextureSetQueue_8036EC78[];
    OSSendMessage(UsedTextureSetQueue_8036EC78, arg0, 0);
}

void PushDecodedTextureSet(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
