#include "common.h"


void FileManager_Destroy(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 FileManager_GetChecksum(u32 size, u8 *data) {
    u32 i;
    s32 sum = 0;

    for (i = 0; i < size; i++) {
        sum += data[i] + 1;
    }
    return sum;
}

