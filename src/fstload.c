#include "common.h"

typedef struct DVDCommandBlock DVDCommandBlock;
typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *block);

typedef struct DVDBB2 {
    u32 bootFilePosition;
    u32 FSTPosition;
    u32 FSTLength;
    u32 FSTMaxLength;
    void *FSTAddress;
    u32 userPosition;
    u32 userLength;
    u32 padding0;
} DVDBB2;

typedef struct DVDDiskID DVDDiskID;

BOOL DVDReadAbsAsyncForBS(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback);
BOOL DVDReadDiskID(DVDCommandBlock *block, DVDDiskID *diskID, DVDCBCallback callback);
void DVDReset(void);

static s32 status;
static DVDBB2 *bb2;
static DVDDiskID *idTmp;
