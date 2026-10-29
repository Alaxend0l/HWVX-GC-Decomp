#include "common.h"

typedef struct AXPROFILE {
    u8 data[0x38];
} AXPROFILE;

static AXPROFILE *__AXProfile;
static u32 __AXMaxProfiles;
static u32 __AXCurrentProfile;
static u32 __AXProfileInitialized;
