#include "common.h"


void PushDecodedAudioBuffer(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}
