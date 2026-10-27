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
