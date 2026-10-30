#include "Condition.h"


Condition::Condition()
{
}

extern "C" void OSReport(const char *msg, ...);
extern "C" void memset(void *, int, unsigned long);
extern "C" long LGDownloadForceEffect(unsigned long device, unsigned long *effectId, unsigned char *desc);
#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
long Condition::DownloadForce(long chan, long effect, unsigned long &device, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char saturationPos, unsigned char saturationNeg, short coefficientPos, short coefficientNeg)
{
struct
{
unsigned char type;
unsigned long duration;
unsigned long startDelay;
struct
{
signed char offset;
unsigned char deadband;
unsigned char saturationPos;
unsigned char saturationNeg;
short coefficientPos;
short coefficientNeg;
} condition[2];
unsigned char pad[8];
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
desc.condition[0].offset = offset;
desc.condition[0].deadband = deadband;
desc.condition[0].saturationPos = saturationPos;
desc.condition[0].saturationNeg = saturationNeg;
desc.condition[0].coefficientPos = coefficientPos;
desc.condition[0].coefficientNeg = coefficientNeg;
desc.condition[1].offset = offset;
desc.condition[1].deadband = deadband;
desc.condition[1].saturationPos = saturationPos;
desc.condition[1].saturationNeg = saturationNeg;
desc.condition[1].coefficientPos = coefficientPos;
desc.condition[1].coefficientNeg = coefficientNeg;
result = LGDownloadForceEffect(device, &effectIds[chan][effect], (unsigned char *)&desc);
if (result < 0)
{
OSReport("ERROR: DownloadForce(condition force) on channel %d returned %d\n", chan, result);
effectIds[chan][effect] = 0xFFFFFFFF;
}
}
else
{
OSReport("ERROR: Trying to download a condition force to channel %d but wheel has not been opened.\n", chan);
}
return result;
}

extern "C" long LGUpdateForceEffect(unsigned long effectId, unsigned char *desc);
long Condition::UpdateForce(long chan, long effect, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char saturationPos, unsigned char saturationNeg, short coefficientPos, short coefficientNeg)
{
struct
{
unsigned char type;
unsigned long duration;
unsigned long startDelay;
struct
{
signed char offset;
unsigned char deadband;
unsigned char saturationPos;
unsigned char saturationNeg;
short coefficientPos;
short coefficientNeg;
} condition[2];
unsigned char pad[8];
} desc;
long result;
memset(&desc, 0, sizeof(desc));
desc.type = type;
desc.duration = duration;
desc.startDelay = startDelay;
desc.condition[0].offset = offset;
desc.condition[0].deadband = deadband;
desc.condition[0].saturationPos = saturationPos;
desc.condition[0].saturationNeg = saturationNeg;
desc.condition[0].coefficientPos = coefficientPos;
desc.condition[0].coefficientNeg = coefficientNeg;
desc.condition[1] = desc.condition[0];
result = LGUpdateForceEffect(effectIds[chan][effect], (unsigned char *)&desc);
if (result < 0)
{
OSReport("ERROR: UpdateForce(condition force) on channel %d returned %d\n", chan, result);
effectIds[chan][effect] = 0xFFFFFFFF;
}
return result;
}

