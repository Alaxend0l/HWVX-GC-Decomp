#include "common.h"

typedef struct OSModuleQueue {
    struct OSModuleInfo *head;
    struct OSModuleInfo *tail;
} OSModuleQueue;

OSModuleQueue __OSModuleInfoList : 0x800030C8;
const void *__OSStringTable : 0x800030D0;

void __OSModuleInit(void) {
    __OSModuleInfoList.head = __OSModuleInfoList.tail = 0;
    __OSStringTable = 0;
}
