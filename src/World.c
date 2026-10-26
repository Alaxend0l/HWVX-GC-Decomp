#include "common.h"


void World_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 ActionItemChangeMode(u32 *action) {
    u32 i;
    u8 *item;
    u8 *obj;

    for (i = 0; i < action[1]; i++) {
        item = (u8 *)action[i + 2];
        if (action[0] == 2) {
            if (*(u32 *)(item + 0x28) == 1) {
                *(u32 *)(item + 0x28) = 0;
                obj = *(u8 **)(item + 0x24);
                if (obj != NULL) {
                    (*(void (**)(void *))(*(u8 **)(obj + 0x8C) + 4))(obj);
                    *(u32 *)(item + 0x18) = *(u32 *)(item + 0x14);
                    *(u8 **)(item + 0x24) = NULL;
                }
            } else {
                *(u32 *)(item + 0x28) = 1;
                *(u32 *)(item + 0x18) = 1;
            }
        } else {
            *(u32 *)(item + 0x28) = action[0];
            if (action[0] == 1) {
                *(u32 *)(item + 0x18) = 1;
            }
            if (action[0] == 0) {
                obj = *(u8 **)(item + 0x24);
                if (obj != NULL) {
                    (*(void (**)(void *))(*(u8 **)(obj + 0x8C) + 4))(obj);
                    *(u32 *)(item + 0x18) = *(u32 *)(item + 0x14);
                    *(u8 **)(item + 0x24) = NULL;
                }
            }
        }
    }
    return 1;
}

