#include "common.h"


void GBAConnectivityInit__Fv(void)
{
    extern u8 GBAfileInfo_803C6C10[0x3C];
    extern u8 ClientImage_80386A40[0x40000];
    extern s32 gLinkFinished;
    extern u32 ImageSize_803C79A0;
    s32 DVDOpen(const char *name, void *fileInfo);
    BOOL DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void *callback, s32 prio);
    void GBACloseCallback__FlP11DVDFileInfo(s32 result, void *fileInfo);
    u32 length;

    gLinkFinished = 0;
    DVDOpen("client_thread.bin", GBAfileInfo_803C6C10);
    length = *(u32 *)(GBAfileInfo_803C6C10 + 0x34);
    ImageSize_803C79A0 = length;
    DVDReadAsyncPrio(GBAfileInfo_803C6C10, ClientImage_80386A40, (length + 0x1F) & ~0x1F, 0, GBACloseCallback__FlP11DVDFileInfo, 2);
}

void GBAConnectivityLoop__Fv(void)
{
    void GbaProc__Fv();

    GbaProc__Fv();
}

