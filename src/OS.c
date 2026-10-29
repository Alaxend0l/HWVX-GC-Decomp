#include "common.h"

#pragma peephole off

struct BootInfo {
    u32 field_0[10];
    u32 field_28;
    u32 field_2C;
    u32 field_30;
    u32 field_34;
    u8 pad_38[0xBC];
    u32 field_F4;
    u8 pad_F8[0x30E6 - 0xF8];
    u16 field_30E6;
    u8 field_30E8;
    u8 field_30E9;
};

struct BootInfo *BootInfo;
u32 *BI2DebugFlag;
u32 BI2DebugFlagHolder;
BOOL __OSIsGcam;
static BOOL AreWeInitialized;
void **OSExceptionTable;
u32 __OSSavedRegionEnd;
u32 __OSSavedRegionStart;
BOOL __OSInIPL;
s64 __OSStartTime;

static u32 DriveInfo[8] __attribute__((aligned(32)));
static u32 DriveBlock[12];

extern u32 __DVDLongFileNameFlag;
extern u32 __PADSpec;
extern u8 __ArenaLo[];
extern u8 __ArenaHi[];
extern u8 _stack_addr[];

volatile u32 __PIRegs[12] : 0xCC003000;


u32 OSGetConsoleType(void)
{
    if (BootInfo == NULL || BootInfo->field_2C == 0)
    {
        return 0x10000002;
    }
    return BootInfo->field_2C;
}

void ClearArena(void)
{
  extern u32 __OSSavedRegionStart;
  extern u32 __OSSavedRegionEnd;
  extern u32 OSGetArenaHi(void);
  extern u32 OSGetArenaLo(void);
  extern s32 OSGetResetCode(void);
  extern void memset();
  u32 regionEnd;
  u32 regionStart;
  u32 *saved;
  if ((((u32) OSGetResetCode()) + 0x80000000u) != 0)
  {
    __OSSavedRegionStart = 0;
    __OSSavedRegionEnd = 0;
    memset((void *) OSGetArenaLo(), 0, OSGetArenaHi() - OSGetArenaLo());
    return;
  }
  saved = (u32 *) 0x812FDFEC;
  regionStart = saved[1];
  regionEnd = saved[0];
  __OSSavedRegionStart = regionStart;
  __OSSavedRegionEnd = regionEnd;
  if (regionStart == 0)
  {
    memset((void *) OSGetArenaLo(), 0, OSGetArenaHi() - OSGetArenaLo());
    return;
  }
  if (OSGetArenaLo() >= __OSSavedRegionStart)
  {
    return;
  }
  if (OSGetArenaHi() <= __OSSavedRegionStart)
  {
    memset((void *) OSGetArenaLo(), 0, OSGetArenaHi() - OSGetArenaLo());
    return;
  }
  memset((void *) OSGetArenaLo(), 0, __OSSavedRegionStart - OSGetArenaLo());
  if (OSGetArenaHi() <= __OSSavedRegionEnd)
  {
    return;
  }
  memset((void *) __OSSavedRegionEnd, 0, OSGetArenaHi() - __OSSavedRegionEnd);
}

void InquiryCallback(s32 result, u32 *block)
{
    switch (block[3])
    {
    case 0:
        *(u16 *)0x800030E6 = (u16)(0x8000 | ((u16 *)DriveInfo)[1]);
        break;
    default:
        *(u16 *)0x800030E6 = 1;
        break;
    }
}

