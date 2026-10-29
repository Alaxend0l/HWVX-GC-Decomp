#include "common.h"

extern volatile u16 *__cpReg;

void GXClearGPMetric(void) {
    __cpReg[2] = 4;
}
