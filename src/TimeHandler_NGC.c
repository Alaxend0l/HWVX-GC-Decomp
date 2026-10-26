#include "common.h"


void Performance_Start(void) {

}

void Release_Performance_Get(u32 *arg0) {
    u32 OSGetTick(void);
    *arg0 = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

