#include "common.h"
typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct OSThreadQueue { struct OSThread *head; struct OSThread *tail; } OSThreadQueue;

typedef struct CARDControl {
    s32 attached;
    s32 result;
    u16 size;
    u8 unkA[0xC - 0xA];
    s32 sectorSize;
    u16 cBlock;
    u8 unk12[0x14 - 0x12];
    s32 latency;
    u8 unk18[0x24 - 0x18];
    s32 mountStep;
    s32 formatStep;
    u8 unk2C[0x80 - 0x2C];
    void *workArea;
    u8 *currentDir;
    u16 *currentFat;
    OSThreadQueue threadQueue;
    u8 cmd[9];
    s32 cmdlen;
    s32 mode;
    s32 retry;
    u32 repeat;
    void *addr;
    s32 offset;
    u8 unkB8[0xBE - 0xB8];
    u16 startBlock;
    struct CARDFileInfo *fileInfo;
    CARDCallback extCallback;
    CARDCallback txCallback;
    CARDCallback exiCallback;
    CARDCallback apiCallback;
    CARDCallback xferCallback;
    CARDCallback eraseCallback;
    CARDCallback unlockCallback;
    u8 alarm[0x28];
    u8 unk108[0x10C - 0x108];
    u8 *diskID;
} CARDControl;

extern CARDControl __CARDBlock[2];

int OSDisableInterrupts(void);
int OSRestoreInterrupts(int level);
void OSCancelAlarm(void *alarm);
void *EXISetExiCallback(s32 chan, void *callback);
int EXIDetach(s32 chan);



typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDMount(s32 chan, s32 fileNo, u8 *stat)
{
    s32 CARDMountAsync(s32 chan, s32 fileNo, u8 *stat, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDMountAsync(chan, fileNo, stat, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}

static void DoUnmount(s32 chan, s32 result) {
    CARDControl *card;
    int enabled;

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    if (card->attached) {
        EXISetExiCallback(chan, 0);
        EXIDetach(chan);
        OSCancelAlarm(&card->alarm);
        card->attached = 0;
        card->result = result;
        card->mountStep = 0;
    }
    OSRestoreInterrupts(enabled);
}

s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
s32 CARDUnmount(s32 chan) {
    CARDControl *card;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    DoUnmount(chan, -3);
    return 0;
}

