#include "common.h"


#pragma push
#pragma peephole on
#pragma scheduling off
BOOL DVDClose(void *fileInfo) {
    s32 DVDCancel(void *block);

    DVDCancel(fileInfo);
    return 1;
}
#pragma pop
void cbForReadSync(void) {
    void OSWakeupThread(void *queue);
    extern u32 __DVDThreadQueue[2];

    OSWakeupThread(__DVDThreadQueue);
}
