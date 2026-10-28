// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"

s32 TRKTargetSupportMask(u8 *mask)
{
    mask[0] = 0x7A;
    mask[1] = 0;
    mask[2] = 0x4F;
    mask[3] = 7;
    mask[4] = 0;
    mask[5] = 0;
    mask[6] = 0;
    mask[7] = 0;
    mask[8] = 0;
    mask[9] = 0;
    mask[0xA] = 0;
    mask[0xB] = 0;
    mask[0xC] = 0;
    mask[0xD] = 0;
    mask[0xE] = 0;
    mask[0xF] = 0;
    mask[0x10] = 1;
    mask[0x11] = 0;
    mask[0x12] = 3;
    mask[0x13] = 0;
    mask[0x14] = 0;
    mask[0x15] = 0;
    mask[0x16] = 0;
    mask[0x17] = 0;
    mask[0x18] = 0;
    mask[0x19] = 0;
    mask[0x1A] = 3;
    mask[0x1B] = 0;
    mask[0x1C] = 0;
    mask[0x1D] = 0;
    mask[0x1E] = 0;
    mask[0x1F] = 0x80;
    return 0;
}

s32 TRKTargetVersions(u8 *targetVersions)
{
    targetVersions[0] = 0;
    targetVersions[1] = 0xA;
    targetVersions[2] = 1;
    targetVersions[3] = 0xA;
    return 0;
}

void fn_80094FB0(void)
{
}
