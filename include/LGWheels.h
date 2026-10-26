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
	struct {
		/* 0x000 per-channel controller state */
		u8		unk0[2];
		/* 0x002 */
		u8		pedalFlags;		/* bit 3 says both pedals are connected */
		/* 0x003 */
		u8		unk3[6];
		/* 0x009 */
		s8		opensErr;		/* 0 once the wheel opened */
		}		unk0[4];			/* 0x000 */
	s16		nonLinValues[4][256];	/* 0x028 non-linear force table */
	u32		overallGainId[4];		/* 0x828 per-channel gain ids, handed to Force (by reference) */
		/* 0x838 */
		u32		unk838[4];		/* device type, as returned by SIProbe */
		/* 0x848 */
		u8		unk848[0x38];
		/* 0x880 */

		/* The wheel's own methods, in main/Wheels. */
		u8		IsConnected(s32 chan);
		/* 0x880 */
		u8		IsOpened(s32 chan);
		/* 0x880 */
		u8		PedalsConnected(s32 chan);
		/* 0x880 */
		f32		CalculateNonLinValue(s32 input, u8 coefficient, s16 min, s16 max);
		/* 0x880 */
		void		GenerateNonLinValues(s32 chan, u8 coefficient);
		/* 0x880 */
} Wheels;

/* The force-feedback effect set: 0xA0 bytes, embedded in LGWheels at 0x10AC. */
typedef struct Force
{
	u8		unk0[0xA0];

		/* The gain one of these effects plays at, in main/Force. */
		void		SetOverallForceGain(u32 &effectId, s32 gain);
		/* 0x0 */
		s32		GetOverallForceGain(u32 &effectId);
		/* 0x0 */
} Force;

/* The effect classes whose methods the port hands to Force: each one is 0xA0
 * bytes and holds the eight effect slots of every channel, with the id of the
 * effect that slot plays (-1 once it is free).  Which effect a class plays is
 * in its name; the bookkeeping they share is this layout. */
typedef struct Condition
{
	u8		playing[4][8];		/* 0x000 is this channel playing this slot? */
	/* 0x020 */
	u32		ids[4][8];		/* 0x020 effect ids, -1 once the slot is free */
	/* 0x0A0 */
} Condition;

typedef struct Constant
{
	u8		playing[4][8];		/* 0x000 is this channel playing this slot? */
	/* 0x020 */
	u32		ids[4][8];		/* 0x020 effect ids, -1 once the slot is free */
	/* 0x0A0 */
} Constant;

typedef struct Periodic
{
	u8		playing[4][8];		/* 0x000 is this channel playing this slot? */
	/* 0x020 */
	u32		ids[4][8];		/* 0x020 effect ids, -1 once the slot is free */
	/* 0x0A0 */
} Periodic;

typedef struct Ramp
{
	u8		playing[4][8];		/* 0x000 is this channel playing this slot? */
	/* 0x020 */
	u32		ids[4][8];		/* 0x020 effect ids, -1 once the slot is free */
	/* 0x0A0 */
} Ramp;

typedef struct LGWheels
{
	u8		unk0[0x28];			/* 0x000 port state */
	u8		nonLinValues[0x800];		/* 0x028 copy of the wheel's table */
		/* 0x828 */
		u32		limbo_pad;		/* unknown; pads the wheel subobject to 0x82C */
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
		}		limper[4];
		/* 0x13FE */
	s16		limbi[4];		/* 0x140E frontal collision force */
		/* 0x1416 */

	/* The port's own methods, defined in src/LGWheels.cpp; these are the ROM's
	 * IsOpened__8LGWheelsFl, PedalsConnected__8LGWheelsFl, SetOverallForceGain__8LGWheelsFll,
	 * GetOverallForceGain__8LGWheelsFl, GenerateNonLinValues__8LGWheelsFlUc and
	 * SameSpringForceParams__8LGWheelsFlScUcs, which hand their arguments to the subobjects. */
	u8		IsOpened(s32 chan);
		/* 0x1416 */
	u8		PedalsConnected(s32 chan);
			/* 0x1416 */
		void		SetOverallForceGain(s32 chan, s32 gain);
		s32		GetOverallForceGain(s32 chan);
		/* 0x1416 */
		void		GenerateNonLinValues(s32 chan, u8 coefficient);
		/* 0x1416 */
		void		InitLGLibrary(void);
		/* 0x1416 */
		/* 0x1416 */
} LGWheels;

#endif /* LGWHEELS_H */
