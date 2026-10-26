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

