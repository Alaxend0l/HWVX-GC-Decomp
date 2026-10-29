#include "common.h"


void Audio_Destroy_(void) {
    M2C_UNK AIReset();
    M2C_UNK ARQReset();
    M2C_UNK ARReset();
    M2C_UNK AXQuit();
    AXQuit();
    AIReset();
    ARQReset();
    ARReset();
}

void Audio_Update_(void) {
    void Audio_Update(s32);
    Audio_Update(0xD05);
}

void MemCpyDone(void) {
    extern s32 g_arqWait;
    g_arqWait = 0;
}
