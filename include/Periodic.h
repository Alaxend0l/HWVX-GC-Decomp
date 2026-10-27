#ifndef PERIODIC_H
#define PERIODIC_H
#include "Force.h"

class Periodic : public Force
{
public:
	Periodic();
	long UpdateForce(long chan, long effect, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char axis, unsigned short magnitude, unsigned short offset, unsigned short phase, short period, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

#endif
