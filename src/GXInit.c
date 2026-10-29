#include "common.h"

typedef struct GXTlutRegion {
    u32 unk0[4];
} GXTlutRegion;

typedef struct GXData {
    u8 unk0[0x2D0];
    GXTlutRegion TlutRegions[20];
} GXData;

extern GXData *gx;
