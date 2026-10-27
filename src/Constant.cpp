#include "Constant.h"


Constant::Constant()
{
}
extern "C" void OSReport(const char *msg, ...);
extern "C" long LGUpdateForceEffect(unsigned long handle, unsigned char *desc);
extern "C" void memset(void *, int, unsigned long);

struct ForceDesc
{
	unsigned char type;
	unsigned long duration;
	unsigned long startDelay;
	short magnitude;
	unsigned short direction;
	unsigned long attackTime;
	unsigned long fadeTime;
	unsigned char attackLevel;
	unsigned char fadeLevel;
	unsigned char pad[10];
};
long Constant::UpdateForce(long chan, long effect, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel)
{
	ForceDesc desc;
	long result;

	memset(&desc, 0, sizeof(desc));

	desc.duration = duration;
	desc.startDelay = startDelay;
	desc.magnitude = magnitude;
	desc.direction = direction;
	desc.attackTime = attackTime;
	desc.attackLevel = attackLevel;

	// fade
		// call
		// error
		// return
}
