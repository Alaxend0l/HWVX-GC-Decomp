#include "common.h"

typedef struct ApploaderHeader {
    char date[16];
    u32 entry;
    u32 size;
    u32 rebootSize;
    u32 reserved2;
} ApploaderHeader;

typedef struct RebootCmd {
    u8 pad[0xC];
    volatile s32 state;
    u8 pad2[0x30 - 0x10];
} RebootCmd;

extern ApploaderHeader Header_802EEE00;
extern u32 SaveStart_803C7258;
extern u32 SaveEnd_803C725C;
extern volatile s32 Prepared_803C7260;

s32 OSDisableInterrupts(void);
s32 OSEnableInterrupts(void);
void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
void DVDInit(void);
s32 DVDSetAutoInvalidation(s32 autoInval);
s32 __DVDPrepareResetAsync(void *callback);
s32 DVDCheckDisk(void);
void __OSDoHotReset(u32 resetCode);
u32 __OSMaskInterrupts(u32 mask);
u32 __OSUnmaskInterrupts(u32 mask);
s32 DVDReadAbsAsyncForBS(void *block, void *addr, s32 length, s32 offset, void *callback);
void ICInvalidateRange(void *addr, u32 nBytes);
void Run(void *entry);

void Callback(void) {
    Prepared_803C7260 = 1;
}
static inline void ReadApploader(RebootCmd *dvdCmd, void *addr, u32 offset, u32 numBytes) {
    while (!Prepared_803C7260) {
    }
    DVDReadAbsAsyncForBS(dvdCmd, addr, numBytes, offset + 0x2440, NULL);
    for (;;) {
        switch (dvdCmd->state) {
        case 0:
            break;
        case 1:
        default:
            continue;
        case -1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            __OSDoHotReset(*(u32 *)0x817FFFFC);
            continue;
        }
        break;
    }
}
