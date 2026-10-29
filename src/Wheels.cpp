#include "common.h"
#include "LGWheels.h"

#pragma readonly_strings on

extern "C" u32 SIProbe(s32 chan);

extern "C" s32 LGOpen(s32 chan, u32 *handle);
extern "C" s32 LGRead(void *status);
extern "C" void OSReport(const char *msg, ...);
extern "C" void memcpy(void *dest, void *src, unsigned long count);
extern "C" void *memset(void *dest, int c, unsigned long count);
Wheels::Wheels()
{
	s32 i;

	for (i = 0; i < 4; i++)
	{
		overallGainId[i] = -1;
		unk0[i].opensErr = -1;
	}
	memset(&unk848[0x10], 0, 0x28);
}

s16 Wheels::ReadAll(void)
{
	s32 i;

	for (i = 0; i < 4; i++)
	{
		if (IsConnected(i) && overallGainId[i] == -1)
		{
			if (LGOpen(i, &overallGainId[i]) < 0)
			{
				OSReport("ERROR: Could not open wheel on channel %d\n", i);
			}
			else
			{
				unk0[i].opensErr = 0;
			}
			break;
		}
	}
	memcpy((void *)&unk848[0x10], (void *)this, 0x28);
	LGRead(this);
	for (i = 0; i < 4; i++)
	{
		if (unk0[i].opensErr == -1 && overallGainId[i] != -1)
		{
			overallGainId[i] = -1;
			return i;
		}
	}
	return -1;
}

u8 Wheels::IsOpened(s32 chan)
{
	return unk0[chan].opensErr == 0;
}

u8 Wheels::IsConnected(s32 chan)
{
	unk838[chan] = SIProbe(chan);
	switch (unk838[chan])
	{
		case 0x08000000:
			return 1;
		}
		return 0;
	}

u8 Wheels::PedalsConnected(s32 chan)
{
	return (unk0[chan].pedalFlags >> 3) & 1;
}

void Wheels::GenerateNonLinValues(s32 chan, u8 coefficient)
{
	s32 i;

	for (i = 0; i < 256; i++)
	{
		nonLinValues[chan][i] = CalculateNonLinValue(i, coefficient, -128, 127);
	}
}

f32 Wheels::CalculateNonLinValue(s32 input, u8 coefficient, s16 min, s16 max)
{
f32 x;
f32 k;
s32 half;
s32 mid;
k = (f32)coefficient / 100.0f;
x = ((f32)input - 127.5f) / 127.5f;
half = (max - min) / 2;
mid = (max + min) / 2;
return (f32)half * ((1.0f - k) * x + k * (x * (x * x))) + (f32)mid;
}

