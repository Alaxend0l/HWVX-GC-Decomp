#include "Force.h"
#include "common.h"


extern "C" void OSReport(const char *msg, ...);
extern "C" s32 LGStartForceEffect(u32 handle);
extern "C" s32 LGDestroyForceEffect(u32 handle);
extern "C" s32 LGSetDeviceProperty(u32 handle, u32 property, s32 value);
Force::Force()
{
}

long Force::Start(long chan, long effect)
{
	s32 result = 0;

	if (effectIds[chan][effect] != -1)
	{
	 result = LGStartForceEffect(effectIds[chan][effect]);
	 if (result < 0)
	 {
		 OSReport("ERROR: Failed to start force effect on channel %d\n", chan);
	 }
	 else
	 {
		playing[chan][effect] = 1;
	 }
 }
 else
 {
	 OSReport("ERROR: Trying to start force effect on channel %d but we have an invalid effectid\n", chan);
	 }
 return result;
}

long Force::Destroy(long chan, long effect)
{
	s32 result = 0;

	if (effectIds[chan][effect] != -1)
	{
	 result = LGDestroyForceEffect(effectIds[chan][effect]);
	 if (result < 0)
	 {
		 OSReport("ERROR: Failed to destroy force effect on channel %d\n", chan);
	 }
	 else
	 {
		playing[chan][effect] = 0;
	 }
	 }
 else
 {
	 OSReport("ERROR: Trying to destroy force effect on channel %d but we have an invalid effectid\n", chan);
	 }
	 return result;
}

void Force::SetOverallForceGain(unsigned long &effectId, long gain)
{
	if (LGSetDeviceProperty(effectId, 3, gain) < 0)
	{
	 OSReport("ERROR: Failed to set overall force gain.\n");
	 }
 }

