#include "common.h"


void __OSModuleInit(void) {
    *(u32 *)0x800030CC = 0;
    *(u32 *)0x800030C8 = 0;
    *(u32 *)0x800030D0 = 0;
}
#include "common.h"

typedef struct OSModuleQueue {
    struct OSModuleInfo *head;
    struct OSModuleInfo *tail;
} OSModuleQueue;

OSModuleQueue __OSModuleInfoList : 0x800030C8;
const void *__OSStringTable : 0x800030D0;
