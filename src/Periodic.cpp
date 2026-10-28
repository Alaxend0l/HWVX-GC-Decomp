#include "Periodic.h"

extern "C" void memset(void *, int, unsigned long);
extern "C" long LGUpdateForceEffect(unsigned long handle, unsigned char *desc);
extern "C" void OSReport(const char *msg, ...);

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on


Periodic::Periodic()
{
}

extern "C" long LGDownloadForceEffect(unsigned long device, unsigned long *effectId, unsigned char *desc);
long Periodic::DownloadForce(long chan, long effect, unsigned long &device, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel)
{
struct
{
unsigned char type;
unsigned long duration;
unsigned long startDelay;
unsigned char magnitude;
unsigned short direction;
unsigned short period;
unsigned short phase;
short offset;
unsigned long attackTime;
unsigned long fadeTime;
unsigned char attackLevel;
unsigned char fadeLevel;
unsigned char pad[2];
} desc;
long result = 0;
if (effectIds[chan][effect] != -1)
{
Destroy(chan, effect);
}
if (device != -1)
{
memset(&desc, 0, sizeof(desc));
desc.type = type;
desc.duration = duration;
desc.startDelay = startDelay;
desc.magnitude = magnitude;
desc.direction = direction;
desc.period = period;
desc.phase = phase;
desc.offset = offset;
desc.attackTime = attackTime;
desc.fadeTime = fadeTime;
desc.attackLevel = attackLevel;
desc.fadeLevel = fadeLevel;
result = LGDownloadForceEffect(device, &effectIds[chan][effect], (unsigned char *)&desc);
if (result < 0)
{
OSReport("ERROR: DownloadForce(periodic force) on channel %d returned %d\n", chan, result);
effectIds[chan][effect] = 0xFFFFFFFF;
}
}
else
{
OSReport("ERROR: Trying to download a periodic force to channel %d but wheel has not been opened.\n", chan);
}
return result;
}

long Periodic::UpdateForce(long chan, long effect, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char axis, unsigned short magnitude, unsigned short offset, unsigned short phase, short period, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel)
{
struct
{
unsigned char type;
unsigned long duration;
unsigned long startDelay;
unsigned char axis;
unsigned short magnitude;
unsigned short offset;
unsigned short phase;
short period;
unsigned long attackTime;
unsigned long fadeTime;
unsigned char attackLevel;
unsigned char fadeLevel;
} desc;
long result;
memset(&desc, 0, sizeof(desc));
desc.type = type;
desc.duration = duration;
desc.startDelay = startDelay;
desc.axis = axis;
desc.magnitude = magnitude;
desc.offset = offset;
desc.phase = phase;
desc.period = period;
desc.attackTime = attackTime;
desc.fadeTime = fadeTime;
desc.attackLevel = attackLevel;
desc.fadeLevel = fadeLevel;
result = LGUpdateForceEffect(effectIds[chan][effect], (unsigned char *)&desc);
if (result < 0)
{
OSReport("ERROR: UpdateForce(periodic force) on channel %d returned %d\n", chan, result);
effectIds[chan][effect] = 0xFFFFFFFF;
}
return result;
}

