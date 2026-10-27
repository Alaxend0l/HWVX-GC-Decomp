#ifndef CONDITION_H
#define CONDITION_H
#include "Force.h"

class Condition : public Force
{
public:
	Condition();

	long DownloadForce(long chan, long effect, unsigned long &device, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char saturationPos, unsigned char saturationNeg, short coefficientPos, short coefficientNeg);
};

#endif
