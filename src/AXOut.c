#include "common.h"


void __AXDSPDoneCallback(void) {
    extern s32 Prepared_803C7260;
    Prepared_803C7260 = 1;
}
