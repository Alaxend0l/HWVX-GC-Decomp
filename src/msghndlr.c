#include "common.h"


extern GeneralGameSettings IsTRKConnected;
s32 GetTRKConnected(void) {
    return IsTRKConnected.difficulty;
}
