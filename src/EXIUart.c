#include "common.h"


u32 OSGetConsoleType(void);
extern u32 BarnacleEnabled;
extern u32 Enabled;
extern s32 Chan;
extern u32 Dev;
s32 InitializeUART(u32 baudRate) {
    if (BarnacleEnabled == 0xA5FF005A) {
        return 0;
    }
    if (!(OSGetConsoleType() & 0x10000000)) {
        Enabled = 0;
        return 2;
    }
    Chan = 0;
    Dev = 1;
    Enabled = 0xA5FF005A;
    return 0;
}
