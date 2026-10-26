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

s32 FileManager_GetDeviceFreeSpace(s32 device, u32 *freeBytes, u32 *totalBytes) {
    freeBytes[1] = 0x32000;
    freeBytes[0] = 0;
    totalBytes[1] = 0x32000;
    totalBytes[0] = 0;
    return 1;
}

