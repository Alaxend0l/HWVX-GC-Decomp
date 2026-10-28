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

/* 4-byte .sdata2 literal used by GbaCheckJBResult__Fl (GBAWrite payload). */
extern u32 g_GBAWritePayload;

void JBCallback__Fll(s32 chan, s32 result)
{
    Param[chan].state = 2;
    Param[chan].result = result;
}

s32 GBAGetStatus(s32, u8 *);
s32 GBARead(s32, void *, u8 *);
s32 GBAWrite(s32, void *, u8 *);
void GameSettings_GameBoyConnected(void);
void GbaOpen__Fl(s32);

#pragma push
#pragma readonly_strings on
#pragma dont_inline off
#pragma auto_inline off
#pragma inline_depth(4)

static inline BOOL GbaReadGameCode(s32 chan)
{
    u32 code;

    if (GBAGetStatus(chan, &Param[chan].status) != 0 || Param[chan].status != 0x28)
    {
        return FALSE;
    }
    if (GBARead(chan, (u8 *)&code, &Param[chan].status) != 0)
    {
        return FALSE;
    }
    if (code == 0x41483845)
    {
        Param[chan].unk58 = code;
    }
    else
    {
        Param[chan].unk58 = 0;
        return FALSE;
    }
    return TRUE;
}

static inline BOOL GbaSendGameCode(s32 chan)
{
    if (GBAGetStatus(chan, &Param[chan].status) != 0 || Param[chan].status != 0x20)
    {
        return FALSE;
    }
    if (GBAWrite(chan, (u8 *)"GHWE", &Param[chan].status) != 0)
    {
        return FALSE;
    }
    if (GBAGetStatus(chan, &Param[chan].status) != 0 || Param[chan].status != 0x30)
    {
        return FALSE;
    }
    if (Param[chan].status == 0x30)
    {
        gLinkFinished = 1;
        GameSettings_GameBoyConnected();
    }
    return TRUE;
}

static inline BOOL GbaHandshake(s32 chan)
{
    u32 cmd;
    u32 i;

    if (!GbaReadGameCode(chan))
    {
        return FALSE;
    }
    if (Param[chan].unk58 != 0x41483845)
    {
        return FALSE;
    }
    if (!GbaSendGameCode(chan))
    {
        return FALSE;
    }
    cmd = 0x60000000;
    if (GBAWrite(chan, (u8 *)&cmd, &Param[chan].status) != 0 || (Param[chan].status & 0x30) != 0x30)
    {
        return FALSE;
    }
    for (i = 0; i < 0x20; i += 4)
    {
        if (GBAGetStatus(chan, &Param[chan].status) != 0 || Param[chan].status != 0x38)
        {
            return FALSE;
        }
        if (GBARead(chan, (u8 *)&Param[chan].recv + i, &Param[chan].status) != 0 ||
            (Param[chan].status & 0x30) != 0x30)
        {
            return FALSE;
        }
    }
    return TRUE;
}

void GbaCheckJBResult__Fl(s32 chan)
{
s32 GBARead(s32, void *, u8 *);
s32 GBAWrite(s32, void *, u8 *);
s32 GBAReset(s32, u8 *);
s32 GBAJoyBootAsync(s32, s32, s32, u8 *, s32, u8 *, void (*)(s32, s32));
u32 OSGetTick(void);
void OSReport(const char *, ...);
void *memset(void *dst, s32 c, u32 n);
s32 buf;
s32 *p58;

if (Param[chan].result == 0)
{
    if (GBAReset(chan, &Param[chan].status) != 0)
    {
        return;
    }
    memset(&Param[chan].send, 0, 0x20);
    Param[chan].send.unk0 = 1;
    Param[chan].send.chan = chan;
    Param[chan].send.unk3 = 1;
    Param[chan].send.hostId = HostId;
    Param[chan].unk58 = 0;
    if (GBAGetStatus(chan, &Param[chan].status) == 0 && Param[chan].status == 0x28)
    {
        if (GBARead(chan, &buf, &Param[chan].status) == 0)
        {
            p58 = &Param[chan].unk58;
            if (Param[chan].unk58 == 0x41483845)
            {
                *p58 = buf;
            }
            else
            {
                *p58 = 0;
            }
        }
        else
        {
            p58 = &Param[chan].unk58;
            *p58 = 0;
        }
        if (*p58 == 0x41483845)
        {
            if (GBAGetStatus(chan, &Param[chan].status) == 0 && Param[chan].status == 0x20)
            {
                GBAWrite(chan, (u8 *)"GHWE", &Param[chan].status);
            }
        }
    }
    return;
}
if (Param[chan].result == 3)
{
    if ((Param[chan].status & 0xA) == 0xA)
    {
        if ((Param[chan].status & 0x30) != 0x10)
        {
            GBARead(chan, &buf, &Param[chan].status);
        }
        return;
    }
    if (GBAGetStatus(chan, &Param[chan].status) != 0)
    {
        return;
    }
    buf = 0;
    if (Param[chan].status != 0x28)
    {
        if (GBARead(chan, &buf, &Param[chan].status) != 0)
        {
            buf = 0;
        }
    }
    else if (buf == 0x41483845)
    {
        Param[chan].unk58 = buf;
    }
    else
    {
        Param[chan].unk58 = 0;
    }
    if (buf == 0x41483845)
    {
        if (GBAGetStatus(chan, &Param[chan].status) != 0)
        {
            return;
        }
        if (Param[chan].status != 0x20)
        {
            return;
        }
        if (GBAWrite(chan, (u8 *)"GHWE", &Param[chan].status) != 0)
        {
            return;
        }
        if (GBAGetStatus(chan, &Param[chan].status) != 0)
        {
            return;
        }
        if (Param[chan].status != 0x30)
        {
            return;
        }
        if (Param[chan].type != 0x47484500)
        {
            return;
        }
        if (GBAGetStatus(chan, &Param[chan].status) != 0)
        {
            return;
        }
        if (Param[chan].status != 0x38)
        {
            return;
        }
        if (GBARead(chan, &Param[chan].unk60, &Param[chan].status) != 0)
        {
            return;
        }
        if ((Param[chan].unk60 & 0x30) != 0x30)
        {
            return;
        }
        Param[chan].state = 4;
        return;
    }
    GBAWrite(chan, &Param[chan].unk64, &Param[chan].status);
    buf = Param[chan].unk64;
    if ((buf & 0x30) != 0x30)
    {
        return;
    }
    if (GBAGetStatus(chan, &Param[chan].status) != 0)
    {
        return;
    }
    if (Param[chan].status != 0x38)
    {
        return;
    }
    if (GBARead(chan, &Param[chan].unk60, &Param[chan].status) != 0)
    {
        return;
    }
    if ((Param[chan].unk60 & 0x30) != 0x30)
    {
        return;
    }
    Param[chan].state = 4;
    return;
}
if ((OSGetTick() - Param[chan].startTick) > ((*(u32 *)0x800000F8 / 4) / 1000) * 4000)
{
    OSReport("Timeout to JOYBOOT (chan=%d).\n", chan);
    Param[chan].state = 0;
    return;
}
Param[chan].state = 1;
GBAJoyBootAsync(chan, chan * 2, 2, Param[chan].program, Param[chan].length,
                &Param[chan].status, JBCallback__Fll);
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

