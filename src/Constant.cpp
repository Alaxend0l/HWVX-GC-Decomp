#include "Constant.h"

extern "C" void OSReport(const char *msg, ...);
extern "C" long LGUpdateForceEffect(unsigned long handle, unsigned char *desc);
extern "C" void memset(void *, int, unsigned long);
extern "C" long LGDownloadForceEffect(unsigned long device, unsigned long *effectId, unsigned char *desc);

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on


Constant::Constant()
{
}

long Constant::DownloadForce(long chan, long effect, unsigned long &device, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel)
{
struct
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
} desc;
long result = 0;
if (effectIds[chan][effect] != -1)
{
Destroy(chan, effect);
}
if (device != -1)
{
memset(&desc, 0, sizeof(desc));
desc.type = 0;
desc.duration = duration;
desc.startDelay = startDelay;
desc.magnitude = magnitude;
desc.direction = direction;
desc.attackTime = attackTime;
desc.attackLevel = attackLevel;
desc.fadeTime = fadeTime;
desc.fadeLevel = fadeLevel;
result = LGDownloadForceEffect(device, &effectIds[chan][effect], (unsigned char *)&desc);
if (result < 0)
{
OSReport("ERROR: DownloadForce(constant force) on channel %d returned %d\n", chan, result);
effectIds[chan][effect] = 0xFFFFFFFF;
}
}
else
{
OSReport("ERROR: Trying to download a constant force to channel %d but wheel has not been opened.\n", chan);
}
return result;
}

extern "C" void OSReport(const char *msg, ...);
extern "C" long LGUpdateForceEffect(unsigned long handle, unsigned char *desc);
extern "C" void memset(void *, int, unsigned long);
extern "C" long LGDownloadForceEffect(unsigned long device, unsigned long *effectId, unsigned char *desc);

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
desc.type = 0;
desc.duration = duration;
desc.startDelay = startDelay;
desc.magnitude = magnitude;
desc.direction = direction;
desc.attackTime = attackTime;
desc.attackLevel = attackLevel;
desc.fadeTime = fadeTime;
desc.fadeLevel = fadeLevel;
result = LGUpdateForceEffect(effectIds[chan][effect], (unsigned char *)&desc);
if (result < 0)
{
OSReport("ERROR: UpdateForce(constant force) on channel %d returned %d\n", chan, result);
effectIds[chan][effect] = 0xFFFFFFFF;
}
return result;
}
