#include "common.h"
#include "LGWheels.h"

extern "C" u32 SIProbe(s32 chan);

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

