#include "common.h"
#include "LGWheels.h"

extern "C" void *memcpy(void *dst, const void *src, u32 n);
extern "C" void LGInit(void);
void LGWheels::InitLGLibrary(void)
{
	LGInit();
}

void LGWheels::ReadAll()
{
	s16 chan;
	s32 i;

	chan = limbo.ReadAll();
	memcpy(this, &limbo, 0x28);
	if (chan != -1)
	{
		i = 0;
		while (i < 8)
		{
			condition.ids[chan][i] = -1;
			condition.playing[chan][i] = 0;
			constantForce.ids[chan][i] = -1;
			constantForce.playing[chan][i] = 0;
			periodicForce.ids[chan][i] = -1;
			periodicForce.playing[chan][i] = 0;
			rampForce.ids[chan][i] = -1;
			rampForce.playing[chan][i] = 0;
			++i;
		}
	}
}

u8 LGWheels::SameSpringForceParams(s32 chan, s8 offset, u8 saturation, s16 coefficient)
{
	if (limp[chan].offet == offset && limp[chan].overflow == saturation && limp[chan].efficiency == coefficient)
	{
		{
			return 1;
		}
	}
	return 0;
}

extern "C" void OSReport(const char *msg, ...);
#pragma push
#pragma use_lmw_stmw on
void LGWheels::PlayConstantForce(s32 chan, s16 magnitude, u16 direction)
{
	if (limbo.IsOpened(chan))
	{
		if (constantForce.playing[chan][0])
		{
			if (!SameConstantForceParams(chan, magnitude, direction))
			{
				if (constantForce.UpdateForce(chan, 0, -1, 0, magnitude, direction, 0, 0, 0, 0) >= 0)
				{
				 constantParams[chan].force = magnitude;
				 constantParams[chan].direction = direction;
				}
			}
		}
	 else
		{
			if (constantForce.ids[chan][0] == -1)
			{
				if (constantForce.DownloadForce(chan, 0, limbo.overallGainId[chan], -1, 0, magnitude, direction, 0, 0, 0, 0) >= 0)
				{
				 constantParams[chan].force = magnitude;
				 constantParams[chan].direction = direction;
				}
			 ((Force *) &constantForce)->Start(chan, 0);
			}
		 else if (SameConstantForceParams(chan, magnitude, direction))
			{
			 ((Force *) &constantForce)->Start(chan, 0);
			}
		 else
			{
				if (constantForce.UpdateForce(chan, 0, -1, 0, magnitude, direction, 0, 0, 0, 0) >= 0)
				{
				 constantParams[chan].force = magnitude;
				 constantParams[chan].direction = direction;
				}
			 ((Force *) &constantForce)->Start(chan, 0);
			}
		}
	}
 else
	{
	OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
	}
}

#pragma push
#pragma use_lmw_stmw on
void LGWheels::PlayDamperForce(s32 chan, u8 coefficient, s16 offset)
{
	if (condition.playing[chan][2])
	{
	 return;
	}
	if (limbo.IsOpened(chan))
	{
	 if (condition.playing[chan][1])
		{
			if (!SameDamperForceParams(chan, coefficient, offset))
			{
			 if (condition.UpdateForce(chan, 1, 8, -1, 0, 0, 0, coefficient, coefficient, offset, offset) >= 0)
				{
				 limper[chan].offet = coefficient;
				 limper[chan].efficiency = offset;
				}
			}
		}
	 else
		{
		 if (condition.ids[chan][1] == -1)
			{
			 if (condition.DownloadForce(chan, 1, limbo.overallGainId[chan], 8, -1, 0, 0, 0, coefficient, coefficient, offset, offset) >= 0)
				{
				 limper[chan].offet = coefficient;
				 limper[chan].efficiency = offset;
				}
			 ((Force *) &condition)->Start(chan, 1);
			}
			 else if (SameDamperForceParams(chan, coefficient, offset))
			{
			 ((Force *) &condition)->Start(chan, 1);
			}
			 else
			{
			 if (condition.UpdateForce(chan, 1, 8, -1, 0, 0, 0, coefficient, coefficient, offset, offset) >= 0)
				{
				 limper[chan].offet = coefficient;
				 limper[chan].efficiency = offset;
				}
			 ((Force *) &condition)->Start(chan, 1);
			}
		}
	}
 else
	{
		OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
	}
}
#pragma pop

#pragma pop
u8 LGWheels::IsOpened(s32 chan)
{
	return limbo.IsOpened(chan);
}

u8 LGWheels::IsConnected(s32 chan)
{
	return limbo.IsConnected(chan);
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

