#include "common.h"
#include "LGWheels.h"

extern "C" void *memcpy(void *dst, const void *src, u32 n);
extern "C" void LGInit(void);
extern "C" void OSReport(const char *msg, ...);
LGWheels::LGWheels()
{
	savedGain = 0xFF;
	unk13CD = 0;
	InitLGLibrary();
	airborne[0] = 0;
	airborne[1] = 0;
	airborne[2] = 0;
	airborne[3] = 0;
}

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
		for (i = 0; i < 8; i++)
		{
			condition.ids[chan][i] = -1;
			condition.playing[chan][i] = 0;
			constantForce.ids[chan][i] = -1;
			constantForce.playing[chan][i] = 0;
			periodicForce.ids[chan][i] = -1;
			periodicForce.playing[chan][i] = 0;
			rampForce.ids[chan][i] = -1;
			rampForce.playing[chan][i] = 0;
		}
	}
}

#pragma push
#pragma use_lmw_stmw on
void LGWheels::PlaySpringForce(s32 chan, s8 offset, u8 saturation, s16 coefficient)
{
	if (limbo.IsOpened(chan))
	{
	 if (condition.playing[chan][0])
		{
		 if (!SameSpringForceParams(chan, offset, saturation, coefficient))
			{
			 if (condition.UpdateForce(chan, 0, 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient) >= 0)
				{
				 limp[chan].offet = offset;
				 limp[chan].overflow = saturation;
				 limp[chan].efficiency = coefficient;
				}
			}
		}
	 else
		{
		 if (condition.ids[chan][0] == -1)
			{
			 if (condition.DownloadForce(chan, 0, limbo.overallGainId[chan], 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient) >= 0)
				{
				 limp[chan].offet = offset;
				 limp[chan].overflow = saturation;
				 limp[chan].efficiency = coefficient;
				}
			 ((Force *) &condition)->Start(chan, 0);
			}
		 else if (SameSpringForceParams(chan, offset, saturation, coefficient))
			{
			 ((Force *) &condition)->Start(chan, 0);
			}
		 else
			{
			 if (condition.UpdateForce(chan, 0, 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient) >= 0)
				{
				 limp[chan].offet = offset;
				 limp[chan].overflow = saturation;
				 limp[chan].efficiency = coefficient;
				}
			 ((Force *) &condition)->Start(chan, 0);
			}
		}
	}
 else
	{
	 OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
	}
}
#pragma pop

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

u8 LGWheels::SameConstantForceParams(s32 chan, s16 magnitude, u16 direction)
{
	if (constantParams[chan].force == magnitude && constantParams[chan].direction == direction)
	{
		return 1;
	}
	return 0;
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

u8 LGWheels::SameDamperForceParams(s32 chan, u8 coefficient, s16 offset)
{
	if (limper[chan].offet == coefficient && limper[chan].efficiency == offset)
	{
		return 1;
	}
	return 0;
}

#pragma push
#pragma use_lmw_stmw on
void LGWheels::PlayFrontalCollisionForce(s32 chan, u8 force)
{
	if (limbo.IsOpened(chan))
	{
	 if (periodicForce.playing[chan][0])
		{
		 if (SameFrontalCollisionForceParams(chan, force))
			{
		 ((Force *) &periodicForce)->Start(chan, 0);
			}
		 else
			{
			 if (periodicForce.UpdateForce(chan, 0, 3, 150, 0, force, 90, 75, 0, 0, 0, 20, 0, 0) >= 0)
				{
				 limbi[chan] = force;
				}
		 ((Force *) &periodicForce)->Start(chan, 0);
			}
		}
	 else
		{
		 if (periodicForce.ids[chan][0] == -1)
			{
			 if (periodicForce.DownloadForce(chan, 0, limbo.overallGainId[chan], 3, 150, 0, force, 90, 75, 0, 0, 0, 20, 0, 0) >= 0)
				{
				 limbi[chan] = force;
				}
		 ((Force *) &periodicForce)->Start(chan, 0);
			}
		 else if (SameFrontalCollisionForceParams(chan, force))
			{
		 ((Force *) &periodicForce)->Start(chan, 0);
			}
		 else
			{
			 if (periodicForce.UpdateForce(chan, 0, 3, 150, 0, force, 90, 75, 0, 0, 0, 20, 0, 0) >= 0)
				{
				 limbi[chan] = force;
				}
		 ((Force *) &periodicForce)->Start(chan, 0);
			}
		}
	}
 else
	{
	 OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
	}
}

u8 LGWheels::SameFrontalCollisionForceParams(s32 chan, s16 force)
{
	return limbi[chan] == force;
}

void LGWheels::PlayCarAirborne(s32 chan)
{
	if (limbo.IsOpened(chan))
	{
		if (GetOverallForceGain(chan))
		{
			savedGain = GetOverallForceGain(chan);
			SetOverallForceGain(chan, 0);
			airborne[chan] = 1;
		}
	}
	else
	{
		OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
	}
}

void LGWheels::StopCarAirborne(s32 chan)
{
	if (limbo.IsOpened(chan))
	{
		SetOverallForceGain(chan, savedGain);
		airborne[chan] = 0;
	}
	else
	{
		OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
	}
}

#pragma pop

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

