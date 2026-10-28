#include "common.h"

typedef struct JoyBusPacket {
    u8 unk0;
    u8 chan;
    u8 unk2;
    u8 unk3;
    s32 hostId;
    u32 key;
    s32 unkC;
    u8 unk10[0x20 - 0x10];
} JoyBusPacket;

typedef struct JoyBusParam {
    s32 state;
    u8 status;
    u8 unk5[0x8 - 0x5];
    u8 *program;
    s32 length;
    s32 result;
    JoyBusPacket send;
    JoyBusPacket recv;
    u32 key;
    s32 unk58;
    u8 unk5C[0x60 - 0x5C];
    u32 unk60;
    u32 unk64;
    u32 type;
    u32 startTick;
    s32 unk70;
} JoyBusParam;

u8 ClientImage[0x40000];
JoyBusParam Param[4];
u8 GBAfileInfo[0x3C];
s32 ImageSize;
s32 HostId;
s32 DiskId;
s32 gLinkFinished;

void JBCallback__Fll(s32 chan, s32 result)
{
    Param[chan].state = 2;
    Param[chan].result = result;
}

void GBAConnectivityInit__Fv(void)
{
    s32 DVDOpen(const char *name, void *fileInfo);
    BOOL DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void *callback, s32 prio);
    void GBACloseCallback__FlP11DVDFileInfo(s32 result, void *fileInfo);
    u32 length;

    gLinkFinished = 0;
    DVDOpen("client_thread.bin", GBAfileInfo);
    length = *(u32 *)(GBAfileInfo + 0x34);
    ImageSize = length;
    DVDReadAsyncPrio(GBAfileInfo, ClientImage, (length + 0x1F) & ~0x1F, 0, GBACloseCallback__FlP11DVDFileInfo, 2);
}

void GBAConnectivityLoop__Fv(void)
{
    void GbaProc__Fv();

    GbaProc__Fv();
}

