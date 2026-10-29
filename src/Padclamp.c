#include "common.h"

typedef struct PADStatus {
    u16 button;
    s8 stickX;
    s8 stickY;
    s8 substickX;
    s8 substickY;
    u8 triggerLeft;
    u8 triggerRight;
    u8 analogA;
    u8 analogB;
    s8 err;
} PADStatus;

typedef struct PADClampRegion {
    u8 minTrigger;
    u8 maxTrigger;
    s8 minStick;
    s8 maxStick;
    s8 xyStick;
    s8 minSubstick;
    s8 maxSubstick;
    s8 xySubstick;
} PADClampRegion;

static PADClampRegion ClampRegion = {
    30, 180, 15, 72, 40, 15, 59, 31,
};

void ClampStick(s8 *px, s8 *py, s8 max, s8 xy, s8 min);
void PADClamp(PADStatus *status) {
    int i;

    for (i = 0; i < 4; i++, status++) {
        if (status->err != 0) {
            continue;
        }
        ClampStick(&status->stickX, &status->stickY, ClampRegion.maxStick, ClampRegion.xyStick, ClampRegion.minStick);
        ClampStick(&status->substickX, &status->substickY, ClampRegion.maxSubstick, ClampRegion.xySubstick, ClampRegion.minSubstick);
        if (status->triggerLeft <= ClampRegion.minTrigger) {
            status->triggerLeft = 0;
        } else {
            if (ClampRegion.maxTrigger < status->triggerLeft) {
                status->triggerLeft = ClampRegion.maxTrigger;
            }
            status->triggerLeft -= ClampRegion.minTrigger;
        }
        if (status->triggerRight <= ClampRegion.minTrigger) {
            status->triggerRight = 0;
        } else {
            if (ClampRegion.maxTrigger < status->triggerRight) {
                status->triggerRight = ClampRegion.maxTrigger;
            }
            status->triggerRight -= ClampRegion.minTrigger;
        }
    }
}
