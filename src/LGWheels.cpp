#include "common.h"
#include "LGWheels.h"

u8 IsOpened__6WheelsFl(Wheels *wheels, s32 chan);
void GenerateNonLinValues__6WheelsFlUc(Wheels *wheels, s32 chan, u8 coefficient);
void SetOverallForceGain__5ForceFRUll(Force *force, u32 *effectId, s32 gain);
s32 GetOverallForceGain__5ForceFRUl(Force *force, u32 *effectId);
void *memcpy(void *dst, const void *src, u32 n);
u8 SameSpringForceParams__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient) {
    u8 *p = (u8 *)wheels + chan * 4;

    if (*(s8 *)(p + 0x13CE) == offset && p[0x13CF] == saturation && *(s16 *)(p + 0x13D0) == coefficient) {
        return 1;
    }
    return 0;
}

u8 LGWheels::IsOpened(s32 chan)
{
	return limbo.IsOpened(chan);
}

u8 LGWheels::PedalsConnected(s32 chan)
{
	return limbo.PedalsConnected(chan);
}

void LGWheels::SetOverallForceGain(s32 chan, s32 gain)
{
	limb.SetOverallForceGain(limbo.overallGainId[chan], gain);
}

s32 LGWheels::GetOverallForceGain(s32 chan)
{
	return limb.GetOverallForceGain(limbo.overallGainId[chan]);
}

void LGWheels::GenerateNonLinValues(s32 chan, u8 coefficient)
{
	limbo.GenerateNonLinValues(chan, coefficient);
	memcpy(nonLinValues, limbo.nonLinValues, 0x800);
}

