#include "common.h"

typedef struct CARDControl {
    u8 unk0[0x30];
    u8 task[0x50];   // 0x30 DSPTaskInfo
    void *workArea;  // 0x80
    u8 unk84[0x110 - 0x84];
} CARDControl;

extern CARDControl __CARDBlock[2];

void DSPSendMailToDSP(u32 mail);
u32 DSPCheckMailToDSP(void);
