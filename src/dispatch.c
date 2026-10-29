#include "common.h"

typedef struct TRKBuffer TRKBuffer;

s32 TRKSetBufferPosition(void *arg0, u32 arg1);
s32 TRKReadBuffer1_ui8(TRKBuffer *b, u8 *data);

s32 TRKDoUnsupported(TRKBuffer *b);
s32 TRKDoConnect(TRKBuffer *b);
s32 TRKDoDisconnect(TRKBuffer *b);
s32 TRKDoReset(TRKBuffer *b);
s32 TRKDoVersions(TRKBuffer *b);
s32 TRKDoSupportMask(TRKBuffer *b);
s32 TRKDoCPUType(TRKBuffer *b);
s32 TRKDoReadMemory(TRKBuffer *b);
s32 TRKDoWriteMemory(TRKBuffer *b);
s32 TRKDoReadRegisters(TRKBuffer *b);
s32 TRKDoWriteRegisters(TRKBuffer *b);
s32 TRKDoFlushCache(TRKBuffer *b);
s32 TRKDoSetOption(TRKBuffer *b);
s32 TRKDoContinue(TRKBuffer *b);
s32 TRKDoStep(TRKBuffer *b);
s32 TRKDoStop(TRKBuffer *b);

s32 (*gTRKDispatchTable[33])(TRKBuffer *) = {
    TRKDoUnsupported, TRKDoConnect, TRKDoDisconnect, TRKDoReset,
    TRKDoVersions, TRKDoSupportMask, TRKDoCPUType, TRKDoUnsupported,
    TRKDoUnsupported, TRKDoUnsupported, TRKDoUnsupported, TRKDoUnsupported,
    TRKDoUnsupported, TRKDoUnsupported, TRKDoUnsupported, TRKDoUnsupported,
    TRKDoReadMemory, TRKDoWriteMemory, TRKDoReadRegisters, TRKDoWriteRegisters,
    TRKDoUnsupported, TRKDoUnsupported, TRKDoFlushCache, TRKDoSetOption,
    TRKDoContinue, TRKDoStep, TRKDoStop, TRKDoUnsupported,
    TRKDoUnsupported, TRKDoUnsupported, TRKDoUnsupported, TRKDoUnsupported,
    TRKDoUnsupported,
};

__declspec(section ".data") u32 gTRKDispatchTableSize;

s32 TRKInitializeDispatcher(void) {
    gTRKDispatchTableSize = 32;
    return 0;
}
