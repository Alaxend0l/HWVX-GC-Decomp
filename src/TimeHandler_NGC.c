#include "common.h"


void Performance_Start(void) {

}

void Performance_End(void) {

}

void Release_Performance_Get(u32 *arg0) {
    u32 OSGetTick(void);
    *arg0 = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

void Release_Performance_Diff(u32 *arg0) {
    u32 OSGetTick(void);
    extern u32 max_time;
    u32 now;

    now = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
    if (now < *arg0) {
        now += max_time;
    }
    *arg0 = now - *arg0;
}

