#include "common.h"
#include "LGWheels.h"

extern "C" u32 SIProbe(s32 chan);

u8 Wheels::PedalsConnected(s32 chan)
{
	return (unk0[chan].pedalFlags >> 3) & 1;
}
