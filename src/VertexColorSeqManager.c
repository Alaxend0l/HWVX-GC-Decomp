#include "common.h"


s32 ActionVertexColorSeqChangeMode(s32 *data) {
    u32 count = data[1];
    u32 i;

    for (i = 0; i < count; i++) {
        u8 *seq = (u8 *)data[i + 2];
        switch (data[0]) {
        case 0:
            *(u32 *)(seq + 8) = 0;
            break;
        case 1:
            *(u32 *)(seq + 8) = 1;
            break;
        case 2:
            if (*(u32 *)(seq + 8) == 0) {
                *(u32 *)(seq + 8) = 1;
            } else {
                *(u32 *)(seq + 8) = 0;
            }
            break;
        }
    }
    return 1;
}
