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

static void cb(s32 result, DVDCommandBlock *block) {
    if (result > 0) {
        switch (status) {
        case 0:
            status = 1;
            DVDReadAbsAsyncForBS(block, bb2, 0x20, 0x420, cb);
            break;
        case 1:
            status = 2;
            DVDReadAbsAsyncForBS(block, bb2->FSTAddress, (bb2->FSTLength + 0x1F) & ~0x1F, bb2->FSTPosition, cb);
            break;
        }
    } else if (result == -1) {
    } else if (result == -4) {
        status = 0;
        DVDReset();
        DVDReadDiskID(block, idTmp, cb);
    }
}
