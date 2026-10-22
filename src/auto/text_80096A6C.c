// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"

void __end__catch(void *arg0) {
    M2C_UNK (*temp_r12)(u32, M2C_UNK);
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0);
    if (temp_r0 != 0U) {
        temp_r12 = M2C_FIELD(arg0, M2C_UNK (**)(u32, M2C_UNK), 8);
        if (temp_r12 != NULL) {
            temp_r12(temp_r0, -1);
        }
    }
}
