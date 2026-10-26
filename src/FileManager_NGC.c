#include "common.h"


s32 stricmp(const char *s1, const char *s2) {
    char c1, c2;

    while (*s1) {
        c2 = tolower(*(u8 *)s2);
        c1 = tolower(*(u8 *)s1);
        if (c1 != c2) {
            c1 = tolower(*(u8 *)s1);
            c2 = tolower(*(u8 *)s2);
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return *s2 ? -1 : 0;
}

s32 FileManager_GetDeviceLastError(u32 device) {
    extern s32 m_cd_drive_status;
    s32 chan;

    if (device == 11 || device == 12) {
        chan = device - 11;
        if (chan < 0) {
            return 0;
        }
        switch (m_memcardstatus_803C7970[chan]) {
        case -6:
            return -3;
        case -128:
        case -5:
            return -4;
        case -2:
            return -6;
        case -9:
            return -1;
        case -100:
            return -2;
        case -13:
            return -5;
        }
    }
    if (device == 1) {
        switch (m_cd_drive_status) {
        case -1:
            return -11;
        case 11:
            return -10;
        case 6:
            return -9;
        case 5:
            return -7;
        case 4:
            return -8;
        }
    }
    return 0;
}

s32 FileManager_GetDeviceFreeSpace(s32 device, u32 *freeBytes, u32 *totalBytes) {
    freeBytes[1] = 0x32000;
    freeBytes[0] = 0;
    totalBytes[1] = 0x32000;
    totalBytes[0] = 0;
    return 1;
}

s8 FileManager_IsDeviceFormatted(s32 device) {
    return m_memcardstatus_803C7970[device - 11] == 0;
}

void File_Close(s32 handle) {
    extern struct { s32 fileInfo; u8 pad[0x100]; } g_filedata_PC[4];
    s32 DVDClose(void *fileInfo);
    void MemCard_Close__Fl(s32 device);
    s32 slot = (s8)(u8)((u32)handle >> 8);
    s32 idx = (s8)(u8)handle;
    s32 device = (s8)(u8)((u32)handle >> 16);

    if (device >= 11) {
        MemCard_Close__Fl(device);
        g_files[idx].slot = -1;
        return;
    }
    if (!(idx >= 0 && idx < 4 && slot == g_files[idx].slot)) {
        return;
    }
    DVDClose((void *)g_filedata_PC[idx].fileInfo);
    Deallocate(g_filedata_PC[idx].fileInfo);
    g_filedata_PC[idx].fileInfo = 0;
    g_files[idx].slot = -1;
}

u32 FileManager_GetDeviceStatus(u32 device) {
    s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectorSize);
    s32 CARDMount(s32 chan, void *workArea, void *detachCallback);
    s32 CARDCheck(s32 chan);
    s32 CARDGetEncoding(s32 chan, u16 *encode);
    s32 CARDFreeBlocks(s32 chan, s32 *byteNotUsed, s32 *filesNotUsed);
    s32 CARDOpen(s32 chan, const char *fileName, CARDFileInfo *fileInfo);
    s32 CARDClose(CARDFileInfo *fileInfo);
    s32 CARDUnmount(s32 chan);
    u16 encoding;
    s32 memSize;
    s32 sectorSize;
    s32 bytesNotUsed;
    s32 filesNotUsed;
    CARDFileInfo fileInfo;
    s32 result;

    if (device == 11 || device == 12) {
        result = CARDProbeEx(device - 11, &memSize, &sectorSize);
        if (result != 0) {
            m_memcardstatus_803C7970[device - 11] = result;
            return 3;
        }
        if (sectorSize != 0x2000) {
            m_memcardstatus_803C7970[device - 11] = -100;
            return 3;
        }
        result = CARDMount(device - 11, CardWorkArea, 0);
        if (result == -6) {
            if (m_memcardstatus_803C7970[device - 11] != -5) {
                result = CARDCheck(device - 11);
            } else {
                result = m_memcardstatus_803C7970[device - 11];
            }
        }
        if ((m_memcardstatus_803C7970[device - 11] = result) != 0) {
            return 3;
        }
        CARDGetEncoding(device - 11, &encoding);
        if (encoding != 0) {
            m_memcardstatus_803C7970[device - 11] = -13;
            CARDUnmount(device - 11);
            return 3;
        }
        CARDFreeBlocks(device - 11, &bytesNotUsed, &filesNotUsed);
        result = CARDOpen(device - 11, g_appInfo[3], &fileInfo);
        if (result == 0) {
            CARDClose(&fileInfo);
        }
        CARDUnmount(device - 11);
        if (result == -4 && (bytesNotUsed == 0 || filesNotUsed == 0)) {
            m_memcardstatus_803C7970[device - 11] = -9;
            return 3;
        }
        return 1;
    }
    if (device == 0) {
        return 1;
    }
    return device == 1;
}

s32 MemCard_Open__FUlPCScUlPl(u32 device, const char *fileName, u32 flags, s32 *unused) {
    u32 chan;
    CARDFileInfo *fileInfo;
    s32 *blockSize;

    chan = device - 11;
    if (FileManager_GetDeviceStatus(device) == 3) {
        return 0;
    }
    m_memcardstatus_803C7970[chan] = CARDMount(chan, CardWorkArea, 0);
    OSReport("CARDMount\n");
    if (m_memcardstatus_803C7970[chan] < 0) {
        OSReport("CARD error %d\n", m_memcardstatus_803C7970[chan]);
        return 0;
    }
    blockSize = &memCardBlockSize[chan];
    CARDGetSectorSize(chan, blockSize);
    fileInfo = &memCardfileInfo[chan];
    m_memcardstatus_803C7970[chan] = CARDOpen(chan, fileName, fileInfo);
    if (m_memcardstatus_803C7970[chan] == 0) {
        return 1;
    }
    if (m_memcardstatus_803C7970[chan] == -4 && (flags & 1)) {
        m_memcardstatus_803C7970[chan] = CARDCreate(chan, fileName, *blockSize, fileInfo);
        if (m_memcardstatus_803C7970[chan] < 0) {
            m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
            OSReport("CARD error %d\n", m_memcardstatus_803C7970[chan]);
            return 0;
        }
        OSReport("CARDCreate\n");
    } else {
        m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
        return 0;
    }
    return 1;
}

void MemCard_Close__Fl(s32 device) {
    s32 chan = device - 11;

    m_memcardstatus_803C7970[chan] = CARDClose(&memCardfileInfo[chan]);
    m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
    OSReport("CARDClose\n");
}

s32 MemCard_Read__FUllPv(u32 device, s32 size, void *data) {
    u8 buffer[0x2000] __attribute__((aligned(32)));
    s32 chan = device - 11;

    m_memcardstatus_803C7970[chan] = CARDRead(&memCardfileInfo[chan], buffer, memCardBlockSize[chan], 0);
    memcpy(data, buffer + 0x840, size);
    return size;
}

s32 MemCard_Write__FUllPv(u32 id, s32 size, void *data) {
    typedef struct CARDStat {
        char fileName[32];
        u32 length;
        u32 time;
        u8 gameName[4];
        u8 company[2];
        u8 bannerFormat;
        u8 __padding;
        u32 iconAddr;
        u16 iconFormat;
        u16 iconSpeed;
        u32 commentAddr;
        u32 offsetBanner;
        u32 offsetBannerTlut;
        u32 offsetIcon[8];
        u32 offsetIconTlut;
        u32 offsetData;
    } CARDStat;
    s32 CARDWrite(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset);
    s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat);
    s32 CARDSetStatus(s32 chan, s32 fileNo, CARDStat *stat);
    extern u8 rawData_800DB404[0x800];
    CARDStat stat;
    u8 buffer[0x2000] __attribute__((aligned(32)));
    s32 chan = id - 11;

    memset(buffer, 0, 0x40);
    sprintf((char *)buffer, "Hot Wheels Velocity X Save");
    sprintf((char *)buffer + 0x20, "Game Data");
    memcpy(buffer + 0x40, rawData_800DB404, 0x800);
    memcpy(buffer + 0x840, data, size);
    m_memcardstatus_803C7970[chan] = CARDWrite(&memCardfileInfo[chan], buffer, memCardBlockSize[chan], 0);
    if (m_memcardstatus_803C7970[chan] < 0) {
        if (m_memcardstatus_803C7970[chan] == -5) {
            CARDDelete(chan, g_appInfo[3]);
        }
        m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
        OSReport("CARD error %d\n", m_memcardstatus_803C7970[chan]);
        return 0;
    }
    OSReport("CARDWrite\n");
    m_memcardstatus_803C7970[chan] = CARDGetStatus(chan, memCardfileInfo[chan].fileNo, &stat);
    stat.commentAddr = 0;
    stat.iconAddr = 0x40;
    stat.bannerFormat = (u8)((stat.bannerFormat & ~3) | 0);
    stat.bannerFormat = (u8)((stat.bannerFormat & ~4) | 0);
    stat.iconFormat = (u16)((stat.iconFormat & ~3) | 2);
    stat.iconSpeed = (u16)((stat.iconSpeed & ~3) | 3);
    stat.iconSpeed = (u16)((stat.iconSpeed & ~(3 << 2)) | (0 << 2));
    m_memcardstatus_803C7970[chan] = CARDSetStatus(chan, memCardfileInfo[chan].fileNo, &stat);
    return 1;
}

void LidHandler_Init(void) {
    s32 DVDOpen(const char *name, void *fileInfo);
    extern u32 m_isopen_803C7984;
    extern u32 m_curtime_803C7988;
    extern u8 m_fileInfo_803867E0[0x3C];

    if (m_isopen_803C7984 == 0) {
        DVDOpen("bsfile", m_fileInfo_803867E0);
        m_isopen_803C7984 = 1;
        m_curtime_803C7988 = 0;
    }
}

void LidHandler_Heartbeat(void) {
    s32 DVDGetCommandBlockStatus(void *block);
    s32 DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void *callback, s32 prio);
    extern u8 cd_test_block[0x20];
    extern s32 g_iDeltaTimeMS;
    extern u32 m_curtime_803C7988;
    extern u8 m_fileInfo_803867E0[0x3C];

    m_curtime_803C7988 += g_iDeltaTimeMS;
    if (m_curtime_803C7988 > 500) {
        if (DVDGetCommandBlockStatus(m_fileInfo_803867E0) == 0) {
            DVDReadAsyncPrio(m_fileInfo_803867E0, cd_test_block, 0x20, 0, NULL, 2);
            m_curtime_803C7988 = 0;
        }
    }
}

