#include "common.h"


s32 DVDSetAutoInvalidation(s32 arg0) {
    extern s32 autoInvalidation;
    s32 temp_r0;

    temp_r0 = autoInvalidation;
    autoInvalidation = arg0;
    return temp_r0;
}

s32 DVDGetCurrentDiskID(void) {
    return 0x80000000;
}