u32 OSGetArenaHi(void);
u32 OSGetArenaLo(void);
void OSSetArenaLo(void *);
void OSSetArenaHi(void *);
void OSReport(const char *, ...);
void DCInvalidateRange(void *, u32);
s32 DVDInquiryAsync(void *, void *, void *);
void DVDInit(void);
void ClearArena(void);
s32 OSDisableInterrupts(void);
s32 OSEnableInterrupts(void);
void PPCDisableSpeculation(void);
void PPCSetFpNonIEEEMode(void);
void OSExceptionInit(void);
void __OSInitSystemCall(void);
void OSInitAlarm(void);
void __OSModuleInit(void);
void __OSInterruptInit(void);
void __OSContextInit(void);
void __OSCacheInit(void);
void EXIInit(void);
void SIInit(void);
void __OSInitSram(void);
void __OSThreadInit(void);
void __OSInitAudioSystem(void);
void __OSInitMemoryProtection(void);
void EnableMetroTRKInterrupts(void);
u32 PPCMfhid2(void);
void PPCMthid2(u32);
s64 __OSGetSystemTime(void);
void *__OSSetInterruptHandler(s16, void *);
void __OSResetSWInterruptHandler(s16, void *);
void OSInit(void)
{
    u32 consoleType;
    void *bi2;

    if (AreWeInitialized == FALSE)
    {
        AreWeInitialized = TRUE;
        __OSStartTime = __OSGetSystemTime();
        OSDisableInterrupts();
        PPCDisableSpeculation();
        PPCSetFpNonIEEEMode();
        BI2DebugFlag = NULL;
        BootInfo = (struct BootInfo *)0x80000000;
        __DVDLongFileNameFlag = 0;
        bi2 = (void *)BootInfo->field_F4;
        if (bi2 != NULL)
        {
            BI2DebugFlag = (u32 *)((u8 *)bi2 + 0xC);
            __PADSpec = *(u32 *)((u8 *)bi2 + 0x24);
            *(u8 *)0x800030E8 = (u8)*BI2DebugFlag;
            *(u8 *)0x800030E9 = (u8)__PADSpec;
        }
        else if (BootInfo->field_34 != 0)
        {
            BI2DebugFlagHolder = *(u8 *)0x800030E8;
            BI2DebugFlag = &BI2DebugFlagHolder;
            __PADSpec = *(u8 *)0x800030E9;
        }
        __DVDLongFileNameFlag = 1;

        OSSetArenaLo((BootInfo->field_30 == 0) ? (void *)__ArenaLo : (void *)BootInfo->field_30);
        if (BootInfo->field_30 == 0 && BI2DebugFlag != NULL && *BI2DebugFlag < 2)
        {
            OSSetArenaLo((void *)(((u32)_stack_addr + 0x1F) & ~0x1F));
        }
        OSSetArenaHi((BootInfo->field_34 == 0) ? (void *)__ArenaHi : (void *)BootInfo->field_34);

        OSExceptionInit();
        __OSInitSystemCall();
        OSInitAlarm();
        __OSModuleInit();
        __OSInterruptInit();
        __OSSetInterruptHandler(0x16, __OSResetSWInterruptHandler);
        __OSContextInit();
        __OSCacheInit();
        EXIInit();
        SIInit();
        __OSInitSram();
        __OSThreadInit();
        __OSInitAudioSystem();
        PPCMthid2(PPCMfhid2() & 0xBFFFFFFF);

        if (BootInfo->field_2C & 0x10000000)
        {
            BootInfo->field_2C = 0x10000004;
        }
        else
        {
            BootInfo->field_2C = 1;
        }
        BootInfo->field_2C += (__PIRegs[11] & 0xF0000000) >> 28;

        if (__OSInIPL == FALSE)
        {
            __OSInitMemoryProtection();
        }

        OSReport("\nDolphin OS $Revision: 54 $.\n");
        OSReport("Kernel built : %s %s\n", "Jun  5 2002", "02:09:12");
        OSReport("Console Type : ");

        consoleType = OSGetConsoleType();
        if ((consoleType & 0x10000000) == 0)
        {
            OSReport("Retail %d\n", consoleType);
        }
        else
        {
            switch (consoleType)
            {
            case 0x10000000:
                OSReport("Mac Emulator\n");
                break;
            case 0x10000001:
                OSReport("PC Emulator\n");
                break;
            case 0x10000002:
                OSReport("EPPC Arthur\n");
                break;
            case 0x10000003:
                OSReport("EPPC Minnow\n");
                break;
            default:
                OSReport("Development HW%d\n", consoleType - 0x10000003);
                break;
            }
        }

        OSReport("Memory %d MB\n", BootInfo->field_28 >> 20);
        OSReport("Arena : 0x%x - 0x%x\n", OSGetArenaLo(), OSGetArenaHi());

        if (BI2DebugFlag != NULL && *BI2DebugFlag >= 2)
        {
            EnableMetroTRKInterrupts();
        }

        ClearArena();
        OSEnableInterrupts();

        if (__OSInIPL == FALSE)
        {
            DVDInit();
            if (__OSIsGcam)
            {
                *(u16 *)0x800030E6 = 0x9000;
                return;
            }
            DCInvalidateRange(DriveInfo, sizeof(DriveInfo));
            DVDInquiryAsync(DriveBlock, DriveInfo, InquiryCallback);
        }
    }
}

void *__OSSetExceptionHandler(u8 exception, void *handler) {
    void *oldHandler;

    oldHandler = OSExceptionTable[exception];
    OSExceptionTable[exception] = handler;
    return oldHandler;
}

#pragma peephole off
void *__OSGetExceptionHandler(u8 exception) {
    return OSExceptionTable[exception];
}

extern volatile u32 __DIRegs[16] : 0xCC006000;
u8 __OSGetDIConfig(void) {
    return (u8)(__DIRegs[9] & 0xFF);
}

