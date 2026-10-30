#include "common.h"
typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDControl {
    s32 attached;              // 0x00
    s32 result;                // 0x04
    u16 size;                  // 0x08
    u8  unkA[0xC - 0xA];
    s32 sectorSize;            // 0x0C
    u16 cBlock;                // 0x10
    u8  unk12[0x14 - 0x12];
    s32 latency;               // 0x14
    u8  unk18[0x24 - 0x18];
    s32 mountStep;             // 0x24
    s32 formatStep;            // 0x28
    u8  unk2C[0x80 - 0x2C];
    void *workArea;            // 0x80
    u8 *currentDir;            // 0x84
    void *buffer;              // 0x88
    u8  unk8C[0x94 - 0x8C];
    u8 cmd[9];                 // 0x94
    s32 cmdlen;                // 0xA0 (aligned)
    s32 mode;                  // 0xA4
    s32 retry;                 // 0xA8
    u32 repeat;                // 0xAC
    void *addr;                // 0xB0
    void *offset;              // 0xB4
    u8 unkB8[0xBE - 0xB8];
    u16 startBlock;            // 0xBE
    CARDFileInfo *fileInfo;      // 0xC0
    CARDCallback extCallback;    // 0xC4
    CARDCallback txCallback;     // 0xC8
    CARDCallback exiCallback;    // 0xCC
    CARDCallback apiCallback;    // 0xD0
    CARDCallback xferCallback;   // 0xD4
    CARDCallback eraseCallback;  // 0xD8
    CARDCallback unlockCallback; // 0xDC
    u8 alarm[0x28];            // 0xE0
    u8 unk108[0x110 - 0x108];
} CARDControl;

extern CARDControl __CARDBlock[2];

u16 *__CARDGetFatBlock(CARDControl *card);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
s32 __CARDRead(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);



typedef void (*CARDCallback)(s32 chan, s32 result);
s32 CARDRead(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset)
{
    s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDReadAsync(fileInfo, buf, length, offset, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(fileInfo->chan);
}
