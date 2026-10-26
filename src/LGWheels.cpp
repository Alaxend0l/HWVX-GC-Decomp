#include "common.h"
#include "LGWheels.h"

u8 IsOpened__6WheelsFl(Wheels *wheels, s32 chan);
void GenerateNonLinValues__6WheelsFlUc(Wheels *wheels, s32 chan, u8 coefficient);
void SetOverallForceGain__5ForceFRUll(Force *force, u32 *effectId, s32 gain);
s32 GetOverallForceGain__5ForceFRUl(Force *force, u32 *effectId);
void *memcpy(void *dst, const void *src, u32 n);
u8 IsOpened__8LGWheelsFl(LGWheels *wheels, s32 chan)
{
			return IsOpened__6WheelsFl(&wheels->limbo, chan);
		}

void SetOverallForceGain__8LGWheelsFll(LGWheels *wheels, s32 chan, s32 gain)
{
	SetOverallForceGain__5ForceFRUll(&wheels->limb, &wheels->limbo.overallGainId[chan], gain);
}

s32 GetOverallForceGain__8LGWheelsFl(LGWheels *wheels, s32 chan)
{
	return GetOverallForceGain__5ForceFRUl(&wheels->limb, &wheels->limbo.overallGainId[chan]);
}

void GenerateNonLinValues__8LGWheelsFlUc(LGWheels *wheels, s32 chan, u8 coefficient)
{
	GenerateNonLinValues__6WheelsFlUc(&wheels->limbo, chan, coefficient);
	memcpy(wheels->nonLinValues, wheels->limbo.nonLinValues, 0x800);
}

