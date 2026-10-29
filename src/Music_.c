#include "common.h"


extern const f32 g_noteTab[128];
#pragma optimization_level 1
u32 GetNoteRate_(u32 rate, u8 note) {
    return rate * g_noteTab[note];
}
