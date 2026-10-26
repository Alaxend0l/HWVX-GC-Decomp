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

