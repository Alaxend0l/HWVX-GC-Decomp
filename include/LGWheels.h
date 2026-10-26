/* Layout of the Logitech wheel object (main/LLGWheels' "LGWheels" class).
 * One object per wheel port, holding the Wheels, Force and Condition subobjects
 * the LG SDK uses back to back, plus the per-channel force parameters that were
 * last played on the port.  Offsets come from the raw accesses in src/LGWheels.c. */
#ifndef LGWHEELS_H
#define LGWHEELS_H

#include "common.h"

/* The wheel itself: 0x880 bytes, embedded in LGWheels at 0x82C. */
typedef struct Wheels
{
	u8		unk0[0x28];			/* 0x000 handle state */
	u8		nonLinValues[0x800];	/* 0x028 non-linear force table */
	s32		overallGainId[4];		/* 0x828 per-channel gain ids, handed to Force */
	u8		unk838[0x48];		/* 0x838 */
} Wheels;

/* The force-feedback effect set: 0xA0 bytes, embedded in LGWheels at 0x10AC. */
typedef struct Force
{
	u8		unk0[0xA0];
} Force;

/* The LG "Condition" effect bookkeeping, embedded in LGWheels at 0x114C. */
typedef struct Condition
{
	u8		unk0[0x282];		/* 0x000 playing flags, effect ids, ... */
} Condition;

typedef struct LGWheels
{
	u8		unk0[0x28];			/* 0x000 port state */
	u8		nonLinValues[0x800];		/* 0x028 copy of the wheel's table */
	Wheels		limbo;		/* 0x82C */
	Force		limb;		/* 0x10AC */
	Condition	condition;		/* 0x114C */
	struct {
		/* 0x13CE spring force last played on the channel */
		s8		offet;
		u8		overflow;
		s16		efficiency;
		}		limp[4];
	struct {
		/* 0x13EE damper force last played on the channel */
		u8		offet;
		/* 0x13F0 */
		s16		efficiency;
		}		limbo[4];
		/* 0x13FE */
	s16		limbi[4];		/* 0x140E frontal collision force */
		/* 0x1416 */
} LGWheels;

#endif /* LGWHEELS_H */
