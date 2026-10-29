#include "common.h"


void ForceIPLReset(void) {
    extern s32 m_ilpreset;
    extern s32 m_reset;
    m_ilpreset = 1;
    m_reset = 1;
}

void Main_Shutdown_NGC(void) {

}

void TextureManager_ReleaseCache(void) {

}

