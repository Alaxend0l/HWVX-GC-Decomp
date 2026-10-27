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
		// FIXA
		// FIXB
		// FIXC
		// FIXD
		// FIXE
		// FIXF
		// FIXG
		// FIXH
		// FIXI
		// FIXJ
		// FIXK
		// FIXL
		// FIXM
		// FIXN
		// FIXO
		// FIXP
		// FIXQ
		// FIXR
		// FIXS
		// FIXT
		// FIXU
		// FIXV
		// FIXW
		// FIXX
		// FIXY
		// FIXZ
		// FIX0
		// FIX1
		// FIX2
		// FIX3
		// FIX4
		// FIX5
		// FIX6
		// FIX7
		// FIX8
		// FIX9
		// FIX10
		// FIX11
		// FIX12
		// FIX13
		// FIX14
		// FIX15
		// FIX16
		// FIX17
		// FIX18
		// FIX19
		// FIX20
		// FIX21
		// FIX22
		// FIX23
		// FIX24
		// FIX25
		// FIX26
		// FIX27
		// FIX28
		// FIX29
		// FIX30
		// FIX31
		// FIX32
		// FIX33
		// FIX34
		// FIX35
		// FIX36
		// FIX37
		// FIX38
		// FIX39
		// FIX40
		// FIX41
		// FIX42
		// FIX43
		// FIX44
		// FIX45
		// FIX46
		// FIX47
		// FIX48
		// FIX49
		// FIX50
		// FIX51
		// FIX52
		// FIX53
		// FIX54
		// FIX55
		// FIX56
		// FIX57
		// FIX58
		// FIX59
		// FIX60
		// FIX61
		// FIX62
		// FIX63
		// FIX64
		// FIX65
		// FIX66
		// FIX67
		// FIX68
		// FIX69
		// FIX70
		// FIX71
		// FIX72
		// FIX73
		// FIX74
		// FIX75
		// FIX76
		// FIX77
		// FIX78
		// FIX79
		// FIX80
		// FIX81
		// FIX82
		// FIX83
		// FIX84
		// FIX85
		// FIX86
		// FIX87
		// FIX88
		// FIX89
		// FIX90
		// FIX91
		// FIX92
		// FIX93
		// FIX94
		// FIX95
		// FIX96
		// FIX97
		// FIX98
		// FIX99
		// FIXA0
		// FIXA1
		// FIXA2
		// FIXA3
		// FIXA4
		// FIXA5
		// FIXA6
		// FIXA7
		// FIXA8
		// FIXA9
		// FIXB0
		// FIXB1
		// FIXB2
		// FIXB3
		// FIXB4
		// FIXB5
		// FIXB6
		// FIXB7
		// FIXB8
		// FIXB9
		// FIXC0
		// FIXC1
		// FIXC2
		// FIXC3
		// FIXC4
		// FIXC5
		// FIXC6
		// FIXC7
		// FIXC8
		// FIXC9
		// FIXD0
		// FIXD1
		// FIXD2
		// FIXD3
		// FIXD4
		// FIXD5
		// FIXD6
		// FIXD7
		// FIXD8
		// FIXD9
		// FIXDE
		// FIXDF
		// FIQUE
		// FIFE
		// FIFE2
		// FIFF
		}
