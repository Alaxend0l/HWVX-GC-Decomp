/* Functions from 0x800057E0 to 0x80095C8C, found by decomp-toolkit. */
#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe *)0xCC008000)

typedef struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

typedef struct GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;

typedef struct GXLightObjInt {
    u32 reserved[3];
    u32 Color;
} GXLightObjInt;

typedef struct ShaderHolderGX {
    void *gx;
    void *shader;
} ShaderHolderGX;

typedef struct ShaderHolder {
    void *shaders[0x700];
    ShaderHolderGX gx[0x80];
    u32 numShaders;
    u32 numGX;
} ShaderHolder;

typedef struct DVDQueue {
    struct DVDQueue *next;
    struct DVDQueue *prev;
} DVDQueue;

extern DVDQueue WaitingQueue_802EFA18[4];

extern volatile u32 next_803C6DF8;

static void DummySRand(u32 seed) {
    next_803C6DF8 = seed;
}

static u32 DummyRand(void) {
    u32 result;

    next_803C6DF8 = next_803C6DF8 * 1103515245 + 12345;
    result = (next_803C6DF8 >> 16) & 0x7FFF;
    return result;
}

typedef struct TRKStepStatus {
    s32 active;
    u8 type;
    u32 count;
    u32 rangeStart;
    u32 rangeEnd;
} TRKStepStatus;

extern TRKStepStatus gTRKStepStatus_800DA0B4;

static s32 TRKTargetDoStep(void) {
    extern u8 gTRKCPUState[];
    extern u8 gTRKState[];

    gTRKStepStatus_800DA0B4.active = 1;
    *(u32 *)(gTRKCPUState + 0x1F8) |= 0x400;
    if (gTRKStepStatus_800DA0B4.type == 0 || gTRKStepStatus_800DA0B4.type == 0x10) {
        gTRKStepStatus_800DA0B4.count--;
    }
    *(s32 *)(gTRKState + 0x98) = 0;
    return 0;
}

void *ThingManagerList_GetByType(void *list, s32 type);
void WildCardRack_Add(void *rack, u8 *item, s16 count);
void Item_Release(void *item);
void MotionState_Draw(void *state);
f32 MagnitudeVector(void *v);
void *Allocate(u32, u32, const char *);
s32 ViewManager_GetViewFormat(void);
s32 Thing_Init(void *thing, s32 arg1);
void Thing_InitAllSlavesNow(void *thing, void *slaves, u8 player);
s32 Thing_AddToGrid(void *thing, s32 type);
void MatrixCache_Free(u32 key);
void MotionState_Release(s32 state);
s8 ThingManagerList_Get(void *list, void *target);
void Trigger_PauseTACSystem(s8 paused);
u8 Trigger_IsTACSystemPaused(void);
void Thing_Release(s32 thing);
void ShowStats(void);
void LightHolder_Delete(s32 index);
void Gfx_EndScene(void);
void Gfx_FlipBuffers(void);
void Player_Init(void *player, s32 arg1);
s8 IsPointWithinVolume(struct Vector4 *point, u8 *volume, f32 *distSq);

typedef struct InterfaceCallBackData {
    u32 data[17];
} InterfaceCallBackData;

typedef struct InterfacePackEntry {
    char *name;
    u32 id;
    u32 unk8;
} InterfacePackEntry;

extern void *g_pCallBack;
extern InterfaceCallBackData g_CallBackData;
extern InterfacePackEntry g_InterfacePackData[10];
extern u32 g_NumInterfacesPlaying;
extern u8 g_carAVTable[];
extern u8 g_creatureVTable[];

static inline u8 *PlayerManager_PopActive(u8 *pm) {
    u8 *player = M2C_FIELD(pm, u8 **, 0x20D8);
    u8 *next;

    if (player == NULL) {
        return NULL;
    }
    next = M2C_FIELD(player, u8 **, 0xB0);
    M2C_FIELD(pm, u8 **, 0x20D8) = next;
    if (next != NULL) {
        M2C_FIELD(next, u8 **, 0xB4) = NULL;
    }
    M2C_FIELD(player, u8 **, 0xB0) = NULL;
    return player;
}

static inline void PlayerManager_UnlinkActive(u8 *pm, u8 *player) {
    if (M2C_FIELD(player, u8 **, 0xB4) == NULL) {
        M2C_FIELD(pm, u8 **, 0x20D8) = M2C_FIELD(player, u8 **, 0xB0);
    } else {
        M2C_FIELD(M2C_FIELD(player, u8 **, 0xB4), u8 **, 0xB0) = M2C_FIELD(player, u8 **, 0xB0);
    }
    if (M2C_FIELD(player, u8 **, 0xB0) != NULL) {
        M2C_FIELD(M2C_FIELD(player, u8 **, 0xB0), u8 **, 0xB4) = M2C_FIELD(player, u8 **, 0xB4);
        M2C_FIELD(player, u8 **, 0xB0) = NULL;
    }
    M2C_FIELD(player, u8 **, 0xB4) = NULL;
}

static inline void PlayerManager_PushFree(u8 *pm, u8 *player) {
    M2C_FIELD(player, u8 **, 0xB0) = M2C_FIELD(pm, u8 **, 0x20DC);
    M2C_FIELD(player, u8 **, 0xB4) = NULL;
    if (M2C_FIELD(player, u8 **, 0xB0) != NULL) {
        M2C_FIELD(M2C_FIELD(player, u8 **, 0xB0), u8 **, 0xB4) = player;
    }
    M2C_FIELD(pm, u8 **, 0x20DC) = player;
}
void GXSetTevIndirect(s32 tev_stage, s32 ind_stage, s32 format, s32 bias_sel, s32 matrix_sel, s32 wrap_s, s32 wrap_t, u8 add_prev, u8 utc_lod, s32 alpha_sel);

s32 AIGetStreamPlayState(void);
s32 AIGetDSPSampleRate(void);
s32 AIGetStreamSampleRate(void);
u8 AIGetStreamVolLeft(void);
u8 AIGetStreamVolRight(void);

typedef struct GXDataTev {
    u8 unk0[0x130];
    u32 tevc[16];
    u32 teva[16];
    u32 tevKsel[8];
} GXDataTev;

typedef struct GXTexRegionInt {
    u32 image1;
    u32 image2;
    u32 pad;
    u8 is32bMipmap;
    u8 isCached;
} GXTexRegionInt;

typedef struct FileTOCCacheEntry {
    u8 unk0[0x4B4];
    u8 valid;
    u8 unk4B5[0x500 - 0x4B5];
} FileTOCCacheEntry;

extern FileTOCCacheEntry g_fileTOCCache[11];

typedef struct MatrixCacheEntry {
    void *mtx;
    u32 key;
} MatrixCacheEntry;

typedef struct MatrixCacheData {
    MatrixCacheEntry small[0x60];
    MatrixCacheEntry medium[0x80];
    MatrixCacheEntry large[4];
} MatrixCacheData;

extern MatrixCacheData *g_pMatrixCacheData;

typedef struct SharedSlaveHolder {
    void *key;
    void *slaves;
} SharedSlaveHolder;

extern SharedSlaveHolder g_aSharedSlaveHolder[64];

typedef struct ThingManagerDARYL {
    u8 unk0[0x6AD10];
    u8 looker[0xFE60];
    void *activeList;
    void *freeList[19];
} ThingManagerDARYL;

extern ThingManagerDARYL g_thingManager_DARYL;

typedef struct NavData {
    u8 *nodes;
    u32 numNodes;
    void *firstWayPoint;
} NavData;

typedef struct NavHolder {
    NavData *data;
    void **objects;
} NavHolder;

extern NavHolder gs_NavHolder_803C6CD8;

typedef struct ControllerSettings {
    u8 data[0x12];
} ControllerSettings;

typedef struct AudioSettings {
    u8 sfxVolume;
    u8 musicVolume;
    u8 speakerMode;
    u8 unk3;
} AudioSettings;

typedef struct GeneralGameSettings {
    s32 difficulty;
    AudioSettings audio;
    ControllerSettings controllers[18];
    u8 unk14C[4];
} GeneralGameSettings;

extern GeneralGameSettings g_generalGameSettings;

typedef struct CodeVariables {
    s32 playerVehicle[4];
    s32 playerItem[4];
    s32 controllerState[4];
    s32 unk30;
    s32 unk34[4];
    s32 unk44[4];
    s32 unk54[4];
    u8 unk64[4];
    u8 unk68[0xE4 - 0x68];
    u8 unkE4;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    s32 unkF8;
    u8 unkFC;
    s32 unk100;
    s32 unk104;
    u8 unk108;
    u8 unk109[0x111 - 0x109];
    u8 unk111;
    u8 unk112[0x118 - 0x112];
    s32 fileIndex;
    u8 unk11C[3];
    s8 bOverwritingFile;
    u8 unk120[0x134 - 0x120];
} CodeVariables;

extern CodeVariables g_codeVariables;

typedef struct SIControl {
    s32 chan;
    u32 poll;
    u32 inputBytes;
    void *input;
    void *callback;
} SIControl;

typedef struct SIPacket {
    s32 chan;
    void *output;
    u32 outputBytes;
    void *input;
    u32 inputBytes;
    void *callback;
    s64 fire;
} SIPacket;

typedef struct SIPacketBss {
    SIPacket packet[4];
    u8 unk80[0x1B0 - 0x80];
    BOOL inputBufferValid[4];
    u32 inputBuffer[4][2];
    u32 inputBufferVcount[4];
} SIPacketBss;

void __GXFlushTextureState(void);

volatile u32 __SIRegs[64] : 0xCC006400;

extern SIControl Si_800D9E70;
extern SIPacketBss Packet_803096E0;
extern s8 g_bValidFileName_803C6CA4[8];

typedef struct HotWheelsGameData {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3[0xF1];
} HotWheelsGameData;

extern HotWheelsGameData g_HotWheelsGameData;

typedef struct TRKEvent {
    u8 eventType;
    s32 eventID;
    s32 msgBufID;
} TRKEvent;

typedef struct TRKEventQueue {
    s32 mutex;
    s32 count;
    s32 next;
    TRKEvent events[2];
    u32 eventID;
} TRKEventQueue;

extern TRKEventQueue gTRKEventQueue;

s32 TRKTerminateSerialHandler(void);
void TRK_board_display(char *str);
s32 TRKInitializeMutex(void *mutex);
s32 TRKAcquireMutex(void *mutex);
s32 TRKReleaseMutex(void *mutex);
void *TRK_memcpy(void *dst, const void *src, u32 n);
void *TRK_memset(void *dst, s32 val, u32 n);

void Dprintf(const char *fmt, ...);

void ProcessManager_Release(char *name);
void InterfaceInputAnimator_HeartBeat(void);
f32 InterfaceInputAnimator_GetLRCounter(void);
f32 InterfaceInputAnimator_GetUDCounter(void);

s32 UnlockSram(s32 commit, u32 offset);

typedef struct Process {
    void (*init)(void);
    void (*update)(void);
    s32 arg;
    u32 flags;
    s32 unk10;
    s32 unk14;
    char name[16];
} Process;

typedef struct ProcessManager {
    u32 count;
    s32 unk4;
    s32 unk8;
    Process processes[27];
} ProcessManager;

extern ProcessManager g_processManager;

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4 __attribute__((aligned(16)));

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

void XformVec(void *mtx, Vector4 *out, Vector4 *in);
void SubMatrixToQuaternion(void *mtx, Vector4 *quat);
void QuaternionMult(Vector4 *a, Vector4 *b, Vector4 *out);
void Thing_GetDynamicMinMax(void *thing, Vector4 *min, Vector4 *max, f32 expand);

typedef union Matrix4 {
    f32 m[4][4];
    f64 d[8];
    Vector4 v[4];
} Matrix4;

typedef struct Thing {
    Vector4 pos;  /* 0x00 */
    Matrix4 mtx;  /* 0x10 */
    Vector4 quat; /* 0x50 */
} Thing;

void MultiplyMatrix(Matrix4 *a, void *b, Matrix4 *out);

typedef struct WorldData {
    void *world;
    s32 unk4;
    s32 unk8[4];
} WorldData;

extern WorldData g_worldData;
void World_Unload(void);
void *World_GetGameData(void);
void GameRules_SetPlayerOrder(u32 *order, u32 count);

s32 TRKTargetCPUMinorType(void);

typedef struct SoundSource {
    u8 unk0[0x24];
    f32 unk24;
    u8 unk28[0x2C - 0x28];
    s32 voice;
    s32 looping;
    u8 unk34[0x49 - 0x34];
    u8 playing;
    u8 unk4A[0x50 - 0x4A];
} SoundSource;

typedef struct DSCPUType {
    u8 cpuMajor;
    u8 cpuMinor;
    u8 bigEndian;
    u8 defaultTypeSize;
    u8 fpTypeSize;
    u8 extended1TypeSize;
    u8 extended2TypeSize;
} DSCPUType;
s32 TRKTargetAccessMemory(void *data, u32 start, u32 *length, s32 space, s32 read);
s32 TRKAppendBuffer1_ui32(void *b, u32 data);
s32 TRKAppendBuffer1_ui16(void *b, u16 data);
s32 TRKPostEvent(void *event);

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

void TRKResetBuffer(TRKBuffer *b, u8 keepData);
s32 TRKMessageSend(TRKBuffer *b);
s32 TRKAppendBuffer(TRKBuffer *b, const void *data, u32 length);
__declspec(section ".data") extern s32 gTRKBigEndian;

volatile u16 __DSPRegs[32] : 0xCC005000;
volatile u32 __AIRegs[8] : 0xCC006C00;
s32 TRKReadBuffer1_ui8(TRKBuffer *b, u8 *data);
TRKBuffer *TRKGetBuffer(s32 idx);
void TRKReleaseBuffer(s32 idx);
s32 TRKTestForPacket(void);
s32 TRKSetBufferPosition(void *arg0, u32 arg1);
void SetTRKConnected(s32 connected);
s32 GetTRKConnected(void);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

void *TRK_memcpy(void *dst, const void *src, u32 n);

/* TRKReadBuffer, inlined into the msgbuf readers (MetroTRK was built with deferred inlining) */
static inline s32 TRKReadBuffer_inline(TRKBuffer *b, void *data, u32 length) {
    s32 error = 0;
    u32 bytesLeft;

    bytesLeft = b->length - b->position;
    if (length > bytesLeft) {
        error = 0x302;
        length = bytesLeft;
    }
    TRK_memcpy(data, b->data + b->position, length);
    b->position += length;
    return error;
}

static inline s32 TRKReadBuffer1_ui8_inline(TRKBuffer *b, u8 *data) {
    return TRKReadBuffer_inline(b, data, 1);
}

static inline s32 TRKAppendBuffer1_ui32_inline(TRKBuffer *b, u32 data) {
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)&data;
    } else {
        byteData = (u8 *)&data;
        bigEndianData = swapBuffer;
        bigEndianData[0] = byteData[3];
        bigEndianData[1] = byteData[2];
        bigEndianData[2] = byteData[1];
        bigEndianData[3] = byteData[0];
    }
    return TRKAppendBuffer(b, bigEndianData, sizeof(data));
}

typedef s16 __OSInterrupt;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, void *context);
extern __OSInterruptHandler *InterruptHandlerTable_803C7240;

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDControl {
    s32 attached;
    s32 result;
    u8 unk8[0xC - 0x8];
    s32 sectorSize;
    u8 unk10[0x24 - 0x10];
    s32 mountStep;
    u8 unk28[0x80 - 0x28];
    void *workArea;
    u8 *currentDir;
    void *buffer;
    u8 unk8C[0xA8 - 0x8C];
    s32 retry;
    u32 repeat;
    void *addr;
    s32 offset;
    u8 unkB8[0xBE - 0xB8];
    u16 startBlock;
    u8 unkC0[0xCC - 0xC0];
    CARDCallback exiCallback;
    CARDCallback apiCallback;
    CARDCallback xferCallback;
    CARDCallback eraseCallback;
    u8 unkDC[0xE0 - 0xDC];
    u8 alarm[0x28];
    u8 unk108[0x110 - 0x108];
} CARDControl;

extern CARDControl __CARDBlock[2];

typedef struct CARDDirCheck {
    u8 padding0[0x3A];
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

static inline CARDDirCheck *__CARDGetDirCheck(u8 *dir) {
    return (CARDDirCheck *)(dir + 0x1FC0);
}

void __CARDSetDiskID(u8 *id);

typedef struct DSPTaskInfo {
    volatile u32 state;
    volatile u32 priority;
    volatile u32 flags;
    u8 unkC[0x38 - 0xC];
    struct DSPTaskInfo *next;
    struct DSPTaskInfo *prev;
} DSPTaskInfo;

extern DSPTaskInfo *__DSP_first_task;
extern DSPTaskInfo *__DSP_last_task;
extern DSPTaskInfo *__DSP_curr_task;

typedef struct CARDFileInfo {
    s32 chan;
    s32 fileNo;
    s32 offset;
    s32 length;
    u16 iBlock;
    u16 __padding;
} CARDFileInfo;
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, void *callback);
s32 __CARDPutControlBlock(void *card, s32 result);
void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDSync(s32 chan);
s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback);
s32 CARDCreateAsync(s32 chan, const char *fileName, u32 size, CARDFileInfo *fileInfo, CARDCallback callback);
s32 CARDDeleteAsync(s32 chan, const char *fileName, CARDCallback callback);
s32 __CARDFreeBlock(s32 chan, u16 nBlock, CARDCallback callback);

s32 fn_800057E0(void) {
    M2C_UNK fn_80012F7C();
    fn_80012F7C();
    return 1;
}

s32 fn_80005804(s32 *arg0) {
    M2C_UNK fn_80067284(s32);
    fn_80067284(*arg0);
    return 1;
}

s32 fn_8000589C(void) {
    return 1;
}

s32 fn_800058A4(void) {
    return 1;
}

s32 fn_800058AC(void) {
    return 1;
}

s32 fn_800058B4(void) {
    return 1;
}

s32 fn_800058BC(void) {
    return 1;
}

void fn_800058C4(void) {
    M2C_UNK fn_800058E8();
    fn_800058E8();
}

void fn_800058E4(void) {

}

void fn_80006C4C(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 1;
    M2C_FIELD(arg0, s8 *, 0xCC) = 0;
}

void fn_80006C60(void) {
    M2C_UNK fn_80047290();
    fn_80047290();
}

void fn_80006C80(void) {
    M2C_UNK fn_8000704C();
    M2C_UNK fn_80060110();
    fn_8000704C();
    fn_80060110();
}

void fn_80007040(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 0x1F4;
}

s32 fn_8000DE50(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB4);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8000F224(u32 arg0) {
    extern s32 lbl_803C6F3C;
    if (arg0 != 0U) {
        lbl_803C6F3C = arg0 - 1;
    }
}

void fn_8000F3F4(void) {

}

void fn_8000F568(void) {
    M2C_UNK fn_800C2684();
    extern s8 lbl_803C6F2C;
    fn_800C2684();
    lbl_803C6F2C = 1;
}

s32 fn_8000F8CC(void) {
    return 1;
}

s32 fn_800101C0(void) {
    return 1;
}

void fn_800101C8(void) {
    M2C_UNK fn_80010228();
    fn_80010228();
}

void fn_8001059C(s8 arg0) {
    extern s8 lbl_803C6F40;
    lbl_803C6F40 = arg0;
}

void fn_80014258(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

f32 fn_80016954(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), f32 *, 0x20);
}

s32 fn_80018D88(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), s32 *, 0x30);
}

s32 fn_80019594(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8002084C(void *arg0) {
    if (arg0 != NULL) {
        M2C_FIELD(arg0, s32 *, 0x300) = 0;
    }
}

void fn_800209A8(void *arg0) {
    if (arg0 != NULL) {
        M2C_FIELD(arg0, s8 *, 0x9D) = 0;
    }
}

void fn_800236FC(void) {
    extern s32 lbl_803C6F48;
    extern s32 lbl_803C6F4C;
    lbl_803C6F4C = 0;
    lbl_803C6F48 = 0;
}

f32 fn_80023B4C(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0x1BC);
}

s32 fn_8002528C(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB8);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_800252B0(s8 arg0) {
    extern s8 lbl_803C6C78;
    lbl_803C6C78 = arg0;
}

f32 fn_80026CA4(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0xCC);
}

f32 fn_8002793C(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), f32 *, 0x20);
}

s32 fn_80027E58(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), s32 *, 0x30);
}

s32 fn_80027EF8(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 fn_800288F0(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_800290FC(void) {
    M2C_UNK fn_80051164();
    fn_80051164();
}

s32 fn_8002BA44(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xD0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

u8 fn_8002D260(void) {
    extern u8 lbl_803C6F50;
    return lbl_803C6F50;
}

s32 fn_8002D268(void) {
    return 0xB;
}

void fn_800338DC(void) {

}

void fn_80034C20(u16 arg0) {
    extern s32 lbl_803C6FB4;
    lbl_803C6FB4 = (s32) arg0;
}

void fn_8003C4A0(void) {

}

s32 fn_80040640(void) {
    return 1;
}

s32 fn_800409E0(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0xD4);
}

s32 fn_800409E8(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 fn_80041E00(void) {
    return 0;
}

void fn_80041E08(void) {

}

void fn_80042378(void) {

}

void fn_800428E4(void) {

}

void fn_800428E8(void) {
    M2C_UNK fn_800A2838(s32);
    extern s32 lbl_803C6FF8;
    fn_800A2838(lbl_803C6FF8);
    lbl_803C6FF8 = 0;
}

void fn_800435B0(void) {
    extern s8 lbl_803C7000;
    lbl_803C7000 = 1;
}

s32 fn_80043A94(void) {
    extern void *lbl_803C6CD8;
    return M2C_FIELD(lbl_803C6CD8, s32 *, 4);
}

void fn_80045848(void) {
    M2C_UNK fn_80045498();
    fn_80045498();
}

void fn_80047750(void *arg0) {
    void *temp_r5;

    temp_r5 = M2C_FIELD(arg0, void **, 0x2C);
    M2C_FIELD(arg0, void **, 0x2C) = NULL;
    M2C_FIELD(arg0, s8 *, 0x30) = 1;
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, s32 *, 0x94) = 0;
    }
}

s32 fn_80048638(void) {
    return 0;
}

void fn_8004906C(void) {

}

f32 fn_8004B134(void *arg0) {
    return M2C_FIELD(M2C_FIELD(arg0, void **, 0x120), f32 *, 0x2C);
}

s32 fn_8004B140(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x120);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8004B1E8(void) {
    extern void *lbl_803C7030;
    M2C_FIELD(lbl_803C7030, s32 *, 0x2000) = 0;
    M2C_FIELD(lbl_803C7030, s32 *, 0x2004) = 0;
}

void fn_8004B6C8(void) {
    M2C_UNK fn_8004B6E8();
    fn_8004B6E8();
}

void fn_8004B798(void) {
    M2C_UNK fn_800A3D08(s32);
    extern s32 lbl_803C703C;
    fn_800A3D08(lbl_803C703C);
}

void fn_8004B8CC(void) {

}

s32 fn_8004CC74(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8004D198(s32 arg0) {
    extern s8 lbl_803C70B0;
    extern s32 lbl_803C70B4;
    lbl_803C70B4 = arg0;
    lbl_803C70B0 = 1;
}

void fn_8004F984(s32 arg0, s32 arg1, f32 farg0) {
    extern s32 lbl_803C707C;
    extern s32 lbl_803C7080;
    extern f32 lbl_803C7084;
    extern f32 lbl_803C7088;
    switch (arg0) {                                 /* irregular */
    case 0:
        lbl_803C7088 = farg0;
        lbl_803C7080 = arg1;
        return;
    case 1:
        lbl_803C7084 = farg0;
        lbl_803C707C = arg1;
        return;
    }
}

f32 fn_8004F9B0(void) {
    extern f32 lbl_803C7090;
    return lbl_803C7090;
}

f32 fn_8004F9B8(void) {
    extern f32 lbl_803C708C;
    return lbl_803C708C;
}

void fn_8004FAD0(s32 arg0) {
    extern s32 lbl_803C70BC;
    lbl_803C70BC = arg0;
}

s32 fn_8004FC40(void) {
    M2C_UNK fn_80063048();
    fn_80063048();
    return 1;
}

s32 fn_80050514(void) {
    return 1;
}

s32 fn_8005051C(void) {
    return 1;
}

s32 fn_80050524(void) {
    return 0;
}

s32 fn_8005052C(void) {
    return 1;
}

void fn_80050534(s32 arg0, s32 arg1) {
    extern s32 lbl_803C70E0;
    extern s32 lbl_803C70F8;
    lbl_803C70E0 = arg0;
    lbl_803C70F8 = arg1;
}

s32 fn_80050F8C(void) {
    extern void *lbl_803C7100;
    return M2C_FIELD(lbl_803C7100, s32 *, 0xD0);
}

s32 fn_800512DC(void) {
    return 1;
}

s32 fn_800512E4(void) {
    return 1;
}

void fn_800512EC(void) {

}

f32 fn_80051D80(void) {
    extern f32 lbl_803C8554;
    return lbl_803C8554;
}

void fn_80051D88(void) {

}

void fn_80051D8C(void) {

}

s32 fn_80051D90(void *arg0) {
    void *temp_r3;

    temp_r3 = M2C_FIELD(arg0, void **, 0x94);
    if (temp_r3 != NULL) {
        return M2C_FIELD(temp_r3, s32 *, 0x2C);
    }
    return 0;
}

void fn_800532C4(s32 arg0) {
    M2C_UNK fn_80055014(s32);
    fn_80055014(arg0 + 0x88);
}

s32 fn_80053448(void) {
    return -1;
}

void fn_80053E6C(void) {

}

s32 fn_80053E94(void) {
    return 0;
}

void fn_80053F58(void) {

}

u32 fn_80053F5C(void *arg0, u32 arg1) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x20);
    if (temp_r0 == arg1) {
        return M2C_FIELD(arg0, u32 *, 0x24);
    }
    return temp_r0;
}

void fn_80053F78(void **arg0, void *arg1) {
    void *temp_r5;

    M2C_FIELD(arg1, void **, 0xA4) = (void *) *arg0;
    M2C_FIELD(arg1, s32 *, 0xA0) = 0;
    temp_r5 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, void **, 0xA0) = arg1;
    }
    *arg0 = arg1;
}

void fn_800561D0(void) {
    M2C_UNK fn_800561F4();
    M2C_UNK fn_800428E4();
    fn_800561F4();
    fn_800428E4();
}

void fn_8005946C(void) {
    M2C_UNK fn_8005948C();
    fn_8005948C();
}

s8 fn_80059A8C(void) {
    s32 fn_8004D05C();
    return (s8) (fn_8004D05C() == 0);
}

s32 fn_80059AB8(void) {
    return 1;
}

s32 fn_80059AC0(void) {
    return 1;
}

s32 fn_80059AC8(void) {
    return 1;
}

s32 fn_80059AD0(void) {
    return 1;
}

s32 fn_80059AD8(void) {
    return 1;
}

s32 fn_80059AE0(void) {
    return 1;
}

void fn_80059AE8(void) {
    M2C_UNK fn_80059B34();
    fn_80059B34();
}

void fn_80059B08(void) {
    extern s32 lbl_803C7120;
    fn_800A2838(lbl_803C7120);
    lbl_803C7120 = 0;
}

void fn_80059F84(s8 arg0) {
    extern s8 lbl_803C7124;
    lbl_803C7124 = arg0;
}

u8 fn_80059F8C(void) {
    extern u8 lbl_803C7124;
    return lbl_803C7124;
}

s32 fn_8005A4E0(void) {
    return 1;
}

s32 fn_8005B314(void) {
    return 1;
}

s32 fn_8005B734(void) {
    return 0;
}

s32 fn_8005B73C(void) {
    return 0;
}

void fn_8005BBF8(void) {

}

void fn_8005BBFC(void) {
    extern s32 lbl_803C7130;
    extern s32 lbl_803C7134;
    lbl_803C7134 = 0;
    lbl_803C7130 = 0;
}

void fn_8005BE54(void) {
    extern s32 lbl_803C7138;
    lbl_803C7138 = 0;
}

void fn_8005DD64(void) {
    M2C_UNK fn_8005E49C();
    fn_8005E49C();
}

void fn_8005F124(void) {

}

void fn_8005F128(void) {

}

void fn_8005F900(void) {

}

void fn_80060574(void) {

}

void fn_80060FE8(s32 arg0, s8 arg1) {
    extern u8 lbl_803C7684;
    s32 temp_r3;
    u8 var_r0;

    temp_r3 = 1 << arg0;
    var_r0 = lbl_803C7684 & ~temp_r3;
    if (arg1 != 0) {
        var_r0 = lbl_803C7684 | temp_r3;
    }
    lbl_803C7684 = var_r0;
}

s32 fn_80061014(void) {
    extern s32 lbl_803C7168;
    return lbl_803C7168;
}

s32 fn_80061FC8(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_80063538(void) {
    M2C_UNK fn_8002E730(M2C_UNK, M2C_UNK);
    fn_8002E730(3, 1);
}

s32 fn_800652CC(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x110);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_80067284(u32 arg0) {
    extern u32 lbl_803C71C8;
    extern s8 lbl_803C71CC;
    lbl_803C71CC = 1;
    lbl_803C71C8 = arg0;
    if ((arg0 < 1U) && (arg0 > 4U)) {
        lbl_803C71C8 = -1U;
    }
}

void fn_800676EC(void) {
    M2C_UNK fn_8006770C();
    fn_8006770C();
}

s32 fn_80068C98(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 ActionCameraSetTargets(void) {
    M2C_UNK CameraManager_SetCameraGlobalMode();
    CameraManager_SetCameraGlobalMode();
    return 1;
}

s32 ActionRoundComplete(s32 *arg0) {
    M2C_UNK GameRules_ForceRoundEnd(s32);
    GameRules_ForceRoundEnd(*arg0);
    return 1;
}

s32 ActionResetGameRound(void *arg0) {
    M2C_UNK GameRules_ResetRound(s8, s8, s8);
    GameRules_ResetRound((s8) (u8) M2C_FIELD(arg0, s32 *, 0), (s8) (u8) M2C_FIELD(arg0, s32 *, 4), (s8) (u8) M2C_FIELD(arg0, s32 *, 8));
    return 1;
}

s32 ActionSetPlayerOrder(void *action) {
    GameRules_SetPlayerOrder(action, 4);
    return 1;
}

s32 ActionSpeakerChangeMode(void) {
    return 1;
}

s32 ActionVehicleAISetNamedPath(void) {
    return 1;
}

s32 ActionSetPlayerVehicle(void) {
    return 1;
}

s32 ActionVehicleRemoveCreature(void) {
    return 1;
}

s32 ActionScriptLightChangeMode(void) {
    return 1;
}

void Action_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Action_Destroy(void) {

}

void Action_Init(void) {
    extern void *m_Actions_800DBC80[80];
    u32 i;

    for (i = 0; i < 80; i++) {
        m_Actions_800DBC80[i] = NULL;
    }
}

void Action_Heartbeat(void) {
    typedef struct Action {
        s32 type;
        void *data;
        s32 unk8;
        s32 unkC;
        s32 unk10;
        s32 delay;
    } Action;
    extern Action *m_Actions_800DBC80[80];
    extern void (*pActionFuncArray_800CF500[])(void *);
    extern s32 g_iDeltaTimeMS;
    u32 i;

    for (i = 0; i < 80; i++) {
        Action *action = m_Actions_800DBC80[i];
        if (action != NULL && action->delay <= 0) {
            pActionFuncArray_800CF500[action->type](action->data);
            if (m_Actions_800DBC80[i]->type != 0x14 || ((void **)m_Actions_800DBC80[i]->data)[1] == NULL) {
                m_Actions_800DBC80[i] = NULL;
            }
        } else if (action != NULL) {
            action->delay -= g_iDeltaTimeMS;
        }
    }
}

void Action_ReleaseAllActions(void) {
    extern u8 m_Actions_800DBC80[];
    memset(m_Actions_800DBC80, 0, 0x140);
}

s8 ActionLoadInterfacePack(s32 *arg0) {
    s32 FileStreaming_LoadDataPack(void *, M2C_UNK, s32, M2C_UNK);
    s32 temp_r3;

    temp_r3 = FileStreaming_LoadDataPack(arg0 + 1, 1, *arg0, 1);
    return (s8) ((u32) ((-1 - temp_r3) | (temp_r3 + 1)) >> 0x1FU);
}

void ActionUnloadInterfacePack(s32 *arg0) {
    M2C_UNK FileStreaming_GetPackID(void *, M2C_UNK, s32);
    M2C_UNK FileStreaming_ReleaseDataPack(M2C_UNK);
    FileStreaming_GetPackID(arg0 + 1, 1, *arg0);
    FileStreaming_ReleaseDataPack(1);
}

s32 ActionMissionObjChangeMode(u32 *action) {
    u32 i;

    for (i = 0; i < action[1]; i++) {
        M2C_FIELD(M2C_FIELD(action[i + 2], u8 **, 4), u8 *, 1) = action[0];
    }
    return 1;
}

s32 ActionNavPointChangeMode(u32 *action) {
    u32 i;

    for (i = 0; i < action[1]; i++) {
        M2C_FIELD(action[i + 2], u8 *, 0x50) = action[0];
    }
    return 1;
}

s32 ActionPauseTACSystem(s32 *action) {
    switch (action[0]) {
    case 7:
        Trigger_PauseTACSystem(1);
        break;
    case 8:
        Trigger_PauseTACSystem(0);
        break;
    case 2:
        Trigger_PauseTACSystem(!(s8)Trigger_IsTACSystemPaused());
        break;
    }
    return 1;
}

void WildCardRack_JettisonAll(void *rack);
#pragma push
#pragma use_lmw_stmw on
s32 ActionVehicleRemoveItem(s32 *action) {
    void *list;
    u32 i;
    void *creature;
    void *rack;
    void *item;

    for (i = 0; i < (u32)action[1]; i++) {
        creature = M2C_FIELD((void *)action[i + 2], void **, 0x28);
        if (creature != NULL) {
            list = (u8 *)creature + 0x344;
            rack = ThingManagerList_GetByType(list, 8);
            switch (action[0]) {
            case 0:
                WildCardRack_JettisonAll(rack);
                break;
            case 1:
                item = ThingManagerList_GetByType(list, 4);
                if (item != NULL) {
                    (*(void (**)(void *))(M2C_FIELD(item, u8 **, 0x8C) + 4))(item);
                }
                break;
            }
            M2C_FIELD(rack, u32 *, 0x74) &= ~0x400200;
        }
    }
    return 1;
}
#pragma pop

void AI_Create(void) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    extern s32 g_volumeTotal;
    extern void *gs_pAsyncEventHead_803C6F08;
    extern void *gs_pAsyncEventTail_803C6F04;
    extern u32 g_attackDelay;

    g_volumeTotal = -1;
    gs_pAsyncEventHead_803C6F08 = NULL;
    gs_pAsyncEventTail_803C6F04 = NULL;
    if (GameSettings_GetCodeVariable(0x12D, 0) >= 3) {
        g_attackDelay = 8000;
    }
}

void AI_Reset(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 1;
    M2C_FIELD(arg0, s8 *, 0xCC) = 0;
}

void AI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void AI_AsyncProcess_80006C80(void) {
    M2C_UNK AI_Car_AsyncProcess();
    M2C_UNK AI_Creature_AsyncProcess();
    AI_Car_AsyncProcess();
    AI_Creature_AsyncProcess();
}

u32 AI_UpdateTimer(u32 *arg0) {
    extern u32 g_iDeltaTimeMS;
    u32 temp_r0;

    temp_r0 = *arg0;
    if (temp_r0 > (u32) g_iDeltaTimeMS) {
        *arg0 = temp_r0 - g_iDeltaTimeMS;
    } else {
        *arg0 = 0U;
    }
    return *arg0;
}

s8 AI_CheckAngleInRange(f32 angle, f32 center, f32 range) {
    s8 inRange = 0;
    f32 diff;

    if (center == 0.0f && range == 0.0f) {
        return 1;
    }
    if (range == 180.0f) {
        return 1;
    }
    if (center > 180.0f) {
        center -= 360.0f;
    }
    diff = angle - center;
    if (diff > 180.0f) {
        diff -= 360.0f;
    } else if (diff < -180.0f) {
        diff += 360.0f;
    }
    if (diff <= range && diff >= -range) {
        inRange = 1;
    }
    return inRange;
}

s32 AI_ProcessEvent(void) {
    extern s16 g_AI_FramesSinceAsyncEventProcessed;
    extern u8 g_AI_Stats[];
    extern void *gs_pAsyncEventHead_803C6F08;
    extern void *gs_pAsyncEventTail_803C6F04;
    u8 *event;

    if (g_AI_FramesSinceAsyncEventProcessed <= 5 && M2C_FIELD(g_AI_Stats, u32 *, 0x1C) >= 250) {
        return 0;
    }
    event = gs_pAsyncEventHead_803C6F08;
    g_AI_FramesSinceAsyncEventProcessed = 0;
    if (event != NULL) {
        gs_pAsyncEventHead_803C6F08 = M2C_FIELD(event, void **, 8);
        if (gs_pAsyncEventHead_803C6F08 == NULL) {
            gs_pAsyncEventTail_803C6F04 = NULL;
        }
        M2C_FIELD(event, u8 *, 0xC) = 0;
        if (M2C_FIELD(event, void **, 0) != NULL) {
            M2C_FIELD(event, void (**)(void *), 4)(M2C_FIELD(event, void **, 0));
        } else {
            M2C_FIELD(event, void (**)(void), 4)();
        }
        return 1;
    }
    return 0;
}

void AI_AddEvent(void *arg0) {
    extern void *gs_pAsyncEventHead_803C6F08;
    extern void *gs_pAsyncEventTail_803C6F04;
    if ((s8) M2C_FIELD(arg0, u8 *, 0xC) == 0) {
        M2C_FIELD(arg0, s32 *, 8) = 0;
        if ((void *) gs_pAsyncEventTail_803C6F04 != NULL) {
            M2C_FIELD(gs_pAsyncEventTail_803C6F04, void **, 8) = arg0;
        } else {
            gs_pAsyncEventHead_803C6F08 = arg0;
        }
        gs_pAsyncEventTail_803C6F04 = arg0;
        M2C_FIELD(arg0, u8 *, 0xC) = 1U;
    }
}

void AI_AsyncHeartbeat(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 0x1F4;
}

void AI_Car_AsyncScanDestructibles(u8 *car) {
    s32 RandomAdd_s32(s32 base, s32 range);
    void *RayHitsObjects_GetObject(void *pos, void *target, f32 *dist, s32 arg3, s32 arg4);
    f32 dist;
    u8 *target;
    u32 index;

    if (M2C_FIELD(car, u8 **, 0x2C) != NULL) {
        index = M2C_FIELD(car, u32 *, 0x4A4);
        target = NULL;
        if (index < 32 && ((u8 **)(car + 0x420))[index] != NULL) {
            target = ((u8 **)(car + 0x420))[index];
        } else if (M2C_FIELD(car, u8 **, 0x3EC) != NULL) {
            target = M2C_FIELD(car, u8 **, 0x3EC) + 0x40;
        }
        M2C_FIELD(car, void **, 0x3E8) = NULL;
        if (target != NULL) {
            dist = 1.0f;
            M2C_FIELD(car, void **, 0x3E8) = RayHitsObjects_GetObject(M2C_FIELD(car, u8 **, 0x2C) + 0x40, target, &dist, 1, 0);
            M2C_FIELD(car, s32 *, 0x608) = RandomAdd_s32(400, 400);
        }
    }
}

void MotionState_ReleaseSlavesByType(s32 state, u8 type) {
    void ThingManagerList_Remove(void **arg0, void *arg1);
    void ThingManagerList_Append(void **arg0, void *arg1);
    void *thing;
    void *next;

    thing = M2C_FIELD(state, void **, 0x14);
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        if (M2C_FIELD(thing, u8 *, 0x83) == type) {
            ((void (*)(void *))M2C_FIELD(thing, void ***, 0x8C)[1])(thing);
            ThingManagerList_Remove((void **)(state + 0x14), thing);
            M2C_FIELD(thing, u32 *, 0x74) &= ~0x200000;
            M2C_FIELD(thing, u32 *, 0x74) |= 0x1000;
            ThingManagerList_Append(&g_thingManager_DARYL.freeList[M2C_FIELD(thing, s32 *, 0xA8)], thing);
        }
        thing = next;
    }
}

#pragma push
#pragma auto_inline off
void MotionState_Release(s32 state) {
    M2C_UNK ThingManagerList_EmancipateSlaves(s32, s32);
    M2C_UNK ThingManagerList_ReleaseAll(s32);
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    if (M2C_FIELD(state, u32 *, 0x10) != 0) {
        ThingManagerList_EmancipateSlaves(M2C_FIELD(state, u32 *, 0x10), state + 0x14);
        MatrixCache_Free(M2C_FIELD(state, u32 *, 0x10));
        M2C_FIELD(state, u32 *, 0x10) = 0;
        M2C_FIELD(state, u8 *, 0x7) = 0;
    }
    ThingManagerList_ReleaseAll(state + 0x14);
    ThingManagerList_RemoveReleasedThings(state + 0x14);
}
#pragma pop

void MotionState_ExtractBlendedKeyFrame(u8 *state, void *keyFrame) {
    void MemCpy32(void *dst, void *src, u32 size);
    void Gfx_AnimExtractKeyFrame(void *anim, f32 time, void *keyFrame, u32 count);
    u8 *anim;

    anim = M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), u8 **, 0x14);
    MemCpy32(keyFrame, M2C_FIELD(anim, void **, 0x10), M2C_FIELD(anim, u32 *, 0x0) << 5);
    M2C_FIELD(state, f32 *, 0x24) = 1.0f;
    Gfx_AnimExtractKeyFrame(M2C_FIELD(M2C_FIELD(state, u8 **, 0x18), void **, 0x10),
                            M2C_FIELD(state, f32 *, 0x20), keyFrame, M2C_FIELD(anim, u32 *, 0x0));
}

#pragma push
#pragma dont_inline on
void MotionState_BuildMatricesWorld(void *arg0, void *arg1, void *motionState, f32 time) {
    u8 frames[0x800];
    void *anim;
    s32 *skel;

    if (M2C_FIELD(motionState, u32 *, 0x10) != 0) {
        anim = M2C_FIELD(motionState, void **, 0);
        skel = M2C_FIELD(anim, s32 **, 0x14);
        MotionState_ExtractBlendedKeyFrame(motionState, frames);
        TEST_AnimTraverseHeirarchyToMatricesWorld(arg0, arg1, frames, *skel, M2C_FIELD(motionState, u32 *, 0x10), time);
    }
}
#pragma pop

s32 AttachThing_Move(void *thing) {
    extern f32 g_fDeltaTimeMS;
    if (!(M2C_FIELD(thing, u32 *, 0x74) & 0x4000)) {
        ExtractColorKey(M2C_FIELD(thing, u8 **, 0xB4) + 0x1C, (u8 *)thing + 0x130, g_fDeltaTimeMS);
        ExtractScaleKey(M2C_FIELD(thing, u8 **, 0xB4) + 0x14, (u8 *)thing + 0xD0, g_fDeltaTimeMS);
        ExtractScaleKey(M2C_FIELD(thing, u8 **, 0xB4) + 0xC, (u8 *)thing + 0x100, g_fDeltaTimeMS);
    }
    return 1;
}

void AttachThing_Draw(void *arg0) {
    M2C_UNK AttachThing_DrawCorona(void *);
    void *Gfx_AddBillBoardSprite(u16);
    M2C_UNK Gfx_SetWorldMatrix(M2C_UNK);
    u8 temp_r0;
    void *temp_r3;
    void *temp_r3_2;

    if (!(M2C_FIELD(arg0, s32 *, 0x74) & 0x10)) {
        Gfx_SetWorldMatrix(0);
        temp_r3 = M2C_FIELD(arg0, void **, 0xB4);
        temp_r0 = M2C_FIELD(temp_r3, u8 *, 0);
        switch ((s32) temp_r0) {                    /* irregular */
        case 0:
            temp_r3_2 = Gfx_AddBillBoardSprite((u16) M2C_FIELD(temp_r3, s32 *, 8));
            M2C_FIELD(temp_r3_2, u8 *, 0x2A) = (u8) M2C_FIELD(M2C_FIELD(arg0, void **, 0xB4), u8 *, 1);
            M2C_FIELD(temp_r3_2, s32 *, 0x24) = (s32) M2C_FIELD(arg0, s32 *, 0x154);
            M2C_FIELD(temp_r3_2, f32 *, 0x1C) = (f32) M2C_FIELD(arg0, f32 *, 0x100);
            M2C_FIELD(temp_r3_2, f32 *, 0xC) = (f32) M2C_FIELD(arg0, f32 *, 0xD0);
            M2C_FIELD(temp_r3_2, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x40);
            M2C_FIELD(temp_r3_2, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x44);
            M2C_FIELD(temp_r3_2, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x48);
            return;
        case 1:
            AttachThing_DrawCorona(arg0);
            break;
        }
    }
}

void AttachGeometry_Draw(void *thing) {
    u8 *data;
    void *geom;
    u32 color;

    data = M2C_FIELD(thing, u8 **, 0xB4);
    geom = M2C_FIELD(data, void **, 0x8);
    if (M2C_FIELD(data, u32 *, 0x1C) != 0) {
        color = M2C_FIELD(thing, u32 *, 0x154);
        Gfx_SetBaseColor(&color);
        Gfx_EnableState(0x10);
    }
    M2C_FIELD(thing, f32 *, 0xDC) = 1.0f;
    Gfx_DrawGeometry_QuaternionScalePos(geom, (u8 *)thing + 0x50, (u8 *)thing + 0xD0, (u8 *)thing + 0x40);
    Gfx_DisableState(0x10);
}

s32 AttachGeometry_Move(Thing *thing) {
    extern f32 g_fDeltaTimeMS;
    extern f32 g_fDeltaTimeSec;
    Quaternion rot;
    Quaternion out;

    ExtractColorKey(M2C_FIELD(thing, u8 **, 0xB4) + 0x1C, (u8 *)thing + 0x130, g_fDeltaTimeMS);
    ExtractScaleKey(M2C_FIELD(thing, u8 **, 0xB4) + 0x14, (u8 *)thing + 0xD0, g_fDeltaTimeMS);
    GenerateQuaternion(M2C_FIELD(M2C_FIELD(thing, u8 **, 0xB4), void **, 0x2C), &rot,
                       6.2831855f * M2C_FIELD(thing, f32 *, 0xC0) * g_fDeltaTimeSec);
    QuaternionMult((Vector4 *)&rot, &thing->quat, (Vector4 *)&out);
    *(Quaternion *)&thing->quat = out;
    return 1;
}

s32 AttachThing_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB4);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 RadarBlip_Init(u8 *blip, u8 *init) {
    typedef struct { f32 x, y, z, w; } Vec4;
    Thing_Init(blip, 0);
    M2C_FIELD(blip, Vec4 *, 0x40) = M2C_FIELD(init, Vec4 *, 0x10);
    M2C_FIELD(blip, void **, 0xB0) = M2C_FIELD(init, void **, 0x0);
    M2C_FIELD(blip, s32 *, 0xB4) = 0;
    M2C_FIELD(blip, f32 *, 0xB8) = 1.0f;
    if (M2C_FIELD(M2C_FIELD(blip, u8 **, 0xB0), u8 *, 0x3) == 0) {
        M2C_FIELD(blip, u32 *, 0x74) |= 0x200;
    }
    return 1;
}

s32 RadarBlip_Move(u8 *blip) {
    typedef struct { f32 x, y, z, w; } Vec4;
    M2C_FIELD(blip, Vec4 *, 0xC0) = M2C_FIELD(blip, Vec4 *, 0x40);
    return 1;
}

#pragma push
#pragma readonly_strings on
void SoundManager_Create(void) {
    extern s8 g_songLoaded_803C6F2C;
    extern SoundSource *g_sources_803C6F38;
    extern void *g_tracks_803C6F30;
    s8 Audio_Create(void);
    int printf(const char *, ...);
    void *Allocate(u32, u32, const char *);

    Dprintf("SoundManager_Create()");
    if (!Audio_Create()) {
        printf("Audio_Create() FAILED!!!!!!!!!!!!!!!\n");
    }
    Dprintf("SoundManager_Create() done");
    g_songLoaded_803C6F2C = 0;
    g_sources_803C6F38 = Allocate(0x1F40, 0, "");
    g_tracks_803C6F30 = Allocate(0x500, 0, "");
}
#pragma pop
void SoundManager_Destroy(void) {
    M2C_UNK Audio_Destroy();
    extern s32 g_sources_803C6F38;
    extern s32 g_tracks_803C6F30;
    Audio_Destroy();
    Deallocate(g_sources_803C6F38);
    Deallocate(g_tracks_803C6F30);
}

void SoundManager_Heartbeat(void) {
    M2C_UNK MusicHearbeat();
    s32 OSDisableInterrupts();
    M2C_UNK OSRestoreInterrupts(s32);
    M2C_UNK SFX_Heartbeat();
    s32 temp_r31;

    temp_r31 = OSDisableInterrupts();
    SFX_Heartbeat();
    MusicHearbeat();
    OSRestoreInterrupts(temp_r31);
}

void SFX_Heartbeat(void) {
    M2C_UNK SFX_ProcessSource(u32);
    extern s32 g_sscount;
    u32 var_r31;

    var_r31 = 0U;
    g_sscount = 0;
    do {
        SFX_ProcessSource(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x64U);
}

void SoundListener_Set(s32 listener, s32 param, void *value) {
    typedef struct SoundListener {
        Vector4 pos;
        Vector4 vel;
        u8 unk20[0x30];
        u8 mtx[0x40];
    } SoundListener;
    extern SoundListener g_listener[4];
    void *memcpy(void *dst, const void *src, u32 n);

    switch (param) {
    case 5:
        g_listener[listener].pos = *(Vector4 *)value;
        break;
    case 6:
        g_listener[listener].vel = *(Vector4 *)value;
        break;
    case 7:
        memcpy(g_listener[listener].mtx, value, 0x40);
        break;
    }
}

void SoundListener_SetTotalListeners(u32 arg0) {
    extern s32 g_numListeners;
    if (arg0 != 0U) {
        g_numListeners = arg0 - 1;
    }
}

void SoundSource_Play(s32 index) {
    extern SoundSource *g_sources_803C6F38;
    g_sources_803C6F38[index].playing = 1;
}

void SoundSource_Stop(s32 index) {
    extern SoundSource *g_sources_803C6F38;
    void ReleaseVoice(s32 voice);

    if (g_sources_803C6F38[index].voice > 0) {
        ReleaseVoice(g_sources_803C6F38[index].voice);
    }
    g_sources_803C6F38[index].voice = -1;
    g_sources_803C6F38[index].unk24 = 0.0f;
    g_sources_803C6F38[index].looping = 0;
}

s8 SoundSource_IsActive(s32 index) {
    extern SoundSource *g_sources_803C6F38;
    return g_sources_803C6F38[index].voice != -1;
}

#pragma push
#pragma auto_inline off
s8 SoundSource_IsLooping(s32 index) {
    extern SoundSource *g_sources_803C6F38;
    return g_sources_803C6F38[index].looping;
}
#pragma pop

void *SoundAudioSample_Play(void *sample) {
    void *voice;

    if (sample == NULL) {
        return NULL;
    }
    voice = (void *)GetVoice(sample, (u8)(rand() % 10 + 5));
    if (voice != NULL) {
        SetVoiceState(voice, 1);
        return voice;
    }
    return NULL;
}

void SoundAudioSample_Set(void *voice, s32 type, s32 value) {
    switch (type) {
    case 0:
        SetVoiceVol(voice, (s16)value);
        break;
    case 1:
        SetVoicePitch(voice, value);
        break;
    case 4:
        SetVoicePos(voice, (s16)value, -0x7FFF);
        break;
    case 9:
        break;
    }
}

void SongManager_StopBKSong(void) {

}

void SongManager_LoadSong(void) {
    M2C_UNK CreateMusic();
    extern s8 g_songLoaded_803C6F2C;
    CreateMusic();
    g_songLoaded_803C6F2C = 1;
}

void SongManager_PlaySong(void) {
    extern s8 g_songLoaded_803C6F2C;
    extern void (*SetMusicState)(s32);
    extern f32 g_globalMusic_800DBE30[];
    void SetMusicVol(u32 vol);

    if (g_songLoaded_803C6F2C) {
        SetMusicState(1);
        SetMusicVol(32767.0 * g_globalMusic_800DBE30[0]);
    }
}

s32 ActionMusicSetSong(void) {
    return 1;
}

void *GetCameraManager(void) {
    extern u8 g_cameraManager[];
    return g_cameraManager;
}

s32 CameraManager_Create(void) {
    return 1;
}

void CameraManager_Destroy(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

u8 CameraManager_GetActiveCameras(void) {
    extern u8 g_cameraManager[];
    return g_cameraManager[0x640];
}

void *CameraManager_GetCameraByIndex(u32 arg0) {
    extern u8 g_cameraManager[];
    if (arg0 >= g_cameraManager[0x640] || arg0 >= 4U) {
        return NULL;
    }
    return g_cameraManager + arg0 * 0x190;
}

#pragma push
#pragma dont_inline on
void CameraManager_Init(void) {
    extern u8 g_cameraManager[];
    extern s8 g_cameraManagerPause;
    extern u8 g_springProxy[];
    g_cameraManagerPause = 0;
    g_cameraManager[0x640] = 0;
    SoundListener_SetTotalListeners(0);
    memset(g_springProxy + 0x60, 0, 0x20);
    memset(g_springProxy + 0x40, 0, 0x20);
    M2C_FIELD(g_springProxy, s32 *, 0x1C0) = 0;
    memset(g_springProxy + 0x140, 0, 0x20);
    memset(g_springProxy + 0xC0, 0, 0x20);
    M2C_FIELD(g_springProxy, s32 *, 0x1C4) = 0;
}
#pragma pop
void CameraManager_Pause(s8 arg0) {
    extern s8 g_cameraManagerPause;
    g_cameraManagerPause = arg0;
}

void Camera_SetPlayer(void **camera, void *player) {
    extern u8 g_cameraManager[];
    void PlayerManager_ClearAllCameraFlags_PRIVATE(void);
    void Player_RefreshInterface(void *player);
    u32 i;
    void *p;

    *camera = player;
    PlayerManager_ClearAllCameraFlags_PRIVATE();
    for (i = 0; i < g_cameraManager[0x640]; i++) {
        p = M2C_FIELD(g_cameraManager + i * 0x190, void **, 0);
        if (p != NULL) {
            M2C_FIELD(p, u32 *, 0x6C) |= 0x10;
        }
    }
    Player_RefreshInterface(player);
}

void CameraManager_ReturnAllToPlayer(void) {
    extern u8 g_cameraManager[];
    extern u32 g_triggerAction_params_default;
    u32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        M2C_FIELD(g_cameraManager + i * 0x190, u32 **, 0xB4) = &g_triggerAction_params_default;
    }
}

void CameraManager_SetCameraGlobalMode(u32 *mode) {
    typedef struct Camera {
        u8 pad0[0xB4];
        u32 *mode;
        f32 modeTime;
        u8 pad1[0x190 - 0xBC];
    } Camera;
    extern u8 g_cameraManager[];
    u32 i;

    if (mode[0] >= 1 && mode[0] <= 4) {
        ((Camera *)g_cameraManager)[mode[0] - 1].mode = mode + 1;
        ((Camera *)g_cameraManager)[mode[0] - 1].modeTime = 0.0f;
        return;
    }
    for (i = 0; i < g_cameraManager[0x640]; i++) {
        ((Camera *)g_cameraManager)[i].mode = mode + (mode[0] == 0);
        ((Camera *)g_cameraManager)[i].modeTime = 0.0f;
    }
}

void *CameraManager_GetCameraFromPlayer(void *player) {
    extern u8 g_cameraManager[];
    u32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        if (M2C_FIELD(g_cameraManager + i * 0x190, void **, 0) == player) {
            return g_cameraManager + i * 0x190;
        }
    }
    return NULL;
}

void CameraManager_ReleaseAllEmitters(void) {
    typedef struct Camera {
        u8 pad0[0x180];
        u8 *emitter;
        u8 pad1[0x190 - 0x184];
    } Camera;
    extern u8 g_cameraManager[];
    void ThingManagerList_RemoveReleasedThings(s32 list);
    u32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        if (((Camera *)g_cameraManager)[i].emitter != NULL) {
            (*(void (***)(void *))(((Camera *)g_cameraManager)[i].emitter + 0x8C))[1](((Camera *)g_cameraManager)[i].emitter);
            ((Camera *)g_cameraManager)[i].emitter = NULL;
        }
    }
    ThingManagerList_RemoveReleasedThings((s32)&g_thingManager_DARYL.activeList);
}

void CarA_GetAngularVelocity(void *car, Vector4 *vel) {
    *vel = M2C_FIELD(car, Vector4 *, 0x410);
}

void CarA_GetInverseLocalInertiaTensor(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

f32 CarA_Transmission_GetTorqueAtWheels(void *car) {
    f32 scale;
    f32 rpm;
    f32 d;
    f32 c;
    f32 b;
    f32 a;

    a = M2C_FIELD(car, f32 *, 0x4C0);
    b = M2C_FIELD(car, f32 *, 0x4C4);
    c = M2C_FIELD(car, f32 *, 0x4C8);
    d = M2C_FIELD(car, f32 *, 0x4CC);
    rpm = M2C_FIELD(car, f32 *, 0x4E4);
    scale = 1.0f;

    if (M2C_FIELD(car, s32 *, 0x470) < 10000) {
        scale = 0.5f;
        rpm *= scale;
    } else if (M2C_FIELD(car, u32 *, 0x48C) & 0x1000) {
        rpm /= M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), f32 *, 0x30);
        scale *= M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), f32 *, 0x34);
    }
    if (rpm < 0.0f) {
        rpm *= -1.0f;
    }
    return (scale * (rpm * (rpm * (a * rpm + b) + c) + d)) /
           M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), f32 **, 0xDC)[M2C_FIELD(car, u8 *, 0x4E8) * 2];
}

void CarA_SetSlavePlayers(void *car) {
    void *slave;

    for (slave = M2C_FIELD(car, void **, 0x344); slave != NULL; slave = M2C_FIELD(slave, void **, 0xA4)) {
        M2C_FIELD(slave, s32 *, 0x94) = M2C_FIELD(car, s32 *, 0x94);
    }
}

f32 CarA_GetMass(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), f32 *, 0x20);
}

void CarA_GetTargetPos(void *car, Vector4 *pos) {
    switch (M2C_FIELD(M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), void **, 0xA0), u8 *, 0x10)) {
    case 0:
        *pos = M2C_FIELD(car, Vector4 *, 0xC0);
        break;
    case 1:
        *pos = M2C_FIELD(M2C_FIELD(car, void **, 0x370), Vector4 *, 0x3D0);
        break;
    }
}

void *CarA_GetOrientedCollision(void *car) {
    void BuildOOBOffset(void *oob, void *pos, void *rot, void *out);
    void BuildOConvexHullOffset(void *out, void *hull, void *pos, void *rot);
    void *coll;

    coll = M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), void **, 0xA0);
    switch (M2C_FIELD(coll, u8 *, 0x10)) {
    case 0:
        if (M2C_FIELD(car, u32 *, 0x74) & 0x180) {
            BuildOOBOffset(M2C_FIELD(coll, void **, 0x44), (u8 *)car + 0x10, (u8 *)car + 0x520, (u8 *)car + 0xC0);
            M2C_FIELD(car, u32 *, 0x74) &= ~0x180;
        }
        return (u8 *)car + 0xC0;
    case 1:
        BuildOConvexHullOffset(M2C_FIELD(car, void **, 0x370), M2C_FIELD(coll, void **, 0x48), (u8 *)car + 0x10, (u8 *)car + 0x520);
        return M2C_FIELD(car, void **, 0x370);
    default:
        return NULL;
    }
}

s32 CarA_GetSurfaceScript(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), s32 *, 0x30);
}

s32 CarA_GetNearestNavPoint(void *car) {
    s32 NavHolder_FindIndexOfNearestNavPoint(void *navHolder);
    s32 RandomAdd_s32(s32 base, s32 range);
    s32 timer;

    if (M2C_FIELD(car, u32 *, 0x3B0) == 0xFFFFFFFF) {
        M2C_FIELD(car, s32 *, 0x3B0) = NavHolder_FindIndexOfNearestNavPoint((u8 *)car + 0x40);
        timer = RandomAdd_s32(300, 100);
        M2C_FIELD(car, s32 *, 0x3B4) = timer;
        M2C_FIELD(car, s32 *, 0x3B4) = timer;
    }
    return M2C_FIELD(car, s32 *, 0x3B0);
}

s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void Drone_GetCameraOrientationInfo(void *drone, void *camera, void *info) {
    M2C_FIELD(info, s32 *, 0x88) = 1;
    M2C_FIELD(info, void **, 0x84) = (u8 *)drone + 0x154;
    M2C_FIELD(info, f32 *, 0x8C) = 1.0f;
    M2C_FIELD(info, f32 *, 0x30) = 0.0f;
    M2C_FIELD(info, f32 *, 0x34) = 1.0f;
    M2C_FIELD(info, f32 *, 0x38) = 0.0f;
}

f32 CarA_GetImpactMultiplier(void *car, void *other, f32 scale) {
    f32 PSVECDotProduct(void *a, void *b);
    f32 impact;

    impact = scale * PSVECDotProduct((u8 *)car + 0x20, (u8 *)other + 0x10);
    if (impact < 0.0f) {
        return 1.0f;
    }
    return 1.0f - impact;
}

s8 RayHitsTerrain(Vector4 *start, Vector4 *end, void *arg2, void *arg3) {
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    Vector4 dir;

    PSVECSubtract(end, start, &dir);
    return RayHitsTerrain_Aux3(start, &dir, arg2, arg3);
}

#pragma push
#pragma use_lmw_stmw on
s8 OBoxHitsBSPZeroTimeEdgesOnly(void *obox, void *bsp, void *arg2) {
    s8 OBoxHitsSegmentZeroTime(void *obox, void *edge, void *arg2);
    u8 *edge;
    u32 i;

    if (M2C_FIELD(bsp, u32 *, 0x3C) == 0 && M2C_FIELD(bsp, u32 *, 0x40) == 0) {
        return 0;
    }
    for (i = 0; i < M2C_FIELD(bsp, u32 *, 0x40); i++) {
        edge = M2C_FIELD(M2C_FIELD(g_worldData.world, void **, 0x0), u8 **, 0x54) + M2C_FIELD(bsp, u16 **, 0x44)[i] * 0x30;
        if (M2C_FIELD(edge, u32 *, 0x20) != g_worldData.unk4) {
            M2C_FIELD(edge, u32 *, 0x20) = g_worldData.unk4;
            if (OBoxHitsSegmentZeroTime(obox, edge, arg2)) {
                return 1;
            }
        }
    }
    return 0;
}
#pragma pop

void BuildOOBOffset(void *oob, void *pos, void *rot, void *out) {
    void BuildOOB(void *oob, void *pos, void *out);
    Matrix4 local;

    local = *(Matrix4 *)pos;
    XformVec(pos, rot, &local.v[3]);
    BuildOOB(oob, &local, out);
    M2C_FIELD(out, Vector4 **, 0xA0) = &((Matrix4 *)pos)->v[2];
    M2C_FIELD(out, Matrix4 **, 0xA4) = (Matrix4 *)pos;
    M2C_FIELD(out, Vector4 **, 0xA8) = &((Matrix4 *)pos)->v[1];
}

void OConvexHullHolder_Init(void) {
    extern u8 g_aOConvexHull[8][0x3E0];
    s32 i;
    for (i = 0; i < 8; i++) {
        *(void **)(g_aOConvexHull[i] + 0x300) = NULL;
    }
}

void *OConvexHullHolder_Get(void *owner) {
    static const char msg[] = "OUT OF CONVEX HULLS!!!!\n";
    extern u8 g_aOConvexHull[8][0x3E0];
    s32 i;
    for (i = 0; i < 8; i++) {
        if (*(void **)(g_aOConvexHull[i] + 0x300) == NULL) {
            *(void **)(g_aOConvexHull[i] + 0x300) = owner;
            return g_aOConvexHull[i];
        }
    }
    Dprintf(msg);
    return NULL;
}

void OConvexHullHolder_Release(void *arg0) {
    if (arg0 != NULL) {
        M2C_FIELD(arg0, s32 *, 0x300) = 0;
    }
}

void *OBoxHolder_Get(void) {
    static const char msg[] = "OUT OF OBOXES!!!!\n";
    extern s8 g_aOBox[0x200][0xF0];
    s32 i;
    for (i = 0; i < 0x200; i++) {
        if (g_aOBox[i][0x9D] == 0) {
            g_aOBox[i][0x9D] = 1;
            return g_aOBox[i];
        }
    }
    Dprintf(msg);
    return NULL;
}

void OBoxHolder_Release(void *arg0) {
    if (arg0 != NULL) {
        M2C_FIELD(arg0, s8 *, 0x9D) = 0;
    }
}

void Dprintf(const char *fmt, ...);
void CollisionPairCache_Clear(void) {
    extern u8 g_aCollisionPairCache[4][0x30];
    s32 i;
    for (i = 0; i < 4; i++) {
        *(s32 *)(g_aCollisionPairCache[i] + 0x24) = 0;
        *(s32 *)(g_aCollisionPairCache[i] + 0x20) = 0;
        *(s32 *)(g_aCollisionPairCache[i] + 0x28) = 99999;
    }
}

void CollisionTriCache_Clear(void) {
    extern u8 g_aCollisionTriPairCache[4][0x30];
    s32 i;
    for (i = 0; i < 4; i++) {
        *(s32 *)(g_aCollisionTriPairCache[i] + 0x24) = 0;
        *(s32 *)(g_aCollisionTriPairCache[i] + 0x20) = 0;
        *(s32 *)(g_aCollisionTriPairCache[i] + 0x28) = 99999;
    }
}

s32 CollisionTriCache_Get(u32 tri0, u32 tri1, Vector4 *a, Vector4 *b) {
    extern u8 g_aCollisionTriPairCache[4][0x30];
    s32 i;
    for (i = 0; i < 4; i++) {
        if (tri0 == *(u32 *)(g_aCollisionTriPairCache[i] + 0x24) &&
            tri1 == *(u32 *)(g_aCollisionTriPairCache[i] + 0x20)) {
            *a = *(Vector4 *)(g_aCollisionTriPairCache[i] + 0x0);
            *b = *(Vector4 *)(g_aCollisionTriPairCache[i] + 0x10);
            *(s32 *)(g_aCollisionTriPairCache[i] + 0x28) = 0;
            return 1;
        }
    }
    return 0;
}

void ShaderHolder_RemoveGX(s32 gx);
void VertexColorSeqManager_RemoveVertices(void *vertices);
void Control_ClearControl(void *control) {
    memset(control, 0, 0x20);
}

void CoronaHolder_Init(void) {
    extern s32 g_coronaRayTest;
    extern s32 g_totalCoronas;
    g_coronaRayTest = 0;
    g_totalCoronas = 0;
}

s32 CoronaHolder_Add(void *corona, Vector4 *pos) {
    typedef struct Corona {
        void *corona;
        u8 unk4[0x10 - 0x4];
        Vector4 pos;
        u8 unk20[0x4C - 0x20];
        u8 unk4C;
        u8 unk4D[0x50 - 0x4D];
        s32 flare;
        s32 unk54;
        u8 unk58;
        u8 unk59[0x60 - 0x59];
    } Corona;
    extern Corona g_aCorona[64];
    extern u32 g_totalCoronas;
    s32 LensFlare_Add(void *flare, s32 arg1, Vector4 *pos);
    Corona *c;

    if (g_totalCoronas >= 64) {
        return -1;
    }
    if (corona == NULL) {
        return -1;
    }
    g_aCorona[g_totalCoronas].corona = corona;
    c = &g_aCorona[g_totalCoronas];
    c->pos = *pos;
    c->flare = -1;
    c->unk58 = 0;
    c->unk54 = 0;
    c->unk4C = 1;
    if (M2C_FIELD(corona, void **, 0x20) != NULL) {
        c->flare = LensFlare_Add(M2C_FIELD(corona, void **, 0x20), 0, pos);
    }
    return g_totalCoronas++;
}

void Creature_GetTargetPos(void *creature, Vector4 *pos) {
    pos->x = M2C_FIELD(creature, f32 *, 0x40);
    pos->y = 4.0f + M2C_FIELD(creature, f32 *, 0x44);
    pos->z = M2C_FIELD(creature, f32 *, 0x48);
    pos->w = 1.0f;
}

f32 Creature_GetMass(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0x1BC);
}

void Creature_Release(s32 arg0) {
    M2C_UNK MotionState_Release(s32);
    M2C_UNK Thing_Release(s32);
    MotionState_Release(arg0 + 0xCC);
    Thing_Release(arg0);
}

void Creature_MicroStep(void *arg0, f32 dt) {
    void AccumulateVector(void *arg0, void *arg1, void *arg2, f32 farg0);
    struct CreatureModel {
        u8 unk0[0x14];
        u32 *numBones;
    };
    struct Creature {
        u8 unk0[0x40];
        Vector4 pos;
        u8 unk50[0x74 - 0x50];
        u32 flags;
        u8 unk78[0xCC - 0x78];
        struct CreatureModel *model;
        u8 unkD0[0xDC - 0xD0];
        Matrix4 *bones;
        u8 unkE0[0x1A0 - 0xE0];
        Vector4 vel;
    } *creature = arg0;
    Vector4 pos;
    u32 i;
    u32 *numBones;

    AccumulateVector(&creature->vel, &creature->pos, &pos, dt);
    creature->pos = pos;
    numBones = creature->model->numBones;
    for (i = 0; i < *numBones; i++) {
        creature->bones[i].v[3].x += creature->vel.x * dt;
        creature->bones[i].v[3].y += creature->vel.y * dt;
        creature->bones[i].v[3].z += creature->vel.z * dt;
    }
    creature->flags &= ~0x4000;
}

void Creature_Draw(void *creature) {
    if (M2C_FIELD(creature, f32 *, 0x1C0) > 0.0f) {
        MotionState_Draw((u8 *)creature + 0xCC);
    }
    M2C_FIELD(creature, s32 *, 0x74) |= 0x4000;
}

s32 Creature_ProcessItemPickup(void *creature, void *item) {
    void *rack;
    void *motion;

    switch (*M2C_FIELD(item, u8 **, 0xC0)) {
    case 3:
        rack = ThingManagerList_GetByType((u8 *)creature + 0xE0, 8);
        if (rack == NULL) {
            break;
        }
        WildCardRack_Add(rack, M2C_FIELD(item, u8 **, 0xC0), M2C_FIELD(item, s16 *, 0xCE));
        Item_Release(item);
        motion = M2C_FIELD(item, void **, 0xD0);
        if (motion != NULL) {
            M2C_FIELD(motion, s32 *, 0x18) = M2C_FIELD(motion, s32 *, 0x14);
        }
        return 1;
    case 0:
    case 1:
    case 2:
        break;
    default:
        break;
    }
    return 0;
}

s32 Creature_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB8);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void DataPack_EnableLoadingSounds(s8 arg0) {
    extern s8 g_bSoundLoadingEnabled;
    g_bSoundLoadingEnabled = arg0;
}

void DataPack_Initialize(void) {
    u32 i;
    for (i = 0; i < 11; i++) {
        g_fileTOCCache[i].valid = 0;
    }
}

extern s32 g_fileID;
extern s32 g_openFileDataPackType;
void File_Close(s32 file);
void *ThingManagerList_GetByType(void *list, s32 type);
void WildCardRack_Add(void *rack, u8 *item, s16 count);
void Item_Release(void *item);
void MotionState_Draw(void *state);
void DataPack_CloseFiles(void) {
    if (g_fileID > 0) {
        File_Close(g_fileID);
        g_fileID = -1;
        g_openFileDataPackType = 0xB;
    }
}

void DataPack_ReleaseQuadPack(void *pack) {
    s32 *gx;
    s32 base;
    u32 i;

    if (M2C_FIELD(pack, void **, 0x28) != NULL) {
        base = M2C_FIELD(pack, s32 *, 0x2C);
        gx = M2C_FIELD(M2C_FIELD(pack, void **, 0x28), s32 **, 0x20);
        for (i = 0; i < M2C_FIELD(M2C_FIELD(pack, void **, 0x28), u32 *, 0x1C); i++) {
            ShaderHolder_RemoveGX(base + *gx);
            gx += 2;
        }
        if (M2C_FIELD(pack, void **, 0x14) != NULL) {
            VertexColorSeqManager_RemoveVertices(M2C_FIELD(pack, void **, 0x14));
        }
        Deallocate(M2C_FIELD(pack, void **, 0x44));
        M2C_FIELD(pack, s32 *, 0x44) = 0;
        M2C_FIELD(pack, s32 *, 0x10) = 0;
        M2C_FIELD(pack, s32 *, 0x14) = 0;
        M2C_FIELD(pack, s32 *, 0x2C) = 0;
        M2C_FIELD(pack, s32 *, 0x28) = 0;
        M2C_FIELD(pack, u8 *, 0x34) = 0;
    }
}

u8 *DataPack_GetScript(void *pack, s32 id) {
    u32 *entries;
    u32 i;
    u8 *table = *(u8 **)pack;

    if (table == NULL) {
        return NULL;
    }
    for (i = 0; i < M2C_FIELD(table, u32 *, 0); i++) {
        entries = M2C_FIELD(table, u32 **, 4);
        if (id == entries[i * 2 + 1]) {
            return (u8 *)entries[i * 2];
        }
    }
    return NULL;
}

void Debris_Draw(u8 *debris) {
    void Gfx_DrawGeometry(void *geom, void *mtx);
    void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);

    if (M2C_FIELD(debris, void **, 0xC0) != NULL) {
        if (M2C_FIELD(debris, u32 *, 0x74) & 0x20000000) {
            Gfx_DrawGeometry(M2C_FIELD(debris, void **, 0xC0), debris + 0x10);
        } else {
            Gfx_DrawGeometry_MatrixNotCached(M2C_FIELD(debris, void **, 0xC0), debris + 0x10);
        }
    }
}

f32 Debris_GetMass(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0xCC);
}

#pragma push
#pragma dont_inline on
void Destructible_Release(u8 *destructible) {
    void Trigger_DestructibleKilled(void *thing);
    void Thing_Release(s32 thing);
    extern u8 g_destructibleVTable[];
    u8 *trigger;
    s8 paused;

    Trigger_DestructibleKilled(destructible);
    if (M2C_FIELD(destructible, u8 **, 0x8C) == g_destructibleVTable) {
        trigger = M2C_FIELD(destructible, u8 **, 0x90);
        if (trigger != NULL) {
            M2C_FIELD(destructible, u8 **, 0x90) = NULL;
            M2C_FIELD(trigger, u32 *, 0x2C) = 0;
            paused = Trigger_IsTACSystemPaused();
            if (paused == 0) {
                M2C_FIELD(trigger, u32 *, 0x30) = 0;
            }
        }
    }
    OBoxHolder_Release(M2C_FIELD(destructible, void **, 0xD0));
    M2C_FIELD(destructible, void **, 0xD0) = NULL;
    OConvexHullHolder_Release(M2C_FIELD(destructible, void **, 0xCC));
    M2C_FIELD(destructible, void **, 0xCC) = NULL;
    Thing_Release((s32)destructible);
}
#pragma pop

void Destructible_Draw(u8 *destructible) {
    f32 Gfx_GetMinCameraDistSquared(void *pos);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);
    void Thing_DrawSharedSlaves(void *thing, void *slaves);
    f32 distSq;
    u8 *data;

    if (M2C_FIELD(destructible, f32 *, 0xC4) == 0.0f) {
        goto draw;
    }
    distSq = Gfx_GetMinCameraDistSquared(destructible + 0x40);
    if ((u32)ViewManager_GetViewFormat() > 1) {
        distSq *= 4.0f;
    }
    if (distSq > M2C_FIELD(destructible, f32 *, 0xC4) * M2C_FIELD(destructible, f32 *, 0xC4)) {
        return;
    }
draw:
    QuaternionToSubMatrix(destructible + 0x50, destructible + 0x10);
    data = M2C_FIELD(destructible, u8 **, 0xC0);
    if (data[4] == 0) {
        Gfx_DrawGeometry_MatrixNotCached(*(void **)data, destructible + 0x10);
    }
    Thing_DrawSharedSlaves(destructible, M2C_FIELD(destructible, u8 **, 0xC0) + 0x18);
}

f32 Destructible_GetMass(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), f32 *, 0x20);
}

void Destructible_GetTargetPos(void *obj, Vector4 *pos) {
    switch (M2C_FIELD(M2C_FIELD(M2C_FIELD(obj, void **, 0xC0), void **, 0x10), u8 *, 0x10)) {
    case 0:
    case 3:
        *pos = *M2C_FIELD(obj, Vector4 **, 0xD0);
        break;
    case 1:
    case 6:
        *pos = M2C_FIELD(M2C_FIELD(obj, void **, 0xCC), Vector4 *, 0x3D0);
        break;
    default:
        *pos = M2C_FIELD(obj, Vector4 *, 0x40);
        break;
    }
}

void *Destructible_GetOrientedCollision(void *thing) {
    extern u8 g_zeroVector[];
    void BuildOOB(void *oob, void *pos, void *out);
    void BuildOConvexHullOffset(void *out, void *hull, void *pos, void *rot);
    void *coll;

    coll = M2C_FIELD(M2C_FIELD(thing, void **, 0xC0), void **, 0x10);
    switch (M2C_FIELD(coll, u8 *, 0x10)) {
    case 0:
    case 3:
        if (M2C_FIELD(thing, u32 *, 0x74) & 0x180) {
            BuildOOB(M2C_FIELD(coll, void **, 0x44), (u8 *)thing + 0x10, M2C_FIELD(thing, void **, 0xD0));
            M2C_FIELD(thing, u32 *, 0x74) &= ~0x180;
        }
        return M2C_FIELD(thing, void **, 0xD0);
    case 1:
    case 6:
        BuildOConvexHullOffset(M2C_FIELD(thing, void **, 0xCC), M2C_FIELD(coll, void **, 0x48), (u8 *)thing + 0x10, g_zeroVector);
        return M2C_FIELD(thing, void **, 0xCC);
    default:
        return NULL;
    }
}

s32 Destructible_GetSurfaceScript(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), s32 *, 0x30);
}

void Destructible_CollideWithRay(void *thing, void *start, void *end, void *hit, f32 maxDist) {
    f32 PhysicalProperties_CollideWithRay(void *props, void *pos, void *start, void *end, void *hit);
    f32 dist;

    dist = PhysicalProperties_CollideWithRay(M2C_FIELD(M2C_FIELD(thing, void **, 0xC0), void **, 0x10), (u8 *)thing + 0x10, start, end, hit);
    if (hit != NULL && dist < maxDist) {
        M2C_FIELD(hit, Vector4 *, 0x30) = *(Vector4 *)thing;
    }
}

s32 Destructible_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void Emitter_Release(void *emitter) {
    extern u8 g_emitterVTable[];
    void Thing_Release(s32 thing);
    void *slot;

    if (M2C_FIELD(emitter, void **, 0x8C) == g_emitterVTable) {
        slot = M2C_FIELD(emitter, void **, 0x90);
        if (slot != NULL) {
            M2C_FIELD(emitter, void **, 0x90) = NULL;
            M2C_FIELD(slot, void **, 0x30) = NULL;
        }
    }
    Thing_Release((s32)emitter);
}

s32 ActionFXPointChangeMode(s32 *data) {
    u32 count = data[1];
    s32 mode = data[0];
    u32 i;

    for (i = 0; i < count; i++) {
        u8 *point = (u8 *)data[i + 2];
        u8 *fx = M2C_FIELD(point, u8 **, 0x30);
        switch (mode) {
        case 0:
            if (fx != NULL) {
                M2C_FIELD(fx, u32 *, 0x74) |= 0x02000000;
            }
            break;
        case 1:
            if (fx != NULL) {
                M2C_FIELD(fx, u32 *, 0x74) &= ~0x02000000;
            }
            break;
        case 2:
            if (M2C_FIELD(fx, u32 *, 0x74) & 0x02000000) {
                if (fx != NULL) {
                    M2C_FIELD(fx, u32 *, 0x74) &= ~0x02000000;
                }
            } else {
                if (fx != NULL) {
                    M2C_FIELD(fx, u32 *, 0x74) |= 0x02000000;
                }
            }
            break;
        }
        M2C_FIELD(point, s32 *, 0x34) = mode;
    }
    return 1;
}

s32 Emitter_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void EventSeq_Release(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 EventSeq_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xD0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

#pragma push
#pragma use_lmw_stmw on
void FileStreaming_Initialize(void) {
    extern u8 *g_StreamingDataPacks[11];
    extern u32 g_MaxStreamingDataPacks[11];
    extern s32 g_numQuadPacksInMemory;
    extern s32 g_numQuadPacksToLoad;
    extern s32 g_itemTimer;
    extern void *g_pDataPackToLoad;
    void *memset(void *, s32, u32);
    u32 i;
    u32 size;

    g_numQuadPacksInMemory = 0;
    g_numQuadPacksToLoad = 0;
    g_itemTimer = 0;
    for (i = 0; i < 11; i++) {
        if (i != 7 && g_MaxStreamingDataPacks[i] != 0) {
            size = g_MaxStreamingDataPacks[i] * 0xA8;
            g_StreamingDataPacks[i] = Allocate(size, 0, "");
            memset(g_StreamingDataPacks[i], 0, size);
        } else {
            g_StreamingDataPacks[i] = NULL;
        }
    }
    g_pDataPackToLoad = NULL;
}
#pragma pop

void FileStreaming_Destroy(void) {
    extern u8 *g_StreamingDataPacks[11];
    s32 FileStreaming_ReleaseAllDataPacks(s32 type, s8 wait);
    void Deallocate(void *);
    u32 i;

    for (i = 0; i < 11; i++) {
        FileStreaming_ReleaseAllDataPacks(i, 1);
        Deallocate(g_StreamingDataPacks[i]);
    }
}

s8 IsLoadingThreadRunning(void) {
    extern s8 g_bLoadingThreadRunning;
    return g_bLoadingThreadRunning;
}

s32 FileStreaming_GetNumAvailableQuadPacks(void) {
    return 0xB;
}

void FileStreaming_LoadingThread(void) {
    extern s8 g_bLoadingThreadRunning;
    extern s8 g_bLoadQuadrants;
    extern s8 g_bLoadDataPack;
    void LoadQuadrants(s32);
    void LoadDataPack(void);
    void EndThread(void);

    g_bLoadingThreadRunning = 1;
    if (g_bLoadQuadrants) {
        LoadQuadrants(0);
    }
    if (g_bLoadDataPack) {
        LoadDataPack();
    }
    g_bLoadQuadrants = 0;
    g_bLoadDataPack = 0;
    g_bLoadingThreadRunning = 0;
    EndThread();
}

#pragma push
#pragma use_lmw_stmw on
s32 FileStreaming_GetPackID(char *name, s32 type, u32 id) {
    extern u8 *g_StreamingDataPacks[11];
    extern u32 g_MaxStreamingDataPacks[11];
    u8 *pack;
    u32 i;

    if (type >= 11) {
        return -1;
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++) {
        pack = g_StreamingDataPacks[type] + i * 0xA8;
        if ((s8)pack[0] != 0 && id == M2C_FIELD(pack, u32 *, 8)) {
            if (name == M2C_FIELD(pack, char **, 4) || strcmp(name, M2C_FIELD(pack, char **, 4)) == 0) {
                return i;
            }
        }
    }
    return -1;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
#pragma auto_inline off
s32 FileStreaming_ReleaseAllDataPacks(s32 type, s8 wait) {
    s32 FileStreaming_ReleaseDataPack(u32 index, s32 type);
    void FileStreaming_HeartBeat(void);
    extern u8 *g_StreamingDataPacks[11];
    extern u32 g_MaxStreamingDataPacks[11];
    u32 i;

    if (type >= 11) {
        return 0;
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++) {
        if (g_StreamingDataPacks[type] != NULL && (s8)g_StreamingDataPacks[type][i * 0xA8 + 1] != 0) {
            M2C_FIELD(g_StreamingDataPacks[type] + i * 0xA8, s32 *, 0xC) = 1;
            FileStreaming_ReleaseDataPack(i, type);
        }
    }
    if (wait) {
        FileStreaming_HeartBeat();
        FileStreaming_HeartBeat();
    }
    return 1;
}
#pragma pop

#pragma push
#pragma auto_inline off
#pragma dont_inline on
void FileStreaming_HeartBeat(void) {
    void DataPack_ReleaseDataPack(void *pack);
    extern u32 g_numDataPacksToRelease;
    extern u32 g_numQuadPacksToRelease;
    extern u8 *g_pPacksToBeReleased[10];
    extern void *g_pQuadPackToBeReleased[11];
    u32 i;

    for (i = 0; i < g_numDataPacksToRelease; i++) {
        DataPack_ReleaseDataPack(g_pPacksToBeReleased[i] + 0x14);
        g_pPacksToBeReleased[i][0] = 0;
    }
    g_numDataPacksToRelease = 0;
    for (i = 0; i < g_numQuadPacksToRelease; i++) {
        DataPack_ReleaseQuadPack(g_pQuadPackToBeReleased[i]);
    }
    g_numQuadPacksToRelease = 0;
}

void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh) {
    extern u8 *g_StreamingDataPacks[11];
    extern u32 g_MaxStreamingDataPacks[11];
    void DataPack_RefreshPtrs(void *pack);
    u8 *pack;

    if (type >= 11 || index >= g_MaxStreamingDataPacks[type]) {
        return NULL;
    }
    pack = g_StreamingDataPacks[type] + index * 0xA8;
    if ((s8)pack[0] == 0 || (s8)pack[1] == 0) {
        return NULL;
    }
    if ((s8)pack[2] == 0 && refresh) {
        DataPack_RefreshPtrs(pack + 0x14);
    }
    g_StreamingDataPacks[type][index * 0xA8 + 2] = 1;
    return g_StreamingDataPacks[type] + index * 0xA8 + 0x14;
}

#pragma pop

void LoadDataPack(void) {
    void DataPack_LoadFile(s32 file, void *pack, s32 type, s32 size, s32 *out);
    extern u8 *g_pDataPackToLoad;
    extern s32 g_dataPackToLoadType;
    s32 result;
    u8 *pack;

    pack = g_pDataPackToLoad;
    if (pack != NULL) {
        g_pDataPackToLoad = NULL;
        pack[1] = 1;
        DataPack_LoadFile(M2C_FIELD(pack, s32 *, 4), pack + 0x14, g_dataPackToLoadType, M2C_FIELD(pack, s32 *, 8), &result);
    }
}

void ForceFeedback_Init(void) {
    extern u8 g_forcefeedback[24][0x60];
    s32 i;

    for (i = 0; i < 24; i++) {
        *(s32 *)g_forcefeedback[i] = 0;
    }
}

void ForceFeedback_Play(void *effect, Vector4 *dir) {
    typedef struct ForceFeedback {
        void *effect;
        u8 unk4[0x1C - 0x4];
        u8 active;
        u8 unk1D[0x30 - 0x1D];
        u8 playing;
        u8 unk31[0x40 - 0x31];
        Vector4 dir;
        s32 unk50;
        s32 unk54;
        s32 unk58;
        u8 unk5C[0x60 - 0x5C];
    } ForceFeedback;
    extern ForceFeedback g_forcefeedback[24];
    s32 i;

    for (i = 0; i < 24; i++) {
        if (g_forcefeedback[i].effect == NULL) {
            g_forcefeedback[i].effect = effect;
            g_forcefeedback[i].active = 1;
            g_forcefeedback[i].playing = 1;
            g_forcefeedback[i].dir = *dir;
            g_forcefeedback[i].unk54 = M2C_FIELD(effect, s32 *, 0x14);
            g_forcefeedback[i].unk58 = M2C_FIELD(effect, s32 *, 0x18);
            g_forcefeedback[i].unk50 = M2C_FIELD(effect, s32 *, 0x10);
            return;
        }
    }
}

void GameSettings_ResetGame(void) {
    s32 i;

    g_codeVariables.unk30 = 1;
    for (i = 0; i < 4; i++) {
        g_codeVariables.unk34[i] = 0;
        g_codeVariables.unk44[i] = 0;
        g_codeVariables.unk54[i] = 0;
        g_codeVariables.unk64[i] = 0;
    }
    g_codeVariables.unkE4 = 0;
    g_codeVariables.unkE8 = 0;
    g_codeVariables.unkEC = 300000;
    g_codeVariables.unkF0 = 10;
    g_codeVariables.unkF4 = 1000;
    g_codeVariables.unkF8 = 3;
    g_codeVariables.unkFC = 0;
    g_codeVariables.unk100 = 3;
    g_codeVariables.unk104 = 1;
    g_codeVariables.unk108 = 1;
    g_codeVariables.unk111 = 1;
}

void GameSettings_SetDefaultControllerState(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_codeVariables.controllerState[i] = 1;
    }
}

void GameSettings_ResetPlayerVehicleItem(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_codeVariables.playerVehicle[i] = -1;
        g_codeVariables.playerItem[i] = -1;
    }
}

u8 GameSettings_GetGameType(void) {
    void *GameSettings_GetMCP();
    void *temp_r3;

    temp_r3 = GameSettings_GetMCP();
    if (temp_r3 != NULL) {
        return M2C_FIELD(temp_r3, u8 *, 1);
    }
    return 0U;
}

u8 *GameSettings_GetMCP(void) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    void *pack;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            return NULL;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    return pMCP;
}

void GameSettings_GameBoyConnected(void) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    u8 *mcp;
    void *pack;
    s32 state;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        state = mcp[1];
    } else {
        state = 0;
    }
    switch (state) {
    case 0:
        g_HotWheelsGameData.unk0 = 1;
        g_HotWheelsGameData.unk1 = 1;
        g_HotWheelsGameData.unk2 = 1;
        break;
    case 1:
        break;
    }
}

s32 GameSettings_GetDifficulty(void) {
    return g_generalGameSettings.difficulty;
}

u8 GameSettings_GetNumOfPlayers(void) {
    s32 PlayerManager_PlayerHumanCount(void);
    return PlayerManager_PlayerHumanCount();
}

ControllerSettings *GameSettings_GetControllerSettings(s32 player) {
    return &g_generalGameSettings.controllers[player - 1];
}

void SetPlayerControls(void);
void SetMusicGVol(u32);
void SetSFXVol(u32);
void SetSpeakerMode(u8);
void OSSetSoundMode(u32);
AudioSettings *GameSettings_GetAudioSettings(void) {
    return &g_generalGameSettings.audio;
}

void GameSettings_ApplyDefaultGameData(void) {
    SetPlayerControls();
    SetMusicGVol((f32)(g_generalGameSettings.audio.musicVolume * 0x7FFF) / 100.0f);
    SetSFXVol((f32)(g_generalGameSettings.audio.sfxVolume * 0x7FFF) / 100.0f);
    SetSpeakerMode(g_generalGameSettings.audio.speakerMode);
    OSSetSoundMode(g_generalGameSettings.audio.speakerMode);
}

void GameSettings_DetermineIfOverwritingFile(void) {
    if (g_bValidFileName_803C6CA4[g_codeVariables.fileIndex - 1]) {
        g_codeVariables.bOverwritingFile = 1;
    } else {
        g_codeVariables.bOverwritingFile = 0;
    }
}

s32 GameSettings_FormatMemoryCard(void) {
    u8 FileManager_FormatDevice(char *device);
    extern s8 g_bRefreshMC_803C6F78;
    extern s8 g_bRefreshMC_FileList_803C6F79;
    extern s8 g_bRefreshMC_FileInfo_803C6F7A;
    u8 result;

    result = FileManager_FormatDevice(M2C_FIELD(&g_codeVariables, char **, 0x114) + 0xA);
    g_bRefreshMC_803C6F78 = 1;
    M2C_FIELD(&g_codeVariables, u8 *, 0x11E) = result;
    g_bRefreshMC_FileList_803C6F79 = 1;
    g_bRefreshMC_FileInfo_803C6F7A = 1;
    return result;
}

s32 Geometry_Create(void) {
    extern u8 *g_pGeometryHandler;
    u32 i;

    g_pGeometryHandler = Allocate(0x24008, 0, "");
    if (g_pGeometryHandler == NULL) {
        return 0;
    }
    M2C_FIELD(g_pGeometryHandler, u32 *, 0x24004) = 0x800;
    M2C_FIELD(g_pGeometryHandler, u32 *, 0x24000) = 0;
    for (i = 0; i < M2C_FIELD(g_pGeometryHandler, u32 *, 0x24004); i++) {
        *(s32 *)(g_pGeometryHandler + i * 0x48) = 0;
        *(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44) = 0;
    }
    return 1;
}

void Geometry_Destroy(void) {
    M2C_UNK Geometry_Init(M2C_UNK);
    extern s32 g_pGeometryHandler;
    Geometry_Init(1);
    Deallocate(g_pGeometryHandler);
    g_pGeometryHandler = 0;
}

void Geometry_Init(s8 clear) {
    void Geometry_FreeGeometry(s32 geometry);
    extern u8 *g_pGeometryHandler;
    u32 i;

    for (i = 0; i < M2C_FIELD(g_pGeometryHandler, u32 *, 0x24004); i++) {
        if (clear == 1) {
            *(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44) = 0;
        }
        Geometry_FreeGeometry(*(s32 *)(g_pGeometryHandler + i * 0x48));
    }
}

void Geometry_FreeGeometry(void *geometry) {
    void Deallocate(void *);
    extern u8 *g_pGeometryHandler;
    u32 i;

    if (geometry != NULL) {
        for (i = 0; i < M2C_FIELD(g_pGeometryHandler, u32 *, 0x24004); i++) {
            if (*(void **)(g_pGeometryHandler + i * 0x48) == geometry) {
                (*(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44))--;
                if (*(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44) < 1) {
                    Deallocate(geometry);
                    *(void **)(g_pGeometryHandler + i * 0x48) = NULL;
                    *(u8 *)(g_pGeometryHandler + i * 0x48 + 4) = 0;
                    M2C_FIELD(g_pGeometryHandler, s32 *, 0x24000)--;
                }
            }
        }
    }
}

void Grid_Release(void) {

}

void Grid_FreeObject(u8 *object) {
    extern u8 gGridManager[];
    u8 *node;
    u8 *next;

    if (object != NULL) {
        for (node = *(u8 **)(object + 8); node != NULL; node = next) {
            next = *(u8 **)(node + 0x10);
            if (*(u8 **)(node + 8) != NULL) {
                *(u8 **)(*(u8 **)(node + 8) + 0xC) = *(u8 **)(node + 0xC);
            }
            if (*(u8 **)(node + 0xC) != NULL) {
                *(u8 **)(*(u8 **)(node + 0xC) + 8) = *(u8 **)(node + 8);
            } else {
                **(u8 ***)(node + 0x14) = *(u8 **)(node + 8);
            }
            (*(s32 *)(*(u8 **)(node + 0x14) + 4))--;
            *(u8 **)(node + 8) = ((u8 **)(gGridManager + 0x1B3B8))[0];
            ((u8 **)(gGridManager + 0x1B3B8))[0] = node;
        }
        *(u8 **)(object + 0x1C) = *(u8 **)(gGridManager + 0x233BC);
        *(u8 **)(gGridManager + 0x233BC) = object;
        *(s32 *)(gGridManager + 0x233C0) += 1;
    }
}

void InterfaceManager_Init(void) {
    extern u8 g_interface[][0x170];
    extern s32 g_TargetArrow[7];
    extern void *g_pInterfaceToEnd;
    extern s32 g_numInterfaceObjectsAllocated;
    extern s32 g_numInterfaceObjectAllocations;
    extern s32 g_numInterfacesToRender;
    extern void *g_playerMessage1;
    extern void *g_playerMessage2;
    extern void *g_playerMessage3;
    u32 i;

    g_numInterfaceObjectsAllocated = 0;
    g_numInterfaceObjectAllocations = 0;
    g_numInterfacesToRender = 0;
    g_pInterfaceToEnd = NULL;
    for (i = 0; i < 0x12; i++) {
        *(void **)g_interface[i] = NULL;
        g_interface[i][0x7D] = i;
    }
    g_TargetArrow[0] = 0;
    g_playerMessage1 = NULL;
    g_TargetArrow[1] = 0;
    g_TargetArrow[3] = 0;
    g_TargetArrow[4] = -1;
    g_TargetArrow[5] = 0;
    g_playerMessage2 = NULL;
    g_playerMessage3 = NULL;
}

void InterfaceManager_SetRadarBlip(u16 arg0) {
    extern s32 g_radarTextureIndex;
    g_radarTextureIndex = (s32) arg0;
}

void InterfaceManager_ReleaseAllInterfaces(void) {
    extern u8 g_interface[][0x170];
    extern s32 g_TargetArrow[7];
    extern void *g_pInterfaceToEnd;
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL) {
            InterfaceManager_ReleaseInterface(i);
        }
    }
    g_TargetArrow[0] = 0;
    g_pInterfaceToEnd = NULL;
    g_TargetArrow[1] = 0;
    g_TargetArrow[3] = 0;
    g_TargetArrow[4] = -1;
    g_TargetArrow[5] = 0;
}

void InterfaceManager_ReleaseAllUnpausedInterfaces(void) {
    extern u8 g_interface[][0x170];
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL && g_interface[i][0x83] == 0) {
            InterfaceManager_ReleaseInterface(i);
        }
    }
}

void InterfaceManager_SetCallBackRoutine(s32 index, void *callback) {
    extern u8 g_interface[][0x170];
    *(void **)(g_interface[index] + 0x90) = callback;
}

void InterfaceManager_SetInfoCallBackRoutine(s32 index, void *callback) {
    extern u8 g_interface[][0x170];
    *(void **)(g_interface[index] + 0x94) = callback;
}

s32 InterfaceManager_GetTotalActiveInterfaces(void) {
    extern u8 g_interface[][0x170];
    s32 count = 0;
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL) {
            count++;
        }
    }
    return count;
}

void InterfaceManager_PauseAllInterface(u8 pause) {
    extern u8 g_interface[][0x170];
    u32 i;

    for (i = 0; i < 0x12; i++) {
        g_interface[i][0x83] = pause;
    }
}

void InterfaceManager_SetOffset(s32 index, f32 x, f32 y) {
    extern u8 g_interface[][0x170];
    u8 *iface = g_interface[index];
    iface[0x82] = 1;
    *(f32 *)(iface + 0x60) = x;
    *(f32 *)(iface + 0x64) = y;
}

void InterfaceManager_SetObjectText(u8 *obj, char **text) {
    u32 strlen(const char *s);
    char *str;

    if (text != NULL) {
        u8 *info = *(u8 **)(*(u8 **)(obj + 0x258) + 4);
        obj[0x248] = 1;
        obj[0x2C] = 1;
        obj[0x54] = 1;
        obj[0x84] = 1;
        obj[0x164] = 1;
        obj[0x194] = 1;
        *(u16 *)(obj + 0x1AA) = 0;
        *(u32 *)(obj + 0x1AC) = *(u32 *)(info + 0x14);
        *(u16 *)(obj + 0x1A8) = 0;
        obj[0x1C0] = 0;
        *(char ***)(obj + 0x1A0) = text;
        *(char **)(obj + 0x1A4) = NULL;
        obj[0x1B1] = 0;
        str = *text;
        if (str != NULL) {
            *(char **)(obj + 0x1A4) = str;
            *(u16 *)(obj + 0x1A8) = strlen(str);
            if (*(s32 *)(obj + 0x1AC) == 0) {
                *(u16 *)(obj + 0x1AA) = *(u16 *)(obj + 0x1A8);
            }
            if (*(u8 *)(*(u8 **)(obj + 0x258) + 0x12) != 0) {
                obj[0x1B1] = 1;
            }
        }
    }
}

void InterfaceManager_EndInterface(s32 index) {
    extern u8 g_interface[][0x170];
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    u8 *iface = g_interface[index];

    if (iface != NULL) {
        InterfaceManager_SetLoadState(iface, 1);
    }
}

void InterfaceManager_LinkThingToObject(s32 index, u32 id, void *thing) {
    typedef struct InterfaceSlot {
        u8 pad0[0x25C];
        u8 *object;
        u8 pad260[0x288 - 0x260];
        void *thing;
        u8 pad28C[0x300 - 0x28C];
    } InterfaceSlot;
    extern u8 g_interface[][0x170];
    u8 *iface = g_interface[index];
    u32 i;

    if (iface != NULL) {
        for (i = 0; i < *(u32 *)iface; i++) {
            if (id == *(u32 *)((*(InterfaceSlot **)(iface + 0x88))[i].object + 0x10)) {
                (*(InterfaceSlot **)(iface + 0x88))[i].thing = thing;
            }
        }
    }
}

void InterfaceManager_SetControllerConfigInputString(void) {

}

void InterfaceManager_Render(void) {
    extern u8 g_interface[][0x170];
    extern u32 g_interfaceRenderOrder[18];
    extern u32 g_numInterfacesToRender;
    void InterfaceManager_RenderInterface(void *iface);
    u32 i;
    u8 *iface;

    for (i = 0; i < g_numInterfacesToRender; i++) {
        iface = g_interface[g_interfaceRenderOrder[i]];
        if (*(void **)iface != NULL && (*(u32 *)(iface + 4) != 1 || *(u32 *)(iface + 0x10) != 0) && iface[0x7F] == 0) {
            InterfaceManager_RenderInterface(iface);
        }
    }
}

s32 ActionInterfacePlay(s32 *arg0) {
    M2C_UNK InterfaceManager_Create(s32, M2C_UNK, M2C_UNK);
    InterfaceManager_Create(*arg0, 0, 0);
    return 1;
}

#pragma push
#pragma use_lmw_stmw on
s32 ActionHUDChangeMode(u32 *action) {
    extern u8 g_cameraManager[];
    void Player_RefreshInterface(void *player);
    void InterfaceManager_ReleaseInterface(u32 index);
    s32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        if (action[1] == 1) {
            Player_RefreshInterface(M2C_FIELD(g_cameraManager + i * 0x190, void **, 0));
        } else {
            InterfaceManager_ReleaseInterface(M2C_FIELD(M2C_FIELD(g_cameraManager + i * 0x190, u8 **, 0), u32 *, 0x68));
            M2C_FIELD(M2C_FIELD(g_cameraManager + i * 0x190, u8 **, 0), s32 *, 0x68) = -1;
        }
    }
    return 1;
}
#pragma pop

s32 InterfaceManager_GetTargetArrowNearestNavPoint(void) {
    extern s32 g_TargetArrow[7];
    s32 NavHolder_FindIndexOfNearestNavPoint(void *pos, s32 hint);

    if (g_TargetArrow[4] == -1 || g_TargetArrow[5] <= 0) {
        g_TargetArrow[4] = NavHolder_FindIndexOfNearestNavPoint((void *)g_TargetArrow[0], g_TargetArrow[4]);
        g_TargetArrow[5] = 300;
    }
    return g_TargetArrow[4];
}

s32 DropShip_IsChildInterfaceReady(s32 index) {
    typedef struct DropShipSlot {
        u8 pad0[0x243];
        u8 type;
        u8 pad244[0x270 - 0x244];
        s32 child;
        u8 pad274[0x300 - 0x274];
    } DropShipSlot;
    extern u8 g_interface[][0x170];
    u32 i;
    u8 *iface = g_interface[index];
    u8 *child;

    for (i = 0; i < *(u32 *)iface; i++) {
        if ((*(DropShipSlot **)(iface + 0x88))[i].type == 6) {
            child = g_interface[(*(DropShipSlot **)(iface + 0x88))[i].child];
            if (child[0x83] == 0) {
                if (*(u32 *)(child + 4) == 0 || (*(u32 *)(child + 4) == 1 && *(u32 *)(child + 0x10) != 0)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

#pragma push
#pragma use_lmw_stmw on
void DropShip_EndChildInterface(s32 index, u32 id) {
    typedef struct DropShipSlot {
        u8 pad0[0x243];
        u8 type;
        u8 pad244[0x25C - 0x244];
        u8 *anim;
        u8 pad260[0x270 - 0x260];
        s32 child;
        u8 pad274[0x300 - 0x274];
    } DropShipSlot;
    extern u8 g_interface[][0x170];
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    u32 i;
    u8 *iface = g_interface[index];
    DropShipSlot *slot;

    for (i = 0; i < *(u32 *)iface; i++) {
        slot = &(*(DropShipSlot **)(iface + 0x88))[i];
        if (slot->type == 6 && *(u16 *)(slot->anim + 0xA) == id &&
            *(u32 *)(g_interface[slot->child] + 4) != 1) {
            InterfaceManager_SetLoadState(g_interface[slot->child], 1);
        }
    }
}
#pragma pop

void DropShip_EndInterface(s32 index) {
    extern u8 g_interface[][0x170];
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    void InterfaceManager_MoveInterface(void *iface, f32 time);
    u8 *iface = g_interface[index];

    *(u16 *)(*(u8 **)(*(u8 **)(iface + 0x1C) + 8) + 6) = 8;
    if (*(u32 *)(iface + 4) != 1) {
        InterfaceManager_SetLoadState(iface, 1);
        InterfaceManager_MoveInterface(iface, 0.0f);
    }
}

f32 DropShip_GetLRTime(s32 index) {
    typedef struct DropShipSlot {
        u8 pad0[0x245];
        u8 state;
        u8 pad246[0x25C - 0x246];
        u8 *anim;
        u8 pad260[0x300 - 0x260];
    } DropShipSlot;
    extern u8 g_interface[][0x170];
    u32 i;
    u8 *iface = g_interface[index];

    for (i = 0; i < *(u32 *)iface; i++) {
        if ((*(DropShipSlot **)(iface + 0x88))[i].state == 5) {
            return *(u16 *)((*(DropShipSlot **)(iface + 0x88))[i].anim + 0xA);
        }
    }
    return 0.0f;
}

void InterfaceManager_SetPlayerMessage(void *message, u32 player) {
    extern void *g_playerMessage1;
    extern void *g_playerMessage2;
    extern void *g_playerMessage3;

    if (player == 0) {
        g_playerMessage1 = message;
    } else if (player == 1) {
        g_playerMessage2 = message;
    } else if (player == 2) {
        g_playerMessage3 = message;
    }
}

s32 ActionEndInterface(void *action) {
    extern u8 g_interface[][0x170];
    extern void *g_pInterfaceToEnd;
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    u32 i;
    s8 found;

    if (M2C_FIELD(action, u32 *, 0) == 0xFFFFFFFF) {
        for (i = 0; i < 0x12; i++) {
            if (*(void **)g_interface[i] != NULL && g_interface[i] != NULL) {
                InterfaceManager_SetLoadState(g_interface[i], 1);
            }
        }
    } else {
        found = 0;
        for (i = 0; i < 0x12; i++) {
            if (*(void **)g_interface[i] != NULL && M2C_FIELD(g_interface[i], void **, 0x20) == M2C_FIELD(action, void **, 4)) {
                if (g_interface[i] != NULL) {
                    InterfaceManager_SetLoadState(g_interface[i], 1);
                }
                found = 1;
            }
        }
        if (found == 0 && M2C_FIELD(action, void **, 8) != NULL) {
            g_pInterfaceToEnd = M2C_FIELD(action, void **, 4);
        }
    }
    return 1;
}

s32 InterfaceManager_AddFont(u8 *font) {
    s32 Gfx_FontManager_AddFont(u16, u8, f32, f32, f32);
    void Gfx_FontManager_SetCharacterSpacing(s32 id, u8 c, f32 x, f32 y);
    u32 i;
    s32 id;
    u8 *spacing;

    id = Gfx_FontManager_AddFont(M2C_FIELD(font, u32 *, 0), font[0xB], font[9], font[0xA], M2C_FIELD(font, f32 *, 0x28));
    for (i = 0; i < M2C_FIELD(font, u32 *, 0x20); i++) {
        spacing = M2C_FIELD(font, u8 **, 0x24) + i * 0xC;
        Gfx_FontManager_SetCharacterSpacing(id, spacing[0], M2C_FIELD(spacing, f32 *, 4), M2C_FIELD(spacing, f32 *, 8));
    }
    return id;
}

s32 Item_Init(u8 *item, u8 *data) {
    void QuaternionToSubMatrix(void *quat, void *mtx);
    f32 RandomAdd(f32 base, f32 range);
    void GenerateQuaternionXYZ(Vector4 *euler, Vector4 *quat);
    void GenerateSpinAngleFromQuaternion(Vector4 *quat, void *axis, f32 *angle);
    s32 NavHolder_FindIndexOfNearestNavPoint(void *pos, s32 hint);
    Vector4 euler;
    Vector4 quat;
    u8 *info;

    Thing_Init(item, (s32)data);
    M2C_FIELD(item, s32 *, 0xD4) = -1;
    M2C_FIELD(item, s32 *, 0xD0) = 0;
    M2C_FIELD(item, u16 *, 0x80) = 2;
    M2C_FIELD(item, f32 *, 0x70) = M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0xC);
    M2C_FIELD(item, f32 *, 0xE8) = M2C_FIELD(item, f32 *, 0x70) * M2C_FIELD(item, f32 *, 0x70);
    M2C_FIELD(item, Vector4 *, 0x40) = M2C_FIELD(data, Vector4 *, 0x10);
    M2C_FIELD(item, f32 *, 0x4C) = 1.0f;
    M2C_FIELD(item, Vector4 *, 0x0) = M2C_FIELD(data, Vector4 *, 0x20);
    M2C_FIELD(item, Vector4 *, 0x50) = M2C_FIELD(data, Vector4 *, 0x0);
    M2C_FIELD(item, s32 *, 0xEC) = 0;
    QuaternionToSubMatrix(item + 0x50, item + 0x10);
    M2C_FIELD(item, f32 *, 0xC8) = M2C_FIELD(data, f32 *, 0x34);
    M2C_FIELD(item, u8 *, 0xE0) = M2C_FIELD(data, u8 *, 0x38);
    if (M2C_FIELD(item, f32 *, 0xC8) == 0.0f) {
        M2C_FIELD(item, f32 *, 0xC8) = 50.0f;
    }
    M2C_FIELD(item, s32 *, 0xD8) = 0;
    M2C_FIELD(item, u32 *, 0x74) |= 0x1000000;
    M2C_FIELD(item, u8 **, 0xC0) = M2C_FIELD(data, u8 **, 0x30);
    info = M2C_FIELD(M2C_FIELD(item, u8 **, 0xC0), u8 **, 0x30);
    if (info != NULL) {
        M2C_FIELD(item, s16 *, 0xCE) = M2C_FIELD(info, s32 *, 0x20);
    }
    euler.x = 0.0f;
    euler.y = 6.2831855f * RandomAdd(M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0x18), M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0x1C));
    euler.z = 0.0f;
    GenerateQuaternionXYZ(&euler, &quat);
    GenerateSpinAngleFromQuaternion(&quat, item + 0xB0, (f32 *)(item + 0xC4));
    M2C_FIELD(item, s16 *, 0xCC) = 1000;
    Thing_InitAllSlavesNow(item, M2C_FIELD(item, u8 **, 0xC0) + 0x38, 0xFF);
    M2C_FIELD(item, u32 *, 0x74) |= 8;
    M2C_FIELD(item, s32 *, 0xD4) = NavHolder_FindIndexOfNearestNavPoint(item + 0x40, -1);
    Thing_AddToGrid(item, 3);
    return 1;
}

void Item_Release(void *item) {
    extern u8 g_itemVTable[];
    void Thing_Release(s32 thing);
    void *slot;

    if (M2C_FIELD(item, void **, 0x8C) == g_itemVTable) {
        slot = M2C_FIELD(item, void **, 0x90);
        if (slot != NULL && M2C_FIELD(slot, void **, 0x24) != NULL) {
            M2C_FIELD(item, void **, 0x90) = NULL;
            M2C_FIELD(slot, void **, 0x24) = NULL;
        }
    }
    Thing_Release((s32)item);
}

s32 Item_PreMove(void) {
    return 1;
}

void Item_Draw(u8 *item) {
    f32 Gfx_GetMinCameraDistSquared(void *pos);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void Thing_DrawSharedSlaves(void *thing, void *slaves);

    if (0.0f != M2C_FIELD(item, f32 *, 0xC8) &&
        Gfx_GetMinCameraDistSquared(item + 0x40) > M2C_FIELD(item, f32 *, 0xC8) * M2C_FIELD(item, f32 *, 0xC8)) {
        return;
    }
    if (M2C_FIELD(item, u32 *, 0x74) & 8) {
        QuaternionToSubMatrix(item + 0x50, item + 0x10);
    }
    Thing_DrawSharedSlaves(item, M2C_FIELD(item, u8 **, 0xC0) + 0x38);
}

s32 Item_GetNearestNavPoint(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0xD4);
}

s32 Item_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void LensFlare_Init(void) {
    extern u8 g_pFlare[64][0x30];
    extern s32 g_activeFlares;
    extern s32 g_LensFlareRayTest;
    s32 i;

    for (i = 0; i < 64; i++) {
        *(s32 *)g_pFlare[i] = 0;
    }
    g_activeFlares = 0;
    g_LensFlareRayTest = 1;
}

s32 LensFlare_Add(void *flare, s32 arg1, Vector4 *pos) {
    typedef struct Flare {
        void *flare;
        s32 unk4;
        u8 unk8[0x10 - 0x8];
        Vector4 pos;
        u8 unk20;
        u8 unk21[0x30 - 0x21];
    } Flare;
    extern Flare g_pFlare[64];
    extern u32 g_activeFlares;
    u32 i;

    if (flare != NULL) {
        for (i = 0; i < 64; i++) {
            if (g_pFlare[i].flare == NULL) {
                g_pFlare[i].unk4 = arg1;
                g_pFlare[i].flare = flare;
                g_pFlare[i].unk20 = 0;
                if (pos != NULL) {
                    g_pFlare[i].pos = *pos;
                }
                break;
            }
        }
        if (i != 64) {
            if (i >= g_activeFlares) {
                g_activeFlares = i + 1;
            }
            return i;
        }
    }
    return -1;
}

void LightHolder_Init(void) {
    extern u8 g_pLight[16][0x80];
    u32 i;

    for (i = 0; i < 16; i++) {
        *(void **)(g_pLight[i] + 0x60) = NULL;
    }
}

void LightHolder_SetTexture(u16 index) {
    void Gfx_SetTextureWrapMode(u16 id, s16 mode);
    extern u16 g_lightTextureIndex;
    g_lightTextureIndex = index;
    Gfx_SetTextureWrapMode(index, 1);
}

void LightHolder_Delete(s32 index) {
    extern struct {
        u8 unk0[0x60];
        void *owner;
        u8 unk64[0x1C];
    } g_pLight[16];
    if (index >= 0) {
        g_pLight[index].owner = NULL;
    }
}

void LightHolder_Render(void) {
    extern u8 g_pLight[16][0x80];
    void Gfx_GetCameraPosition(Vector4 *pos);
    void Gfx_ScreenOverlay(GXColor *color, u8 mode);
    void DistanceXYZ(void *a, void *b, f32 *dist);
    Vector4 camera;
    f32 dist;
    GXColor color;
    u8 *light;
    u8 *pos;
    u8 *owner;
    u32 i;

    Gfx_GetCameraPosition(&camera);
    for (i = 0; i < 16; i++) {
        light = g_pLight[i];
        owner = *(u8 **)(light + 0x60);
        if (owner != NULL) {
            if (*(u8 **)(light + 0x64) != NULL) {
                pos = *(u8 **)(light + 0x64) + 0x40;
            } else {
                pos = light;
            }
            if (owner[0] != 0) {
                DistanceXYZ(&camera, pos, &dist);
                if (dist <= *(f32 *)(light + 0x40)) {
                    color = *(GXColor *)(light + 0x34);
                    Gfx_ScreenOverlay(&color, (*(u8 **)(light + 0x60))[3]);
                }
            }
        }
    }
}

void LightHolder_SetRenderState(void *camera) {
    extern u8 g_pLight[16][0x80];
    u32 CompareDistances(void *arg0, void *arg1, f32 farg0);
    void Gfx_AddPointLight(GXColor *color, void *position, f32 radius);
    void Gfx_RemovePointLights(void);
    u8 *light;
    u32 i;
    u8 *pos;
    u8 *owner;
    GXColor color;

    Gfx_RemovePointLights();
    for (i = 0; i < 16; i++) {
        light = g_pLight[i];
        owner = *(u8 **)(light + 0x60);
        if (owner != NULL && owner[1] != 0) {
            if (*(u8 **)(light + 0x64) != NULL) {
                pos = *(u8 **)(light + 0x64) + 0x40;
            } else {
                pos = light;
            }
            if (CompareDistances(camera, pos, *(f32 *)(light + 0x40)) != 1) {
                color = *(GXColor *)(light + 0x34);
                Gfx_AddPointLight(&color, pos, *(f32 *)(light + 0x40));
            }
        }
    }
}

void LineManager_Init(void) {
    extern u8 g_lineManager[128][0xF0];
    u32 i;

    for (i = 0; i < 128; i++) {
        *(s32 *)(g_lineManager[i] + 0x50) = 0;
    }
}

void LineManager_Render(void) {
    typedef struct LinePoints {
        u32 data[8];
    } LinePoints;
    typedef struct Line {
        LinePoints points;
        u8 unk20[0x50 - 0x20];
        u32 active;
        u8 unk54[0x78 - 0x54];
        u32 unk78;
        u8 unk7C[0xA8 - 0x7C];
        u32 colour;
        u8 unkAC[0xE0 - 0xAC];
        u8 unkE0;
        u8 unkE1[0xF0 - 0xE1];
    } Line;
    typedef struct LineFX {
        LinePoints points;
        u32 colour;
        u32 unk24;
        u8 unk28;
    } LineFX;
    extern Line g_lineManager[128];
    LineFX *Gfx_Add3DLineFX(void);
    u32 i;
    LineFX *fx;

    for (i = 0; i < 128; i++) {
        if (g_lineManager[i].active != 0) {
            fx = Gfx_Add3DLineFX();
            fx->points = g_lineManager[i].points;
            fx->colour = g_lineManager[i].colour;
            fx->unk24 = g_lineManager[i].unk78;
            fx->unk28 = g_lineManager[i].unkE0;
        }
    }
}

s32 LogHandler_Create(void) {
    return 0;
}

void LogHandler_Destroy(void) {

}

void Looker_Draw(void) {

}

s32 Main_Loop(void) {
    extern s32 (*Heartbeat_Process)(void);
    s32 StateManager_QueuedState(void);
    s32 StateManager_GotoState(s32 state);
    s32 queued;

    queued = StateManager_QueuedState();
    if (queued != -1 && queued != StateManager_GotoState(queued)) {
        return 0;
    }
    return Heartbeat_Process();
}

s32 Main_Shutdown(void) {
    void PlayerManager_Destroy(void);
    void ThingManager_DARYL_Destroy(void);
    void Input_Destroy(void);
    void ShaderHolder_Destroy(void);
    void VertexColorSeqManager_Destroy(void);
    void MatrixCache_Destroy(void);
    void World_Destroy(void);
    void Geometry_Destroy(void);
    void StateManager_Destroy(void);
    void CameraManager_Destroy(void);
    void Trigger_Destroy(void);
    void Action_Destroy(void);
    void LogHandler_Destroy(void);
    void FileManager_Destroy(void);
    void Gfx_Destroy(void);
    void StateGameManager_Release(void);
    void FileStreaming_Destroy(void);
    void SoundManager_Destroy(void);
    void HeapDump(void);

    PlayerManager_Destroy();
    ThingManager_DARYL_Destroy();
    Input_Destroy();
    ShaderHolder_Destroy();
    VertexColorSeqManager_Destroy();
    MatrixCache_Destroy();
    World_Destroy();
    Geometry_Destroy();
    StateManager_Destroy();
    CameraManager_Destroy();
    Trigger_Destroy();
    Action_Destroy();
    LogHandler_Destroy();
    FileManager_Destroy();
    Gfx_Destroy();
    StateGameManager_Release();
    FileStreaming_Destroy();
    SoundManager_Destroy();
    HeapDump();
    return 1;
}

#pragma push
#pragma use_lmw_stmw on
void MatrixCache_Create(void) {
    u8 *buf;
    u32 i;

    g_pMatrixCacheData = Allocate(0x2A720, 0, "");
    buf = (u8 *)g_pMatrixCacheData + sizeof(MatrixCacheData);
    for (i = 0; i < 0x60; i++) {
        g_pMatrixCacheData->small[i].key = (u32)buf;
        g_pMatrixCacheData->small[i].mtx = NULL;
        buf += 0x100;
    }
    for (i = 0; i < 0x80; i++) {
        g_pMatrixCacheData->medium[i].key = (u32)buf;
        g_pMatrixCacheData->medium[i].mtx = NULL;
        buf += 0x400;
    }
    for (i = 0; i < 4; i++) {
        g_pMatrixCacheData->large[i].key = (u32)buf;
        g_pMatrixCacheData->large[i].mtx = NULL;
        buf += 0x1000;
    }
}
#pragma pop
void MatrixCache_Init(void) {

}

void MatrixCache_Destroy(void) {
    Deallocate(g_pMatrixCacheData);
    g_pMatrixCacheData = NULL;
}

void MatrixCache_Free(u32 key) {
    MatrixCacheData *data = g_pMatrixCacheData;
    u32 i;

    for (i = 0; i < 4; i++) {
        if (data->large[i].key == key) {
            data->large[i].mtx = NULL;
            return;
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (data->medium[i].key == key) {
            data->medium[i].mtx = NULL;
            return;
        }
    }
    for (i = 0; i < 0x60; i++) {
        if (data->small[i].key == key) {
            data->small[i].mtx = NULL;
            return;
        }
    }
}

void NavHolder_Init(NavData *data) {
    u32 i;

    gs_NavHolder_803C6CD8.data = data;
    gs_NavHolder_803C6CD8.objects = NULL;
    if (data->numNodes != 0) {
        gs_NavHolder_803C6CD8.objects = (void **)Allocate(data->numNodes * 4, 0, "");
    }
    for (i = 0; i < gs_NavHolder_803C6CD8.data->numNodes; i++) {
        gs_NavHolder_803C6CD8.objects[i] = (void *)Grid_AddObject(5, i, gs_NavHolder_803C6CD8.data->nodes + i * 0x60, gs_NavHolder_803C6CD8.data->nodes + i * 0x60);
    }
}

void NavHolder_Release(void) {
    Deallocate(M2C_FIELD(&gs_NavHolder_803C6CD8, s32 *, 4));
    M2C_FIELD(&gs_NavHolder_803C6CD8, s32 *, 0) = 0;
    M2C_FIELD(&gs_NavHolder_803C6CD8, s32 *, 4) = 0;
}

s32 NavHolder_IsLinked(s32 from, s32 to) {
    u8 i;

    if (from < 0 || to < 0) {
        return 0;
    }
    if (from == to) {
        return 1;
    }
    for (i = 0; i < M2C_FIELD(gs_NavHolder_803C6CD8.data->nodes + from * 0x60, u32 *, 0x1C); i++) {
        if (to == M2C_FIELD(gs_NavHolder_803C6CD8.data->nodes + from * 0x60, s32 **, 0x20)[i]) {
            return 1;
        }
    }
    return 0;
}

#pragma peephole on
void CreateParticleGX();
void CreateParticleSprite();
void LineManager_Add();
void NavHolder_FillPointerArrayPath(u8 *point, u8 **array, u32 max) {
    u8 **out = array;
    u32 i = 0;
    while (point != NULL && i < max) {
        *out++ = point;
        i++;
        point = *(u8 **)(point + 0x38);
    }
    if (i < max) {
        array[i] = NULL;
    }
}
void NavHolder_UseMainRouteOnly(void) {
    extern s8 gs_useMainRouteOnly_803C7000;
    gs_useMainRouteOnly_803C7000 = 1;
}

extern NavHolder gs_NavHolder_803C6CD8;

s32 NavHolder_GetTotalNavPoints(void) {
    return gs_NavHolder_803C6CD8.data->numNodes;
}

u8 *NavHolder_GetNavPoint(s32 index) {
    u8 *point = NULL;
    if (gs_NavHolder_803C6CD8.data != NULL && index < (s32)gs_NavHolder_803C6CD8.data->numNodes) {
        point = gs_NavHolder_803C6CD8.data->nodes + index * 0x60;
    }
    return point;
}

void *NavHolder_GetFirstWayPoint(void) {
    return gs_NavHolder_803C6CD8.data->firstWayPoint;
}

#pragma push
#pragma use_lmw_stmw on
void ParticleManager_Render(void) {
    void ParticleGX_Render(void *particle);
    void ParticleSprite_Render(void *sprite);
    typedef struct {
        u8 unk0[0x94];
        u8 flags;
        u8 unk95[0xB0 - 0x95];
    } ParticleSprite;
    typedef struct {
        u8 unk0[0xB4];
        u8 flags;
        u8 unkB5[0x100 - 0xB5];
    } ParticleGX;
    typedef struct {
        struct {
            ParticleGX particles[0x280];
            u8 unk28000[0x10];
        } groups[3];
        ParticleSprite sprites[0x200];
        u8 unk8E030[0x10];
    } ParticleManager;
    extern ParticleManager g_particleManager;
    extern s32 g_numParticlesCreatedThisFrame;
    ParticleManager *pm = &g_particleManager;
    u32 j;
    u32 i;
    ParticleSprite *sprite;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 0x280; j++) {
            if (!(pm->groups[i].particles[j].flags & 8)) {
                ParticleGX_Render(&pm->groups[i].particles[j]);
            }
        }
    }
    for (j = 0, sprite = pm->sprites; j < 0x200; j++, sprite++) {
        if (!(pm->sprites[j].flags & 8)) {
            ParticleSprite_Render(sprite);
        }
    }
    g_numParticlesCreatedThisFrame = 0;
}
#pragma pop

void ParticleSprite_Render(void *sprite) {
    void *Gfx_AddBillBoardSprite(u16);
    void *bb;

    bb = Gfx_AddBillBoardSprite(M2C_FIELD(M2C_FIELD(M2C_FIELD(sprite, void **, 0x90), void **, 4), u32 **, 0x24)[M2C_FIELD(sprite, u8 *, 0x97)]);
    M2C_FIELD(bb, u8 *, 0x2A) = M2C_FIELD(sprite, u8 *, 0x95);
    M2C_FIELD(bb, s32 *, 0x24) = M2C_FIELD(sprite, s32 *, 0x54);
    M2C_FIELD(bb, f32 *, 0x1C) = 1.0f - M2C_FIELD(sprite, f32 *, 0x80);
    M2C_FIELD(bb, f32 *, 0xC) = M2C_FIELD(sprite, f32 *, 0x60) * M2C_FIELD(sprite, f32 *, 0x8C);
    M2C_FIELD(bb, f32 *, 0) = M2C_FIELD(sprite, f32 *, 0);
    M2C_FIELD(bb, f32 *, 4) = M2C_FIELD(sprite, f32 *, 4);
    M2C_FIELD(bb, f32 *, 8) = M2C_FIELD(sprite, f32 *, 8);
}

void Mover_GetTrackPos(void *mover, f32 *pos, f32 time) {
    void Gfx_AnimExtractKeyFrame(void *anim, f32 time, void *keyFrame, u32 count);
    f32 keyFrame[8];

    Gfx_AnimExtractKeyFrame(M2C_FIELD(mover, void **, 0xD4), time, keyFrame, 1);
    pos[0] = keyFrame[4];
    pos[1] = keyFrame[5];
    pos[2] = keyFrame[6];
}

void Mover_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void PhysicalObject_GetInverseLocalInertiaTensor(void *obj, Quaternion *out) {
    *out = *(Quaternion *)((u8 *)obj + 0x120);
}

s32 Playback_GetElement(void) {
    u32 File_Read(s32 file, u32 size);
    extern s32 g_fileIOHandle;
    extern s32 g_pauseState;
    extern s32 g_playbackCache[];
    extern u32 g_playbackFinish;
    extern u32 g_playbackPos;
    extern s32 g_playbackStatus;
    u32 bytesRead;
    s32 element;

    if (g_playbackPos == g_playbackFinish) {
        File_Close(g_fileIOHandle);
        g_fileIOHandle = -1;
        element = *(g_playbackCache + g_playbackPos);
        g_playbackStatus = 0;
        g_pauseState = 1;
    } else {
        element = *(g_playbackCache + g_playbackPos++);
        if (g_playbackPos == 0x1000) {
            bytesRead = File_Read(g_fileIOHandle, 0x4000);
            g_playbackPos = 0;
            if (bytesRead != 0x4000) {
                g_playbackFinish = bytesRead - 1;
            }
        }
    }
    return element;
}

void Playback_PutElement(s32 element) {
    void File_Write(s32 file, u32 size);
    extern s32 g_fileIOHandle;
    extern s32 g_playbackCache[0x1000];
    extern u32 g_playbackPos;
    u32 pos = g_playbackPos++;

    g_playbackCache[pos] = element;
    if (g_playbackPos == 0x1000) {
        File_Write(g_fileIOHandle, 0x4000);
        g_playbackPos = 0;
    }
}

s8 Player_IsHuman(void *player) {
    extern void Player_DefaultControlReader(void);
    s32 isHuman = 0;

    if (player != NULL && *(void **)((u8 *)player + 0x20) == Player_DefaultControlReader) {
        isHuman = 1;
    }
    return isHuman;
}

void PlayerHuman_Init(void *player) {
    void PlayerHuman_SetDefaultControls(void *player, s32 arg);

    Player_Init(player, 0);
    M2C_FIELD(player, s32 *, 0xC8) = 0;
    PlayerHuman_SetDefaultControls(player, 0);
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    M2C_FIELD(player, void **, 0x114) = (u8 *)player + 0x118;
}

#pragma push
#pragma dont_inline on
void Player_Init(void *player, s32 arg1) {
    s32 i;

    Control_ClearControl(player);
    M2C_FIELD(player, u8 *, 0x40) = 0;
    M2C_FIELD(player, u8 *, 0x31) = 0;
    M2C_FIELD(player, s32 *, 0x3C) = 0;
    M2C_FIELD(player, s32 *, 0x68) = -1;
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    memset((u8 *)player + 0x70, 0, 0x38);
    M2C_FIELD(player, u8 *, 0x71) = 1;
    M2C_FIELD(player, s32 *, 0x34) = 0;
    for (i = 0; i < 8; i++) {
        ((s16 *)((u8 *)player + 0x94))[i] = 30;
    }
    M2C_FIELD(player, s16 *, 0xA2) = 0;
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    M2C_FIELD(player, s16 *, 0xAC) = -1;
    M2C_FIELD(player, s16 *, 0xAE) = -1;
}
#pragma pop

void Player_SetName(void *player, s32 port, const char *name) {
    char *strcpy(char *dst, const char *src);
    s32 Input_GetPlayerDevice(s32 port);

    strcpy((char *)player + 0x40, name);
    M2C_FIELD(player, s32 *, 0x60) = port;
    if (Player_IsHuman(player)) {
        M2C_FIELD(player, s32 *, 0xC8) = Input_GetPlayerDevice(M2C_FIELD(player, s32 *, 0x60));
    }
}

void Player_ReleaseInterface(void *player) {
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 index = M2C_FIELD(player, u32 *, 0x68);

    if (index != 0xFFFFFFFF) {
        InterfaceManager_ReleaseInterface(index);
    }
}

s32 Player_InputHeartbeat(void *arg0) {
    return M2C_FIELD(arg0, s8 (**)(), 0x20)() != 0;
}

void Player_GetInterfaceInfo(void *player, void *info) {
    int sprintf(char *buf, const char *fmt, ...);
    void *thing;

    if (M2C_FIELD(player, void **, 0x2C) != NULL) {
        M2C_FIELD(info, u8 *, 0x80) = 0;
        thing = M2C_FIELD(player, void **, 0x2C);
        M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *, void *), 0x20)(thing, info);
    } else {
        M2C_FIELD(info, u8 *, 0x80) = 2;
        if (M2C_FIELD(player, u8 *, 0x30) != 0) {
            M2C_FIELD(info, u8 *, 0x80) = 1;
            M2C_FIELD(player, u8 *, 0x30) = 0;
        }
    }
    if (M2C_FIELD(player, u8 *, 0x71) != 0) {
        sprintf((char *)info + 0x74, "%s", M2C_FIELD(player, char **, 0x7C));
        M2C_FIELD(player, u8 *, 0x71) = 0;
    }
}

void Player_DetachThingDeath(void *arg0) {
    void *temp_r5;

    temp_r5 = M2C_FIELD(arg0, void **, 0x2C);
    M2C_FIELD(arg0, void **, 0x2C) = NULL;
    M2C_FIELD(arg0, s8 *, 0x30) = 1;
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, s32 *, 0x94) = 0;
    }
}

void PlayerHuman_Heartbeat(void *player) {
    void ForceFeedback_GetForce(void *ff, u8 *a, u8 *b, s32 *c);
    void Input_SetAlternativeForceFeedback(s32 input, u8 a, u8 b);
    void Player_Heartbeat(void *player);
    s32 c;
    u8 a;
    u8 b;
    void *car;

    car = M2C_FIELD(player, void **, 0x2C);
    if (car != NULL && !(M2C_FIELD(car, u32 *, 0x74) & 2)) {
        ForceFeedback_GetForce((u8 *)car + 0x40, &a, &b, &c);
        Input_SetAlternativeForceFeedback(M2C_FIELD(player, s32 *, 0xC8), a, b);
    }
    Player_Heartbeat(player);
}

s32 Player_NULL(void) {
    return 0;
}

#pragma push
#pragma dont_inline on
s32 Player_AIControlReader(s8 *ctrl) {
    extern u32 g_playbackStatus;
    s32 element;

    if (M2C_FIELD(ctrl, u32 *, 0x6C) & 1) {
        return 0;
    }
    if (g_playbackStatus == 2) {
        element = Playback_GetElement();
        ctrl[0] = element;
        ctrl[2] = (s8)(element & 0xFFFF0000) >> 16;
        element = Playback_GetElement();
        ctrl[1] = element;
        ctrl[3] = (s8)(element & 0xFFFF0000) >> 16;
    } else if (g_playbackStatus == 1) {
        Playback_PutElement(ctrl[0] + (ctrl[2] << 16));
        Playback_PutElement(ctrl[1] + (ctrl[3] << 16));
    }
    return 1;
}
#pragma pop

void ProcessManager_Create(void) {

}

char *strcpy(char *dst, const char *src);
void FileStreaming_HeartBeat(void);
s32 Playback_GetElement(void);
void Playback_PutElement(s32 element);
void ProcessManager_Init(void) {
    g_processManager.count = 0;
}

#pragma push
#pragma use_lmw_stmw on
void ProcessManager_Add(char *name, void (*init)(void), void (*update)(void), s32 arg, u32 flags) {
    u32 index = g_processManager.count++;

    if (name != NULL) {
        strcpy(g_processManager.processes[index].name, name);
    } else {
        strcpy(g_processManager.processes[index].name, "");
    }
    if (init != NULL) {
        init();
    }
    g_processManager.processes[index].init = init;
    g_processManager.processes[index].update = update;
    g_processManager.processes[index].arg = arg;
    g_processManager.processes[index].flags = flags;
    g_processManager.processes[index].unk10 = 0;
    g_processManager.processes[index].unk14 = 0;
}

void ProcessManager_Heartbeat(u32 activeOnly) {
    s32 StateManager_CurrState(void);
    extern u32 g_StopInput;
    u32 i;

    if (g_StopInput != 0 && StateManager_CurrState() != 5) {
        return;
    }
    FileStreaming_HeartBeat();
    for (i = 0; i < g_processManager.count; i++) {
        if (activeOnly == 0 || (g_processManager.processes[i].flags & 1)) {
            if (g_processManager.processes[i].update != NULL) {
                g_processManager.processes[i].update();
            }
        }
    }
}

void ProcessManager_Render(u32 all) {
    u32 i;

    for (i = 0; i < g_processManager.count; i++) {
        if ((void (*)(void))g_processManager.processes[i].arg != NULL) {
            if (all != 0 || (g_processManager.processes[i].flags & 1)) {
                ((void (*)(void))g_processManager.processes[i].arg)();
            }
        }
    }
}

#pragma push
#pragma dont_inline on
void Projectile_Release(void *projectile) {
    void LensFlare_Delete(s32 flare);
    extern u8 g_projectileVTable[];

    LensFlare_Delete(M2C_FIELD(projectile, s32 *, 0x130));
    Thing_Release((s32)projectile);
    LightHolder_Delete(M2C_FIELD(projectile, s32 *, 0x134));
    M2C_FIELD(projectile, void **, 0x8C) = g_projectileVTable;
}
#pragma pop

#pragma pop

void Projectile_DynamicCollideImpact(void *proj, void *dir, s32 damage, f32 force) {
    void Projectile_DynamicCollisionCorrection(void *);
    f32 scale;

    M2C_FIELD(proj, s32 *, 0x78) -= damage;
    if (dir != NULL) {
        scale = force / M2C_FIELD(M2C_FIELD(proj, void **, 0x120), f32 *, 0x2C);
        M2C_FIELD(proj, f32 *, 0) = M2C_FIELD(dir, f32 *, 0x10) * scale + M2C_FIELD(proj, f32 *, 0);
        M2C_FIELD(proj, f32 *, 4) = M2C_FIELD(dir, f32 *, 0x14) * scale + M2C_FIELD(proj, f32 *, 4);
        M2C_FIELD(proj, f32 *, 8) = M2C_FIELD(dir, f32 *, 0x18) * scale + M2C_FIELD(proj, f32 *, 8);
        Projectile_DynamicCollisionCorrection(proj);
    }
}

f32 Projectile_GetMass(void *arg0) {
    return M2C_FIELD(M2C_FIELD(arg0, void **, 0x120), f32 *, 0x2C);
}

s32 Projectile_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x120);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void ShaderHolder_Create(void) {
    M2C_UNK Deallocate(void *);
    void *Allocate(u32, u32, const char *);
    void ShaderHolder_Init(void);
    extern ShaderHolder *g_pShaderHolder;
    if (g_pShaderHolder != NULL) {
        Deallocate(g_pShaderHolder);
    }
    g_pShaderHolder = Allocate(0x2008, 0, "");
    memset(g_pShaderHolder, 0, 0x2008);
    ShaderHolder_Init();
}

void ShaderHolder_Destroy(void) {
    M2C_UNK Deallocate(void *);
    extern void *g_pShaderHolder;
    if (g_pShaderHolder) {
        Deallocate(g_pShaderHolder);
        g_pShaderHolder = NULL;
    }
}

void ShaderHolder_Init(void) {
    extern void *g_pShaderHolder;
    M2C_FIELD(g_pShaderHolder, s32 *, 0x2000) = 0;
    M2C_FIELD(g_pShaderHolder, s32 *, 0x2004) = 0;
}

void ShaderHolder_RemoveShader(void *id) {
    extern ShaderHolder *g_pShaderHolder;
    u32 i;

    if (g_pShaderHolder == NULL) {
        return;
    }
    for (i = 0; i < g_pShaderHolder->numShaders; i++) {
        if (M2C_FIELD(g_pShaderHolder->shaders[i], void **, 0x5C) == id) {
            if (--M2C_FIELD(g_pShaderHolder->shaders[i], s32 *, 0x64) == 0) {
                g_pShaderHolder->numShaders--;
                g_pShaderHolder->shaders[i] = g_pShaderHolder->shaders[g_pShaderHolder->numShaders];
            }
            return;
        }
    }
}

void ShaderHolder_AddGX(void *gx, u32 index) {
    extern ShaderHolder *g_pShaderHolder;
    u32 i;
    u32 n;

    if (gx == NULL || index >= g_pShaderHolder->numShaders) {
        return;
    }
    for (i = 0; i < g_pShaderHolder->numGX; i++) {
        if (g_pShaderHolder->gx[i].gx == gx) {
            return;
        }
    }
    n = g_pShaderHolder->numGX++;
    g_pShaderHolder->gx[n].gx = gx;
    g_pShaderHolder->gx[n].shader = g_pShaderHolder->shaders[index];
}

void ShaderHolder_RemoveGX(s32 gx) {
    extern ShaderHolder *g_pShaderHolder;
    u32 i;

    if (g_pShaderHolder == NULL) {
        return;
    }
    for (i = 0; i < g_pShaderHolder->numGX; i++) {
        if (g_pShaderHolder->gx[i].gx == (void *)gx) {
            g_pShaderHolder->numGX--;
            g_pShaderHolder->gx[i] = g_pShaderHolder->gx[g_pShaderHolder->numGX];
            return;
        }
    }
}

void ShowStats_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void ShowStats_Init(void) {
    s32 Gfx_FontManager_AddFont(u16, u8, f32, f32, f32);
    void Gfx_SetDebugFont(void);
    extern s32 debugFont;
    extern s32 g_debugDataFlag;
    extern u8 g_timeBar[0x20];
    extern s32 g_timeBarOn;
    u8 *font;

    if (World_GetGameData() != NULL) {
        font = M2C_FIELD(World_GetGameData(), u8 **, 0xC8);
        g_debugDataFlag = 1;
        memset(g_timeBar, 0, 0x20);
        g_timeBarOn = 0;
        if (font != NULL) {
            debugFont = Gfx_FontManager_AddFont(M2C_FIELD(font, u32 *, 0), font[0xB], font[9], font[0xA], M2C_FIELD(font, f32 *, 0x28));
            Gfx_SetDebugFont();
        }
    }
}

void ShowStats_Release(void) {
    M2C_UNK Gfx_FontManager_ReleaseFont(s32);
    extern s32 debugFont;
    Gfx_FontManager_ReleaseFont(debugFont);
}

void ShowStats_StartFrame(void) {
    M2C_UNK Performance_Start();
    M2C_UNK Release_Performance_Get(s32);
    extern u8 g_timeBar[0x20];
    extern s32 g_timeBarOn;
    extern s32 g_pActiveTimeBar;

    g_timeBarOn = !g_timeBarOn;
    g_pActiveTimeBar = (s32)(g_timeBar + g_timeBarOn * 0x10);
    Performance_Start();
    Release_Performance_Get(g_pActiveTimeBar);
}

void ShowStats_BeginScene(void) {
    M2C_UNK Gfx_BeginScene();
    M2C_UNK Release_Performance_Diff(s32);
    M2C_UNK Release_Performance_Get(s32);
    extern s32 g_pActiveTimeBar;
    Release_Performance_Diff(g_pActiveTimeBar);
    Release_Performance_Get(g_pActiveTimeBar + 4);
    Gfx_BeginScene();
}

void ShowStats_EndScene(void) {
    void Render_TimeBars(void *bar);
    void Release_Performance_Diff(s32);
    void Release_Performance_Get(s32);
    void Performance_End(void);
    extern u32 g_debugDataFlag;
    extern u8 g_timeBar[0x20];
    extern s32 g_timeBarOn;
    extern s32 g_pActiveTimeBar;
    extern u32 g_bDrawTimeBars;
    extern s32 g_TripleBufferSleepQTime;
    u8 *bar = g_timeBar + !g_timeBarOn * 0x10;

    if (g_debugDataFlag != 0 && g_bDrawTimeBars != 0) {
        Render_TimeBars(bar);
    }
    Release_Performance_Diff(g_pActiveTimeBar + 4);
    Release_Performance_Get(g_pActiveTimeBar + 8);
    Gfx_EndScene();
    Release_Performance_Diff(g_pActiveTimeBar + 8);
    M2C_FIELD(g_pActiveTimeBar, s32 *, 0xC) = g_TripleBufferSleepQTime;
    Gfx_FlipBuffers();
    ShowStats();
    Performance_End();
}

void ShowStats(void) {

}

static void SoundSeq_RemoveFromQueue(u8 *seq) {
    void Trigger_SoundSequenceQueueEmptied(void);
    u8 *next;

    next = M2C_FIELD(seq, u8 **, 0xC8);
    if (next != NULL) {
        M2C_FIELD(next, u32 *, 0x74) &= ~0x80000000;
        M2C_FIELD(seq, u8 **, 0xC8) = NULL;
    } else {
        Trigger_SoundSequenceQueueEmptied();
    }
}

void SoundSeq_Release(u8 *seq) {
    void Trigger_SoundSequenceQueueEmptied(void);
    void SoundSource_Stop(s32 index);
    void Thing_Release(void *thing);
    u8 *next;
    u8 *p;
    u32 i;

    p = seq;
    if (M2C_FIELD(seq, u32 *, 0x74) & 0x40000000) {
        next = M2C_FIELD(p, u8 **, 0xC8);
        if (next != NULL) {
            M2C_FIELD(next, u32 *, 0x74) &= ~0x80000000;
            M2C_FIELD(p, u8 **, 0xC8) = NULL;
        } else {
            Trigger_SoundSequenceQueueEmptied();
        }
    }
    for (i = 0; i < 4; i++) {
        if (M2C_FIELD(p, s32 *, 0xD0) >= 0 && SoundSource_IsLooping(M2C_FIELD(p, s32 *, 0xD0))) {
            SoundSource_Stop(M2C_FIELD(p, s32 *, 0xD0));
        }
        M2C_FIELD(p, s32 *, 0xD0) = -1;
        p += 4;
    }
    Thing_Release(seq);
}
s32 SoundSeq_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 SoundSequence_GetTotalElementsInQueue(void) {
    u32 ThingManager_GetActiveSoundSequences(u8 **list, u32 max);
    u8 *list[128];
    u32 count;
    s32 total;

    total = 0;
    count = ThingManager_GetActiveSoundSequences(list, 128);
    while (count != 0) {
        count--;
        if (M2C_FIELD(list[count], u32 *, 0x74) & 0x40000000) {
            total++;
        }
    }
    return total;
}

void ExitStateGamePlay(s32 arg0) {
    extern s8 g_bExitStateGamePlay;
    extern s32 g_targetGameState;
    g_targetGameState = arg0;
    g_bExitStateGamePlay = 1;
}

#pragma push
#pragma use_lmw_stmw on
#pragma dont_inline on
void SetPlayerControls(void) {
    u8 PlayerManager_PlayerHumanCount(void);
    void *PlayerManager_GetHumanPlayerByIndex(u8 index);
    void PlayerHuman_SetControlBinding(void *player, s32 binding);
    void PlayerHuman_SetDefaultControls(void *player, s32 arg);
    void Input_SetForceFeedbackActive(s32 chan, u8 active);
    u8 *mcp;
    u32 i;
    u32 count;
    void *player;
    ControllerSettings *settings;

    mcp = GameSettings_GetMCP();
    count = PlayerManager_PlayerHumanCount();
    for (i = 0; i < count; i++) {
        player = PlayerManager_GetHumanPlayerByIndex(i);
        settings = GameSettings_GetControllerSettings(M2C_FIELD(player, s32 *, 0x60));
        if (mcp != NULL && M2C_FIELD(mcp, u32 *, 0x3C) != 0) {
            if (settings->data[1] >= M2C_FIELD(mcp, u32 *, 0x3C)) {
                settings->data[1] = 0;
            }
            PlayerHuman_SetControlBinding(player, M2C_FIELD(*M2C_FIELD(mcp, void ****, 0x40)[settings->data[1]], s32 *, 0x8));
        } else {
            PlayerHuman_SetDefaultControls(player, settings->data[1]);
        }
        Input_SetForceFeedbackActive(M2C_FIELD(player, s32 *, 0xC8), settings->data[0]);
    }
}
#pragma pop

#pragma push
#pragma dont_inline on
void StateGamePlay_Pause(s32 pause) {
    extern u8 g_bFreezeWorld;
    void SoundManager_Pause(u8 pause);
    void Input_ForceFeedbackPause(u8 pause);
    void InterfaceManager_PauseAllInterface(u8 pause);
    u8 bPause;

    CameraManager_Pause(pause);
    bPause = pause;
    SoundManager_Pause(bPause);
    Input_ForceFeedbackPause(bPause);
    InterfaceManager_PauseAllInterface(bPause);
    g_bFreezeWorld = 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
s32 StateGamePlay_Shutdown(void) {
    extern s8 g_iTimeShift;
    extern u8 g_bFreezeWorld;
    extern void (*DestroyMusic)(void);
    void Trigger_PauseTACSystem(s8 arg0);
    void Gfx_BeginScene(void);
    void Gfx_EndScene(void);
    void Gfx_FlipBuffers(void);
    void NavHolder_Release(void);
    void CameraManager_ReleaseAllEmitters(void);
    void ThingManager_DARYL_Release(void);
    void MatrixCache_Init(void);
    void Grid_Release(void);
    void SongManager_StopBKSong(void);
    void MovieManager_Destroy(void);
    void ShowStats_Release(void);
    void PlayerManager_Release(void);
    void World_Unload(void);
    void ViewManager_SetSinglePlayer(void);
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    void Gfx_ClearScreen(void);

    Trigger_PauseTACSystem(1);
    while (IsLoadingThreadRunning()) {
    }
    g_iTimeShift = 0;
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    NavHolder_Release();
    CameraManager_ReleaseAllEmitters();
    ThingManager_DARYL_Release();
    MatrixCache_Init();
    Grid_Release();
    DestroyMusic();
    SongManager_StopBKSong();
    MovieManager_Destroy();
    ShowStats_Release();
    PlayerManager_Release();
    Trigger_PauseTACSystem(0);
    World_Unload();
    ViewManager_SetSinglePlayer();
    GameSettings_SetCodeVariable(0x19B, 0, 1);
    g_bFreezeWorld = 0;
    Gfx_ClearScreen();
    return 1;
}
#pragma pop

void GamePlay_Interface(void *msg) {
    void GamePlay_Startup(void);
    extern struct {
        void (*handleInput)(void *msg);
        void (*update)(void *msg);
    } g_handleInterfaceGP;

    switch (M2C_FIELD(msg, u16 *, 4)) {
    case 3:
        GamePlay_Startup();
        if (g_handleInterfaceGP.handleInput != NULL) {
            g_handleInterfaceGP.handleInput(msg);
        }
        break;
    case 1:
        if (g_handleInterfaceGP.update != NULL) {
            g_handleInterfaceGP.update(msg);
        }
        break;
    case 2:
    default:
        if (g_handleInterfaceGP.handleInput != NULL) {
            g_handleInterfaceGP.handleInput(msg);
        }
        break;
    }
}

void GamePlay_Startup(void) {
    extern void GamePlay_PauseOptions_HandleInput(void *msg);
    extern struct {
        void (*handleInput)(void *msg);
        void (*update)(void);
    } g_handleInterfaceGP;

    g_handleInterfaceGP.handleInput = GamePlay_PauseOptions_HandleInput;
    g_handleInterfaceGP.update = NULL;
}

s32 TriggerInterfacePackEnd(u32 *pack) {
    int strcmp(const char *, const char *);
    u32 i;
    u32 id;
    InterfacePackEntry *entry;

    i = 0;
    id = pack[0];
    for (entry = g_InterfacePackData; i < g_NumInterfacesPlaying; entry++, i++) {
        if (id == entry->id && strcmp(entry->name, (char *)(pack + 1)) == 0) {
            return 0;
        }
    }
    return 1;
}

static const char s_InterfaceInputName[] = "InterfaceInput";

void ArmageddonDropShipInfo(void *arg0) {
    M2C_FIELD(arg0, f32 *, 0x24) = InterfaceInputAnimator_GetLRCounter();
    M2C_FIELD(arg0, f32 *, 0x28) = InterfaceInputAnimator_GetUDCounter();
}

#pragma push
#pragma dont_inline on
void InterfaceInputAnimator_Initialize(void) {
    extern f32 g_LR_Counter;
    extern f32 g_UD_Counter;
    extern f32 g_LR_Max_Min;
    extern f32 g_UD_Max_Min;
    g_LR_Counter = 0.0f;
    g_UD_Counter = 0.0f;
    g_LR_Max_Min = 0.0f;
    g_UD_Max_Min = 0.0f;
    ProcessManager_Add((char *)s_InterfaceInputName, NULL, InterfaceInputAnimator_HeartBeat, 0, 1);
}
#pragma pop
void InterfaceInputAnimator_Release(void) {
    ProcessManager_Release((char *)s_InterfaceInputName);
}

void ProcessManager_Release(char *name);
void InterfaceInputAnimator_HeartBeat(void);
f32 InterfaceInputAnimator_GetLRCounter(void);
f32 InterfaceInputAnimator_GetUDCounter(void);
void InterfaceInputAnimator_SetCounter(s32 which, f32 value) {
    extern f32 g_LR_Counter;
    extern f32 g_UD_Counter;
    switch (which) {
    case 0:
        g_LR_Counter = value;
        break;
    case 1:
        g_UD_Counter = value;
        break;
    }
}

void InterfaceInputAnimator_Count(s32 arg0, s32 arg1, f32 farg0) {
    extern s32 g_LR_Direction;
    extern f32 g_LR_Max_Min;
    extern s32 g_UD_Direction;
    extern f32 g_UD_Max_Min;
    switch (arg0) {                                 /* irregular */
    case 0:
        g_LR_Max_Min = farg0;
        g_LR_Direction = arg1;
        return;
    case 1:
        g_UD_Max_Min = farg0;
        g_UD_Direction = arg1;
        return;
    }
}

f32 InterfaceInputAnimator_GetLRCounter(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

f32 InterfaceInputAnimator_GetUDCounter(void) {
    extern f32 g_UD_Counter;
    return g_UD_Counter;
}

void InterfaceInputAnimator_SetCallBack(void *callBack, InterfaceCallBackData *data) {
    g_pCallBack = callBack;
    if (callBack != NULL) {
        g_CallBackData = *data;
    }
}

void InterfaceInputAnimator_HeartBeat(void) {
    extern f32 g_fDeltaTimeMS;
    extern s32 g_LR_Direction;
    extern s32 g_UD_Direction;
    extern f32 g_LR_Counter;
    extern f32 g_UD_Counter;
    extern f32 g_LR_Max_Min;
    extern f32 g_UD_Max_Min;
    f32 dt = g_fDeltaTimeMS;

    if (g_LR_Direction == 0) {
        g_LR_Counter += dt;
        if (g_LR_Counter > g_LR_Max_Min) {
            g_LR_Counter = g_LR_Max_Min;
        }
    } else {
        g_LR_Counter -= dt;
        if (g_LR_Counter < g_LR_Max_Min) {
            g_LR_Counter = g_LR_Max_Min;
        }
    }
    if (g_UD_Direction == 0) {
        g_UD_Counter += dt;
        if (g_UD_Counter > g_UD_Max_Min) {
            g_UD_Counter = g_UD_Max_Min;
        }
    } else {
        g_UD_Counter -= dt;
        if (g_UD_Counter < g_UD_Max_Min) {
            g_UD_Counter = g_UD_Max_Min;
        }
    }
    if (g_pCallBack != NULL) {
        ((void (*)(InterfaceCallBackData *))g_pCallBack)(&g_CallBackData);
    }
}

void StateMovie_SetMovieToPlay(s32 arg0) {
    extern s32 g_moviePlaybackName;
    g_moviePlaybackName = arg0;
}

s32 StateMovie_Shutdown(void) {
    M2C_UNK StateGameManager_RefreshMCPTextures();
    StateGameManager_RefreshMCPTextures();
    return 1;
}

s32 StateRenderTest_Shutdown(void) {
    return 1;
}

s32 StateShutdown_Init(void) {
    return 1;
}

s32 StateShutdown_Heartbeat(void) {
    return 0;
}

s32 StateShutdown_Shutdown(void) {
    return 1;
}

void StateTitleScreen_SetMenuFile(s32 arg0, s32 arg1) {
    extern s32 g_mainMenuFile;
    extern s32 g_mainMenuFileID;
    g_mainMenuFile = arg0;
    g_mainMenuFileID = arg1;
}

#pragma push
#pragma dont_inline on
s32 StateTitleScreen_Shutdown(void) {
    M2C_UNK Gfx_EndScene();
    M2C_UNK Gfx_FlipBuffers();
    M2C_UNK InterfaceManager_ReleaseAllInterfaces();
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);
    extern u8 g_bPlayingFMV;
    extern s32 g_pTitleDataPack;
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    Gfx_BeginScene();
    Gfx_EndScene();
    Gfx_FlipBuffers();
    ShowStats_Release();
    if ((s8) g_bPlayingFMV == 0) {
        InterfaceManager_ReleaseAllInterfaces();
        FileStreaming_ReleaseAllDataPacks(1, 1);
        g_pTitleDataPack = 0;
    }
    return 1;
}

#pragma push
#pragma dont_inline on
s32 StateTitleScreen_Heartbeat(void) {
    M2C_UNK ProcessManager_Heartbeat(M2C_UNK);
    M2C_UNK ProcessManager_Render(M2C_UNK);
    M2C_UNK ShowStats_EndScene();
    M2C_UNK TextureManager_ReleaseCache();
    M2C_UNK ViewManager_PrepForRender();
    M2C_UNK Wheel_PlayAllAirborne();
    extern u8 g_clearTextureCache;
    ShowStats_StartFrame();
    ViewManager_PrepForRender();
    ProcessManager_Heartbeat(0);
    ShowStats_BeginScene();
    ProcessManager_Render(1);
    Wheel_PlayAllAirborne();
    ShowStats_EndScene();
    if ((s8) g_clearTextureCache == 0) {
        TextureManager_ReleaseCache();
        g_clearTextureCache = 1;
    }
    return 1;
}
#pragma pop

#pragma pop

void StateManager_Destroy(void) {
    M2C_UNK Deallocate(void *);
    extern void *g_pStateManager;
    if (g_pStateManager != NULL) {
        Deallocate(g_pStateManager);
        g_pStateManager = NULL;
    }
}

s32 StateManager_CurrState(void) {
    extern void *g_pStateManager;
    return M2C_FIELD(g_pStateManager, s32 *, 0xD0);
}

s32 StateManager_QueuedState(void) {
    extern void *g_pStateManager;
    s32 state;

    state = M2C_FIELD(g_pStateManager, s32 *, 0xD4);
    M2C_FIELD(g_pStateManager, s32 *, 0xD4) = -1;
    return state;
}

s32 StateManager_QueueChange(s32 state) {
    extern void *g_pStateManager;
    u32 FileManager_GetDeviceStatus(s32);
    if (M2C_FIELD(g_pStateManager, s32 *, 0xD4) == -1) {
        if (state == 8 || FileManager_GetDeviceStatus(0) == 1) {
            M2C_FIELD(g_pStateManager, s32 *, 0xD4) = state;
        }
    }
    return M2C_FIELD(g_pStateManager, s32 *, 0xD4);
}

void State_GetGameStateStr(s32 state, char *str) {
    char *strcpy(char *dst, const char *src);
    switch (state) {
    case 0:
        strcpy(str, "STATE_NONE");
        break;
    case 4:
        strcpy(str, "STATE_XXXXXXXXXXX");
        break;
    case 5:
        strcpy(str, "STATE_XXXXXXXA");
        break;
    case 8:
        strcpy(str, "STATE_XXXXXXXB");
        break;
    case 11:
        strcpy(str, "STATE_XXXXXXXXXXXXXXXXXXXX");
        break;
    case 12:
        strcpy(str, "STATE_XXXX");
        break;
    case -1:
    default:
        strcpy(str, "STATE_NONE");
        break;
    }
}

s32 Thing_Init(void *thing, s32 arg1) {
    extern Matrix4 g_identity;
    M2C_FIELD(thing, f32 *, 0x8) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x4) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x0) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x68) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x64) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x60) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x70) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x5C) = 1.0f;
    M2C_FIELD(thing, f32 *, 0x50) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x54) = 0.0f;
    M2C_FIELD(thing, f32 *, 0x58) = 0.0f;
    M2C_FIELD(thing, s32 *, 0x74) = 0;
    M2C_FIELD(thing, s16 *, 0x80) = 0;
    M2C_FIELD(thing, Matrix4 *, 0x10) = g_identity;
    M2C_FIELD(thing, s32 *, 0x94) = 0;
    M2C_FIELD(thing, u8 *, 0x82) = 0xFF;
    M2C_FIELD(thing, u8 *, 0x83) = 0xFF;
    M2C_FIELD(thing, s32 *, 0x88) = 0;
    M2C_FIELD(thing, s32 *, 0x84) = 0;
    return 1;
}

#pragma push
#pragma dont_inline on
void Thing_Release(s32 thing) {
    M2C_UNK Grid_FreeObject(void *);
    M2C_UNK ThingManagerList_EmancipateSlaves(s32, s32);
    M2C_UNK ThingManagerList_ReleaseAll(s32);
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    void *player;

    M2C_FIELD(thing, u32 *, 0x74) |= 0x200;
    M2C_FIELD(thing, u32 *, 0x74) |= 2;
    M2C_FIELD(thing, u32 *, 0x74) |= 0x200000;
    if (M2C_FIELD(thing, void **, 0x84) != NULL) {
        Grid_FreeObject(M2C_FIELD(thing, void **, 0x84));
    }
    M2C_FIELD(thing, void **, 0x84) = NULL;
    ThingManagerList_EmancipateSlaves(thing + 0x10, thing + 0x88);
    ThingManagerList_ReleaseAll(thing + 0x88);
    ThingManagerList_RemoveReleasedThings(thing + 0x88);
    player = M2C_FIELD(thing, void **, 0x94);
    if (player != NULL && M2C_FIELD(player, void **, 0x2C) == (void *)thing) {
        Player_DetachThingDeath(player);
    }
}
#pragma pop

s32 Thing_PreMove(void) {
    return 1;
}

s32 Thing_Move(void) {
    return 1;
}

void Thing_Draw(void) {

}

f32 Thing_GetMass(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

void Thing_DynamicCollideImpact(void) {

}

void Thing_DynamicRotation(void) {

}

s32 Thing_GetSource(void *arg0) {
    void *temp_r3;

    temp_r3 = M2C_FIELD(arg0, void **, 0x94);
    if (temp_r3 != NULL) {
        return M2C_FIELD(temp_r3, s32 *, 0x2C);
    }
    return 0;
}

s8 Thing_HasThing(u8 *thing, void *target) {
    s8 found;
    u8 *vtable = M2C_FIELD(thing, u8 **, 0x8C);

    if (vtable == g_carAVTable) {
        if ((found = ThingManagerList_Get(thing + 0x344, target)) != 0) {
            return found;
        }
    } else if (vtable == g_creatureVTable) {
        if ((found = ThingManagerList_Get(thing + 0xE0, target)) != 0) {
            return found;
        }
    }
    return ThingManagerList_Get(thing + 0x88, target);
}

#pragma push
#pragma use_lmw_stmw on
s32 TriggerAirboxEntered(u32 *trigger) {
    f32 dist;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;
    u8 *volume;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        if (M2C_FIELD(thing, u32 *, 4) == 1) {
            for (j = 0; j < trigger[numThings + 1]; j++) {
                volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x28);
                if (volume != NULL && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 TriggerCreatureVolumeEntered(u32 *trigger) {
    f32 dist;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;
    u8 *volume;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        if (M2C_FIELD(thing, u32 *, 4) == 1) {
            for (j = 0; j < trigger[numThings + 1]; j++) {
                volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x1C);
                if (volume != NULL && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 TriggerMovableVolumeEntered(u32 *trigger) {
    f32 dist;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;
    u8 *volume;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numThings + 1]; j++) {
            volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x10);
            if (volume != NULL && M2C_FIELD(thing, u32 *, 4) == 1 && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                return 1;
            }
        }
    }
    return 0;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 TriggerAirboxExited(u32 *trigger) {
    f32 dist;
    u8 *thing;
    u32 i;
    u32 numThings;
    u32 j;
    u8 *volume;

    numThings = trigger[0];
    for (i = 0; i < numThings; i++) {
        thing = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numThings + 1]; j++) {
            volume = M2C_FIELD(trigger[numThings + 2 + j], u8 **, 0x28);
            if (M2C_FIELD(thing, u32 *, 4) == 1 && IsPointWithinVolume((Vector4 *)(volume + 0x40), thing, &dist)) {
                return 0;
            }
        }
    }
    return 1;
}
#pragma pop

s32 ActionAirboxChangeMode(s32 *data) {
    u32 count = data[1];
    s32 mode = data[0];
    u32 i;

    for (i = 0; i < count; i++) {
        u8 *airbox = (u8 *)data[i + 2];
        switch (mode) {
        case 0:
            *(u32 *)(airbox + 4) = 0;
            break;
        case 2:
            if (*(u32 *)(airbox + 4) == 1) {
                *(u32 *)(airbox + 4) = 0;
            } else {
                *(u32 *)(airbox + 4) = 1;
            }
            break;
        case 1:
        default:
            *(u32 *)(airbox + 4) = 1;
            break;
        }
    }
    return 1;
}

s32 TriggerP2PActivated(u32 *data) {
    u32 n = data[0];
    u32 i;
    u32 j;
    u32 k = n + 1;

    for (i = 0; i < n; i++) {
        for (j = 0; j < data[k]; j++) {
            if (M2C_FIELD((data + 1)[j + k], s32 *, 0x74) & 0x800) {
                return 1;
            }
        }
    }
    return 0;
}

void Thing_GetInterfaceInfo(void *thing, void *info) {
    M2C_FIELD(info, s32 *, 0x10) = M2C_FIELD(thing, s32 *, 0x78);
    M2C_FIELD(info, u32 *, 0x14) = 0.681818f * MagnitudeVector(thing);
    M2C_FIELD(info, s32 *, 0x18) = 0;
    M2C_FIELD(info, s32 *, 0x4) = 0;
}

f32 MagnitudeVector(void *v);
void Thing_SetSlavePlayers(void *thing) {
    void *slave;

    for (slave = M2C_FIELD(thing, void **, 0x88); slave != NULL; slave = M2C_FIELD(slave, void **, 0xA4)) {
        M2C_FIELD(slave, s32 *, 0x94) = M2C_FIELD(thing, s32 *, 0x94);
    }
}

void Thing_InitAllSlavesNow(void *thing, void *slaves, u8 player) {
    void ThingManagerList_InitAllSlavesNow(void *list, void *slaves, u8 player);
    ThingManagerList_InitAllSlavesNow((u8 *)thing + 0x88, slaves, player);
}

void Thing_SlaveTransform(Thing *thing, void *mtx) {
    Matrix4 local;

    local = thing->mtx;
    MultiplyMatrix(&local, mtx, &thing->mtx);
    SubMatrixToQuaternion(&thing->mtx, &thing->quat);
}

void Thing_SlaveTransformQuaternionOnly(Thing *thing, void *mtx) {
    Vector4 rot;
    Vector4 quat;
    Vector4 pos;

    quat = thing->quat;
    pos = thing->mtx.v[3];
    XformVec(mtx, &pos, &thing->mtx.v[3]);
    SubMatrixToQuaternion(mtx, &rot);
    QuaternionMult(&rot, &quat, &thing->quat);
}

void *Thing_GetSurfaceScript(void) {
    extern u8 g_defaultSurfaceScript[];
    return g_defaultSurfaceScript;
}

void Thing_GetInverseLocalInertiaTensor(void *thing, f32 *tensor) {
    tensor[0] = 0.0f;
    tensor[1] = 0.0f;
    tensor[2] = 0.0f;
}

s32 Thing_GetNearestNavPoint(void) {
    return -1;
}

s32 Thing_AddToGrid(void *thing, s32 type) {
    Vector4 min;
    Vector4 max;

    Thing_GetDynamicMinMax(thing, &min, &max, 0.0f);
    M2C_FIELD(thing, s32 *, 0x84) = Grid_AddObject(type, thing, &min, &max);
    return M2C_FIELD(thing, s32 *, 0x84);
}

void Thing_GetStaticRotationalMinMax(void *thing, Vector4 *min, Vector4 *max) {
    *max = *(Vector4 *)((u8 *)thing + 0x40);
    *min = *max;
    min->x -= M2C_FIELD(thing, f32 *, 0x70);
    min->y -= M2C_FIELD(thing, f32 *, 0x70);
    min->z -= M2C_FIELD(thing, f32 *, 0x70);
    max->x += M2C_FIELD(thing, f32 *, 0x70);
    max->y += M2C_FIELD(thing, f32 *, 0x70);
    max->z += M2C_FIELD(thing, f32 *, 0x70);
}

void Thing_DynamicGridMove(void *thing, f32 expand) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    Vector4 min;
    Vector4 max;

    if (M2C_FIELD(thing, u32 *, 0x84) != 0) {
        Thing_GetDynamicMinMax(thing, &min, &max, expand);
        Grid_MoveObject(M2C_FIELD(thing, s32 *, 0x84), &min, &max);
    }
}

void Thing_StaticGridMove(void *thing) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    Vector4 min;
    Vector4 max;

    if (M2C_FIELD(thing, u32 *, 0x84) != 0) {
        Thing_GetDynamicMinMax(thing, &min, &max, 0.0f);
        Grid_MoveObject(M2C_FIELD(thing, s32 *, 0x84), &min, &max);
    }
}

void Thing_RotationalGridMove(void *thing) {
    void Grid_MoveObject(s32 obj, Vector4 *min, Vector4 *max);
    Vector4 min;
    Vector4 max;

    if (M2C_FIELD(thing, u32 *, 0x84) != 0) {
        Thing_GetStaticRotationalMinMax(thing, &min, &max);
        Grid_MoveObject(M2C_FIELD(thing, s32 *, 0x84), &min, &max);
    }
}

void Thing_CollideWithRay(void) {

}

void Thing_GetTargetPos(void *thing, Quaternion *pos) {
    *pos = *(Quaternion *)((u8 *)thing + 0x40);
}

s32 Thing_GetOrientedCollision(void) {
    return 0;
}

s32 Thing_HitsTerrain(void *thing, void *arg1, void *arg2) {
    s32 OBoxHitsTerrain(void *obox, void *thing, void *arg1, void *arg2);
    s32 RayHitsTerrain_Aux3(void *pos, void *thing, void *arg1, void *arg2);

    if (g_worldData.world == NULL) {
        return 0;
    }
    switch (M2C_FIELD(thing, s16 *, 0x80)) {
    case 3:
        return RayHitsTerrain_Aux3((u8 *)thing + 0x40, thing, arg1, arg2);
    case 1:
        return OBoxHitsTerrain(M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void *(**)(void *), 0x40)(thing), thing, arg1, arg2);
    }
    return 0;
}

void Thing_PassiveTargeting(void) {

}

u32 CollisionInfo_GetOther(void *arg0, u32 arg1) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x20);
    if (temp_r0 == arg1) {
        return M2C_FIELD(arg0, u32 *, 0x24);
    }
    return temp_r0;
}

void ThingManagerList_Append(void **arg0, void *arg1) {
    void *temp_r5;

    M2C_FIELD(arg1, void **, 0xA4) = (void *) *arg0;
    M2C_FIELD(arg1, s32 *, 0xA0) = 0;
    temp_r5 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, void **, 0xA0) = arg1;
    }
    *arg0 = arg1;
}

void ThingManagerList_Remove(void **arg0, void *arg1) {
    void *temp_r5;
    void *temp_r5_2;

    temp_r5 = M2C_FIELD(arg1, void **, 0xA0);
    if (temp_r5 == NULL) {
        *arg0 = M2C_FIELD(arg1, void **, 0xA4);
    } else {
        M2C_FIELD(temp_r5, void **, 0xA4) = (void *) M2C_FIELD(arg1, void **, 0xA4);
    }
    temp_r5_2 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5_2 != NULL) {
        M2C_FIELD(temp_r5_2, void **, 0xA0) = (void *) M2C_FIELD(arg1, void **, 0xA0);
        M2C_FIELD(arg1, void **, 0xA4) = NULL;
    }
    M2C_FIELD(arg1, void **, 0xA0) = NULL;
}

void *ThingManagerList_GetByType(void *list, s32 type) {
    void *thing;

    for (thing = *(void **)list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (M2C_FIELD(thing, s32 *, 0xA8) == type) {
            return thing;
        }
    }
    return NULL;
}

s8 ThingManagerList_Get(void *list, void *target) {
    void *thing;

    for (thing = *(void **)list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (target == thing) {
            return 1;
        }
    }
    return 0;
}

void ThingManagerList_ReleaseAll(void **list) {
    void *thing;
    void *next;

    thing = *list;
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *), 4)(thing);
        thing = next;
    }
}

void SharedSlaveHolder_Release(void) {
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    void *thing;
    void *next;
    SharedSlaveHolder *holder;
    u32 i;

    holder = g_aSharedSlaveHolder;
    i = 0;
    do {
        if (holder->key != NULL) {
            for (thing = holder->slaves; thing != NULL; thing = next) {
                next = M2C_FIELD(thing, void **, 0xA4);
                ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[1])(thing);
            }
            ThingManagerList_RemoveReleasedThings((s32)&holder->slaves);
            holder->key = NULL;
        }
        i++;
        holder++;
    } while (i < 64);
}

extern Vector4 g_zeroVector;
extern void *g_pSourcePlayer;
void *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id);
void ThingManagerList_InitAllSlavesNow(void *list, void *slaves, u8 player);
static inline s32 SharedSlaveHolder_Get(void *shared) {
    s32 i;

    for (i = 0; i < 64; i++) {
        if (g_aSharedSlaveHolder[i].key == shared) {
            return i;
        }
    }
    for (i = 0; i < 64; i++) {
        if (g_aSharedSlaveHolder[i].key == NULL) {
            g_aSharedSlaveHolder[i].key = shared;
            ThingManagerList_InitAllSlavesNow(&g_aSharedSlaveHolder[i].slaves, (u8 *)shared + 4, 0xFF);
            return i;
        }
    }
    return 64;
}

#pragma push
#pragma use_lmw_stmw on
void ThingManagerList_TriggerSlaves(void **list, u8 *trigger, Vector4 *a, Vector4 *b, Vector4 *c, u8 player) {
    u8 *shared;
    u8 id;
    void *thing;
    void *slave = NULL;

    switch (trigger[0x10]) {
    case 1: {
        struct { Vector4 a; Vector4 b; Vector4 c; Vector4 zero; void *ptr; void *player; f32 f; } p;
        p.ptr = M2C_FIELD(trigger, void **, 0x18);
        p.c = *c;
        p.player = g_pSourcePlayer;
        p.zero = g_zeroVector;
        if (p.ptr != NULL && *(u32 *)p.ptr != 0) {
            p.player = g_pSourcePlayer;
            p.f = 0.0f;
            p.a = *a;
            p.b = *b;
            slave = ThingManager_DARYL_CreateSlave(1, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 2: {
        struct { Vector4 a; Vector4 b; Vector4 c; void *ptr; void *player; f32 f; } p;
        if (M2C_FIELD(trigger, void **, 0x18) != NULL) {
            p.ptr = M2C_FIELD(trigger, void **, 0x18);
            p.f = 0.0f;
            p.c = *c;
            p.player = g_pSourcePlayer;
            p.a = *a;
            p.a.w = 1.0f;
            p.b = *b;
            slave = ThingManager_DARYL_CreateSlave(2, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 3: {
        struct { Vector4 a; void *ptr; u8 flag; } p;
        void *ptr = M2C_FIELD(trigger, void **, 0x18);
        if (ptr != NULL && ptr != NULL) {
            p.a = *a;
            p.ptr = ptr;
            p.flag = 1;
            slave = ThingManager_DARYL_CreateSlave(0x10, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 4: {
        Vector4 p;
        p = *(Vector4 *)trigger;
        p.w = 1.0f;
        slave = ThingManager_DARYL_CreateSlave(8, &p, M2C_FIELD(trigger, u16 *, 0x12));
        break;
    }
    case 5: {
        struct { void *ptr; Vector4 pos; s32 flag; void *player; } p;
        if (M2C_FIELD(trigger, void **, 0x18) != NULL) {
            p.ptr = M2C_FIELD(trigger, void **, 0x18);
            p.pos = *(Vector4 *)trigger;
            p.flag = 1;
            p.player = g_pSourcePlayer;
            slave = ThingManager_DARYL_CreateSlave(5, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 6: {
        struct { void *ptr; Vector4 pos; s32 flag; void *player; } p;
        if ((u32)ViewManager_GetViewFormat() > 1 && *M2C_FIELD(trigger, u8 **, 0x18) != 0) {
            break;
        }
        if (M2C_FIELD(trigger, void **, 0x18) != NULL) {
            p.ptr = M2C_FIELD(trigger, void **, 0x18);
            p.player = g_pSourcePlayer;
            p.pos = *(Vector4 *)trigger;
            p.flag = 0;
            slave = ThingManager_DARYL_CreateSlave(5, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 7: {
        struct { Vector4 a; Vector4 b; Vector4 c; void *ptr; s32 time; void *player; f32 f; } p;
        if (M2C_FIELD(trigger, void **, 0x18) != NULL) {
            p.ptr = M2C_FIELD(trigger, void **, 0x18);
            p.f = 0.0f;
            p.time = 0x14;
            p.c = *c;
            p.player = g_pSourcePlayer;
            p.a = *a;
            p.a.w = 1.0f;
            p.b = *b;
            slave = ThingManager_DARYL_CreateSlave(0xC, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 8: {
        struct { Vector4 unk; Vector4 a; Vector4 b; void *ptr; s32 flag; } p;
        void *ptr = M2C_FIELD(trigger, void **, 0x18);
        if (ptr != NULL) {
            p.a = *a;
            p.b = *b;
            p.ptr = ptr;
            p.flag = 0;
            slave = ThingManager_DARYL_CreateSlave(0xF, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 9: {
        struct { void *ptr; Vector4 a; } p;
        void *ptr = M2C_FIELD(trigger, void **, 0x18);
        if (ptr != NULL) {
            p.a = *a;
            p.ptr = ptr;
            slave = ThingManager_DARYL_CreateSlave(0x11, &p, M2C_FIELD(trigger, u16 *, 0x12));
        }
        break;
    }
    case 10:
        shared = M2C_FIELD(trigger, u8 **, 0x18);
        if (shared != NULL && *(u32 *)shared == 0) {
            *(u32 *)shared = 1;
            M2C_FIELD(M2C_FIELD(shared, void **, 8), u8 *, 0x1D) = SharedSlaveHolder_Get(shared);
        }
        break;
    case 0x63:
        id = M2C_FIELD(trigger, u16 *, 0x12);
        for (thing = *list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
            if (M2C_FIELD(thing, u8 *, 0x82) == id && M2C_FIELD(thing, u8 *, 0x83) == player) {
                (*(void (**)(void *))(M2C_FIELD(thing, u8 **, 0x8C) + 4))(thing);
            }
        }
        break;
    }

    if (slave != NULL) {
        M2C_FIELD(slave, u8 *, 0x83) = player;
        M2C_FIELD(slave, void **, 0xA4) = *list;
        M2C_FIELD(slave, void **, 0xA0) = NULL;
        if (M2C_FIELD(slave, void **, 0xA4) != NULL) {
            M2C_FIELD(M2C_FIELD(slave, void **, 0xA4), void **, 0xA0) = slave;
        }
        *list = slave;
    }
}

#pragma push
#pragma dont_inline on
void ThingManager_DARYL_Create(void) {
    void ThingManager_DARYL_CreateMain(void);

    OBoxHolder_Init();
    OConvexHullHolder_Init();
    ThingManager_DARYL_CreateMain();
    CollisionPairCache_Clear();
    CollisionTriCache_Clear();
    memset(g_aSharedSlaveHolder, 0, sizeof(g_aSharedSlaveHolder));
}

void ThingManager_DARYL_Destroy(void) {
    void *thing;
    u32 i;

    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[1])(thing);
    }
    g_thingManager_DARYL.activeList = NULL;
    for (i = 0; i < 18; i++) {
        g_thingManager_DARYL.freeList[i] = NULL;
    }
}

#pragma pop

#pragma pop
void ThingManager_DARYL_Init(void) {
    M2C_UNK ThingManager_DARYL_Release();
    M2C_UNK MatrixCache_Init();
    ThingManager_DARYL_Release();
    MatrixCache_Init();
}

#pragma push
#pragma dont_inline on
void ThingManager_DARYL_Release(void) {
    void ThingManagerList_ReleaseAndRemoveAll(void *list);
    void ThingManager_DARYL_CreateMain(void);

    SharedSlaveHolder_Release();
    ThingManagerList_ReleaseAndRemoveAll(&g_thingManager_DARYL.activeList);
    ThingManagerList_ReleaseAndRemoveAll(&g_thingManager_DARYL.activeList);
    ThingManager_DARYL_CreateMain();
}
#pragma pop

void ThingManager_MoveLookerOnly(void) {
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    extern u8 g_soundSeqVTable[];
    void *thing;

    ((void (*)(void *))(M2C_FIELD(&g_thingManager_DARYL, void ***, 0x6AD9C))[2])(g_thingManager_DARYL.looker);
    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (M2C_FIELD(thing, u8 **, 0x8C) == g_soundSeqVTable && (M2C_FIELD(thing, u32 *, 0x74) & 0x100000)) {
            ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[3])(thing);
        }
    }
    ThingManagerList_RemoveReleasedThings((s32)&g_thingManager_DARYL.activeList);
}

void ThingManager_DARYL_Heartbeat(void) {
    M2C_UNK ThingManager_MoveLookerOnly();
    M2C_UNK ThingManager_MoveThings_DARYL_SUBTICK();
    extern u8 g_bFreezeWorld;
    if ((s8) g_bFreezeWorld != 0) {
        ThingManager_MoveLookerOnly();
        return;
    }
    ThingManager_MoveThings_DARYL_SUBTICK();
}


void ThingManager_DARYL_Render(void) {
    extern void *g_pNoRayCollideThing;
    s32 Gfx_FrustrumCullSphereTestAllWindows(void *, f32);
    void Thing_DrawSlaves(void *);
    void *thing;

    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (!(M2C_FIELD(thing, u32 *, 0x74) & 0x200200) &&
            Gfx_FrustrumCullSphereTestAllWindows((u8 *)thing + 0x40, M2C_FIELD(thing, f32 *, 0x70))) {
            ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[4])(thing);
            g_pNoRayCollideThing = thing;
            Thing_DrawSlaves(thing);
        }
    }
    g_pNoRayCollideThing = NULL;
}

void *ThingManager_DARYL_CreateThing(s32 type, void *params) {
    void *thing = g_thingManager_DARYL.freeList[type];

    if (thing == NULL) {
        thing = NULL;
    } else {
        g_thingManager_DARYL.freeList[type] = M2C_FIELD(thing, void **, 0xA4);
        if (g_thingManager_DARYL.freeList[type] != NULL) {
            M2C_FIELD(g_thingManager_DARYL.freeList[type], void **, 0xA0) = NULL;
        }
        M2C_FIELD(thing, void **, 0xA4) = NULL;
    }
    if (thing != NULL) {
        M2C_FIELD(thing, void **, 0xA4) = g_thingManager_DARYL.activeList;
        M2C_FIELD(thing, void **, 0xA0) = NULL;
        if (M2C_FIELD(thing, void **, 0xA4) != NULL) {
            M2C_FIELD(M2C_FIELD(thing, void **, 0xA4), void **, 0xA0) = thing;
        }
        g_thingManager_DARYL.activeList = thing;
        ((void (*)(void *, void *))(M2C_FIELD(thing, void ***, 0x8C))[0])(thing, params);
        return thing;
    }
    return NULL;
}

void *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id) {
    void *slave = g_thingManager_DARYL.freeList[type];

    if (slave == NULL) {
        slave = NULL;
    } else {
        g_thingManager_DARYL.freeList[type] = M2C_FIELD(slave, void **, 0xA4);
        if (g_thingManager_DARYL.freeList[type] != NULL) {
            M2C_FIELD(g_thingManager_DARYL.freeList[type], void **, 0xA0) = NULL;
        }
        M2C_FIELD(slave, void **, 0xA4) = NULL;
    }
    if (slave != NULL) {
        ((void (*)(void *, void *))(M2C_FIELD(slave, void ***, 0x8C))[0])(slave, params);
        M2C_FIELD(slave, u8 *, 0x82) = id;
        M2C_FIELD(slave, u32 *, 0x74) |= 0x20000000;
        return slave;
    }
    return NULL;
}

s8 RayHitsTerrainAndObjects(void *arg0, void *arg1, void *arg2, void *arg3) {
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    s8 RayHitsObjects_Aux(void *, void *, void *, void *);
    s8 hitTerrain;

    if (g_worldData.world == NULL || *(void **)g_worldData.world == NULL) {
        return 0;
    }
    hitTerrain = RayHitsTerrain_Aux3(arg0, arg1, arg2, arg3);
    if (RayHitsObjects_Aux(arg0, arg1, arg2, arg3)) {
        return 1;
    }
    return hitTerrain;
}

s8 OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(void *obox, void *thing, void *arg2) {
    s8 OBoxHitsTerrainZeroTimeEdgesOnly(void *, void *);
    s32 OBox_CollideAllThingsZeroTime(void *, void *, void *);
    if (!(M2C_FIELD(thing, u32 *, 0x74) & 0x40000) && OBoxHitsTerrainZeroTimeEdgesOnly(obox, arg2)) {
        return 1;
    }
    return OBox_CollideAllThingsZeroTime(obox, thing, arg2);
}

s32 ThingManager_GetActiveCars(void **cars) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 8; i++) {
        u8 *car = g_thingManager_DARYL.unk0 + 0x60310 + i * 0x5A0;
        if (!(M2C_FIELD(car, u32 *, 0x74) & 0x201000)) {
            cars[count++] = car;
        }
    }
    return count;
}

s32 ThingManager_GetActiveSoundSequences(void **seqs) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 128; i++) {
        u8 *seq = g_thingManager_DARYL.unk0 + 0x70770 + i * 0xE0;
        if (!(M2C_FIELD(seq, u32 *, 0x74) & 0x201000)) {
            seqs[count++] = seq;
        }
    }
    return count;
}

s32 GetAllTargets(void **targets) {
    return ThingManager_GetActiveCars(targets);
}

s8 TriggerVehicleDamage(u32 *params) {
    u32 i;
    u32 count;
    u8 **things;
    s32 *saved;
    u32 type;
    s32 amount;
    s32 result;

    count = params[0];
    things = (u8 **)&params[1];
    result = 0;
    saved = (s32 *)&params[count + 3];
    type = params[count + 1];
    amount = params[count + 2];
    for (i = 0; i < count; i++) {
        u8 *vehicle = M2C_FIELD(*things, u8 **, 0x28);
        if (type == 0x2A) {
            s32 damage = *(s32 *)(vehicle + 0x78);
            if (damage < *saved - amount) {
                result = 1;
            }
            *saved = damage;
        }
        if (type == 0x2B && *(s32 *)(vehicle + 0x78) < *(s32 *)(*(u8 **)(*(u8 **)(vehicle + 0x3BC) + 0xA0) + 0x14) / amount) {
            result = 1;
        }
        things++;
        saved++;
    }
    return result;
}

s32 TriggerPlayerVehicleGear(u32 *params) {
    extern u8 g_cameraManager[];
    u8 *vehicle = M2C_FIELD(*(u8 **)g_cameraManager, u8 **, 0x2C);

    if (vehicle != NULL && vehicle[0x4E8] == params[1]) {
        return 1;
    }
    return 0;
}

s32 TriggerPlayerVehicleInAir(u32 *params) {
    extern u8 g_cameraManager[];
    u8 *vehicle;
    u32 inAir;

    if (*(u8 **)g_cameraManager != NULL) {
        vehicle = M2C_FIELD(*(u8 **)g_cameraManager, u8 **, 0x2C);
    } else {
        vehicle = NULL;
    }
    inAir = params[2];
    if (inAir == 0 && vehicle != NULL && *(s32 *)(vehicle + 0x460) >= (s32)params[1]) {
        params[2] = 1;
        return 1;
    }
    if (inAir != 0 && vehicle != NULL && *(s32 *)(vehicle + 0x460) == 0) {
        params[2] = 0;
        return 1;
    }
    return 0;
}

void Trail_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Trail_Init(void) {
    extern u8 g_TrialData[1000][0x60];
    extern s32 g_nextTrail;
    extern f32 g_trailTimer;
    s32 i;

    for (i = 0; i < 1000; i++) {
        *(s32 *)(g_TrialData[i] + 0x50) = 0;
    }
    g_nextTrail = 0;
    g_trailTimer = 0.0f;
}

#pragma push
#pragma use_lmw_stmw on
void Trail_Render(void) {
    extern u8 g_TrialData[1000][0x60];
    void *Gfx_Begin_GroupQuadFlat(u16 texture, s32 arg1, s32 arg2);
    void Gfx_End_GroupTri(u32 count);
    void MemCpy32(void *dst, void *src, u32 size);
    u8 *trail;
    u32 i;
    u16 texture;
    u32 count;
    u8 *dst;

    count = 0;
    texture = 0xFFFF;
    dst = NULL;
    for (i = 0; i < 1000; i++) {
        trail = g_TrialData[i];
        if (*(s32 *)(trail + 0x50) > 0 && *(u16 *)(trail + 0x5E) != 0) {
            if (texture != *(u16 *)(trail + 0x5C)) {
                if (count != 0) {
                    Gfx_End_GroupTri(count);
                    count = 0;
                }
                dst = Gfx_Begin_GroupQuadFlat(*(u16 *)(trail + 0x5C), 7, 0);
                texture = *(u16 *)(trail + 0x5C);
            }
            MemCpy32(dst, trail, 0x50);
            dst += 0x50;
            count++;
        }
    }
    if (count != 0) {
        Gfx_End_GroupTri(count);
    }
}
#pragma pop

s8 TriggerSoundSeqQueueEnd(void) {
    s32 SoundSequence_GetTotalElementsInQueue();
    return (s8) (SoundSequence_GetTotalElementsInQueue() == 0);
}

s32 TriggerMoveableEnd(void) {
    return 1;
}

s32 TriggerCreaturePickedUp(void) {
    return 1;
}

s32 TriggerTrue(void) {
    return 1;
}

s32 TriggerGameRoundsComplete(void) {
    return 1;
}

s32 TriggerGameRoundXComplete(void) {
    return 1;
}

s32 TriggerItemDropped(void) {
    return 1;
}

void Trigger_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Trigger_Destroy(void) {
    extern s32 m_triggerData_803C7120;
    Deallocate(m_triggerData_803C7120);
    m_triggerData_803C7120 = 0;
}

void Trigger_PauseTACSystem(s8 arg0) {
    extern s8 g_bTACSystemPaused_803C7124;
    g_bTACSystemPaused_803C7124 = arg0;
}

u8 Trigger_IsTACSystemPaused(void) {
    extern u8 g_bTACSystemPaused_803C7124;
    return g_bTACSystemPaused_803C7124;
}

#pragma push
#pragma readonly_strings on
s32 Trigger_AddTrigger(s32 *trigger) {
    extern u32 TriggerStats_800D3C58[][3];
    extern char *triggerLabelsArray_800D3BE8[];
    extern s32 **m_Triggers_802C1890[];
    extern s8 g_bTACSystemPaused_803C7124;
    int sprintf(char *buf, const char *fmt, ...);
    char buf[0x200];
    s32 type = trigger[0];

    if (TriggerStats_800D3C58[type][0] == TriggerStats_800D3C58[type][1]) {
        sprintf(buf, "Could not add the trigger: \"%s\", please notify Peter.", triggerLabelsArray_800D3BE8[type]);
        return 0;
    }
    m_Triggers_802C1890[type][TriggerStats_800D3C58[type][0]++] = trigger;
    if (trigger[0] == 8) {
        trigger[9] = 1;
    } else {
        trigger[9] = 0x10;
    }
    g_bTACSystemPaused_803C7124 = 0;
    return 1;
}

s32 Trigger_ReleaseTrigger(s32 *trigger) {
    extern u32 TriggerStats_800D3C58[][3];
    extern s32 **m_Triggers_802C1890[];
    u32 i;

    for (i = 0; i < TriggerStats_800D3C58[trigger[0]][0]; i++) {
        if (m_Triggers_802C1890[trigger[0]][i] == trigger) {
            TriggerStats_800D3C58[trigger[0]][0]--;
            m_Triggers_802C1890[trigger[0]][i] = m_Triggers_802C1890[trigger[0]][TriggerStats_800D3C58[trigger[0]][0]];
            return 1;
        }
    }
    return 0;
}

#pragma pop

s32 TriggerLevelStart(void) {
    return 1;
}

s32 TriggerInterfaceEnd(void) {
    return 1;
}

s32 TriggerVehicleKilled(u32 *data) {
    u32 i;

    for (i = 0; i < data[0]; i++) {
        u8 *vehicle = M2C_FIELD((data + 1)[i], u8 **, 0x28);
        if (vehicle != NULL && (M2C_FIELD(vehicle, s32 *, 0x74) & 2)) {
            return 1;
        }
    }
    return 0;
}

s32 TriggerDesructibleKilled(u32 *data) {
    u32 i;

    for (i = 0; i < data[0]; i++) {
        if (M2C_FIELD((data + 1)[i], u32 *, 0x30) == 3) {
            return 1;
        }
    }
    return 0;
}

s32 TriggerCreatureKilled(void) {
    return 0;
}

s32 TriggerCreatureDroppedOff(void) {
    return 0;
}

s32 ActionVertexColorSeqChangeMode(s32 *data) {
    u32 count = data[1];
    u32 i;

    for (i = 0; i < count; i++) {
        u8 *seq = (u8 *)data[i + 2];
        switch (data[0]) {
        case 0:
            *(u32 *)(seq + 8) = 0;
            break;
        case 1:
            *(u32 *)(seq + 8) = 1;
            break;
        case 2:
            if (*(u32 *)(seq + 8) == 0) {
                *(u32 *)(seq + 8) = 1;
            } else {
                *(u32 *)(seq + 8) = 0;
            }
            break;
        }
    }
    return 1;
}

void VertexColorSeqManager_Create(void) {

}

void VertexColorSeqManager_Destroy(void) {
    extern s32 g_iTotalVertexColorSeq;
    extern s32 g_iTotalVertexColorTiles;
    g_iTotalVertexColorSeq = 0;
    g_iTotalVertexColorTiles = 0;
}

void VertexColorSeqManager_Heartbeat(void) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x30];
    } VertexColorTile;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorTiles;
    extern f32 g_fDeltaTimeMS;
    f32 dt;
    u32 i;

    dt = g_fDeltaTimeMS;
    for (i = 0; i < g_iTotalVertexColorTiles; i++) {
        ExtractColorKey(M2C_FIELD(g_VertexColorTiles[i].light, void **, 0x4), g_VertexColorTiles[i].key, dt);
    }
}

void VertexColorSeqManager_AddVertices(void *vertices) {
    extern s32 g_iTotalVertexColorSeq;
    extern void *g_pVertexColorSeqList[64];
    if (vertices != NULL) {
        g_pVertexColorSeqList[g_iTotalVertexColorSeq++] = vertices;
    }
}

void VertexColorSeqManager_RemoveVertices(void *vertices) {
    extern s32 g_iTotalVertexColorSeq;
    extern void *g_pVertexColorSeqList[64];
    u32 i;

    for (i = 0; i < g_iTotalVertexColorSeq; i++) {
        if (g_pVertexColorSeqList[i] == vertices) {
            g_iTotalVertexColorSeq--;
            g_pVertexColorSeqList[i] = g_pVertexColorSeqList[g_iTotalVertexColorSeq];
            return;
        }
    }
}

s32 VertexColorSeqManager_GetLightIndex(void *light) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x30];
    } VertexColorTile;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorTiles;
    u32 i;

    for (i = 0; i < g_iTotalVertexColorTiles; i++) {
        if (light == g_VertexColorTiles[i].light) {
            return i;
        }
    }
    return -1;
}

void VertexColorSeqManager_GetLightColor(u32 index, u32 *color) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x24];
        u32 color;
        u8 unk2C[0x8];
    } VertexColorTile;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorTiles;

    if (index < g_iTotalVertexColorTiles) {
        *color = g_VertexColorTiles[index].color;
    }
}

void VolumeManager_Create(void) {
    extern s32 g_VolumeHolder;
    g_VolumeHolder = 0;
}

void VolumeManager_Release(void) {
    extern s32 g_VolumeHolder;
    void SetReverb(s32 a, s32 b, s32 c, s32 d);
    g_VolumeHolder = 0;
    SetReverb(0, 0, 0, 0);
    SetReverb(1, 0, 0, 0);
}

void VolumeManager_Initialize(void) {
    extern s32 g_VolumeHolder;
    extern u8 g_bReset;
    void SetReverb(s32 a, s32 b, s32 c, s32 d);
    g_VolumeHolder = 0;
    SetReverb(0, 0, 0, 0);
    SetReverb(1, 0, 0, 0);
    g_VolumeHolder = M2C_FIELD(g_worldData.world, s32 *, 0x4);
    g_bReset = 1;
}

s32 VolumeManager_GetNumVolumesOfType(u32 type) {
    void *world = g_worldData.world;
    s32 count = 0;
    u32 i;

    for (i = 0; i < M2C_FIELD(world, u32 *, 0x68); i++) {
        if (type == M2C_FIELD(M2C_FIELD(world, u8 **, 0x6C) + i * 0x30, u32 *, 0xC)) {
            count++;
        }
    }
    return count;
}

void *VolumeManager_GetVolumeTypeByIndex(u32 type, u32 index) {
    void *world = g_worldData.world;
    u32 i;

    for (i = 0; i < M2C_FIELD(world, u32 *, 0x68); i++) {
        if (type == M2C_FIELD(M2C_FIELD(world, u8 **, 0x6C) + i * 0x30, u32 *, 0xC)) {
            if (index == 0) {
                return M2C_FIELD(world, u8 **, 0x6C) + i * 0x30;
            }
            index--;
        }
    }
    return NULL;
}

s32 VolumeManager_GetNumVolumesOfTypeByCell(s32 type, s32 cell) {
    extern s32 g_VolumeHolder;
    if ((void *)g_VolumeHolder != NULL) {
        return M2C_FIELD(M2C_FIELD(g_VolumeHolder, u8 **, 0x10) + cell * 0x38 + type * 8, s32 *, 0x0);
    }
    return 0;
}

s32 VolumeManager_GetVolumeTypeByIndexByCell(s32 type, s32 index, s32 cell) {
    typedef struct VolumeList {
        s32 count;
        s32 *volumes;
    } VolumeList;
    typedef struct VolumeCell {
        VolumeList types[7];
    } VolumeCell;
    extern s32 g_VolumeHolder;
    VolumeCell *cells = M2C_FIELD(g_VolumeHolder, VolumeCell **, 0x10);
    return cells[cell].types[type].volumes[index];
}

s8 IsPointWithinVolume(Vector4 *point, u8 *volume, f32 *distSq) {
    s8 BSPN0_CollideWithVector(void *bsp, Vector4 *point);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    Vector4 diff;

    switch (M2C_FIELD(volume, s32 *, 0x8)) {
    case 0:
        return BSPN0_CollideWithVector(M2C_FIELD(volume, void **, 0x28), point);
    case 1:
        PSVECSubtract(point, (Vector4 *)(volume + 0x10), &diff);
        *distSq = PSVECSquareMag(&diff);
        return *distSq < M2C_FIELD(volume, f32 *, 0x24);
    }
    return 0;
}

void WildCardRack_Release(s32 arg0) {
    s32 var_r31;
    u32 var_r30;

    var_r30 = 0U;
    var_r31 = arg0;
    do {
        MotionState_Release(var_r31 + 0xB0);
        var_r30 += 1;
        var_r31 += 0x50;
    } while (var_r30 < 0xCU);
    Thing_Release(arg0);
}

void WildCardRack_JettisonAll(void *rack) {
    void WildCardRack_Jettison(void *rack, u8 slot);
    u32 i;

    if (rack != NULL) {
        for (i = 0; i < 12; i++) {
            WildCardRack_Jettison(rack, i);
        }
    }
}

s32 WildCardRack_Invalidate(u8 *rack, u32 start, u32 end) {
    u32 i;
    u32 addr;

    for (i = 0; i < 12; i++) {
        addr = *(u32 *)(rack + i * 0x50 + 0xFC);
        if (addr >= start && addr < end) {
            return 1;
        }
    }
    return 0;
}

s32 World_Create(void) {
    extern s32 m_VertIn;
    extern s32 g_drawShadowsFlag;
    m_VertIn = 0;
    g_drawShadowsFlag = 1;
    g_worldData.world = NULL;
    return 1;
}

void World_Destroy(void) {
    if (g_worldData.world != NULL) {
        World_Unload();
    }
}

void World_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

#pragma push
#pragma dont_inline on
void World_Refresh(void) {
    void Gfx_SetEnvmap(u16 a, u16 b, s32 c, f32 d, f32 e);
    void Gfx_SetFog(GXColor *color, s32 type, f32 start, f32 end);
    void Gfx_SetFarClippingPlane(f32 farPlane);
    u8 *gameData = World_GetGameData();
    GXColor color;

    if (gameData != NULL) {
        Gfx_SetEnvmap(*(u32 *)(gameData + 0x8C), *(u32 *)(gameData + 0x90), 1, *(f32 *)(gameData + 0xC), *(f32 *)(gameData + 0x10));
        color = **(GXColor **)(gameData + 0xC0);
        Gfx_SetFog(&color, 0, *(f32 *)(gameData + 0x14), *(f32 *)(gameData + 0x18));
        SongManager_PlaySong();
        Gfx_SetFarClippingPlane(*(f32 *)(gameData + 0x20));
    } else {
        Gfx_SetFarClippingPlane(0.0f);
    }
}
#pragma pop

void World_Unload(void) {
    void FileStreaming_ReleaseQuadPacks(void);
    void VolumeManager_Release(void);
    void Gfx_SetFog(GXColor *color, s32 type, f32 start, f32 end);
    u32 color[2];

    FileStreaming_ReleaseQuadPacks();
    FileStreaming_ReleaseAllDataPacks(4, 1);
    FileStreaming_ReleaseAllDataPacks(5, 1);
    FileStreaming_ReleaseAllDataPacks(6, 1);
    FileStreaming_ReleaseAllDataPacks(0, 1);
    FileStreaming_ReleaseAllDataPacks(2, 1);
    FileStreaming_ReleaseAllDataPacks(8, 1);
    FileStreaming_ReleaseAllDataPacks(1, 1);
    FileStreaming_ReleaseAllDataPacks(9, 1);
    g_worldData.world = NULL;
    g_worldData.unk8[0] = 0;
    g_worldData.unk8[1] = 0;
    g_worldData.unk8[2] = 0;
    VolumeManager_Release();
    color[1] = 0;
    color[0] = 0;
    Gfx_SetFog((GXColor *)color, 0, 1.0f, 0.0f);
}

u32 World_GetTotalTiles_Car(void) {
    return M2C_FIELD(g_worldData.world, u32 *, 0x8);
}

u32 World_GetTotalTiles_Creature(void) {
    return M2C_FIELD(g_worldData.world, u32 *, 0x40);
}

void *World_GetTileCar(u32 index) {
    void *world = g_worldData.world;
    if (index < M2C_FIELD(world, u32 *, 0x8)) {
        return M2C_FIELD(world, u8 **, 0xC) + index * 0x60;
    }
    return NULL;
}

void *World_GetTileCreature(u32 index) {
    void *world = g_worldData.world;
    if (index < M2C_FIELD(world, u32 *, 0x40)) {
        return M2C_FIELD(world, u8 **, 0x44) + index * 0x40;
    }
    return NULL;
}

void *World_GetTilePath(u32 index) {
    if (index < M2C_FIELD(g_worldData.world, u32 *, 0x10)) {
        return M2C_FIELD(g_worldData.world, u8 **, 0x14) + index * 0x20;
    }
    return NULL;
}

void *World_GetTileSpeaker(u32 index) {
    if (index < M2C_FIELD(g_worldData.world, u32 *, 0x28)) {
        return M2C_FIELD(g_worldData.world, u8 **, 0x2C) + index * 0x40;
    }
    return NULL;
}

void *World_GetTileLight(u32 index) {
    if (index < M2C_FIELD(g_worldData.world, u32 *, 0x38)) {
        return M2C_FIELD(g_worldData.world, u8 **, 0x3C) + index * 0x20;
    }
    return NULL;
}

void *World_GetVolume(u32 index) {
    void *world = g_worldData.world;
    if (index < M2C_FIELD(world, u32 *, 0x68)) {
        return M2C_FIELD(world, u8 **, 0x6C) + index * 0x30;
    }
    return NULL;
}

void *World_GetNavHolder(void) {
    return M2C_FIELD(g_worldData.world, void **, 0x78);
}

void *World_GetGameData(void) {
    void *world = g_worldData.world;
    if (world != NULL) {
        return M2C_FIELD(world, void **, 0x9C);
    }
    return NULL;
}

void World_ClearSweep(void) {
    u32 i;
    void *data;

    data = M2C_FIELD(g_worldData.world, void **, 0x0);
    if (M2C_FIELD(data, u32 *, 0x58) == 0) {
        for (i = 0; i < M2C_FIELD(M2C_FIELD(g_worldData.world, void **, 0x0), u32 *, 0x48); i++) {
            M2C_FIELD(M2C_FIELD(M2C_FIELD(g_worldData.world, void **, 0x0), u8 **, 0x4C) + i * 0x20, s32 *, 0x18) = 0;
        }
    } else if (M2C_FIELD(data, u32 *, 0x58) == 1) {
        for (i = 0; i < M2C_FIELD(M2C_FIELD(g_worldData.world, void **, 0x0), u32 *, 0x50); i++) {
            M2C_FIELD(M2C_FIELD(M2C_FIELD(g_worldData.world, void **, 0x0), u8 **, 0x54) + i * 0x30, s32 *, 0x20) = 0;
        }
    }
    g_worldData.unk4 = 1;
}

void World_NextSweep(void) {
    if (++g_worldData.unk4 == 0) {
        World_ClearSweep();
    }
}

s32 ActionVariableChangeMode(u32 *action) {
    u32 i;
    u8 *var;

    for (i = 0; i < action[1]; i++) {
        var = M2C_FIELD(*(void **)action[i + 2], u8 **, 0xC);
        if (var != NULL) {
            *var = action[0];
        }
    }
    return 1;
}

s32 ActionTimerChangeMode(u32 *action) {
    u8 **item;
    u32 i;

    for (i = 0; i < action[2]; i++) {
        item = (u8 **)action[i + 3];
        M2C_FIELD(M2C_FIELD(*item, u8 **, 0xC), u8 *, 0) = action[0];
        M2C_FIELD(M2C_FIELD(*item, u8 **, 0xC), u8 *, 1) = action[1];
    }
    return 1;
}

void PlayerManager_Create(void) {

}

void PlayerManager_Destroy(void) {

}

void PlayerManager_Release(void) {
    extern u8 g_playerManager[0x20E0];
    void Player_ReleaseInterface(void *player);
    u8 *player;

    while ((player = PlayerManager_PopActive(g_playerManager)) != NULL) {
        Player_ReleaseInterface(player);
        PlayerManager_PushFree(g_playerManager, player);
    }
}

#pragma push
#pragma dont_inline on
u8 PlayerManager_PlayerHumanCount(void) {
    extern u8 g_playerManager[0x20E0];
    u8 count = 0;
    void *player;

    for (player = M2C_FIELD(g_playerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        if (Player_IsHuman(player)) {
            count++;
        }
    }
    return count;
}
#pragma pop

u8 PlayerManager_PlayerCount(void) {
    extern u8 g_playerManager[0x20E0];
    u8 count = 0;
    void *player;

    for (player = M2C_FIELD(g_playerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        count++;
    }
    return count;
}

void *PlayerManager_GetPlayerByIndex(u8 index) {
    extern u8 *g_pPlayerManager;
    u8 i = 0;
    void *player;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        if (i == index) {
            return player;
        }
        i++;
    }
    return NULL;
}

#pragma push
#pragma dont_inline on
void *PlayerManager_GetHumanPlayerByIndex(u8 index) {
    extern u8 *g_pPlayerManager;
    void *player;
    u8 i = 0;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        if (Player_IsHuman(player)) {
            if (i == index) {
                return player;
            }
            i++;
        }
    }
    return NULL;
}

void PlayerManager_Heartbeat(void) {
    extern u8 *g_pPlayerManager;
    extern u32 g_attackDelay;
    extern u32 g_iDeltaTimeMS;
    extern s16 g_AI_FramesSinceAsyncEventProcessed;
    extern u8 g_AI_AsyncProcess[16];
    void *player;
    void *next;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        Player_InputHeartbeat(player);
    }
    if (g_attackDelay > g_iDeltaTimeMS) {
        g_attackDelay -= g_iDeltaTimeMS;
    } else {
        g_attackDelay = 0;
    }
    g_AI_FramesSinceAsyncEventProcessed++;
    if (g_worldData.world != NULL) {
        if ((s8)g_AI_AsyncProcess[0xC] == 0) {
            AI_AddEvent(g_AI_AsyncProcess);
        }
        while ((s8)AI_ProcessEvent()) {
        }
    }
    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = next) {
        next = M2C_FIELD(player, void **, 0xB0);
        M2C_FIELD(player, void (**)(void *), 0x28)(player);
    }
}

#pragma pop

void PlayerManager_Render_DEBUG(void) {

}

void PlayerManager_ClearAllCameraFlags_PRIVATE(void) {
    extern u8 *g_pPlayerManager;
    void *player;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        M2C_FIELD(player, u32 *, 0x6C) &= ~0x10;
    }
}

s32 PlayerManager_RemovePlayer(void *player) {
    extern u8 g_playerManager[0x20E0];
    extern u8 *g_pPlayerManager;
    u8 *pm;
    u8 *p;

    for (p = M2C_FIELD(g_playerManager, u8 **, 0x20D8); p != NULL; p = M2C_FIELD(p, u8 **, 0xB0)) {
        if (p == player) {
            break;
        }
    }
    if (p == NULL) {
        return 0;
    }
    PlayerManager_UnlinkActive(g_pPlayerManager, p);
    pm = g_pPlayerManager;
    M2C_FIELD(p, u8 **, 0xB0) = M2C_FIELD(pm, u8 **, 0x20DC);
    M2C_FIELD(p, u8 **, 0xB4) = NULL;
    if (M2C_FIELD(p, u8 **, 0xB0) != NULL) {
        M2C_FIELD(M2C_FIELD(p, u8 **, 0xB0), u8 **, 0xB4) = p;
    }
    M2C_FIELD(pm, u8 **, 0x20DC) = p;
    return 1;
}

void AI_Creature_Heartbeat(void) {

}

void ViewManager_Init(void) {
    extern void *m_Views_802CD5E0[5];
    extern u8 g_cameraManager[];
    extern s32 g_viewFormat;
    m_Views_802CD5E0[0] = g_cameraManager;
    m_Views_802CD5E0[1] = g_cameraManager;
    m_Views_802CD5E0[2] = g_cameraManager;
    m_Views_802CD5E0[3] = g_cameraManager;
    m_Views_802CD5E0[4] = g_cameraManager;
    g_viewFormat = 0;
}

void ViewManager_SetCamera(void *camera, s32 view) {
    extern void *m_Views_802CD5E0[5];
    m_Views_802CD5E0[view] = camera;
}

void ViewManager_PrepForRender(void) {
    extern void *m_Views_802CD5E0[5];
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Camera_PrepareForRender(void *camera);
    u32 i;

    for (i = 0; i < 5; i++) {
        if (g_CurrentViews & (1 << i)) {
            Gfx_SetRenderWindow(i);
            Camera_PrepareForRender(m_Views_802CD5E0[i]);
        }
    }
}

void ViewManager_EnableView(s32 arg0, s8 arg1) {
    extern u8 g_CurrentViews;
    s32 temp_r3;
    u8 var_r0;

    temp_r3 = 1 << arg0;
    var_r0 = g_CurrentViews & ~temp_r3;
    if (arg1 != 0) {
        var_r0 = g_CurrentViews | temp_r3;
    }
    g_CurrentViews = var_r0;
}

s32 ViewManager_GetViewFormat(void) {
    extern s32 g_viewFormat;
    return g_viewFormat;
}

void ViewManager_SetSinglePlayer(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    Gfx_SetRenderWindow(0);
    g_CurrentViews |= 1 << 0;
    g_viewFormat = 1;
    g_CurrentViews &= ~(1 << 1);
    g_CurrentViews &= ~(1 << 2);
    g_CurrentViews &= ~(1 << 3);
    g_CurrentViews &= ~(1 << 4);
}

void ViewManager_SetTwoPlayerHorizontal(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4);
    g_CurrentViews &= ~(1 << 0);
    Gfx_SetRenderWindow(1);
    Gfx_SetRenderWindowSize(0, 0, 0x27F, 0xEF, 0);
    Gfx_SetRenderWindow(2);
    Gfx_SetRenderWindowSize(0, 0xF0, 0x27F, 0x1DF, 0);
    g_CurrentViews &= ~(1 << 3);
    g_viewFormat = 2;
    g_CurrentViews &= ~(1 << 4);
}

void ViewManager_SetThreePlayer(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4);
    g_CurrentViews &= ~(1 << 0);
    Gfx_SetRenderWindow(1);
    Gfx_SetRenderWindowSize(0, 0, 0x13F, 0xEF, 0);
    Gfx_SetRenderWindow(2);
    Gfx_SetRenderWindowSize(0x140, 0, 0x27F, 0xEF, 0);
    Gfx_SetRenderWindow(3);
    Gfx_SetRenderWindowSize(0, 0xF0, 0x13F, 0x1DF, 0);
    g_CurrentViews &= ~(1 << 4);
    g_viewFormat = 4;
}

void ViewManager_SetFourPlayer(void) {
    extern s32 g_viewFormat;
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4);
    g_CurrentViews &= ~(1 << 0);
    Gfx_SetRenderWindow(1);
    Gfx_SetRenderWindowSize(0, 0, 0x13F, 0xEF, 0);
    Gfx_SetRenderWindow(2);
    Gfx_SetRenderWindowSize(0x140, 0, 0x27F, 0xEF, 0);
    Gfx_SetRenderWindow(3);
    Gfx_SetRenderWindowSize(0, 0xF0, 0x13F, 0x1DF, 0);
    Gfx_SetRenderWindow(4);
    Gfx_SetRenderWindowSize(0x140, 0xF0, 0x27F, 0x1DF, 0);
    g_viewFormat = 5;
}

void Tracer_Release(void *thing) {
    extern struct { u8 pad[0x4CE]; s16 active; u8 pad2[0x10]; } tracers_802CD600[];

    tracers_802CD600[M2C_FIELD(thing, u32 *, 0xB4)].active = 0;
    M2C_FIELD(thing, s32 *, 0xB0) = 0;
    Thing_Release((s32)thing);
}

s32 Tracer_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void StateGameManager_RefreshMCPTextures(void) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    void Gfx_UnLoadTexture(u16 id);
    void DataPack_RefreshTextures(void *pack);
    void *pack;
    u32 i;

    pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
    if (pack != NULL) {
        for (i = 0; i < M2C_FIELD(pack, u32 *, 0x2C); i++) {
            Gfx_UnLoadTexture(M2C_FIELD(pack, u16 **, 0x30)[i]);
        }
        DataPack_RefreshTextures(pack);
    }
}

#pragma push
#pragma readonly_strings on
#pragma dont_inline on
s32 StateGameManager_Init(void) {
    extern struct { u8 *mcp; s32 unk4; } g_GameManagerState;
    extern s32 g_gameDifficulty;
    void ShowStats_Init(void);
    void ProcessManager_Init(void);
    void TimeHandler_Heartbeat(void);
    void Input_Init(void);
    void Input_Heartbeat(void);
    void SoundManager_Init(void);
    void SoundManager_Heartbeat(void);
    void InterfaceManager_Init(void);
    void InterfaceManager_Heartbeat(void);
    void InterfaceManager_Render(void);
    void ThingManager_DARYL_Init(void);
    void ThingManager_DARYL_Heartbeat(void);
    void ThingManager_DARYL_Render(void);
    void LidHandler_Init(void);
    void LidHandler_Heartbeat(void);
    void CameraManager_Init(void);
    void PlayerManager_Init(void);
    void Grid_Init(Vector4 *min, Vector4 *max, s32 x, s32 z);
    void *CameraManager_GetNewCamera(void);
    void GameSetting_Init(void);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void *camera = NULL;
    Vector4 min;
    Vector4 max;

    ShowStats_Init();
    ProcessManager_Init();
    ProcessManager_Add("TimeHandler", NULL, TimeHandler_Heartbeat, 0, 0);
    ProcessManager_Add("Input", Input_Init, Input_Heartbeat, 0, 1);
    ProcessManager_Add("SoundManager", SoundManager_Init, SoundManager_Heartbeat, 0, 1);
    ProcessManager_Add("Interface", InterfaceManager_Init, InterfaceManager_Heartbeat, (s32)InterfaceManager_Render, 1);
    ProcessManager_Add("ThingManager", ThingManager_DARYL_Init, ThingManager_DARYL_Heartbeat, (s32)ThingManager_DARYL_Render, 0);
    ProcessManager_Add("LidHandler", LidHandler_Init, LidHandler_Heartbeat, 0, 1);
    CameraManager_Init();
    PlayerManager_Init();
    min.x = min.y = min.z = -500.0f;
    max.x = max.y = max.z = 500.0f;
    Grid_Init(&min, &max, 0x10, 0x10);
    if (M2C_FIELD(GetCameraManager(), u8 *, 0x640) == 0) {
        camera = CameraManager_GetNewCamera();
    }
    ViewManager_SetCamera(camera, 0);
    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        GameSettings_SetCodeVariable(0xC8, 0, g_gameDifficulty);
        GameSettings_SetCodeVariable(0x198, 0, 0);
        g_GameManagerState.unk4 = M2C_FIELD(g_GameManagerState.mcp, s32 *, 0x2C) - 1;
    }
    if (g_GameManagerState.mcp == NULL) {
        GameSetting_Init();
        if ((g_GameManagerState.mcp = GameSettings_GetMCP()) == NULL) {
            StateManager_QueueChange(0xB);
            return 1;
        }
        if (*g_GameManagerState.mcp != 0) {
            StateManager_QueueChange(0xB);
            return 1;
        }
        g_GameManagerState.unk4 = 0;
    }
    return 1;
}

s32 StateGameManager_Shutdown(void) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    void FileStreaming_HeartBeat(void);
    void ShowStats_Release(void);
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);

    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        FileStreaming_ReleaseAllDataPacks(1, 1);
    }
    FileStreaming_HeartBeat();
    ShowStats_Release();
    return 1;
}

#pragma pop

void StateGameManager_Release(void) {
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);
    FileStreaming_ReleaseAllDataPacks(3, 1);
}

s32 StateGameManager_GetProgScript(u32 index) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    extern struct { u8 *mcp; s32 unk4; } g_GameManagerState;

    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        if (g_GameManagerState.mcp != NULL) {
            return M2C_FIELD(M2C_FIELD(g_GameManagerState.mcp, u8 **, 0x24), s32 *, 8);
        }
        return 0;
    }
    if (g_GameManagerState.mcp == NULL || index >= M2C_FIELD(g_GameManagerState.mcp, u32 *, 0x34)) {
        return 0;
    }
    return M2C_FIELD(g_GameManagerState.mcp, s32 **, 0x38)[index];
}

#pragma push
#pragma dont_inline on
void StateGameManager_InterfaceHandler(void *event) {
    u32 GameSettings_GetCodeVariable(s32, s32);
    extern u8 g_bInterfacePlaying;
    switch (M2C_FIELD(event, u16 *, 4)) {
    case 5:
        if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
            StateManager_QueueChange(0xB);
        }
        g_bInterfacePlaying = 0;
        break;
    }
}
#pragma pop

void StateProgressionManager_SetProgressionID(s32 id, s32 subID) {
    extern s32 g_GameProgressionID;
    extern s32 g_GameProgressionState[8];
    g_GameProgressionID = id;
    g_GameProgressionState[2] = subID;
}

void StateProgressionManager_GetProgressionID(s32 *id, s32 *subID) {
    extern s32 g_GameProgressionID;
    extern s32 g_GameProgressionState[8];
    *id = g_GameProgressionID;
    *subID = g_GameProgressionState[2];
}

void StateProgressionManager_RestartCurrentLevel(void) {
    extern u8 g_bRestartCurrentLevel;
    void ExitStateGamePlay(s32 state);
    g_bRestartCurrentLevel = 1;
    ExitStateGamePlay(0xB);
}

s32 ActionEndLevel(s32 *arg0) {
    extern u8 g_bRestartCurrentLevel;
    extern s32 g_GameProgressionEndState[2];

    if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
        ExitStateGamePlay(0xC);
    } else {
        g_bRestartCurrentLevel = 0;
        g_GameProgressionEndState[0] = arg0[0];
        g_GameProgressionEndState[1] = arg0[1];
        ExitStateGamePlay(0xB);
    }
    return 1;
}

s32 Pod_Init(u8 *pod, u8 *init) {
    u8 *motion;

    Thing_Init(pod, 0);
    motion = pod + 0xB0;
    M2C_FIELD(pod, void **, 0x110) = M2C_FIELD(init, void **, 0x20);
    M2C_FIELD(pod, s32 *, 0x114) = 0;
    M2C_FIELD(pod, s32 *, 0x118) = 0;
    M2C_FIELD(pod, s32 *, 0x40) = M2C_FIELD(init, s32 *, 0x0);
    M2C_FIELD(pod, s32 *, 0x44) = M2C_FIELD(init, s32 *, 0x4);
    M2C_FIELD(pod, s32 *, 0x48) = M2C_FIELD(init, s32 *, 0x8);
    M2C_FIELD(pod, s32 *, 0x4C) = M2C_FIELD(init, s32 *, 0xC);
    M2C_FIELD(pod, s32 *, 0xB0) = M2C_FIELD(M2C_FIELD(pod, u8 **, 0x110), s32 *, 0x3C);
    M2C_FIELD(pod, f32 *, 0xD4) = 0.0f;
    MotionState_Init(motion, 0);
    MotionState_BuildMatricesWorld(pod + 0x50, pod + 0x40, motion, 1.0f);
    return 1;
}

extern u8 g_bRestartCurrentLevel;
extern s32 g_GameProgressionEndState[2];
void Pod_Release(s32 arg0) {
    MotionState_Release(arg0 + 0xB0);
    Thing_Release(arg0);
}

s32 Pod_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x110);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s8 Player_IsHuman(void *player);
#pragma push
#pragma dont_inline on
s32 CarA_DoFreeStyleStunts(void *car) {
    s32 threshold;
    s8 *pad;

    threshold = M2C_FIELD(M2C_FIELD(M2C_FIELD(car, void **, 0x3BC), void **, 0x8C), s32 *, 0x28);
    if (M2C_FIELD(car, void **, 0x94) == NULL || !Player_IsHuman(M2C_FIELD(car, void **, 0x94))) {
        return 0;
    }
    pad = M2C_FIELD(car, s8 **, 0x94);
    if (*(u8 *)(pad + 0x10) == 0 && *(u8 *)(pad + 0x1A) == 0 &&
        pad[0] > -0x40 && pad[0] < 0x40 && pad[1] > -0x40 && pad[1] < 0x40) {
        return 0;
    }
    if (M2C_FIELD(car, s32 *, 0x460) > threshold && M2C_FIELD(car, s32 *, 0x404) > threshold &&
        !(M2C_FIELD(car, s32 *, 0x48C) & 0x40002)) {
        return 1;
    }
    return 0;
}

void CarA_SlaveWeaponHeartbeat(s32 car) {
    void MotionState_InitSlavesTimed(s32 state, s32 slaves, u8 type);
    void ThingManagerList_RemoveReleasedThings(s32 list);
    extern s32 g_iDeltaTimeMS;
    s32 *timer;
    u32 i;

    for (i = 0; i < M2C_FIELD(M2C_FIELD(car, s32 *, 0x3BC), u32 *, 0xE8); i++) {
        timer = (s32 *)(car + i * 4 + 0x46C);
        if (*timer < 10000) {
            MotionState_InitSlavesTimed(car + 0x330, M2C_FIELD(M2C_FIELD(car, s32 *, 0x3BC), s32 *, 0xEC) + i * 0xC + 4, i + 0xF0);
            ThingManagerList_RemoveReleasedThings(car + 0x344);
            *timer += g_iDeltaTimeMS;
        }
    }
}

void CarA_SlaveWeaponStop(void *car, s32 weapon) {
    void MotionState_ReleaseSlavesByType(void *state, u8 type);
    void ThingManagerList_RemoveReleasedThings(void *list);

    ((s32 *)((u8 *)car + 0x46C))[weapon] = 10001;
    MotionState_ReleaseSlavesByType((u8 *)car + 0x330, weapon + 0xF0);
    ThingManagerList_RemoveReleasedThings((u8 *)car + 0x344);
}

#pragma pop

void GameRules_ForceRoundEnd(u32 arg0) {
    extern s8 g_bRoundEnd_803C71CC;
    extern u32 g_playerNumThatWon_803C71C8;
    g_bRoundEnd_803C71CC = 1;
    g_playerNumThatWon_803C71C8 = arg0;
    if ((arg0 < 1U) && (arg0 > 4U)) {
        g_playerNumThatWon_803C71C8 = -1U;
    }
}

void GameRules_SetPlayerOrder(u32 *order, u32 count) {
    void *PlayerManager_GetPlayerByIndex(u8);
    u8 PlayerManager_PlayerCount(void);
    u32 i;
    u32 numPlayers;
    u32 j;
    void *player;

    numPlayers = PlayerManager_PlayerCount();
    for (i = 0; i < numPlayers; i++) {
        player = PlayerManager_GetPlayerByIndex(i);
        for (j = 0; j < count; j++) {
            if (M2C_FIELD(player, u32 *, 0xA8) == order[j]) {
                M2C_FIELD(player, s32 *, 0x60) = j + 1;
                break;
            }
        }
    }
}

void GSI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void GSI_VehicleItemInit(void) {
    typedef struct VehicleInterfaceData {
        s32 unk0;
        s32 unk4;
        s32 unk8;
        s32 unkC;
        s32 unk10;
        s32 unk14;
        u8 unk18[0x24 - 0x18];
        void *car;
        u8 unk28[0x2C - 0x28];
        u8 unk2C[8];
        u8 unk34[0x38 - 0x34];
    } VehicleInterfaceData;
    extern VehicleInterfaceData g_vehicleInterfaceData[4];
    void PlayerManager_Init(void);
    u32 i;

    for (i = 0; i < 4; i++) {
        memset(&g_vehicleInterfaceData[i], 0, sizeof(VehicleInterfaceData));
        g_vehicleInterfaceData[i].unk0 = -1;
        g_vehicleInterfaceData[i].unkC = -1;
        g_vehicleInterfaceData[i].unk4 = -1;
        g_vehicleInterfaceData[i].unk8 = -1;
        g_vehicleInterfaceData[i].unk14 = -1;
        memset(g_vehicleInterfaceData[i].unk2C, 0xFF, 8);
    }
    PlayerManager_Init();
}

void GSI_VehicleSelectionInfo(u8 *info) {
    typedef struct VehicleInterfaceData {
        u32 unk0;
        s32 unk4;
        s32 unk8;
        s32 unkC;
        s32 unk10;
        s32 unk14;
        u8 unk18[0x24 - 0x18];
        void *car;
        u8 unk28[0x2C - 0x28];
        u8 unk2C[8];
        u8 unk34[0x38 - 0x34];
    } VehicleInterfaceData;
    extern VehicleInterfaceData g_vehicleInterfaceData[4];
    u32 i;
    void *vehicle;

    for (i = 0; i < 4; i++) {
        if (*(u32 *)info == g_vehicleInterfaceData[i].unk0 && g_vehicleInterfaceData[i].car != NULL) {
            vehicle = M2C_FIELD(g_vehicleInterfaceData[i].car, void **, 0x3BC);
            info[0xC9] = M2C_FIELD(M2C_FIELD(vehicle, u8 **, 0x84), u8 *, 0x34);
            info[0xCA] = M2C_FIELD(M2C_FIELD(vehicle, u8 **, 0x84), u8 *, 0x35);
            info[0xCB] = M2C_FIELD(M2C_FIELD(vehicle, u8 **, 0x84), u8 *, 0x36);
            info[0xCC] = M2C_FIELD(M2C_FIELD(vehicle, u8 **, 0x84), u8 *, 0x37);
            return;
        }
    }
}

void FlashHolder_Init(void) {
    extern u8 g_pFlash[16][0x80];
    u32 i;

    for (i = 0; i < 16; i++) {
        *(void **)(g_pFlash[i] + 0x5C) = NULL;
    }
}

s32 FlashHolder_Add(void *flash, Vector4 *pos, s32 arg2, f32 time) {
    typedef struct Flash {
        Vector4 pos;
        u8 unk10[0x3C - 0x10];
        u8 active;
        u8 unk3D[0x58 - 0x3D];
        u8 visible;
        u8 unk59[0x5C - 0x59];
        void *flash;
        s32 unk60;
        u8 unk64[0x68 - 0x64];
        s32 unk68;
        u8 unk6C[0x74 - 0x6C];
        f32 time;
        u8 unk78[0x80 - 0x78];
    } Flash;
    extern Flash g_pFlash[16];
    s32 i;

    for (i = 0; i < 16; i++) {
        if (g_pFlash[i].flash == NULL) {
            g_pFlash[i].flash = flash;
            g_pFlash[i].pos = *pos;
            g_pFlash[i].unk60 = arg2;
            g_pFlash[i].unk68 = M2C_FIELD(flash, s32 *, 0xC);
            g_pFlash[i].active = 1;
            g_pFlash[i].visible = 1;
            g_pFlash[i].time = time;
            return i;
        }
    }
    return -1;
}

void FlashHolder_Render(void) {
    extern u8 g_pFlash[16][0x80];
    void Gfx_GetCameraPosition(Vector4 *pos);
    void Gfx_ScreenOverlay(GXColor *color, u8 mode);
    Vector4 camera;
    GXColor color;
    u8 *flash;
    u8 *owner;
    u32 i;

    Gfx_GetCameraPosition(&camera);
    for (i = 0; i < 16; i++) {
        flash = g_pFlash[i];
        owner = *(u8 **)(flash + 0x5C);
        if (owner != NULL && owner[1] != 0) {
            color = *(GXColor *)(flash + 0x34);
            Gfx_ScreenOverlay(&color, **(u8 **)(owner + 0x20));
        }
    }
}

void BulletHole_Init(void) {
    extern u8 g_bulletData[512][0x60];
    extern s32 g_nextBullet;
    s32 i;

    for (i = 0; i < 512; i++) {
        *(s32 *)(g_bulletData[i] + 0x50) = 0;
    }
    g_nextBullet = 1;
}

#pragma push
#pragma use_lmw_stmw on
void BulletHole_Render(void) {
    extern u8 g_bulletData[512][0x60];
    void *Gfx_Begin_GroupQuadFlat(u16 texture, s32 arg1, s32 arg2);
    void Gfx_End_GroupTri(u32 count);
    void MemCpy32(void *dst, void *src, u32 size);
    u8 *bullet;
    u32 i;
    u16 texture;
    u32 count;
    u8 *dst;

    count = 0;
    texture = 0xFFFF;
    dst = NULL;
    for (i = 0; i < 512; i++) {
        bullet = g_bulletData[i];
        if (*(s32 *)(bullet + 0x50) > 0) {
            if (texture != *(u16 *)(bullet + 0x5C)) {
                if (count != 0) {
                    Gfx_End_GroupTri(count);
                    count = 0;
                }
                dst = Gfx_Begin_GroupQuadFlat(*(u16 *)(bullet + 0x5C), 7, 0);
                texture = *(u16 *)(bullet + 0x5C);
            }
            MemCpy32(dst, bullet, 0x50);
            dst += 0x50;
            count++;
        }
    }
    if (count != 0) {
        Gfx_End_GroupTri(count);
    }
}
#pragma pop

void Motion_Release(s32 arg0) {
    MotionState_Release(arg0 + 0xB0);
    Thing_Release(arg0);
}

s32 Motion_Move(u8 *motion) {
    void MotionState_Heartbeat(void *state, void *arg1, void *thing);
    MotionState_BuildMatricesWorld(motion + 0x50, motion + 0x40, motion + 0xB0, 1.0f);
    MotionState_Heartbeat(motion + 0xB0, motion + 0x10, motion);
    return 1;
}

void Motion_Draw(u8 *motion) {
    extern u8 g_aMatStack[4096];
    void MotionState_Draw2(void *state, void *arg1, void *matStack);
    if (M2C_FIELD(motion, u32 *, 0xC0) != 0) {
        MotionState_Draw(motion + 0xB0);
    } else {
        MotionState_Draw2(motion + 0xB0, motion + 0x10, g_aMatStack);
    }
}

s32 Motion_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 PPCMfmsr(void) {
    return M2C_ERROR(/* unknown instruction: mfmsr $r3 */);
}

#pragma push
#pragma peephole off
#pragma scheduling off
s32 OSGetConsoleType(void) {
    extern u8 *BootInfo_803C71E0;
    u32 type;

    if (BootInfo_803C71E0 != NULL && (type = M2C_FIELD(BootInfo_803C71E0, u32 *, 0x2C)) != 0) {
        return type;
    } else {
        return 0x10000002;
    }
}

void InquiryCallback(s32 result, void *block) {
    extern u16 DriveInfo_802EED60[16];
    switch (M2C_FIELD(block, s32 *, 0xC)) {
    case 0:
        *(u16 *)0x800030E6 = 0x8000 | DriveInfo_802EED60[1];
        break;
    default:
        *(u16 *)0x800030E6 = 1;
        break;
    }
}
#pragma pop

extern void **OSExceptionTable_803C71F4;
void *__OSSetExceptionHandler(u8 exception, void *handler) {
    void *oldHandler;
    void **entry = &OSExceptionTable_803C71F4[exception];

    oldHandler = *entry;
    *entry = handler;
    return oldHandler;
}

void *__OSGetExceptionHandler(u8 exception) {
    return *(OSExceptionTable_803C71F4 + exception);
}

volatile u32 __DIRegs[16] : 0xCC006000;
u8 __OSGetDIConfig(void) {
    volatile u32 *regs = (volatile u32 *)0xCC006000;
    return (u8)regs[9];
}

void OSInitAlarm(void) {
    extern void *AlarmQueue_803C7210[2];
    void *__OSGetExceptionHandler(u8 exception);
    void *__OSSetExceptionHandler(u8 exception, void *handler);
    void DecrementerExceptionHandler(void);
    if (__OSGetExceptionHandler(8) != DecrementerExceptionHandler) {
        AlarmQueue_803C7210[0] = AlarmQueue_803C7210[1] = NULL;
        __OSSetExceptionHandler(8, DecrementerExceptionHandler);
    }
}

typedef struct Cell {
    struct Cell *prev;
    struct Cell *next;
    s32 size;
} Cell;

typedef struct HeapDesc {
    s32 size;
    Cell *free;
    Cell *allocated;
} HeapDesc;

extern HeapDesc *HeapArray_803C7218;
extern s32 NumHeaps_803C721C;
extern void *ArenaStart_803C7220;
extern void *ArenaEnd_803C7224;
extern s32 __OSCurrHeap;

void OSCreateAlarm(s32 *arg0) {
    *arg0 = 0;
}

Cell *DLInsert(Cell *list, Cell *cell) {
    Cell *prev;
    Cell *next;

    for (next = list, prev = NULL; next; prev = next, next = next->next) {
        if (cell <= next) {
            break;
        }
    }
    cell->next = next;
    cell->prev = prev;
    if (next) {
        next->prev = cell;
        if ((u8 *)cell + cell->size == (u8 *)next) {
            cell->size += next->size;
            next = next->next;
            cell->next = next;
            if (next) {
                next->prev = cell;
            }
        }
    }
    if (prev) {
        prev->next = cell;
        if ((u8 *)prev + prev->size == (u8 *)cell) {
            prev->size += cell->size;
            prev->next = next;
            if (next) {
                next->prev = prev;
            }
        }
        return list;
    } else {
        return cell;
    }
}

#pragma push
#pragma dont_inline on
void OSFreeToHeap(s32 heap, void *ptr) {
    HeapDesc *hd;
    Cell *cell;
    Cell *list;

    cell = (Cell *)((u8 *)ptr - 0x20);
    hd = &HeapArray_803C7218[heap];
    list = hd->allocated;
    if (cell->next) {
        cell->next->prev = cell->prev;
    }
    if (cell->prev == NULL) {
        list = cell->next;
    } else {
        cell->prev->next = cell->next;
    }
    hd->allocated = list;
    hd->free = DLInsert(hd->free, cell);
}
#pragma pop
s32 OSSetCurrentHeap(s32 arg0) {
    s32 temp_r0;

    temp_r0 = __OSCurrHeap;
    __OSCurrHeap = arg0;
    return temp_r0;
}

void *OSInitAlloc(void *arenaStart, void *arenaEnd, s32 maxHeaps) {
    u32 arraySize;
    s32 i;

    arraySize = sizeof(HeapDesc) * maxHeaps;
    HeapArray_803C7218 = arenaStart;
    NumHeaps_803C721C = maxHeaps;
    for (i = 0; i < NumHeaps_803C721C; i++) {
        HeapDesc *hd = &HeapArray_803C7218[i];
        hd->size = -1;
        hd->free = hd->allocated = NULL;
    }
    __OSCurrHeap = -1;
    arenaStart = (void *)((u8 *)HeapArray_803C7218 + arraySize);
    arenaStart = (void *)(((u32)arenaStart + 0x1F) & ~0x1F);
    ArenaStart_803C7220 = arenaStart;
    ArenaEnd_803C7224 = (void *)((u32)arenaEnd & ~0x1F);
    return arenaStart;
}

s32 OSCreateHeap(void *start, void *end) {
    s32 heap;
    HeapDesc *hd;
    Cell *cell;

    start = (void *)(((u32)start + 0x1F) & ~0x1F);
    end = (void *)((u32)end & ~0x1F);
    for (heap = 0; heap < NumHeaps_803C721C; heap++) {
        hd = &HeapArray_803C7218[heap];
        if (hd->size < 0) {
            hd->size = (u8 *)end - (u8 *)start;
            cell = (Cell *)start;
            cell->prev = NULL;
            cell->next = NULL;
            cell->size = hd->size;
            hd->free = cell;
            hd->allocated = NULL;
            return heap;
        }
    }
    return -1;
}

s32 fn_80069F84(s32 arg0) {
    extern s32 lbl_803C6D70;
    s32 temp_r0;

    temp_r0 = lbl_803C6D70;
    lbl_803C6D70 = arg0;
    return temp_r0;
}

s32 OSGetArenaHi(void) {
    extern s32 __OSArenaHi_803C7228;
    return __OSArenaHi_803C7228;
}

s32 OSGetArenaLo(void) {
    extern s32 __OSArenaLo_803C6D78;
    return __OSArenaLo_803C6D78;
}

void OSSetArenaHi(s32 arg0) {
    extern s32 __OSArenaHi_803C7228;
    __OSArenaHi_803C7228 = arg0;
}

void OSSetArenaLo(s32 arg0) {
    extern s32 __OSArenaLo_803C6D78;
    __OSArenaLo_803C6D78 = arg0;
}

void DCFlushRangeNoSync(void *addr, u32 nBytes) {
    if (nBytes > 0) {
        u32 n;
        if ((u32)addr & 0x1F) {
            nBytes += 0x20;
        }
        n = (nBytes + 0x1F) >> 5;
        do {
            __dcbf(addr, 0);
            addr = (u8 *)addr + 0x20;
        } while (--n);
    }
}

void DCZeroRange(void *addr, u32 nBytes) {
    if (nBytes > 0) {
        u32 n;
        if ((u32)addr & 0x1F) {
            nBytes += 0x20;
        }
        n = (nBytes + 0x1F) >> 5;
        do {
            __dcbz(addr, 0);
            addr = (u8 *)addr + 0x20;
        } while (--n);
    }
}

void ICInvalidateRange(void *addr, u32 nBytes) {
    if (nBytes > 0) {
        u32 n;
        if ((u32)addr & 0x1F) {
            nBytes += 0x20;
        }
        n = (nBytes + 0x1F) >> 5;
        do {
            __icbi(addr, 0);
            addr = (u8 *)addr + 0x20;
        } while (--n);
        __sync();
        __isync();
    }
}

void LCEnable(void) {
    void __LCEnable(void);
    s32 enabled;

    enabled = OSDisableInterrupts();
    __LCEnable();
    OSRestoreInterrupts(enabled);
}

#pragma push
#pragma peephole off
u32 LCStoreData(void *destAddr, void *srcAddr, u32 nBytes) {
    void LCStoreBlocks(void *destTag, void *srcAddr, u32 numBlocks);
    u32 numBlocks = (nBytes + 31) / 32;
    u32 numTransactions = (numBlocks + 128 - 1) / 128;

    while (numBlocks > 0) {
        if (numBlocks < 128) {
            LCStoreBlocks(destAddr, srcAddr, numBlocks);
            numBlocks = 0;
        } else {
            LCStoreBlocks(destAddr, srcAddr, 0);
            numBlocks -= 128;
            destAddr = (void *)((u32)destAddr + 4096);
            srcAddr = (void *)((u32)srcAddr + 4096);
        }
    }
    return numTransactions;
}
#pragma pop

#pragma push
#pragma peephole off
void L2GlobalInvalidate(void) {
    u32 PPCMfl2cr(void);
    void PPCMtl2cr(u32 val);
    void DBPrintf(const char *fmt, ...);

    __sync();
    PPCMtl2cr(PPCMfl2cr() & ~0x80000000);
    __sync();
    PPCMtl2cr(PPCMfl2cr() | 0x00200000);
    while (PPCMfl2cr() & 0x00000001) {
    }
    PPCMtl2cr(PPCMfl2cr() & ~0x00200000);
    while (PPCMfl2cr() & 0x00000001) {
        DBPrintf(">>> L2 INVALIDATE : SHOULD NEVER HAPPEN\n");
    }
}
#pragma pop

s32 OSGetCurrentContext(void) {
    return *(s32 *)0x800000D4;
}

void OSClearContext(register void *context) {
    M2C_FIELD(context, u16 *, 0x1A0) = 0;
    M2C_FIELD(context, u16 *, 0x1A2) = 0;
    if (context == *(void **)0x800000D8) {
        *(void **)0x800000D8 = NULL;
    }
}

void __OSContextInit(void) {
    void OSSwitchFPUContext(void);
    void *__OSSetExceptionHandler(u8 exception, void *handler);
    void DBPrintf(const char *fmt, ...);

    __OSSetExceptionHandler(7, OSSwitchFPUContext);
    *(u32 *)0x800000D8 = 0;
    DBPrintf("FPU-unavailable handler installed\n");
}

u16 OSGetFontEncode(void) {
    static u16 fontEncode = 0xFFFF;
    u16 encode;

    if (fontEncode <= 1) {
        return fontEncode;
    }
    switch (*(u32 *)0x800000CC) {
    case 0:
        if (*(volatile u16 *)0xCC00206E & 2) {
            encode = 1;
        } else {
            encode = 0;
        }
        fontEncode = encode;
        break;
    case 1:
    case 2:
    default:
        fontEncode = 0;
    }
    return fontEncode;
}

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler) {
    __OSInterruptHandler *table;
    __OSInterruptHandler oldHandler;

    table = InterruptHandlerTable_803C7240;
    oldHandler = table[interrupt];
    table[interrupt] = handler;
    return oldHandler;
}

u32 SetInterruptMask(u32 mask, u32 current);
void ExternalInterruptHandler(void);
u32 __OSMaskInterrupts(u32 global);
__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt) {
    return InterruptHandlerTable_803C7240[interrupt];
}

#pragma push
#pragma peephole off
u32 SetInterruptMask(u32 mask, u32 current) {
    u32 reg;

    switch (__cntlzw(mask)) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        reg = 0;
        if (!(current & 0x80000000)) {
            reg |= 0x1;
        }
        if (!(current & 0x40000000)) {
            reg |= 0x2;
        }
        if (!(current & 0x20000000)) {
            reg |= 0x4;
        }
        if (!(current & 0x10000000)) {
            reg |= 0x8;
        }
        if (!(current & 0x08000000)) {
            reg |= 0x10;
        }
        ((volatile u16 *)0xCC004000)[0xE] = (u16)reg;
        mask &= ~0xF8000000;
        break;
    case 5:
    case 6:
    case 7:
        reg = __DSPRegs[5];
        reg &= ~0x1F8;
        if (!(current & 0x04000000)) {
            reg |= 0x10;
        }
        if (!(current & 0x02000000)) {
            reg |= 0x40;
        }
        if (!(current & 0x01000000)) {
            reg |= 0x100;
        }
        __DSPRegs[5] = (u16)reg;
        mask &= ~0x07000000;
        break;
    case 8:
        reg = __AIRegs[0];
        reg &= ~0x2C;
        if (!(current & 0x00800000)) {
            reg |= 0x4;
        }
        __AIRegs[0] = reg;
        mask &= ~0x00800000;
        break;
    case 9:
    case 10:
    case 11:
        reg = ((volatile u32 *)0xCC006800)[0];
        reg &= ~0x2C0F;
        if (!(current & 0x00400000)) {
            reg |= 0x1;
        }
        if (!(current & 0x00200000)) {
            reg |= 0x4;
        }
        if (!(current & 0x00100000)) {
            reg |= 0x400;
        }
        ((volatile u32 *)0xCC006800)[0] = reg;
        mask &= ~0x00700000;
        break;
    case 12:
    case 13:
    case 14:
        reg = ((volatile u32 *)0xCC006800)[5];
        reg &= ~0xC0F;
        if (!(current & 0x00080000)) {
            reg |= 0x1;
        }
        if (!(current & 0x00040000)) {
            reg |= 0x4;
        }
        if (!(current & 0x00020000)) {
            reg |= 0x400;
        }
        ((volatile u32 *)0xCC006800)[5] = reg;
        mask &= ~0x000E0000;
        break;
    case 15:
    case 16:
        reg = ((volatile u32 *)0xCC006800)[10];
        reg &= ~0xF;
        if (!(current & 0x00010000)) {
            reg |= 0x1;
        }
        if (!(current & 0x00008000)) {
            reg |= 0x4;
        }
        ((volatile u32 *)0xCC006800)[10] = reg;
        mask &= ~0x00018000;
        break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
        reg = 0xF0;
        if (!(current & 0x00004000)) {
            reg |= 0x800;
        }
        if (!(current & 0x00000800)) {
            reg |= 0x8;
        }
        if (!(current & 0x00000400)) {
            reg |= 0x4;
        }
        if (!(current & 0x00000200)) {
            reg |= 0x2;
        }
        if (!(current & 0x00000100)) {
            reg |= 0x1;
        }
        if (!(current & 0x00000080)) {
            reg |= 0x100;
        }
        if (!(current & 0x00000040)) {
            reg |= 0x1000;
        }
        if (!(current & 0x00002000)) {
            reg |= 0x200;
        }
        if (!(current & 0x00001000)) {
            reg |= 0x400;
        }
        if (!(current & 0x00000020)) {
            reg |= 0x2000;
        }
        ((volatile u32 *)0xCC003000)[1] = reg;
        mask &= ~0x00007FE0;
        break;
    default:
        break;
    }
    return mask;
}
#pragma pop
u32 __OSMaskInterrupts(u32 global) {
    s32 enabled;
    u32 prev;
    u32 local;
    u32 mask;

    enabled = OSDisableInterrupts();
    prev = *(u32 *)0x800000C4;
    local = *(u32 *)0x800000C8;
    mask = global & ~(prev | local);
    global |= prev;
    *(u32 *)0x800000C4 = global;
    while (mask) {
        mask = SetInterruptMask(mask, global | local);
    }
    OSRestoreInterrupts(enabled);
    return prev;
}

u32 __OSUnmaskInterrupts(u32 global) {
    s32 enabled;
    u32 prev;
    u32 local;
    u32 mask;

    enabled = OSDisableInterrupts();
    prev = *(u32 *)0x800000C4;
    local = *(u32 *)0x800000C8;
    mask = global & (prev | local);
    global = prev & ~global;
    *(u32 *)0x800000C4 = global;
    while (mask) {
        mask = SetInterruptMask(mask, global | local);
    }
    OSRestoreInterrupts(enabled);
    return prev;
}

void __OSModuleInit(void) {
    *(u32 *)0x800030C8 = *(u32 *)0x800030CC = 0;
    *(u32 *)0x800030D0 = 0;
}

void OSInitMessageQueue(void *mq, void *msgArray, s32 msgCount) {
    void OSInitThreadQueue(void *queue);
    OSInitThreadQueue(mq);
    OSInitThreadQueue((u8 *)mq + 8);
    M2C_FIELD(mq, void **, 0x10) = msgArray;
    M2C_FIELD(mq, s32 *, 0x14) = msgCount;
    M2C_FIELD(mq, s32 *, 0x18) = 0;
    M2C_FIELD(mq, s32 *, 0x1C) = 0;
}

s32 OSSendMessage(void *mq, void *msg, s32 flags) {
    void OSSleepThread(void *queue);
    void OSWakeupThread(void *queue);
    s32 enabled;
    s32 lastIndex;

    enabled = OSDisableInterrupts();
    while (M2C_FIELD(mq, s32 *, 0x14) <= M2C_FIELD(mq, s32 *, 0x1C)) {
        if (!(flags & 1)) {
            OSRestoreInterrupts(enabled);
            return 0;
        } else {
            OSSleepThread(mq);
        }
    }
    lastIndex = (M2C_FIELD(mq, s32 *, 0x18) + M2C_FIELD(mq, s32 *, 0x1C)) % M2C_FIELD(mq, s32 *, 0x14);
    M2C_FIELD(mq, void ***, 0x10)[lastIndex] = msg;
    M2C_FIELD(mq, s32 *, 0x1C)++;
    OSWakeupThread((u8 *)mq + 8);
    OSRestoreInterrupts(enabled);
    return 1;
}

void __OSUnlockAllMutex(void *thread) {
    void OSWakeupThread(void *queue);
    void *mutex;
    void *next;

    while ((mutex = M2C_FIELD(thread, void **, 0x2F4)) != NULL) {
        next = M2C_FIELD(mutex, void **, 0x10);
        if (next == NULL) {
            M2C_FIELD(thread, void **, 0x2F8) = NULL;
        } else {
            M2C_FIELD(next, void **, 0x14) = NULL;
        }
        M2C_FIELD(thread, void **, 0x2F4) = next;
        M2C_FIELD(mutex, s32 *, 0xC) = 0;
        M2C_FIELD(mutex, void **, 0x8) = NULL;
        OSWakeupThread(mutex);
    }
}

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct SramControlBlock {
    u8 sram[64];
    u32 offset;
    s32 enabled;
    s32 locked;
    s32 sync;
    void (*callback)(void);
} SramControlBlock;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u16 _padding1;
} OSSramEx;

extern SramControlBlock Scb_802EEE20;

static inline void *LockSram(u32 offset) {
    u8 *sram = Scb_802EEE20.sram + offset;
    s32 enabled;
    enabled = OSDisableInterrupts();
    if (Scb_802EEE20.locked != 0) {
        OSRestoreInterrupts(enabled);
        return NULL;
    }
    Scb_802EEE20.enabled = enabled;
    Scb_802EEE20.locked = 1;
    return sram;
}

void Callback(void) {
    extern s32 Prepared_803C7260;
    Prepared_803C7260 = 1;
}

void OSRegisterResetFunction(void *func) {
    typedef struct OSResetFunctionInfo {
        void *func;
        u32 priority;
        struct OSResetFunctionInfo *next;
        struct OSResetFunctionInfo *prev;
    } OSResetFunctionInfo;
    typedef struct OSResetFunctionQueue {
        OSResetFunctionInfo *head;
        OSResetFunctionInfo *tail;
    } OSResetFunctionQueue;
    extern OSResetFunctionQueue ResetFunctionQueue_803C7268;
    OSResetFunctionInfo *info = func;
    OSResetFunctionInfo *tmp;
    OSResetFunctionInfo *iter;

    for (iter = ResetFunctionQueue_803C7268.head; iter && iter->priority <= info->priority; iter = iter->next) {
    }
    if (iter == NULL) {
        OSResetFunctionInfo **tail = &ResetFunctionQueue_803C7268.tail;
        if ((tmp = *tail) == NULL) {
            ResetFunctionQueue_803C7268.head = info;
        } else {
            tmp->next = info;
        }
        info->prev = tmp;
        info->next = NULL;
        *tail = info;
        return;
    }
    info->next = iter;
    tmp = iter->prev;
    iter->prev = info;
    info->prev = tmp;
    if (tmp == NULL) {
        ResetFunctionQueue_803C7268.head = info;
    } else {
        tmp->next = info;
    }
}

u32 OSGetResetCode(void) {
    if (*(u8 *)0x800030E2 != 0) {
        return 0x80000000;
    }
    return (((volatile u32 *)0xCC003000)[9] & ~7) >> 3;
}

void WriteSramCallback(void) {
    s32 WriteSram(void *, u32, u32);

    Scb_802EEE20.sync = WriteSram(Scb_802EEE20.sram + Scb_802EEE20.offset, Scb_802EEE20.offset, 0x40 - Scb_802EEE20.offset);
    if (Scb_802EEE20.sync) {
        Scb_802EEE20.offset = 0x40;
    }
}

OSSram *__OSLockSram(void) {
    return LockSram(0);
}

OSSramEx *__OSLockSramEx(void) {
    return LockSram(0x14);
}

s32 __OSUnlockSram(s32 commit) {
    return UnlockSram(commit, 0);
}

s32 __OSUnlockSramEx(s32 commit) {
    return UnlockSram(commit, 0x14);
}

s32 fn_8006AC34(void) {
    return *(s32 *)0x800000D4;
}

void fn_8006D134(void) {
    extern s32 lbl_803C7260;
    lbl_803C7260 = 1;
}

s32 __OSSyncSram(void) {
    return Scb_802EEE20.sync;
}

u32 OSGetSoundMode(void) {
    u32 mode;
    OSSram *sram = __OSLockSram();

    mode = (sram->flags & 4) ? 1 : 0;
    __OSUnlockSram(0);
    return mode;
}

void OSSetSoundMode(u32 mode) {
    OSSram *sram;

    mode = (mode & 1) << 2;
    sram = LockSram(0);
    if (mode == (sram->flags & 4)) {
        UnlockSram(0, 0);
        return;
    }
    sram->flags &= ~4;
    sram->flags |= mode;
    UnlockSram(1, 0);
}

u16 OSGetWirelessID(s32 chan) {
    OSSramEx *sram;
    u16 id;

    sram = LockSram(0x14);
    id = sram->wirelessPadID[chan];
    UnlockSram(0, 0x14);
    return id;
}

extern u8 __OSSystemCallVectorStart[];
extern u8 __OSSystemCallVectorEnd[];
void DCFlushRangeNoSync(void *addr, u32 nBytes);
void OSSetWirelessID(s32 chan, u16 id) {
    OSSramEx *sram;

    sram = LockSram(0x14);
    if (sram->wirelessPadID[chan] != id) {
        sram->wirelessPadID[chan] = id;
        UnlockSram(1, 0x14);
    } else {
        UnlockSram(0, 0x14);
    }
}

#pragma push
#pragma dont_inline on
void __OSInitSystemCall(void) {
    void *addr = (void *)0x80000C00;

    memcpy(addr, __OSSystemCallVectorStart, __OSSystemCallVectorEnd - __OSSystemCallVectorStart);
    DCFlushRangeNoSync(addr, 0x100);
    __sync();
    ICInvalidateRange(addr, 0x100);
}
#pragma pop

void OSInitThreadQueue(void *arg0) {
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 0) = 0;
}

s32 OSGetCurrentThread(void) {
    return *(s32 *)0x800000E4;
}

void UnsetRun(void *arg0) {
    extern s32 RunQueueBits_803C7290;
    void *temp_r4;
    void *temp_r5;
    void *temp_r6;

    temp_r4 = M2C_FIELD(arg0, void **, 0x2E0);
    temp_r5 = M2C_FIELD(arg0, void **, 0x2DC);
    temp_r6 = M2C_FIELD(arg0, void **, 0x2E4);
    if (temp_r4 == NULL) {
        M2C_FIELD(temp_r5, void **, 4) = temp_r6;
    } else {
        M2C_FIELD(temp_r4, void **, 0x2E4) = temp_r6;
    }
    if (temp_r6 == NULL) {
        M2C_FIELD(temp_r5, void **, 0) = temp_r4;
    } else {
        M2C_FIELD(temp_r6, void **, 0x2E0) = temp_r4;
    }
    if ((void *) M2C_FIELD(temp_r5, void **, 0) == NULL) {
        RunQueueBits_803C7290 &= ~(1 << (0x1F - M2C_FIELD(arg0, s32 *, 0x2D0)));
    }
    M2C_FIELD(arg0, void **, 0x2DC) = NULL;
}

s32 __OSGetEffectivePriority(void *thread) {
    s32 priority;
    void *blocked;
    void *mutex;

    priority = M2C_FIELD(thread, s32 *, 0x2D4);
    for (mutex = M2C_FIELD(thread, void **, 0x2F4); mutex; mutex = M2C_FIELD(mutex, void **, 0x10)) {
        blocked = M2C_FIELD(mutex, void **, 0);
        if (blocked && M2C_FIELD(blocked, s32 *, 0x2D0) < priority) {
            priority = M2C_FIELD(blocked, s32 *, 0x2D0);
        }
    }
    return priority;
}

void __OSReschedule(void) {
    void SelectThread(s32 yield);
    extern s32 RunQueueHint_803C7294;

    if (RunQueueHint_803C7294) {
        SelectThread(0);
    }
}

s64 __OSGetSystemTime(void) {
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    s64 OSGetTime(void);
    s32 enabled;
    s64 *timeAdjustAddr = (s64 *)0x800030D8;
    s64 result;

    enabled = OSDisableInterrupts();
    result = *timeAdjustAddr + OSGetTime();
    OSRestoreInterrupts(enabled);
    return result;
}

s32 __get_time(void) {
    M2C_UNK OSGetTime(M2C_UNK);
    u64 __div2i(M2C_UNK, u32);
    u32 temp_r31;

    temp_r31 = (u32) *(u32 *)0x800000F8 >> 2U;
    OSGetTime(0x80000000);
    return (u32) __div2i(0, temp_r31) + 0xBC17C200;
}

void __init_user(void) {
    void __init_cpp(void);
    __init_cpp();
}

void __init_cpp(void) {
    extern void (*_ctors[])(void);
    void (**constructor)(void);

    for (constructor = _ctors; *constructor; constructor++) {
        (*constructor)();
    }
}

void _ExitProcess(void) {
    void PPCHalt(void);
    PPCHalt();
}

void DBInit(void) {
    extern u32 *__DBInterface;
    extern s32 DBVerbose;
    extern void __DBExceptionDestination(void);
    __DBInterface = (u32 *)0x80000040;
    __DBInterface[2] = (u32)__DBExceptionDestination + 0x80000000;
    DBVerbose = 1;
}

void __DBExceptionDestinationAux(void) {
    void OSReport(const char *, ...);
    void OSDumpContext(void *);
    void PPCHalt(void);
    u32 *contextAddr = (u32 *)0xC0;
    void *context = (void *)(*contextAddr + 0x80000000);

    OSReport("DBExceptionDestination\n");
    OSDumpContext(context);
    PPCHalt();
}

s32 fn_8006E668(void) {
    return *(s32 *)0x800000E4;
}

s32 __DBIsExceptionMarked(u8 arg0) {
    extern void *__DBInterface;
    return M2C_FIELD(__DBInterface, s32 *, 4) & (1 << arg0);
}

void DBPrintf(char *str, ...) {
}

void C_MTXOrtho(f32 m[4][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f) {
    f32 tmp;

    tmp = 1.0f / (r - l);
    m[0][0] = 2.0f * tmp;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = tmp * -(r + l);
    tmp = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = 2.0f * tmp;
    m[1][2] = 0.0f;
    m[1][3] = tmp * -(t + b);
    tmp = 1.0f / (f - n);
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f * tmp;
    m[2][3] = -f * tmp;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

f32 PSVECDotProduct(void *a, void *b) {
    M2C_ERROR(/* unknown instruction: ps_mul $f2, $f2, $f3 */);
    M2C_ERROR(/* unknown instruction: ps_madd $f3, $f5, $f4, $f2 */);
    return M2C_ERROR(/* unknown instruction: ps_sum0 $f1, $f3, $f2, $f2 */);
}

f32 PSVECSquareDistance(f32 *a, f32 *b) {
    f32 dx = a[0] - b[0];
    f32 dy = a[1] - b[1];
    f32 dz = a[2] - b[2];
    return dx * dx + dy * dy + dz * dz;
}

#pragma push
#pragma dont_inline on
void __DVDInitWA(void) {
    extern s32 CommandList_802EF860[][5];
    extern s32 NextCommandNumber_803C72EC;
    void __DVDLowSetWAType(u32 type, u32 location);

    NextCommandNumber_803C72EC = 0;
    CommandList_802EF860[0][0] = -1;
    __DVDLowSetWAType(0, 0);
    OSInitAlarm();
}
#pragma pop

void AlarmHandler(void *alarm, void *context) {
    M2C_UNK DVDLowSeek(u32, void (*)(u32));
    M2C_UNK Read(void *, u32, u32, void (*)(u32));
    extern s32 CommandList_802EF860[][5];
    extern s32 NextCommandNumber_803C72EC;
    s32 *command;

    command = CommandList_802EF860[NextCommandNumber_803C72EC];
    if (command[0] == 1) {
        NextCommandNumber_803C72EC++;
        Read((void *)command[1], command[2], command[3], (void (*)(u32))command[4]);
    } else if (command[0] == 2) {
        NextCommandNumber_803C72EC++;
        DVDLowSeek(command[3], (void (*)(u32))command[4]);
    }
}

#pragma push
#pragma dont_inline on
void AlarmHandlerForTimeout(void *alarm, void *context) {
    extern void (*Callback_803C72B0)(u32);
    void __OSMaskInterrupts(u32 mask);
    void OSSetCurrentContext(void *context);
    u8 tmpContext[0x2C8];
    void (*callback)(u32);

    __OSMaskInterrupts(0x400);
    OSClearContext(tmpContext);
    OSSetCurrentContext(tmpContext);
    callback = Callback_803C72B0;
    Callback_803C72B0 = NULL;
    if (callback) {
        callback(0x10);
    }
    OSClearContext(tmpContext);
    OSSetCurrentContext(context);
}

void SeekTwiceBeforeRead(void *addr, u32 length, u32 offset, void *callback) {
    void DVDLowSeek(u32 offset, void *callback);
    extern s32 CommandList_802EF860[15];
    extern u32 WorkAroundSeekLocation_803C72D0;
    extern s32 NextCommandNumber_803C72EC;
    u32 offsetToSeek;

    if ((offset & ~0x7FFF) == 0) {
        offsetToSeek = 0;
    } else {
        offsetToSeek = (offset & ~0x7FFF) + WorkAroundSeekLocation_803C72D0;
    }
    CommandList_802EF860[0] = 2;
    CommandList_802EF860[3] = offsetToSeek;
    CommandList_802EF860[4] = (s32)callback;
    CommandList_802EF860[5] = 1;
    CommandList_802EF860[6] = (s32)addr;
    CommandList_802EF860[7] = length;
    CommandList_802EF860[8] = offset;
    CommandList_802EF860[9] = (s32)callback;
    CommandList_802EF860[10] = -1;
    NextCommandNumber_803C72EC = 0;
    DVDLowSeek(offsetToSeek, callback);
}

#pragma push
#pragma dont_inline on
s32 DVDLowSeek(u32 offset, void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    ((volatile u32 *)0xCC006000)[2] = 0xAB000000;
    ((volatile u32 *)0xCC006000)[3] = offset >> 2;
    ((volatile u32 *)0xCC006000)[7] = 1;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

s32 DVDLowWaitCoverClose(void *callback) {
    extern void *Callback_803C72B0;
    extern s32 WaitingCoverClose_803C72C4;
    extern s32 StopAtNextInt_803C72A8;

    Callback_803C72B0 = callback;
    WaitingCoverClose_803C72C4 = 1;
    StopAtNextInt_803C72A8 = 0;
    __DIRegs[1] = 2;
    return 1;
}

#pragma push
#pragma dont_inline on
s32 DVDLowStopMotor(void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    ((volatile u32 *)0xCC006000)[2] = 0xE3000000;
    ((volatile u32 *)0xCC006000)[7] = 1;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

#pragma pop

#pragma push
#pragma dont_inline on
s32 DVDLowRequestError(void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    ((volatile u32 *)0xCC006000)[2] = 0xE0000000;
    ((volatile u32 *)0xCC006000)[7] = 1;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
s32 DVDLowInquiry(void *info, void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    ((volatile u32 *)0xCC006000)[2] = 0x12000000;
    ((volatile u32 *)0xCC006000)[4] = 0x20;
    ((volatile u32 *)0xCC006000)[5] = (u32)info;
    ((volatile u32 *)0xCC006000)[6] = 0x20;
    ((volatile u32 *)0xCC006000)[7] = 3;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
s32 DVDLowAudioStream(u32 subcmd, u32 length, u32 offset, void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    ((volatile u32 *)0xCC006000)[2] = subcmd | 0xE1000000;
    ((volatile u32 *)0xCC006000)[3] = offset >> 2;
    ((volatile u32 *)0xCC006000)[4] = length;
    ((volatile u32 *)0xCC006000)[7] = 1;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
s32 DVDLowRequestAudioStatus(u32 subcmd, void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    ((volatile u32 *)0xCC006000)[2] = subcmd | 0xE2000000;
    ((volatile u32 *)0xCC006000)[7] = 1;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
s32 DVDLowAudioBufferConfig(s32 enable, u32 size, void *callback) {
    extern void *Callback_803C72B0;
    extern s32 StopAtNextInt_803C72A8;
    extern s32 AlarmForTimeout_802EF8C8[10];
    void OSSetAlarm(void *alarm, s64 tick, void *handler);
    void AlarmHandlerForTimeout(void *alarm, void *context);
    u32 mask;
    s64 timeout;

    Callback_803C72B0 = callback;
    StopAtNextInt_803C72A8 = 0;
    mask = enable ? 0x10000 : 0;
    ((volatile u32 *)0xCC006000)[2] = 0xE4000000 | mask | size;
    ((volatile u32 *)0xCC006000)[7] = 1;
    timeout = (*(u32 *)0x800000F8 / 4) * 10;
    OSCreateAlarm(AlarmForTimeout_802EF8C8);
    OSSetAlarm(AlarmForTimeout_802EF8C8, timeout, AlarmHandlerForTimeout);
    return 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
#pragma use_lmw_stmw on
void DVDLowReset(void) {
    extern s32 ResetOccurred_803C72C0;
    extern s64 LastResetEnd_803C72B8;
    u32 reg;
    s64 resetStart;

    ((volatile u32 *)0xCC006000)[1] = 2;
    reg = ((volatile u32 *)0xCC003000)[9];
    ((volatile u32 *)0xCC003000)[9] = (reg & ~4) | 1;
    resetStart = __OSGetSystemTime();
    while ((__OSGetSystemTime() - resetStart) < (s64)((12 * ((*(u32 *)0x800000F8 / 4) / 125000)) / 8)) {
    }
    ((volatile u32 *)0xCC003000)[9] = reg | 5;
    ResetOccurred_803C72C0 = 1;
    LastResetEnd_803C72B8 = __OSGetSystemTime();
}
#pragma pop

s32 SIGetType(s32 chan);
s32 SIDecodeType(s32 type);
void SISetSamplingRate(u32 msec);
s32 SIProbe(s32 chan) {
    return SIDecodeType(SIGetType(chan));
}

void SIRefreshSamplingRate(void) {
    extern u32 SamplingRate_803C75A0;
    SISetSamplingRate(SamplingRate_803C75A0);
}

s32 DVDLowBreak(void) {
    extern s32 Breaking_803C72C8;
    extern s32 StopAtNextInt_803C72A8;
    StopAtNextInt_803C72A8 = 1;
    Breaking_803C72C8 = 1;
    return 1;
}

void *DVDLowClearCallback(void) {
    extern void *Callback_803C72B0;
    void *old;

    ((volatile u32 *)0xCC006000)[1] = 0;
    old = Callback_803C72B0;
    Callback_803C72B0 = NULL;
    return old;
}

void __DVDFSInit(void) {
    extern void *BootInfo_803C72F0;
    extern u32 *FstStart_803C72F4;
    extern char *FstStringStart_803C72F8;
    extern u32 MaxEntryNum_803C72FC;

    BootInfo_803C72F0 = (void *)0x80000000;
    FstStart_803C72F4 = *(u32 **)0x80000038;
    if (FstStart_803C72F4) {
        MaxEntryNum_803C72FC = FstStart_803C72F4[2];
        FstStringStart_803C72F8 = (char *)FstStart_803C72F4 + MaxEntryNum_803C72FC * 12;
    }
}

s32 DVDOpen(char *fileName, void *fileInfo) {
    extern u32 *FstStart_803C72F4;
    s32 DVDConvertPathToEntrynum(char *pathPtr);
    s32 DVDGetCurrentDir(char *path, u32 maxlen);
    void OSReport(const char *msg, ...);
    s32 entry;
    char currentDir[128];

    entry = DVDConvertPathToEntrynum(fileName);
    if (0 > entry) {
        DVDGetCurrentDir(currentDir, 128);
        OSReport("Warning: DVDOpen(): file '%s' was not found under %s.\n", fileName, currentDir);
        return 0;
    }
    if ((FstStart_803C72F4[entry * 3] & 0xFF000000) ? 1 : 0) {
        return 0;
    }
    M2C_FIELD(fileInfo, u32 *, 0x30) = FstStart_803C72F4[entry * 3 + 1];
    M2C_FIELD(fileInfo, u32 *, 0x34) = FstStart_803C72F4[entry * 3 + 2];
    M2C_FIELD(fileInfo, void **, 0x38) = NULL;
    M2C_FIELD(fileInfo, s32 *, 0xC) = 0;
    return 1;
}

s32 DVDGetCurrentDir(char *path, u32 maxlen) {
    extern u32 *FstStart_803C72F4;
    extern u32 currentDirectory_803C7300;
    u32 entryToPath(u32 entry, char *path, u32 maxlen);
    u32 loc;
    u32 entry;
    s32 result;

    entry = currentDirectory_803C7300;
    loc = entryToPath(entry, path, maxlen);
    if (loc == maxlen) {
        path[maxlen - 1] = '\0';
        result = 0;
    } else {
        if ((FstStart_803C72F4[entry * 3] & 0xFF000000) ? 1 : 0) {
            if (loc == maxlen - 1) {
                path[loc] = '\0';
                result = 0;
                goto end;
            }
            path[loc++] = '/';
        }
        path[loc] = '\0';
        result = 1;
    }
end:
    return result;
}

#pragma push
#pragma use_lmw_stmw on
s32 DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void *callback, s32 prio) {
    void OSPanic(const char *file, s32 line, const char *msg, ...);
    s32 DVDReadAbsAsyncPrio(void *block, void *addr, s32 length, s32 offset, void *callback, s32 prio);
    void cbForReadAsync(s32 result, void *block);

    if (!((0 <= offset) && ((u32)offset < M2C_FIELD(fileInfo, u32 *, 0x34)))) {
        OSPanic("dvdfs.c", 0x2E3, "DVDReadAsync(): specified area is out of the file  ");
    }
    if (!((0 <= offset + length) && ((u32)(offset + length) < M2C_FIELD(fileInfo, u32 *, 0x34) + 32))) {
        OSPanic("dvdfs.c", 0x2E9, "DVDReadAsync(): specified area is out of the file  ");
    }
    M2C_FIELD(fileInfo, void **, 0x38) = callback;
    DVDReadAbsAsyncPrio(fileInfo, addr, length, M2C_FIELD(fileInfo, s32 *, 0x30) + offset, cbForReadAsync, prio);
    return 1;
}
#pragma pop

void stateCoverClosed_CMD(void *block) {
    M2C_UNK DVDLowReadDiskID(void *, void (*)(u32));
    void cbForStateCoverClosed(u32);
    extern u8 tmpBuffer_802EF940[];
    DVDLowReadDiskID(tmpBuffer_802EF940, cbForStateCoverClosed);
}

void cbForStateCoverClosed(u32 intType) {
    M2C_UNK DVDLowRequestError(void (*)(u32));
    M2C_UNK DVDReset();
    M2C_UNK __DVDStoreErrorCode(u32);
    void cbForStateError(u32);
    void cbForStateGettingError(u32);
    M2C_UNK stateCheckID();
    extern s32 NumInternalRetry_803C7344;
    extern void *executing_803C7310;
    if (intType == 16) {
        M2C_FIELD(executing_803C7310, s32 *, 0xC) = -1;
        __DVDStoreErrorCode(0x01234568);
        DVDReset();
        cbForStateError(0);
        return;
    }
    if (intType & 1) {
        NumInternalRetry_803C7344 = 0;
        stateCheckID();
    } else {
        DVDLowRequestError(cbForStateGettingError);
    }
}

void stateMotorStopped(void) {
    M2C_UNK DVDLowWaitCoverClose(void (*)(u32));
    void cbForStateMotorStopped(u32);
    DVDLowWaitCoverClose(cbForStateMotorStopped);
}

s32 DVDGetDriveStatus(void) {
    extern M2C_UNK DummyCommandBlock_802EF9C0;
    extern s32 FatalErrorFlag_803C7328;
    extern s32 PausingFlag_803C7320;
    extern void *executing_803C7310;
    s32 temp_r30;
    s32 var_r31;
    void *temp_r31;

    temp_r30 = OSDisableInterrupts();
    if ((s32) FatalErrorFlag_803C7328 != 0) {
        var_r31 = -1;
    } else if ((s32) PausingFlag_803C7320 != 0) {
        var_r31 = 8;
    } else {
        temp_r31 = executing_803C7310;
        if (temp_r31 == NULL) {
            var_r31 = 0;
        } else if (temp_r31 == &DummyCommandBlock_802EF9C0) {
            var_r31 = 0;
        } else {
            OSDisableInterrupts();
            var_r31 = M2C_FIELD(temp_r31, s32 *, 0xC);
            if (var_r31 == 3) {
                var_r31 = 1;
            }
            OSRestoreInterrupts();
        }
    }
    OSRestoreInterrupts(temp_r30);
    return var_r31;
}

void cbForReadAsync(s32 result, void *block) {
    if (M2C_FIELD(block, void (**)(s32, void *), 0x38)) {
        M2C_FIELD(block, void (**)(s32, void *), 0x38)(result, block);
    }
}

void cbForReadSync(void) {
    M2C_UNK OSWakeupThread(M2C_UNK *);
    extern M2C_UNK __DVDThreadQueue;
    OSWakeupThread(&__DVDThreadQueue);
}

void stateReadingFST(void) {
    M2C_UNK DVDLowRead(void *, u32, u32, void (*)(u32));
    void cbForStateReadingFST(u32);
    extern void (*LastState)(void);
    extern void *bootInfo_803C7318;
    extern u8 tmpBuffer_802EF940[];
    LastState = stateReadingFST;
    DVDLowRead(M2C_FIELD(bootInfo_803C7318, void **, 0x38), (M2C_FIELD(tmpBuffer_802EF940, u32 *, 8) + 31) & ~31, M2C_FIELD(tmpBuffer_802EF940, u32 *, 4), cbForStateReadingFST);
}

void cbForStateReadingFST(u32 intType) {
    void stateGettingError(void);
    M2C_UNK stateReady_800725A4();
    void stateTimeout(void);
    extern u8 DummyCommandBlock_802EF9C0[];
    extern s32 NumInternalRetry_803C7344;
    extern void *executing_803C7310;
    void *finished;

    if (intType == 16) {
        M2C_FIELD(executing_803C7310, s32 *, 0xC) = -1;
        stateTimeout();
        return;
    }
    if (intType & 1) {
        NumInternalRetry_803C7344 = 0;
        finished = executing_803C7310;
        executing_803C7310 = DummyCommandBlock_802EF9C0;
        M2C_FIELD(finished, s32 *, 0xC) = 0;
        if (M2C_FIELD(finished, void (**)(s32, void *), 0x28)) {
            M2C_FIELD(finished, void (**)(s32, void *), 0x28)(0, finished);
        }
        stateReady_800725A4();
    } else {
        stateGettingError();
    }
}

void cbForStateError(u32 intType) {
    void stateTimeout(void);
    M2C_UNK stateReady_800725A4();
    extern void *executing_803C7310;
    extern u8 DummyCommandBlock_802EF9C0[];
    extern s32 FatalErrorFlag_803C7328;
    extern s32 Canceling_803C7330;
    extern void (*CancelCallback_803C7334)(s32, void *);
    void *finished;

    if (intType == 16) {
        M2C_FIELD(executing_803C7310, s32 *, 0xC) = -1;
        stateTimeout();
        return;
    }
    finished = executing_803C7310;
    executing_803C7310 = DummyCommandBlock_802EF9C0;
    FatalErrorFlag_803C7328 = 1;
    if (M2C_FIELD(finished, void (**)(s32, void *), 0x28)) {
        M2C_FIELD(finished, void (**)(s32, void *), 0x28)(-1, finished);
    }
    if (Canceling_803C7330) {
        Canceling_803C7330 = 0;
        if (CancelCallback_803C7334) {
            CancelCallback_803C7334(0, finished);
        }
    }
    stateReady_800725A4();
}

void stateTimeout(void) {
    M2C_UNK __DVDStoreErrorCode(u32);
    M2C_UNK DVDReset();
    __DVDStoreErrorCode(0x01234568);
    DVDReset();
    cbForStateError(0);
}

void stateGettingError(void) {
    M2C_UNK DVDLowRequestError(void (*)(u32));
    void cbForStateGettingError(u32);
    DVDLowRequestError(cbForStateGettingError);
}

s32 CategorizeError(u32 error) {
    extern u32 LastError_803C7340;
    extern s32 NumInternalRetry_803C7344;
    extern void *executing_803C7310;

    if (error == 0x20400) {
        LastError_803C7340 = error;
        return 1;
    }
    error &= 0xFFFFFF;
    if (error == 0x62800 || error == 0x23A00 || error == 0xB5A01) {
        return 0;
    }
    NumInternalRetry_803C7344++;
    if (NumInternalRetry_803C7344 == 2) {
        if (error == LastError_803C7340) {
            LastError_803C7340 = error;
            return 1;
        } else {
            LastError_803C7340 = error;
            return 2;
        }
    }
    LastError_803C7340 = error;
    if (error == 0x31100 || M2C_FIELD(executing_803C7310, u32 *, 8) == 5) {
        return 2;
    }
    return 3;
}

void cbForUnrecoveredErrorRetry(u32 intType) {
    void __DVDStoreErrorCode(u32 error);
    void DVDReset(void);
    void cbForStateError(u32 intType);
    void DVDLowStopMotor(void (*callback)(u32));
    extern u8 *executing_803C7310;

    if (intType == 16) {
        M2C_FIELD(executing_803C7310, s32 *, 0xC) = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }
    M2C_FIELD(executing_803C7310, s32 *, 0xC) = -1;
    if (intType & 2) {
        __DVDStoreErrorCode(0x1234567);
        DVDLowStopMotor(cbForStateError);
    } else {
        __DVDStoreErrorCode(*(volatile u32 *)(0xCC006000 + 0x20));
        DVDLowStopMotor(cbForStateError);
    }
}

void stateGoToRetry(void) {
    M2C_UNK DVDLowStopMotor(void (*)(u32));
    void cbForStateGoToRetry(u32);
    DVDLowStopMotor(cbForStateGoToRetry);
}

void stateCheckID2(void) {
    void DVDLowRead(void *addr, u32 length, u32 offset, void (*callback)(u32));
    void cbForStateCheckID2(u32 intType);
    extern u8 tmpBuffer_802EF940[128];

    DVDLowRead(tmpBuffer_802EF940, 0x20, 0x420, cbForStateCheckID2);
}

void cbForStateCheckID2(u32 intType) {
    M2C_UNK DVDLowRead(void *, u32, u32, void (*)(u32));
    M2C_UNK DVDLowRequestError(void (*)(u32));
    M2C_UNK DVDReset();
    M2C_UNK __DVDStoreErrorCode(u32);
    void cbForStateError(u32);
    void cbForStateGettingError(u32);
    void cbForStateReadingFST(u32);
    void stateReadingFST(void);
    extern void (*LastState)(void);
    extern s32 NumInternalRetry_803C7344;
    extern void *bootInfo_803C7318;
    extern void *executing_803C7310;
    extern u8 tmpBuffer_802EF940[];
    if (intType == 16) {
        M2C_FIELD(executing_803C7310, s32 *, 0xC) = -1;
        __DVDStoreErrorCode(0x01234568);
        DVDReset();
        cbForStateError(0);
        return;
    }
    if (intType & 1) {
        NumInternalRetry_803C7344 = 0;
        LastState = stateReadingFST;
        DVDLowRead(M2C_FIELD(bootInfo_803C7318, void **, 0x38), (M2C_FIELD(tmpBuffer_802EF940, u32 *, 8) + 31) & ~31, M2C_FIELD(tmpBuffer_802EF940, u32 *, 4), cbForStateReadingFST);
    } else {
        DVDLowRequestError(cbForStateGettingError);
    }
}

s32 DVDSetAutoInvalidation(s32 arg0) {
    extern s32 autoInvalidation_803C6DA8;
    s32 temp_r0;

    temp_r0 = autoInvalidation_803C6DA8;
    autoInvalidation_803C6DA8 = arg0;
    return temp_r0;
}

s32 DVDCancel(void *arg0) {
    s32 DVDCancelAsync(M2C_UNK *);
    M2C_UNK OSSleepThread(M2C_UNK *);
    extern M2C_UNK __DVDThreadQueue;
    extern M2C_UNK cbForCancelSync;
    s32 temp_r31;
    s32 temp_r3;
    u32 temp_r3_2;

    if (DVDCancelAsync(&cbForCancelSync) == 0) {
        return -1;
    }
    temp_r31 = OSDisableInterrupts();
loop_3:
    temp_r3 = M2C_FIELD(arg0, s32 *, 0xC);
    if (((u32) (temp_r3 + 1) > 1U) && (temp_r3 != 0xA) && ((temp_r3 != 3) || ((temp_r3_2 = M2C_FIELD(arg0, u32 *, 8), (((u32) (temp_r3_2 - 4) > 1U) != 0)) && (temp_r3_2 != 0xDU) && (temp_r3_2 != 0xFU)))) {
        OSSleepThread(&__DVDThreadQueue);
        goto loop_3;
    }
    OSRestoreInterrupts(temp_r31);
    return 0;
}

void cbForCancelSync(void) {
    M2C_UNK OSWakeupThread(M2C_UNK *);
    extern M2C_UNK __DVDThreadQueue;
    OSWakeupThread(&__DVDThreadQueue);
}

s32 DVDGetCurrentDiskID(void) {
    return 0x80000000;
}

#pragma push
#pragma peephole off
void __DVDClearWaitingQueue(void) {
    DVDQueue *q;

    q = &WaitingQueue_802EFA18[0];
    q->next = q;
    q->prev = q;
    q = &WaitingQueue_802EFA18[1];
    q->next = q;
    q->prev = q;
    q = &WaitingQueue_802EFA18[2];
    q->next = q;
    q->prev = q;
    q = &WaitingQueue_802EFA18[3];
    q->next = q;
    q->prev = q;
}

s32 __DVDPushWaitingQueue(s32 prio, DVDQueue *block) {
    s32 enabled;
    DVDQueue *q;

    enabled = OSDisableInterrupts();
    q = &WaitingQueue_802EFA18[prio];
    q->prev->next = block;
    block->prev = q->prev;
    block->next = q;
    q->prev = block;
    OSRestoreInterrupts(enabled);
    return 1;
}
#pragma pop

DVDQueue *__DVDPopWaitingQueue(void) {
    s32 enabled;
    s32 i;
    DVDQueue *q;
    DVDQueue *tmp;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; i++) {
        if (WaitingQueue_802EFA18[i].next != &WaitingQueue_802EFA18[i]) {
            OSRestoreInterrupts(enabled);
            enabled = OSDisableInterrupts();
            q = &WaitingQueue_802EFA18[i];
            tmp = q->next;
            q->next = tmp->next;
            tmp->next->prev = q;
            OSRestoreInterrupts(enabled);
            tmp->next = NULL;
            tmp->prev = NULL;
            return tmp;
        }
    }
    OSRestoreInterrupts(enabled);
    return NULL;
}

s32 __DVDCheckWaitingQueue(void) {
    s32 enabled;
    u32 i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; i++) {
        if (WaitingQueue_802EFA18[i].next != &WaitingQueue_802EFA18[i]) {
            OSRestoreInterrupts(enabled);
            return 1;
        }
    }
    OSRestoreInterrupts(enabled);
    return 0;
}

s32 __DVDDequeueWaitingQueue(void **block) {
    s32 OSDisableInterrupts();
    M2C_UNK OSRestoreInterrupts();
    void **prev;
    void **next;

    OSDisableInterrupts();
    prev = (void **)block[1];
    next = (void **)block[0];
    if (prev == NULL || next == NULL) {
        OSRestoreInterrupts();
        return 0;
    }
    prev[0] = next;
    next[1] = prev;
    OSRestoreInterrupts();
    return 1;
}

void __DVDStoreErrorCode(u32 error) {
    u8 ErrorCode2Num(u32);
    u8 *__OSLockSramEx(void);
    M2C_UNK __OSUnlockSramEx(s32);
    u8 num;
    u32 hi;

    if (error == 0x01234567) {
        num = 0xFF;
    } else if (error == 0x01234568) {
        num = 0xFE;
    } else {
        hi = error >> 24;
        num = ErrorCode2Num(error & 0xFFFFFF);
        if (hi >= 6) {
            hi = 6;
        }
        num = num + hi * 30;
    }
    __OSLockSramEx()[0x24] = num;
    __OSUnlockSramEx(1);
}

s32 VISetPostRetraceCallback(s32 cb) {
    extern s32 PostCB_803C737C;
    s32 OSDisableInterrupts();
    M2C_UNK OSRestoreInterrupts(s32);
    s32 enabled;
    s32 old;

    old = PostCB_803C737C;
    enabled = OSDisableInterrupts();
    PostCB_803C737C = cb;
    OSRestoreInterrupts(enabled);
    return old;
}

u8 *getTiming_800742A0(u32 mode) {
    extern u8 timing_800D5850[][0x26];
    u8 *timing = timing_800D5850[0];

    switch (mode) {
    case 0:
        return timing;
    case 1:
        return timing + 0x26;
    case 2:
        return timing + 0x4C;
    case 3:
        return timing + 0x72;
    case 20:
        return timing;
    case 21:
        return timing + 0x26;
    case 4:
        return timing + 0x98;
    case 5:
        return timing + 0xBE;
    case 6:
        return timing + 0xE4;
    case 7:
        return timing + 0x10A;
    case 8:
        return timing + 0x4C;
    case 9:
        return timing + 0x72;
    default:
        return NULL;
    }
}

typedef struct VITiming {
    u8 unk0[0x18];
    u16 nhlines;
    u16 hlw;
} VITiming;

extern VITiming *CurrTiming_803C73A8;

static u32 getCurrentHalfLine(void) {
    u32 hcount;
    u32 vcount0;
    u32 vcount;

    vcount = ((volatile u16 *)0xCC002000)[22] & 0x7FF;
    do {
        vcount0 = vcount;
        hcount = ((volatile u16 *)0xCC002000)[23] & 0x7FF;
        vcount = ((volatile u16 *)0xCC002000)[22] & 0x7FF;
    } while (vcount0 != vcount);
    return ((vcount - 1) << 1) + ((hcount - 1) / CurrTiming_803C73A8->hlw);
}

static u32 getCurrentFieldEvenOdd(void) {
    u32 hcount;
    u32 vcount0;
    u32 vcount;

    vcount = ((volatile u16 *)0xCC002000)[22] & 0x7FF;
    do {
        vcount0 = vcount;
        hcount = ((volatile u16 *)0xCC002000)[23] & 0x7FF;
        vcount = ((volatile u16 *)0xCC002000)[22] & 0x7FF;
    } while (vcount0 != vcount);
    return ((((vcount - 1) << 1) + ((hcount - 1) / CurrTiming_803C73A8->hlw)) < CurrTiming_803C73A8->nhlines) ? 1 : 0;
}

u32 VIGetCurrentLine(void) {
    void OSRestoreInterrupts(s32);
    u32 halfLine;
    VITiming *tm;
    s32 enabled;

    tm = CurrTiming_803C73A8;
    enabled = OSDisableInterrupts();
    halfLine = getCurrentHalfLine();
    OSRestoreInterrupts(enabled);
    if (halfLine >= tm->nhlines) {
        halfLine -= tm->nhlines;
    }
    return halfLine >> 1;
}

u32 VIGetTvFormat(void) {
    void OSRestoreInterrupts(s32);
    extern s32 CurrTvMode_803C73AC;
    u32 format;
    s32 enabled;

    enabled = OSDisableInterrupts();
    switch (CurrTvMode_803C73AC) {
    case 0:
    case 3:
        format = 0;
        break;
    case 1:
    case 4:
        format = 1;
        break;
    case 5:
    case 2:
        format = CurrTvMode_803C73AC;
        break;
    }
    OSRestoreInterrupts(enabled);
    return format;
}

void PADOriginUpdateCallback(s32 chan, u32 error, void *context) {
    void OSSetWirelessID(s32, u16);
    void SIDisablePolling(u32);
    void UpdateOrigin(s32);
    void OSRestoreInterrupts(s32);
    extern u32 CheckingBits_803C73CC;
    extern u32 EnabledBits_803C73BC;
    extern u32 PendingBits_803C73D0;
    extern u32 WaitingBits_803C73C8;
    u32 chanBit;
    s32 enabled;

    chanBit = 0x80000000 >> chan;
    if (!(EnabledBits_803C73BC & chanBit)) {
        return;
    }
    if (!(error & 0xF)) {
        UpdateOrigin(chan);
    }
    if (error & 8) {
        enabled = OSDisableInterrupts();
        SIDisablePolling(chanBit);
        EnabledBits_803C73BC &= ~chanBit;
        WaitingBits_803C73C8 &= ~chanBit;
        CheckingBits_803C73CC &= ~chanBit;
        PendingBits_803C73D0 &= ~chanBit;
        OSSetWirelessID(chan, 0);
        OSRestoreInterrupts(enabled);
    }
}

void PADSetSpec(s32 spec) {
    extern void (*MakeStatus_803C6DD8)(s32, void *, u32 *);
    void SPEC0_MakeStatus(s32, void *, u32 *);
    void SPEC1_MakeStatus(s32, void *, u32 *);
    void SPEC2_MakeStatus_80076F6C(s32, void *, u32 *);
    extern s32 Spec_803C6DD4;
    extern s32 __PADSpec;

    __PADSpec = 0;
    switch (spec) {
    case 0:
        MakeStatus_803C6DD8 = SPEC0_MakeStatus;
        break;
    case 1:
        MakeStatus_803C6DD8 = SPEC1_MakeStatus;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        MakeStatus_803C6DD8 = SPEC2_MakeStatus_80076F6C;
        break;
    }
    Spec_803C6DD4 = spec;
}

static void SamplingHandler(s16 interrupt, void *context) {
    M2C_UNK OSClearContext(void *);
    M2C_UNK OSSetCurrentContext(void *);
    extern void (*SamplingCallback_803C73D4)(void);
    u32 exceptionContext[0x2C8 / 4];

    if (SamplingCallback_803C73D4) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        SamplingCallback_803C73D4();
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
    }
}

void SIControlSteering(s32 chan, u32 control, s32 level) {
    s32 SISetCommand(s32 chan, u32 command);
    s32 SITransferCommands(void);
    extern u32 __SISteeringEnableBits;
    s32 enabled;
    u32 command;

    control &= 0x600;
    if (level <= -128) {
        command = 0;
    } else if (level >= 128) {
        command = 0x100;
    } else {
        command = level + 128;
    }
    command = (command | control) & 0x7FF;
    enabled = OSDisableInterrupts();
    if (__SISteeringEnableBits & (0x80000000 >> chan)) {
        SISetCommand(chan, command | 0x300000);
        SITransferCommands();
    }
    OSRestoreInterrupts(enabled);
}

s32 EXIClearInterrupts(s32 chan, s32 exi, s32 tc, s32 ext) {
    u32 cpr;
    u32 prev;

    prev = cpr = ((volatile u32 *)0xCC006800)[chan * 5];
    cpr &= 0x7F5;
    if (exi) {
        cpr |= 2;
    }
    if (tc) {
        cpr |= 8;
    }
    if (ext) {
        cpr |= 0x800;
    }
    ((volatile u32 *)0xCC006800)[chan * 5] = cpr;
    return prev;
}

s32 EXIProbeEx(s32 chan) {
    if (EXIProbe(chan)) {
        return 1;
    } else if (((s32 *)0x800030C0)[chan]) {
        return 0;
    } else {
        return -1;
    }
}

s32 EXIDetach(s32 arg0) {
    M2C_UNK __OSMaskInterrupts(u32, s32);
    extern M2C_UNK Ecb_80309980;
    s32 temp_r30;
    s32 temp_r4;
    s32 temp_r4_2;
    void *temp_r31;

    temp_r4 = arg0 << 6;
    temp_r31 = &Ecb_80309980 + temp_r4;
    temp_r30 = OSDisableInterrupts(&Ecb_80309980, temp_r4);
    if (!(M2C_FIELD(temp_r31, s32 *, 0xC) & 8)) {
        OSRestoreInterrupts(temp_r30);
        return 1;
    }
    if ((M2C_FIELD(temp_r31, s32 *, 0xC) & 0x10) && ((u32) M2C_FIELD(temp_r31, u32 *, 0x18) == 0U)) {
        OSRestoreInterrupts(temp_r30);
        return 0;
    }
    temp_r4_2 = M2C_FIELD(temp_r31, s32 *, 0xC) & 0xFFFFFFF7;
    M2C_FIELD(temp_r31, s32 *, 0xC) = temp_r4_2;
    __OSMaskInterrupts(0x500000U >> (arg0 * 3), temp_r4_2);
    OSRestoreInterrupts(temp_r30);
    return 1;
}

void EXIIntrruptHandler(s16 interrupt, void *context) {
    extern u8 Ecb_80309980[3][0x40];
    void OSClearContext(void *context);
    void OSSetCurrentContext(void *context);
    typedef void (*EXICallback)(s32 chan, void *context);
    s32 chan;
    EXICallback callback;
    u8 exceptionContext[0x2C8];

    chan = (interrupt - 9) / 3;
    ((volatile u32 *)0xCC006800)[chan * 5] = ((volatile u32 *)0xCC006800)[chan * 5] & 0x7F5 | 2;
    callback = *(EXICallback *)(Ecb_80309980[chan] + 0);
    if (callback) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        callback(chan, context);
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
    }
}

s32 EXIGetID(s32 chan, u32 dev, u32 *id);
s32 OSGetConsoleType(void);
s32 EXIGetState(s32 chan) {
    extern u8 Ecb_80309980[3][0x40];
    return *(s32 *)(Ecb_80309980[chan] + 0xC);
}

s32 InitializeUART(void) {
    s32 OSGetConsoleType();
    extern s32 BarnacleEnabled_803C75C4;
    extern s32 Chan_803C75B8;
    extern s32 Dev_803C75BC;
    extern s32 Enabled_803C75C0;
    if ((u32) (BarnacleEnabled_803C75C4 + 0x5A010000) == 0x5AU) {
        return 0;
    }
    if (!(OSGetConsoleType() & 0x10000000)) {
        Enabled_803C75C0 = 0;
        return 2;
    }
    Enabled_803C75C0 = 0xA5FF005A;
    Chan_803C75B8 = 0;
    Dev_803C75BC = 1;
    return 0;
}

static void SamplingHandler(s16 interrupt, void *context);
void *PADSetSamplingCallback(void *callback) {
    M2C_UNK SIRegisterPollingHandler(void *);
    M2C_UNK SIUnregisterPollingHandler(void *);
    extern void *SamplingCallback_803C73D4;
    void *prev;

    prev = SamplingCallback_803C73D4;
    SamplingCallback_803C73D4 = callback;
    if (callback) {
        SIRegisterPollingHandler(SamplingHandler);
    } else {
        SIUnregisterPollingHandler(SamplingHandler);
    }
    return prev;
}

BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL enabled;
    BOOL prev;

    enabled = OSDisableInterrupts();
    if (*(u8 *)0x800030E3 & 0x40) {
        prev = TRUE;
    } else {
        prev = FALSE;
    }
    *(u8 *)0x800030E3 &= ~0x40;
    if (disable) {
        *(u8 *)0x800030E3 |= 0x40;
    }
    OSRestoreInterrupts(enabled);
    return prev;
}

void AIInitDMA(u32 start_addr, u32 length) {
    BOOL old;

    old = OSDisableInterrupts();
    *(u16 *)0xCC005030 = (u16)((*(u16 *)0xCC005030 & ~0x3FF) | (start_addr >> 16));
    *(u16 *)0xCC005032 = (u16)((*(u16 *)0xCC005032 & ~0xFFE0) | (0xFFFF & start_addr));
    *(u16 *)0xCC005036 = (u16)((*(u16 *)0xCC005036 & ~0x7FFF) | (u16)((length >> 5) & 0xFFFF));
    OSRestoreInterrupts(old);
}

void AIStartDMA(void) {
    M2C_FIELD((void *)0xCC005000, u16 *, 0x36) = (u16) (M2C_FIELD((void *)0xCC005000, u16 *, 0x36) | 0x8000);
}

s32 AIGetStreamPlayState(void) {
    return *(s32 *)0xCC006C00 & 1;
}

s32 AIGetDSPSampleRate(void) {
    return (((u32) *(u32 *)0xCC006C00 >> 6U) & 1) ^ 1;
}

s32 AIGetStreamSampleRate(void) {
    return ((u32) *(u32 *)0xCC006C00 >> 1U) & 1;
}

u8 AIGetStreamVolLeft(void) {
    return (u8) *(s32 *)0xCC006C04;
}

void AISetStreamVolRight(u8 vol) {
    __AIRegs[1] = (__AIRegs[1] & ~0xFF00) | ((vol & 0xFF) << 8);
}

u8 AIGetStreamVolRight(void) {
    return (u8) ((u32) *(u32 *)0xCC006C04 >> 8U);
}

void AIReset(void) {
    extern s32 __AI_init_flag_803C73F0;
    __AI_init_flag_803C73F0 = 0;
}

void __AISHandler(s16 interrupt, void *context) {
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    extern void (*__AIS_Callback_803C73E0)(u32);
    u32 exceptionContext[0x2C8 / 4];

    __AIRegs[0] |= 8;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AIS_Callback_803C73E0) {
        __AIS_Callback_803C73E0(__AIRegs[2]);
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

void __AIDHandler(s16 interrupt, void *context) {
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    void __AICallbackStackSwitch(void *cb);
    extern void (*__AID_Callback_803C73E4)(void);
    extern u8 *__CallbackStack_803C73E8;
    u32 exceptionContext[0x2C8 / 4];
    u16 tmp;

    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0xA0) | 8);
    __DSPRegs[5] = tmp;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AID_Callback_803C73E4) {
        if (__CallbackStack_803C73E8) {
            __AICallbackStackSwitch(__AID_Callback_803C73E4);
        } else {
            __AID_Callback_803C73E4();
        }
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

#pragma push
#pragma dont_inline on
u32 ARInit(u32 *stack_index_addr, u32 num_entries) {
    void OSRestoreInterrupts(s32);
    void __OSUnmaskInterrupts(u32);
    void __ARChecksize(void);
    void __ARHandler(s16, void *);
    extern s32 __AR_init_flag_803C743C;
    extern void *__AR_Callback_803C7420;
    extern u32 __AR_FreeBlocks_803C7434;
    extern u32 __AR_StackPointer_803C7430;
    extern u32 *__AR_BlockLength_803C7438;
    s32 old;

    if (__AR_init_flag_803C743C == 1) {
        return 0x4000;
    }
    old = OSDisableInterrupts();
    __AR_Callback_803C7420 = NULL;
    __OSSetInterruptHandler(6, __ARHandler);
    __OSUnmaskInterrupts(0x02000000);
    __AR_FreeBlocks_803C7434 = num_entries;
    __AR_StackPointer_803C7430 = 0x4000;
    __AR_BlockLength_803C7438 = stack_index_addr;
    __DSPRegs[13] = (u16)((__DSPRegs[13] & ~0xFF) | (__DSPRegs[13] & 0xFF));
    __ARChecksize();
    __AR_init_flag_803C743C = 1;
    OSRestoreInterrupts(old);
    return __AR_StackPointer_803C7430;
}
#pragma pop
void ARReset(void) {
    extern s32 __AR_init_flag_803C743C;
    __AR_init_flag_803C743C = 0;
}

s32 ARGetBaseAddress(void) {
    return 0x4000;
}

s32 ARGetSize(void) {
    extern s32 __AR_Size_803C7424;
    return __AR_Size_803C7424;
}

void __ARHandler(s16 interrupt, void *context) {
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    extern void (*__AR_Callback_803C7420)(void);
    u32 exceptionContext[0x2C8 / 4];
    u16 tmp;

    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0x88) | 0x20);
    __DSPRegs[5] = tmp;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AR_Callback_803C7420) {
        __AR_Callback_803C7420();
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

void __ARQCallbackHack(void) {

}

void __ARQInterruptServiceRoutine(void) {
    typedef struct ARQRequest {
        struct ARQRequest *next;
        u32 owner;
        u32 type;
        u32 priority;
        u32 source;
        u32 dest;
        u32 length;
        void (*callback)(u32);
    } ARQRequest;
    extern ARQRequest *__ARQRequestQueueHi_803C7440;
    extern u32 __ARQRequestPendingHi_803C7450;
    extern u32 __ARQRequestPendingLo_803C7454;
    extern void (*__ARQCallbackHi_803C7458)(u32);
    extern void (*__ARQCallbackLo_803C745C)(u32);
    void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);
    void __ARQServiceQueueLo(void);

    if (__ARQCallbackHi_803C7458) {
        __ARQCallbackHi_803C7458(__ARQRequestPendingHi_803C7450);
        __ARQRequestPendingHi_803C7450 = 0;
        __ARQCallbackHi_803C7458 = NULL;
    } else if (__ARQCallbackLo_803C745C) {
        __ARQCallbackLo_803C745C(__ARQRequestPendingLo_803C7454);
        __ARQRequestPendingLo_803C7454 = 0;
        __ARQCallbackLo_803C745C = NULL;
    }
    if (__ARQRequestQueueHi_803C7440) {
        if (__ARQRequestQueueHi_803C7440->type == 0) {
            ARStartDMA(__ARQRequestQueueHi_803C7440->type, __ARQRequestQueueHi_803C7440->source, __ARQRequestQueueHi_803C7440->dest, __ARQRequestQueueHi_803C7440->length);
        } else {
            ARStartDMA(__ARQRequestQueueHi_803C7440->type, __ARQRequestQueueHi_803C7440->dest, __ARQRequestQueueHi_803C7440->source, __ARQRequestQueueHi_803C7440->length);
        }
        __ARQCallbackHi_803C7458 = __ARQRequestQueueHi_803C7440->callback;
        __ARQRequestPendingHi_803C7450 = (u32)__ARQRequestQueueHi_803C7440;
        __ARQRequestQueueHi_803C7440 = __ARQRequestQueueHi_803C7440->next;
    }
    if (__ARQRequestPendingHi_803C7450 == 0) {
        __ARQServiceQueueLo();
    }
}

void ARQInit(void) {
    extern s32 __ARQ_init_flag_803C7464;
    extern u32 __ARQRequestQueueHi_803C7440;
    extern u32 __ARQRequestQueueLo_803C7448;
    extern u32 __ARQChunkSize_803C7460;
    extern u32 __ARQRequestPendingHi_803C7450;
    extern u32 __ARQRequestPendingLo_803C7454;
    extern u32 __ARQCallbackHi_803C7458;
    extern u32 __ARQCallbackLo_803C745C;
    void __ARQInterruptServiceRoutine(void);
    void *ARRegisterDMACallback(void *callback);

    if (__ARQ_init_flag_803C7464 == 1) {
        return;
    }
    __ARQRequestQueueHi_803C7440 = __ARQRequestQueueLo_803C7448 = 0;
    __ARQChunkSize_803C7460 = 0x1000;
    ARRegisterDMACallback(__ARQInterruptServiceRoutine);
    __ARQRequestPendingHi_803C7450 = 0;
    __ARQRequestPendingLo_803C7454 = 0;
    __ARQCallbackHi_803C7458 = 0;
    __ARQCallbackLo_803C745C = 0;
    __ARQ_init_flag_803C7464 = 1;
}

void ARQReset(void) {
    extern s32 __ARQ_init_flag_803C7464;
    __ARQ_init_flag_803C7464 = 0;
}

void ARQFlushQueue(void) {
    extern u32 __ARQCallbackLo_803C745C;
    extern s32 __ARQRequestPendingLo_803C7454;
    extern s32 __ARQRequestQueueHi_803C7440;
    extern s32 __ARQRequestQueueLo_803C7448;
    extern s32 __ARQRequestTailHi_803C7444;
    extern s32 __ARQRequestTailLo_803C744C;
    s32 level;
    level = OSDisableInterrupts();
    __ARQRequestQueueHi_803C7440 = 0;
    __ARQRequestTailHi_803C7444 = 0;
    __ARQRequestQueueLo_803C7448 = 0;
    __ARQRequestTailLo_803C744C = 0;
    if (__ARQCallbackLo_803C745C == 0) {
        __ARQRequestPendingLo_803C7454 = 0;
    }
    OSRestoreInterrupts(level);
}

void AXInit(void) {
    M2C_UNK __AXAllocInit();
    M2C_UNK __AXVPBInit();
    M2C_UNK __AXSPBInit();
    M2C_UNK __AXAuxInit();
    M2C_UNK __AXClInit();
    M2C_UNK __AXOutInit();
    __AXAllocInit();
    __AXVPBInit();
    __AXSPBInit();
    __AXAuxInit();
    __AXClInit();
    __AXOutInit();
}

void AXQuit(void) {
    M2C_UNK __AXAllocQuit();
    M2C_UNK __AXVPBQuit();
    M2C_UNK __AXSPBQuit();
    M2C_UNK __AXAuxQuit();
    M2C_UNK __AXClQuit();
    M2C_UNK __AXOutQuit();
    __AXAllocQuit();
    __AXVPBQuit();
    __AXSPBQuit();
    __AXAuxQuit();
    __AXClQuit();
    __AXOutQuit();
}

#pragma push
#pragma peephole off
void *__AXGetStackHead(u32 priority) {
    extern void *__AXStackHead_802EFC40[32];
    void **head = &__AXStackHead_802EFC40[priority];
    return *head;
}
#pragma pop
void __AXServiceCallbackStack(void) {
    void *__AXPopCallbackStack();
    M2C_UNK __AXPushFreeStack(void *);
    M2C_UNK __AXRemoveFromStack(void *);
    void *pvpb;

    for (pvpb = __AXPopCallbackStack(); pvpb != NULL; pvpb = __AXPopCallbackStack()) {
        if (M2C_FIELD(pvpb, u32 *, 0xC) != 0) {
            if (M2C_FIELD(pvpb, void (**)(void *), 0x10) != NULL) {
                M2C_FIELD(pvpb, void (**)(void *), 0x10)(pvpb);
            }
            __AXRemoveFromStack(pvpb);
            __AXPushFreeStack(pvpb);
        }
    }
}

void __AXAllocInit(void) {
    extern void *__AXCallbackStack_803C7468;
    extern void *__AXStackHead_802EFC40[32];
    extern void *__AXStackTail_802EFCC0[32];
    u32 i;

    __AXCallbackStack_803C7468 = NULL;
    for (i = 0; i < 32; i++) {
        __AXStackTail_802EFCC0[i] = NULL;
        __AXStackHead_802EFC40[i] = NULL;
    }
}

void __AXAllocQuit(void) {
    extern void *__AXCallbackStack_803C7468;
    extern void *__AXStackHead_802EFC40[32];
    extern void *__AXStackTail_802EFCC0[32];
    u32 i;

    __AXCallbackStack_803C7468 = NULL;
    for (i = 0; i < 32; i++) {
        __AXStackTail_802EFCC0[i] = NULL;
        __AXStackHead_802EFC40[i] = NULL;
    }
}

void __AXPushFreeStack(void *p) {
    extern void *__AXStackHead_802EFC40[32];
    M2C_FIELD(p, void **, 0) = __AXStackHead_802EFC40[0];
    __AXStackHead_802EFC40[0] = p;
    M2C_FIELD(p, u32 *, 0xC) = 0;
}

void __AXPushCallbackStack(void *arg0) {
    extern void *__AXCallbackStack_803C7468;
    M2C_FIELD(arg0, void **, 8) = (void *) __AXCallbackStack_803C7468;
    __AXCallbackStack_803C7468 = arg0;
}

void __AXAuxQuit(void) {
    extern s32 __AXCallbackAuxA_803C7470;
    extern s32 __AXCallbackAuxB_803C7474;
    __AXCallbackAuxA_803C7470 = 0;
    __AXCallbackAuxB_803C7474 = 0;
}

void __AXGetAuxAInput(u32 **out) {
    extern void *__AXCallbackAuxA_803C7470;
    extern u32 __AXAuxDspWritePosition_803C7490;
    extern u32 __AXBufferAuxA_802EFD40[3][480];
    if (__AXCallbackAuxA_803C7470) {
        *out = __AXBufferAuxA_802EFD40[__AXAuxDspWritePosition_803C7490];
    } else {
        *out = NULL;
    }
}

void __AXGetAuxAInputDpl2(u32 **out) {
    extern u32 __AXAuxDspWritePosition_803C7490;
    extern u32 __AXBufferAuxB_802F13C0[3][480];
    *out = &__AXBufferAuxB_802F13C0[__AXAuxDspWritePosition_803C7490][320];
}

void __AXGetAuxAOutput(u32 **out) {
    extern u32 __AXAuxDspReadPosition_803C7494;
    extern u32 __AXBufferAuxA_802EFD40[3][480];
    *out = __AXBufferAuxA_802EFD40[__AXAuxDspReadPosition_803C7494];
}

void __AXGetAuxAOutputDpl2R(u32 **out) {
    extern u32 __AXAuxDspReadPosition_803C7494;
    extern u32 __AXBufferAuxA_802EFD40[3][480];
    *out = &__AXBufferAuxA_802EFD40[__AXAuxDspReadPosition_803C7494][160];
}

void __AXGetAuxAOutputDpl2Ls(u32 **out) {
    extern u32 __AXAuxDspReadPosition_803C7494;
    extern u32 __AXBufferAuxA_802EFD40[3][480];
    *out = &__AXBufferAuxA_802EFD40[__AXAuxDspReadPosition_803C7494][320];
}

void __AXGetAuxAOutputDpl2Rs(u32 **out) {
    extern u32 __AXAuxDspReadPosition_803C7494;
    extern u32 __AXBufferAuxB_802F13C0[3][480];
    *out = &__AXBufferAuxB_802F13C0[__AXAuxDspReadPosition_803C7494][320];
}

void __AXGetAuxBInput(u32 **out) {
    extern void *__AXCallbackAuxB_803C7474;
    extern u32 __AXAuxDspWritePosition_803C7490;
    extern u32 __AXBufferAuxB_802F13C0[3][480];
    if (__AXCallbackAuxB_803C7474) {
        *out = __AXBufferAuxB_802F13C0[__AXAuxDspWritePosition_803C7490];
    } else {
        *out = NULL;
    }
}

void __AXGetAuxBOutput(u32 **out) {
    extern u32 __AXAuxDspReadPosition_803C7494;
    extern u32 __AXBufferAuxB_802F13C0[3][480];
    *out = __AXBufferAuxB_802F13C0[__AXAuxDspReadPosition_803C7494];
}

void __AXGetAuxBForDPL2(u32 **out) {
    extern u32 __AXAuxDspWritePositionDpl2_803C7498;
    extern u32 __AXBufferAuxB_802F13C0[3][480];
    *out = __AXBufferAuxB_802F13C0[__AXAuxDspWritePositionDpl2_803C7498];
}

void __AXGetAuxBOutputDPL2(u32 **out) {
    extern u32 __AXAuxDspReadPositionDpl2_803C749C;
    extern u32 __AXBufferAuxB_802F13C0[3][480];
    *out = __AXBufferAuxB_802F13C0[__AXAuxDspReadPositionDpl2_803C749C];
}

void AXRegisterAuxACallback(s32 arg0, s32 arg1) {
    extern s32 __AXCallbackAuxA_803C7470;
    extern s32 __AXContextAuxA_803C7478;
    __AXCallbackAuxA_803C7470 = arg0;
    __AXContextAuxA_803C7478 = arg1;
}

void AXRegisterAuxBCallback(s32 arg0, s32 arg1) {
    extern s32 __AXCallbackAuxB_803C7474;
    extern s32 __AXContextAuxB_803C747C;
    __AXCallbackAuxB_803C7474 = arg0;
    __AXContextAuxB_803C747C = arg1;
}

s32 __AXGetCommandListCycles(void) {
    extern s32 __AXCommandListCycles_803C74B0;
    return __AXCommandListCycles_803C74B0;
}

void __AXClQuit(void) {

}

void __AXDSPInitCallback(void) {
    extern s32 __AXDSPInitFlag_803C74D4;
    __AXDSPInitFlag_803C74D4 = 1;
}

void __AXDSPDoneCallback(void) {
    extern s32 __AXDSPDoneFlag_803C74D8;
    __AXDSPDoneFlag_803C74D8 = 1;
}

void __AXOutInitDSP(void) {
    extern u8 axDspSlave[];
    extern u16 axDspSlaveLength;
    extern u8 __AXOutBuffer_802F3040[];
    extern s32 __AXDSPInitFlag_803C74D4;
    extern s32 __AXDSPDoneFlag_803C74D8;
    void __AXDSPInitCallback(void *task);
    void __AXDSPResumeCallback(void *task);
    void __AXDSPDoneCallback(void *task);
    s32 DSPCheckInit(void);
    void DSPInit(void);
    void *DSPAddTask(void *task);

    M2C_FIELD(__AXOutBuffer_802F3040, void **, 0x7C4) = axDspSlave;
    M2C_FIELD(__AXOutBuffer_802F3040, u32 *, 0x7C8) = axDspSlaveLength;
    M2C_FIELD(__AXOutBuffer_802F3040, u32 *, 0x7CC) = 0;
    M2C_FIELD(__AXOutBuffer_802F3040, void **, 0x7D0) = __AXOutBuffer_802F3040 + 0x808;
    M2C_FIELD(__AXOutBuffer_802F3040, u32 *, 0x7D4) = 0x2000;
    M2C_FIELD(__AXOutBuffer_802F3040, u32 *, 0x7D8) = 0;
    M2C_FIELD(__AXOutBuffer_802F3040, u16 *, 0x7DC) = 0x10;
    M2C_FIELD(__AXOutBuffer_802F3040, u16 *, 0x7DE) = 0x30;
    M2C_FIELD(__AXOutBuffer_802F3040, void **, 0x7E0) = __AXDSPInitCallback;
    M2C_FIELD(__AXOutBuffer_802F3040, void **, 0x7E4) = __AXDSPResumeCallback;
    M2C_FIELD(__AXOutBuffer_802F3040, void **, 0x7E8) = __AXDSPDoneCallback;
    M2C_FIELD(__AXOutBuffer_802F3040, u32 *, 0x7EC) = 0;
    M2C_FIELD(__AXOutBuffer_802F3040, u32 *, 0x7BC) = 0;
    __AXDSPInitFlag_803C74D4 = 0;
    __AXDSPDoneFlag_803C74D8 = 0;
    if (!DSPCheckInit()) {
        DSPInit();
    }
    DSPAddTask(__AXOutBuffer_802F3040 + 0x7B8);
    while (__AXDSPInitFlag_803C74D4 == 0) {
    }
}

void __AXOutQuit(void) {
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    void AIStopDMA(void);
    void DSPHalt(void);
    u32 DSPGetDMAStatus(void);
    void DSPReset(void);
    extern void *__AXUserFrameCallback_803C74D0;
    s32 old;

    old = OSDisableInterrupts();
    __AXUserFrameCallback_803C74D0 = NULL;
    AIStopDMA();
    DSPHalt();
    while (DSPGetDMAStatus()) {
    }
    DSPReset();
    OSRestoreInterrupts(old);
}

void AXRegisterCallback(s32 arg0) {
    extern s32 __AXUserFrameCallback_803C74D0;
    __AXUserFrameCallback_803C74D0 = arg0;
}

void *__AXGetStudio(void) {
    extern u8 __AXStudio_802F7860[];
    return __AXStudio_802F7860;
}

void __AXSPBInit(void) {
    extern s32 __AXSpbAAL_803C74EC;
    extern s32 __AXSpbAAR_803C74F0;
    extern s32 __AXSpbAAS_803C74F4;
    extern s32 __AXSpbABL_803C74F8;
    extern s32 __AXSpbABR_803C74FC;
    extern s32 __AXSpbABS_803C7500;
    extern s32 __AXSpbAL_803C74E0;
    extern s32 __AXSpbAR_803C74E4;
    extern s32 __AXSpbAS_803C74E8;
    __AXSpbABS_803C7500 = 0;
    __AXSpbABR_803C74FC = 0;
    __AXSpbABL_803C74F8 = 0;
    __AXSpbAAS_803C74F4 = 0;
    __AXSpbAAR_803C74F0 = 0;
    __AXSpbAAL_803C74EC = 0;
    __AXSpbAS_803C74E8 = 0;
    __AXSpbAR_803C74E4 = 0;
    __AXSpbAL_803C74E0 = 0;
}

void __AXSPBQuit(void) {

}

void __AXDepopVoice(void *pvpb) {
    extern s32 __AXSpbAL_803C74E0;
    extern s32 __AXSpbAR_803C74E4;
    extern s32 __AXSpbAS_803C74E8;
    extern s32 __AXSpbAAL_803C74EC;
    extern s32 __AXSpbAAR_803C74F0;
    extern s32 __AXSpbAAS_803C74F4;
    extern s32 __AXSpbABL_803C74F8;
    extern s32 __AXSpbABR_803C74FC;
    extern s32 __AXSpbABS_803C7500;

    __AXSpbAL_803C74E0 += M2C_FIELD(pvpb, s16 *, 0x52);
    __AXSpbAAL_803C74EC += M2C_FIELD(pvpb, s16 *, 0x54);
    __AXSpbABL_803C74F8 += M2C_FIELD(pvpb, s16 *, 0x56);
    __AXSpbAR_803C74E4 += M2C_FIELD(pvpb, s16 *, 0x58);
    __AXSpbAAR_803C74F0 += M2C_FIELD(pvpb, s16 *, 0x5A);
    __AXSpbABR_803C74FC += M2C_FIELD(pvpb, s16 *, 0x5C);
    __AXSpbAS_803C74E8 += M2C_FIELD(pvpb, s16 *, 0x5E);
    __AXSpbAAS_803C74F4 += M2C_FIELD(pvpb, s16 *, 0x60);
    __AXSpbABS_803C7500 += M2C_FIELD(pvpb, s16 *, 0x62);
}

s32 __AXGetNumVoices(void) {
    extern s32 __AXNumVoices_803C7510;
    return __AXNumVoices_803C7510;
}

void *__AXGetPBs(void) {
    extern u8 __AXPB_802F78A0[];
    return __AXPB_802F78A0;
}

void __AXSetPBDefault(void *p) {
    u8 *pb = p;
    *(u16 *)(pb + 0x146) = 0;
    *(u16 *)(pb + 0x16E) = 0;
    *(u32 *)(pb + 0x1C) = 0xA4;
    *(u32 *)(pb + 0x28) = 0;
    *(u32 *)(pb + 0x24) = 0;
    *(u8 **)(pb + 0x30) = pb + 0x34;
    *(u16 *)(pb + 0x184) = 0;
    *(u16 *)(pb + 0x182) = 0;
    *(u16 *)(pb + 0x180) = 0;
    *(u16 *)(pb + 0x17E) = 0;
    *(u16 *)(pb + 0x17C) = 0;
}

void __AXVPBQuit(void) {

}

void *__AXGetCurrentProfile(void) {
    extern u32 __AXCurrentProfile_803C7520;
    extern u32 __AXMaxProfiles_803C751C;
    extern u32 __AXProfileInitialized_803C7524;
    extern u8 (*__AXProfile_803C7518)[0x38];
    u8 (*profile)[0x38];

    if (__AXProfileInitialized_803C7524) {
        profile = __AXProfile_803C7518 + __AXCurrentProfile_803C7520++;
        __AXCurrentProfile_803C7520 %= __AXMaxProfiles_803C751C;
        return profile;
    }
    return NULL;
}

void AXFXDelayInit(void *arg0) {
    M2C_UNK AXFXDelaySettings(void *);
    OSDisableInterrupts();
    M2C_FIELD(arg0, s32 *, 0x30) = 0;
    M2C_FIELD(arg0, s32 *, 0x34) = 0;
    M2C_FIELD(arg0, s32 *, 0x38) = 0;
    OSRestoreInterrupts();
    AXFXDelaySettings(arg0);
}

s32 AXFXDelayShutdown(void *delay) {
    extern void (*__AXFXFree)(void *);
    s32 OSDisableInterrupts();
    M2C_UNK OSRestoreInterrupts(s32);
    s32 enabled;

    enabled = OSDisableInterrupts();
    if (M2C_FIELD(delay, void **, 0x30) != NULL) {
        __AXFXFree(M2C_FIELD(delay, void **, 0x30));
    }
    if (M2C_FIELD(delay, void **, 0x34) != NULL) {
        __AXFXFree(M2C_FIELD(delay, void **, 0x34));
    }
    if (M2C_FIELD(delay, void **, 0x38) != NULL) {
        __AXFXFree(M2C_FIELD(delay, void **, 0x38));
    }
    OSRestoreInterrupts(enabled);
    return 1;
}

void *__AXFXAllocFunction_8007BC38(u32 size) {
    extern s32 __OSCurrHeap;
    void *OSAllocFromHeap(s32, u32);
    return OSAllocFromHeap(__OSCurrHeap, size);
}

void *__AXFXFreeFunction_8007BC60(u32 size) {
    extern s32 __OSCurrHeap;
    void *OSFreeToHeap(s32, u32);
    return OSFreeToHeap(__OSCurrHeap, size);
}

s32 DSPCheckMailToDSP(void) {
    return ((u16) *(u16 *)0xCC005000 >> 0xFU) & 1;
}

s32 DSPCheckMailFromDSP(void) {
    return ((u16) *(u16 *)0xCC005004 >> 0xFU) & 1;
}

u32 DSPReadMailFromDSP(void) {
    return (((volatile u16 *)0xCC005000)[2] << 16) | ((volatile u16 *)0xCC005000)[3];
}

void DSPSendMailToDSP(u32 mail) {
    volatile u16 *regs = (volatile u16 *)0xCC000000;
    regs[0x2800] = mail >> 16;
    regs[0x2801] = mail;
}

void __DSP_debug_printf(char *str, ...);
void DSPInit(void) {
    extern s32 __DSP_init_flag_803C7528;
    extern DSPTaskInfo *__DSP_tmp_task;
    extern void __DSPHandler(__OSInterrupt, void *);
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32);
    s32 oldInt;
    u16 reg;

    __DSP_debug_printf("DSPInit(): Build Date: %s %s\n", "Apr  5 2004", "04:15:32");
    if (__DSP_init_flag_803C7528 == 1) {
        return;
    }
    oldInt = OSDisableInterrupts();
    __OSSetInterruptHandler(7, __DSPHandler);
    __OSUnmaskInterrupts(0x01000000);
    reg = ((volatile u16 *)0xCC005000)[5];
    reg = (reg & ~0xA8) | 0x800;
    ((volatile u16 *)0xCC005000)[5] = reg;
    reg = ((volatile u16 *)0xCC005000)[5];
    reg = reg & ~0xAC;
    ((volatile u16 *)0xCC005000)[5] = reg;
    __DSP_tmp_task = NULL;
    __DSP_curr_task = NULL;
    __DSP_last_task = NULL;
    __DSP_first_task = NULL;
    __DSP_init_flag_803C7528 = 1;
    OSRestoreInterrupts(oldInt);
}

s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag_803C7528;
    return __DSP_init_flag_803C7528;
}

DSPTaskInfo *DSPAssertTask(DSPTaskInfo *task) {
    extern DSPTaskInfo *__DSP_rude_task;
    extern s32 __DSP_rude_task_pending;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32);
    s32 old;
    s32 old2;

    old = OSDisableInterrupts();
    if (__DSP_curr_task == task) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = 1;
        OSRestoreInterrupts(old);
        return task;
    }
    if (task->priority < __DSP_curr_task->priority) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = 1;
        if (__DSP_curr_task->state == 1) {
            old2 = OSDisableInterrupts();
            __DSPRegs[5] = (u16)((__DSPRegs[5] & ~0xA8) | 2);
            OSRestoreInterrupts(old2);
        }
        OSRestoreInterrupts(old);
        return task;
    }
    OSRestoreInterrupts(old);
    return NULL;
}

void DSPReset(void) {
    extern s32 __DSP_init_flag_803C7528;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32);
    s32 oldInt;
    u16 reg;

    oldInt = OSDisableInterrupts();
    reg = ((volatile u16 *)0xCC005000)[5];
    ((volatile u16 *)0xCC005000)[5] = (reg & ~0xA8) | 0x801;
    __DSP_init_flag_803C7528 = 0;
    OSRestoreInterrupts(oldInt);
}

s32 DSPGetDMAStatus(void) {
    return *(u16 *)0xCC00500A & 0x200;
}

void __DSP_debug_printf(char *str, ...) {
}

void __DSP_insert_task(DSPTaskInfo *task) {
    DSPTaskInfo *temp;

    if (__DSP_first_task == NULL) {
        __DSP_first_task = __DSP_last_task = __DSP_curr_task = task;
        task->next = task->prev = NULL;
    } else {
        temp = __DSP_first_task;
        while (temp) {
            if (task->priority < temp->priority) {
                task->prev = temp->prev;
                temp->prev = task;
                task->next = temp;
                if (task->prev == NULL) {
                    __DSP_first_task = task;
                } else {
                    task->prev->next = task;
                }
                break;
            }
            temp = temp->next;
        }
        if (temp == NULL) {
            __DSP_last_task->next = task;
            task->next = NULL;
            task->prev = __DSP_last_task;
            __DSP_last_task = task;
        }
    }
}

void __DSP_remove_task(DSPTaskInfo *task) {
    task->flags = 0;
    task->state = 3;
    if (__DSP_first_task == task) {
        if (task->next) {
            __DSP_first_task = task->next;
            task->next->prev = NULL;
        } else {
            __DSP_curr_task = NULL;
            __DSP_last_task = NULL;
            __DSP_first_task = NULL;
        }
    } else if (__DSP_last_task == task) {
        __DSP_last_task = task->prev;
        task->prev->next = NULL;
        __DSP_curr_task = __DSP_first_task;
    } else {
        __DSP_curr_task = task->next;
        task->prev->next = task->next;
        task->next->prev = task->prev;
    }
}

void __CARDDefaultApiCallback(void) {

}

void __CARDSyncCallback(s32 chan, s32 result) {
    void OSWakeupThread(void *queue);

    OSWakeupThread((u8 *)&__CARDBlock[chan] + 0x8C);
}

s32 EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 EXIUnlock(s32 chan);
s32 OSCancelAlarm(void *alarm);
s32 __CARDReadStatus(s32 chan, u8 *status);
s32 __CARDClearStatus(s32 chan);
static s32 Retry(s32 chan);
static void __CARDExiHandler(s32 chan) {
    CARDControl *card;
    CARDCallback callback;
    u8 status;
    s32 result;

    card = &__CARDBlock[chan];

    OSCancelAlarm(card->alarm);

    if (!card->attached) {
        return;
    }

    if (!EXILock(chan, 0, 0)) {
        result = -128;
        goto fatal;
    }

    if ((result = __CARDReadStatus(chan, &status)) < 0 || (result = __CARDClearStatus(chan)) < 0) {
        goto error;
    }

    if (status & 0x18) {
        result = -5;
    } else {
        result = 0;
    }
    if (result == -5 && --card->retry > 0) {
        result = Retry(chan);
        if (result >= 0) {
            return;
        }
        goto fatal;
    }

error:
    EXIUnlock(chan);

fatal:
    callback = card->exiCallback;
    if (callback) {
        card->exiCallback = 0;
        callback(chan, result);
    }
}

void TimeoutHandler(void *alarm, void *context) {
    void EXISetExiCallback(s32 chan, void *callback);
    s32 chan;
    CARDControl *card;
    CARDCallback callback;

    for (chan = 0; chan < 2; ++chan) {
        card = &__CARDBlock[chan];
        if (alarm == card->alarm) {
            break;
        }
    }
    if (!card->attached) {
        return;
    }
    EXISetExiCallback(chan, NULL);
    callback = card->exiCallback;
    if (callback) {
        card->exiCallback = NULL;
        callback(chan, -5);
    }
}

void CARDInit(void) {
    void DSPInit(void);
    void OSInitAlarm(void);
    void OSInitThreadQueue(void *queue);
    void OSCreateAlarm(void *alarm);
    void OSRegisterResetFunction(void *info);
    extern u8 ResetFunctionInfo_800D96D8[];
    s32 chan;
    CARDControl *card;

    if (*(u8 **)((u8 *)&__CARDBlock[0] + 0x10C) != NULL && *(u8 **)((u8 *)&__CARDBlock[1] + 0x10C) != NULL) {
        return;
    }
    DSPInit();
    OSInitAlarm();
    for (chan = 0; chan < 2; chan++) {
        card = &__CARDBlock[chan];
        card->result = -3;
        OSInitThreadQueue((u8 *)card + 0x8C);
        OSCreateAlarm(card->alarm);
    }
    __CARDSetDiskID((u8 *)0x80000000);
    OSRegisterResetFunction(ResetFunctionInfo_800D96D8);
}

void __CARDSetDiskID(u8 *id) {
    extern u8 __CARDDiskNone[32];

    *(u8 **)((u8 *)&__CARDBlock[0] + 0x10C) = id ? id : __CARDDiskNone;
    *(u8 **)((u8 *)&__CARDBlock[1] + 0x10C) = id ? id : __CARDDiskNone;
}

s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard) {
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    CARDControl *card;
    s32 enabled;
    s32 result;

    card = &__CARDBlock[chan];
    if (chan < 0 || chan >= 2 || *(u8 **)((u8 *)card + 0x10C) == NULL) {
        return -128;
    }
    enabled = OSDisableInterrupts();
    if (!card->attached) {
        result = -3;
    } else if (card->result == -1) {
        result = -1;
    } else {
        card->result = -1;
        result = 0;
        card->apiCallback = NULL;
        *pcard = card;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 __CARDSync(s32 chan) {
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    void OSSleepThread(void *queue);
    CARDControl *block;
    s32 result;
    s32 enabled;

    block = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    while ((result = (chan < 0 || chan >= 2) ? -128 : block->result) == -1) {
        OSSleepThread((u8 *)block + 0x8C);
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 OnReset_8007DB90(s32 arg0) {
    s32 CARDUnmount(M2C_UNK);
    if ((arg0 == 0) && ((CARDUnmount(0) == -1) || (CARDUnmount(1) == -1))) {
        return 0;
    }
    return 1;
}

s32 DummyLen(void) {
    u32 OSGetTick(void);
    u32 shift;
    u32 tries;
    s32 len;

    shift = 1;
    tries = 0;
    DummySRand(OSGetTick());
    len = (DummyRand() & 0x1F) + 1;
    while (len < 4 && tries < 10) {
        u32 seed = OSGetTick() << shift;
        shift++;
        if (shift > 16) {
            shift = 1;
        }
        DummySRand(seed);
        tries++;
        len = (DummyRand() & 0x1F) + 1;
    }
    if (len < 4) {
        len = 4;
    }
    return len;
}

void InitCallback(void *task) {
    void DSPSendMailToDSP(u32 mail);
    u32 DSPCheckMailToDSP(void);
    s32 chan;
    CARDControl *card;
    void *data;

    for (chan = 0; chan < 2; ++chan) {
        card = &__CARDBlock[chan];
        if ((void *)((u8 *)card + 0x30) == task) {
            break;
        }
    }
    data = card->workArea;
    DSPSendMailToDSP(0xFF000000);
    while (DSPCheckMailToDSP()) {
    }
    DSPSendMailToDSP((u32)data);
    while (DSPCheckMailToDSP()) {
    }
}

s32 __CARDRead(s32 chan, void *addr, u32 length, void *dst, CARDCallback callback) {
    s32 __CARDReadSegment(s32 chan, CARDCallback callback);
    void BlockReadCallback(s32 chan, s32 result);
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (!card->attached) {
        return -3;
    }
    card->xferCallback = callback;
    card->repeat = length / 512;
    card->addr = addr;
    card->offset = (s32)dst;
    return __CARDReadSegment(chan, BlockReadCallback);
}

s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, void *callback) {
    s32 __CARDWritePage(s32 chan, CARDCallback callback);
    void BlockWriteCallback(s32 chan, s32 result);
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (!card->attached) {
        return -3;
    }
    card->xferCallback = (CARDCallback)callback;
    card->repeat = (u32)length / 128;
    card->addr = (void *)addr;
    card->offset = (s32)dst;
    return __CARDWritePage(chan, BlockWriteCallback);
}

s32 __CARDGetFatBlock(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x88);
}

static void WriteCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u16 *fat;
    u16 *fatBack;

    card = &__CARDBlock[chan];
    if (result >= 0) {
        fat = (u16 *)((u8 *)card->workArea + 0x6000);
        fatBack = (u16 *)((u8 *)card->workArea + 0x8000);
        if (card->buffer == fat) {
            card->buffer = fatBack;
            memcpy(fatBack, fat, 0x2000);
        } else {
            card->buffer = fat;
            memcpy(fat, fatBack, 0x2000);
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, void *callback);
s32 __CARDPutControlBlock(void *card, s32 result);
static void WriteCallback(s32 chan, s32 result);

static void EraseCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u16 *fat;
    u32 addr;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }
    fat = (u16 *)__CARDGetFatBlock(card);
    addr = ((u32)fat - (u32)card->workArea) / 0x2000 * card->sectorSize;
    result = __CARDWrite(chan, addr, 0x2000, fat, WriteCallback);
    if (result >= 0) {
        return;
    }
error:
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDUpdateDir(s32 chan, CARDCallback callback) {
    void __CARDCheckSum(void *ptr, s32 length, u16 *checksum, u16 *checksumInv);
    void DCStoreRange(void *addr, u32 nBytes);
    s32 __CARDEraseSector(s32 chan, u32 addr, void *callback);
    void EraseCallback(s32 chan, s32 result);
    u8 *dir;
    CARDDirCheck *check;
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (!card->attached) {
        return -3;
    }
    dir = card->currentDir;
    check = __CARDGetDirCheck(dir);
    ++check->checkCode;
    __CARDCheckSum(dir, 0x1FFC, &check->checkSum, &check->checkSumInv);
    DCStoreRange(dir, 0x2000);
    card->eraseCallback = callback;
    return __CARDEraseSector(chan, card->sectorSize * ((u32)(dir - (u8 *)card->workArea) / 0x2000), EraseCallback);
}

s32 __CARDFreeBlock(s32 chan, u16 nBlock, CARDCallback callback) {
    s32 __CARDUpdateFatBlock(s32 chan, u16 *fat, CARDCallback callback);
    CARDControl *card;
    u16 *fat;
    u16 nextBlock;

    card = &__CARDBlock[chan];
    if (!card->attached) {
        return -3;
    }
    fat = *(u16 **)((u8 *)card + 0x88);
    while (nBlock != 0xFFFF) {
        if (nBlock < 5 || *(u16 *)((u8 *)card + 0x10) <= nBlock) {
            return -6;
        }
        nextBlock = fat[nBlock];
        fat[nBlock] = 0;
        nBlock = nextBlock;
        ++fat[3];
    }
    return __CARDUpdateFatBlock(chan, fat, callback);
}

s32 __CARDUpdateFatBlock(s32 chan, u16 *fat, void *callback) {
    void __CARDCheckSum(void *ptr, s32 length, u16 *checksum, u16 *checksumInv);
    void DCStoreRange(void *addr, u32 nBytes);
    s32 __CARDEraseSector(s32 chan, u32 addr, void *callback);
    void EraseCallback(s32 chan, s32 result);
    CARDControl *card;

    ++fat[2];
    card = &__CARDBlock[chan];
    __CARDCheckSum(fat + 2, 0x1FFC, fat, fat + 1);
    DCStoreRange(fat, 0x2000);
    *(void **)((u8 *)card + 0xD8) = callback;
    return __CARDEraseSector(chan, *(s32 *)((u8 *)card + 0xC) * ((u32)((u8 *)fat - *(u8 **)((u8 *)card + 0x80)) / 0x2000), EraseCallback);
}

void *__GXDefaultTexRegionCallback(void *obj, s32 mapID) {
    extern u8 *gx;
    s32 GXGetTexObjFmt(void *arg0);
    s32 fmt;
    u32 n;

    fmt = GXGetTexObjFmt(obj);
    if (fmt != 8 && fmt != 9 && fmt != 10) {
        n = (*(u32 *)(gx + 0x2C8))++;
        return gx + ((n & 7) * 0x10 + 0x208);
    }
    n = (*(u32 *)(gx + 0x2CC))++;
    return gx + ((n & 3) * 0x10 + 0x288);
}

#pragma push
#pragma peephole off
void *__GXDefaultTlutRegionCallback(u32 idx) {
    extern u8 *gx;
    void *ret;
    if (idx >= 0x14) {
        ret = 0;
    } else {
        ret = gx + (idx * 0x10 + 0x2D0);
    }
    return ret;
}
#pragma pop
void GXInitFifoBase(void *fifo, void *base, u32 size) {
    void GXInitFifoLimits(void *arg0, s32 arg1, s32 arg2);
    void GXInitFifoPtrs(void *fifo, void *readPtr, void *writePtr);
    u8 *f = fifo;
    *(void **)(f + 0x0) = base;
    *(void **)(f + 0x4) = (u8 *)base + size - 4;
    *(u32 *)(f + 0x8) = size;
    *(s32 *)(f + 0x1C) = 0;
    GXInitFifoLimits(fifo, size - 0x4000, (size >> 1) & ~0x1F);
    GXInitFifoPtrs(fifo, base, base);
}

s32 CARDMountAsync(s32 chan, void *workArea, CARDCallback detachCallback, CARDCallback attachCallback);
void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDSync(s32 chan);
s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
s32 __CARDFormatRegionAsync(s32 chan, u16 encode, CARDCallback callback);
u16 OSGetFontEncode(void);
static void DoUnmount(s32 chan, s32 result) {
    CARDControl *card;
    int enabled;

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    if (card->attached) {
        EXISetExiCallback(chan, 0);
        EXIDetach(chan);
        OSCancelAlarm(card->alarm);
        card->attached = 0;
        card->result = result;
        card->mountStep = 0;
    }
    OSRestoreInterrupts(enabled);
}

s32 CARDUnmount(s32 chan) {
    CARDControl *card;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    DoUnmount(chan, -3);
    return 0;
}

s32 CARDFormat(s32 chan) {
    s32 result = __CARDFormatRegionAsync(chan, OSGetFontEncode(), __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}

s32 __CARDCompareFileName(void *ent, const char *fileName) {
    char *entName;
    char c1;
    char c2;
    int n;

    entName = (char *)ent + 8;
    n = 32;
    while (0 <= --n) {
        if ((c1 = *entName++) != (c2 = *fileName++)) {
            return 0;
        } else if (c2 == '\0') {
            return 1;
        }
    }
    if (*fileName == '\0') {
        return 1;
    }
    return 0;
}

s32 __CARDAccess(void *card, u8 *ent) {
    extern u8 __CARDDiskNone[32];
    int memcmp(const void *a, const void *b, u32 n);
    u8 *diskID;

    if (ent[0] == 0xFF) {
        return -4;
    }
    if (M2C_FIELD(card, u8 **, 0x10C) == __CARDDiskNone
        || (memcmp(ent, M2C_FIELD(card, u8 **, 0x10C), 4) == 0
            && memcmp(ent + 4, M2C_FIELD(card, u8 **, 0x10C) + 4, 2) == 0)) {
        return 0;
    }
    return -10;
}

s32 __CARDIsPublic(void *arg0) {
    if ((u8) M2C_FIELD(arg0, u8 *, 0) == 0xFF) {
        return -4;
    }
    if (M2C_FIELD(arg0, u8 *, 0x34) & 4) {
        return 0;
    }
    return -0xA;
}

s32 CARDClose(CARDFileInfo *fileInfo) {
    CARDControl *card;
    s32 result;

    result = __CARDGetControlBlock(M2C_FIELD(fileInfo, s32 *, 0), &card);
    if (result < 0) {
        return result;
    }
    M2C_FIELD(fileInfo, s32 *, 0) = -1;
    return __CARDPutControlBlock(card, 0);
}

s32 __CARDGetDirBlock(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x84);
}

s32 __CARDIsOpened(void) {
    return 0;
}

s32 CARDCreate(s32 chan, const char *fileName, u32 size, CARDFileInfo *fileInfo) {
    s32 result = CARDCreateAsync(chan, fileName, size, fileInfo, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}

void GXInitFifoLimits(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
}

void __GXFifoReadEnable(void) {
    extern u8 *gx;
    extern volatile u16 *__cpReg;
    u32 *reg = (u32 *)(gx + 8);
    *reg = (*reg & ~1) | 1;
    __cpReg[1] = *(u32 *)(gx + 8);
}

void __GXFifoReadDisable(void) {
    extern u8 *gx;
    extern volatile u16 *__cpReg;
    u32 *reg = (u32 *)(gx + 8);
    *reg = *reg & ~1;
    __cpReg[1] = *(u32 *)(gx + 8);
}

void __GXFifoLink(u8 en) {
    extern u8 *gx;
    extern volatile u16 *__cpReg;
    u32 bit;
    if (en) {
        bit = 1;
    } else {
        bit = 0;
    }
    *(u32 *)(gx + 8) = (*(u32 *)(gx + 8) & ~0x10) | (bit << 4);
    __cpReg[1] = *(u32 *)(gx + 8);
}

void __GXWriteFifoIntEnable(u8 hiWatermarkEn, u8 loWatermarkEn) {
    extern u8 *gx;
    extern volatile u16 *__cpReg;
    u32 *reg;
    reg = (u32 *)(gx + 8);
    *reg = (*reg & ~4) | ((u32)hiWatermarkEn << 2);
    reg = (u32 *)(gx + 8);
    *reg = (*reg & ~8) | ((u32)loWatermarkEn << 3);
    __cpReg[1] = *(u32 *)(gx + 8);
}

void __GXWriteFifoIntReset(u8 hiWatermarkClr, u8 loWatermarkClr) {
    extern u8 *gx;
    extern u16 *__cpReg;

    *(u32 *)(gx + 0x10) = (*(u32 *)(gx + 0x10) & ~1) | hiWatermarkClr;
    *(u32 *)(gx + 0x10) = (*(u32 *)(gx + 0x10) & ~2) | (loWatermarkClr << 1);
    __cpReg[2] = *(u32 *)(gx + 0x10);
}

void __GXSetVCD(void) {
    extern void *const gx;
    void __GXXfVtxSpecs(void);

    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x50;
    GXWGFifo.u32 = M2C_FIELD(gx, u32 *, 0x14);
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = 0x60;
    GXWGFifo.u32 = M2C_FIELD(gx, u32 *, 0x18);
    __GXXfVtxSpecs();
}

#pragma push
#pragma scheduling off
void GXGetVtxDesc(u32 attr, s32 *type) {
    typedef struct GXDataVcd {
        u8 unk0[0x14];
        u32 vcdLo;
        u32 vcdHi;
        u8 unk1C[0x41C - 0x1C];
        u8 hasNrms;
        u8 hasBiNrms;
    } GXDataVcd;
    extern GXDataVcd *const gx;
    u32 cpType;

    switch (attr) {
    case 0:
        cpType = gx->vcdLo & 1;
        break;
    case 1:
        cpType = (gx->vcdLo >> 1) & 1;
        break;
    case 2:
        cpType = (gx->vcdLo >> 2) & 1;
        break;
    case 3:
        cpType = (gx->vcdLo >> 3) & 1;
        break;
    case 4:
        cpType = (gx->vcdLo >> 4) & 1;
        break;
    case 5:
        cpType = (gx->vcdLo >> 5) & 1;
        break;
    case 6:
        cpType = (gx->vcdLo >> 6) & 1;
        break;
    case 7:
        cpType = (gx->vcdLo >> 7) & 1;
        break;
    case 8:
        cpType = (gx->vcdLo >> 8) & 1;
        break;
    case 9:
        cpType = (gx->vcdLo >> 9) & 3;
        break;
    case 10:
        cpType = gx->hasNrms ? (gx->vcdLo >> 11) & 3 : 0;
        break;
    case 25:
        cpType = gx->hasBiNrms ? (gx->vcdLo >> 11) & 3 : 0;
        break;
    case 11:
        cpType = (gx->vcdLo >> 13) & 3;
        break;
    case 12:
        cpType = (gx->vcdLo >> 15) & 3;
        break;
    case 13:
        cpType = gx->vcdHi & 3;
        break;
    case 14:
        cpType = (gx->vcdHi >> 2) & 3;
        break;
    case 15:
        cpType = (gx->vcdHi >> 4) & 3;
        break;
    case 16:
        cpType = (gx->vcdHi >> 6) & 3;
        break;
    case 17:
        cpType = (gx->vcdHi >> 8) & 3;
        break;
    case 18:
        cpType = (gx->vcdHi >> 10) & 3;
        break;
    case 19:
        cpType = (gx->vcdHi >> 12) & 3;
        break;
    case 20:
        cpType = (gx->vcdHi >> 14) & 3;
        break;
    default:
        cpType = 0;
        break;
    }
    *type = cpType;
}
#pragma pop

void GXGetVtxDescv(s32 *vcd) {
    void GXGetVtxDesc(s32 attr, s32 *type);
    s32 attr;

    for (attr = 0; attr < 26; attr++) {
        vcd[attr * 2] = attr;
        GXGetVtxDesc(attr, &vcd[attr * 2 + 1]);
    }
    vcd[attr * 2] = 0xFF;
}

#pragma push
#pragma peephole off
void GXClearVtxDesc(void) {
    extern void *gx;
    M2C_FIELD(gx, u32 *, 0x14) = 0;
    M2C_FIELD(gx, u32 *, 0x14) = (M2C_FIELD(gx, u32 *, 0x14) & ~0x600) | 0x200;
    M2C_FIELD(gx, u32 *, 0x18) = 0;
    M2C_FIELD(gx, u8 *, 0x41C) = 0;
    M2C_FIELD(gx, u8 *, 0x41D) = 0;
    M2C_FIELD(gx, u32 *, 0x4F4) |= 8;
}
#pragma pop
void __GXSetVAT(void) {
    typedef struct {
        u8 pad0[0x1C];
        u32 vatA[8];
        u32 vatB[8];
        u32 vatC[8];
        u8 pad7C[0x4F2 - 0x7C];
        u8 dirtyVAT;
    } GXVatData;
    extern GXVatData *const gx;
    u8 i;

    for (i = 0; i < 8; i++) {
        if (gx->dirtyVAT & (1 << i)) {
            GXWGFifo.u8 = 8;
            GXWGFifo.u8 = i | 0x70;
            GXWGFifo.u32 = gx->vatA[i];
            GXWGFifo.u8 = 8;
            GXWGFifo.u8 = i | 0x80;
            GXWGFifo.u32 = gx->vatB[i];
            GXWGFifo.u8 = 8;
            GXWGFifo.u8 = i | 0x90;
            GXWGFifo.u32 = gx->vatC[i];
        }
    }
    gx->dirtyVAT = 0;
}

void GXSetArray(s32 attr, void *base_ptr, u8 stride) {
    extern u8 *gx;
    s32 cpAttr;
    u32 phyAddr;
    s32 idx;

    cpAttr = attr;
    if (cpAttr == 0x19) {
        cpAttr = 10;
    }
    cpAttr -= 9;
    phyAddr = (u32)base_ptr & 0x3FFFFFFF;
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = cpAttr | 0xA0;
    GXWGFifo.u32 = phyAddr;
    idx = cpAttr - 12;
    if (idx >= 0 && idx < 4) {
        ((u32 *)(gx + 0x88))[idx] = phyAddr;
    }
    GXWGFifo.u8 = 8;
    GXWGFifo.u8 = cpAttr | 0xB0;
    GXWGFifo.u32 = stride;
    idx = cpAttr - 12;
    if (idx >= 0 && idx < 4) {
        ((u32 *)(gx + 0x98))[idx] = stride;
    }
}

void GXInvalidateVtxCache(void) {
    *(s8 *)0xCC008000 = 0x48;
}

void GXSetNumTexGens(u8 nTexGens) {
    extern u8 *gx;
    u32 *reg = (u32 *)(gx + 0x204);
    *reg = (*(u32 *)(gx + 0x204) & ~0xF) | nTexGens;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x103F;
    GXWGFifo.u32 = nTexGens;
    *(u32 *)(gx + 0x4F4) |= 4;
}

void GXSetMisc(s32 token, u32 val) {
    extern u8 *gx;

    switch (token) {
    case 1:
        *(u16 *)(gx + 0x4) = val;
        *(u16 *)(gx + 0x0) = !*(u16 *)(gx + 0x4);
        *(u16 *)(gx + 0x2) = 1;
        if (*(u16 *)(gx + 0x4)) {
            *(u32 *)(gx + 0x4F4) |= 8;
        }
        break;
    case 2:
        *(u8 *)(gx + 0x4F1) = (0 < val);
        break;
    case 0:
    default:
        break;
    }
}

void GXFlush(void) {
    extern u8 *gx;
    void __GXSetDirtyState(void);
    void PPCSync(void);

    if (*(u32 *)(gx + 0x4F4) != 0) {
        __GXSetDirtyState();
    }
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    GXWGFifo.u32 = 0;
    PPCSync();
}

void GXPixModeSync(void) {
    extern void *const gx;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.s32 = M2C_FIELD(gx, s32 *, 0x1DC);
    M2C_FIELD(gx, u16 *, 2) = 0;
}

void GXPokeAlphaRead(s32 mode) {
    extern volatile u16 *__peReg;
    u32 reg;

    reg = 0;
    reg = (reg & ~3) | mode;
    reg = (reg & ~4) | 4;
    __peReg[4] = reg;
}

void GXPokeAlphaUpdate(u8 update_enable) {
    extern volatile u16 *__peReg;
    u16 reg;

    reg = __peReg[1];
    reg = (reg & ~0x10) | (update_enable << 4);
    __peReg[1] = reg;
}

void GXPokeBlendMode(s32 type, s32 src_factor, s32 dst_factor, s32 op) {
    extern volatile u16 *__peReg;
    u32 reg;
    u32 blend;

    blend = 1;
    reg = __peReg[1];
    if (type != 1 && type != 3) {
        blend = 0;
    }
    reg = (reg & ~1) | blend;
    reg = (reg & ~0x800) | ((type == 3) << 11);
    reg = (reg & ~2) | ((type == 2) << 1);
    reg = (reg & ~0xF000) | (op << 12);
    reg = (reg & ~0x700) | (src_factor << 8);
    reg = (reg & ~0xE0) | (dst_factor << 5);
    reg = (reg & 0xFFFFFF) | 0x41000000;
    __peReg[1] = reg;
}

void GXPokeColorUpdate(u8 update_enable) {
    extern volatile u16 *__peReg;
    __peReg[1] = (__peReg[1] & ~0x8) | (update_enable << 3);
}

void GXPokeDither(u8 dither) {
    extern void *__peReg;
    u16 *reg = (u16 *)__peReg + 1;
    *reg = (*reg & ~0x4) | (dither << 2);
}

void GXPokeZMode(u8 compare_enable, s32 func, u8 update_enable) {
    extern void *__peReg;
    M2C_FIELD(__peReg, u16 *, 0) = (((compare_enable & ~0xE) | (func << 1)) & ~0x10) | (update_enable << 4);
}

void GXTokenInterruptHandler(s16 interrupt, void *context) {
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    extern void (*TokenCB_803C7578)(u16);
    extern volatile u16 *__peReg;
    u16 token;
    u32 exceptionContext[0x2C8 / 4];
    u32 reg;

    token = __peReg[7];
    if (TokenCB_803C7578) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        TokenCB_803C7578(token);
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
    }
    reg = __peReg[5];
    reg = (reg & ~4) | 4;
    __peReg[5] = reg;
}

void GXFinishInterruptHandler(s16 interrupt, void *context) {
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    void OSWakeupThread(void *);
    extern void (*DrawDoneCB_803C757C)(void);
    extern u8 DrawDone_803C7580;
    extern u8 FinishQueue_803C7584[8];
    extern volatile u16 *__peReg;
    u32 exceptionContext[0x2C8 / 4];
    u32 reg;

    reg = __peReg[5];
    reg = (reg & ~8) | 8;
    __peReg[5] = reg;
    DrawDone_803C7580 = 1;
    if (DrawDoneCB_803C757C) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        DrawDoneCB_803C757C();
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
    }
    OSWakeupThread(FinishQueue_803C7584);
}

void GXTokenInterruptHandler(s16 interrupt, void *context);
void GXFinishInterruptHandler(s16 interrupt, void *context);
void __GXPEInit(void) {
    void *__OSSetInterruptHandler(s16, void *);
    void __OSUnmaskInterrupts(u32);
    void OSInitThreadQueue(void *queue);
    extern u8 FinishQueue_803C7584[8];
    extern volatile u16 *__peReg;
    u32 reg;

    __OSSetInterruptHandler(0x12, GXTokenInterruptHandler);
    __OSSetInterruptHandler(0x13, GXFinishInterruptHandler);
    OSInitThreadQueue(FinishQueue_803C7584);
    __OSUnmaskInterrupts(0x2000);
    __OSUnmaskInterrupts(0x1000);
    reg = __peReg[5];
    reg = (reg & ~4) | 4;
    reg = (reg & ~8) | 8;
    reg = (reg & ~1) | 1;
    reg = (reg & ~2) | 2;
    __peReg[5] = reg;
}

void __GXSetDirtyState(void) {
    extern u8 *gx;
    void __GXSetSUTexRegs();
    void __GXUpdateBPMask();
    void __GXSetGenMode();
    void __GXSetVCD();
    void __GXSetVAT();
    void __GXCalculateVLim();

    if (*(u32 *)(gx + 0x4F4) & 1) {
        __GXSetSUTexRegs();
    }
    if (*(u32 *)(gx + 0x4F4) & 2) {
        __GXUpdateBPMask();
    }
    if (*(u32 *)(gx + 0x4F4) & 4) {
        __GXSetGenMode();
    }
    if (*(u32 *)(gx + 0x4F4) & 8) {
        __GXSetVCD();
    }
    if (*(u32 *)(gx + 0x4F4) & 0x10) {
        __GXSetVAT();
    }
    if (*(u32 *)(gx + 0x4F4) & 0x18) {
        __GXCalculateVLim();
    }
    *(u32 *)(gx + 0x4F4) = 0;
}

void __GXSendFlushPrim(void) {
    extern u8 *gx;
    u32 i;
    u16 vNum;
    u32 numD;

    vNum = *(u16 *)(gx + 4);
    numD = vNum * *(u16 *)(gx + 6);
    GXWGFifo.u8 = 0x98;
    GXWGFifo.u16 = vNum;
    for (i = 0; i < numD; i += 4) {
        GXWGFifo.u32 = 0;
    }
    *(u16 *)(gx + 2) = 1;
}

void GXSetLineWidth(u8 width, u32 texOffsets) {
    typedef struct GXDataLp {
        u8 unk0[2];
        u16 bpSentNot;
        u8 unk4[0x7C - 4];
        u32 lpSize;
    } GXDataLp;
    extern GXDataLp *gx;
    u32 *reg;

    reg = &gx->lpSize;
    *reg = (gx->lpSize & ~0xFF) | width;
    reg = &gx->lpSize;
    *reg = (gx->lpSize & ~0x70000) | (texOffsets << 16);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->lpSize;
    gx->bpSentNot = 0;
}

void __GXSetSUTexRegs(void);
void __GXUpdateBPMask(void);
void __GXSetGenMode(void);
void __GXSetVCD(void);
void __GXSetVAT(void);
void __GXCalculateVLim(void);
void GXSetPointSize(u8 pointSize, u32 texOffsets) {
    typedef struct GXDataLp {
        u8 unk0[2];
        u16 bpSentNot;
        u8 unk4[0x7C - 4];
        u32 lpSize;
    } GXDataLp;
    extern GXDataLp *gx;
    u32 *reg;

    reg = &gx->lpSize;
    *reg = (*reg & ~0xFF00) | ((u32)pointSize << 8);
    reg = &gx->lpSize;
    *reg = (gx->lpSize & ~0x380000) | (texOffsets << 19);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->lpSize;
    gx->bpSentNot = 0;
}

void GXEnableTexOffsets(s32 coord, u8 line_enable, u8 point_enable) {
    extern u8 *gx;
    ((u32 *)(gx + 0xB8))[coord] = (((u32 *)(gx + 0xB8))[coord] & ~0x40000) | ((u32)line_enable << 18);
    ((u32 *)(gx + 0xB8))[coord] = (((u32 *)(gx + 0xB8))[coord] & ~0x80000) | ((u32)point_enable << 19);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = ((u32 *)(gx + 0xB8))[coord];
    *(u16 *)(gx + 2) = 0;
}

void GXSetCullMode(s32 mode) {
    extern u8 *gx;
    s32 hwMode;

    switch (mode) {
    case 1:
        hwMode = 2;
        break;
    case 2:
        hwMode = 1;
        break;
    default:
        hwMode = mode;
        break;
    }
    *(u32 *)(gx + 0x204) = (*(u32 *)(gx + 0x204) & ~0xC000) | (hwMode << 14);
    *(u32 *)(gx + 0x4F4) |= 4;
}

void GXSetCoPlanar(u8 enable) {
    extern u8 *gx;
    *(u32 *)(gx + 0x204) = (*(u32 *)(gx + 0x204) & ~0x80000) | ((u32)enable << 19);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0xFE080000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *(u32 *)(gx + 0x204);
}

void __GXSetGenMode(void) {
    extern u8 *const gx;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *(u32 *)(gx + 0x204);
    *(u16 *)(gx + 2) = 0;
}

void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    extern u8 *gx;
    u32 *reg;

    *(u32 *)(gx + 0x1E0) = 0;
    reg = (u32 *)(gx + 0x1E0);
    *reg = (*reg & ~0x3FF) | left;
    reg = (u32 *)(gx + 0x1E0);
    *reg = (*reg & ~0xFFC00) | (top << 10);
    reg = (u32 *)(gx + 0x1E0);
    *reg = (*reg & 0xFFFFFF) | 0x49000000;
    *(u32 *)(gx + 0x1E4) = 0;
    reg = (u32 *)(gx + 0x1E4);
    *reg = (*reg & ~0x3FF) | (wd - 1);
    reg = (u32 *)(gx + 0x1E4);
    *reg = (*reg & ~0xFFC00) | ((ht - 1) << 10);
    reg = (u32 *)(gx + 0x1E4);
    *reg = (*reg & 0xFFFFFF) | 0x4A000000;
}

void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    extern u8 *gx;
    u32 *reg;

    *(u32 *)(gx + 0x1F0) = 0;
    reg = (u32 *)(gx + 0x1F0);
    *reg = (*reg & ~0x3FF) | left;
    reg = (u32 *)(gx + 0x1F0);
    *reg = (*reg & ~0xFFC00) | (top << 10);
    reg = (u32 *)(gx + 0x1F0);
    *reg = (*reg & 0xFFFFFF) | 0x49000000;
    *(u32 *)(gx + 0x1F4) = 0;
    reg = (u32 *)(gx + 0x1F4);
    *reg = (*reg & ~0x3FF) | (wd - 1);
    reg = (u32 *)(gx + 0x1F4);
    *reg = (*reg & ~0xFFC00) | ((ht - 1) << 10);
    reg = (u32 *)(gx + 0x1F4);
    *reg = (*reg & 0xFFFFFF) | 0x4A000000;
}

void GXSetDispCopyDst(u16 wd, u16 ht) {
    extern u8 *gx;
    u16 stride;
    u32 *reg;

    stride = (u16)(wd * 2);
    *(u32 *)(gx + 0x1E8) = 0;
    reg = (u32 *)(gx + 0x1E8);
    *reg = (*reg & ~0x3FF) | (stride >> 5);
    reg = (u32 *)(gx + 0x1E8);
    *reg = (*reg & 0xFFFFFF) | 0x4D000000;
}

void GXSetDispCopyFrame2Field(s32 mode) {
    extern u8 *gx;
    u32 *reg;

    reg = (u32 *)(gx + 0x1EC);
    *reg = (*reg & ~0x3000) | (mode << 12);
    reg = (u32 *)(gx + 0x1FC);
    *reg = *reg & ~0x3000;
}

void GXSetCopyClamp(s32 clamp) {
    extern u8 *gx;
    u8 clmpB;
    u8 clmpT;

    clmpT = (clamp & 1) == 1;
    clmpB = (clamp & 2) == 2;
    *(u32 *)(gx + 0x1EC) = (*(u32 *)(gx + 0x1EC) & ~1) | clmpT;
    *(u32 *)(gx + 0x1EC) = (*(u32 *)(gx + 0x1EC) & ~2) | (clmpB << 1);
    *(u32 *)(gx + 0x1FC) = (*(u32 *)(gx + 0x1FC) & ~1) | clmpT;
    *(u32 *)(gx + 0x1FC) = (*(u32 *)(gx + 0x1FC) & ~2) | (clmpB << 1);
}

void GXSetCopyClear(GXColor clear_clr, u32 clear_z) {
    extern u8 *gx;
    u32 reg;

    reg = clear_clr.r;
    reg = (reg & ~0xFF00) | (clear_clr.a << 8);
    reg = (reg & 0xFFFFFF) | 0x4F000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    reg = clear_clr.b;
    reg = (reg & ~0xFF00) | (clear_clr.g << 8);
    reg = (reg & 0xFFFFFF) | 0x50000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    reg = clear_z & 0xFFFFFF;
    reg = (reg & 0xFFFFFF) | 0x51000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetDispCopyGamma(s32 gamma) {
    extern u8 *gx;
    u8 *g = gx;
    *(u32 *)(g + 0x1EC) = (*(u32 *)(g + 0x1EC) & ~0x180) | (gamma << 7);
}

void GXClearBoundingBox(void) {
    extern u8 *gx;
    u16 *bpSentNot = (u16 *)(gx + 2);

    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0x550003FF;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0x560003FF;
    *bpSentNot = 0;
}

void GXInitLightAttnK(void *arg0, f32 farg0, f32 farg1, f32 farg2) {
    M2C_FIELD(arg0, f32 *, 0x1C) = farg0;
    M2C_FIELD(arg0, f32 *, 0x20) = farg1;
    M2C_FIELD(arg0, f32 *, 0x24) = farg2;
}

void GXInitLightPos(void *arg0, f32 farg0, f32 farg1, f32 farg2) {
    M2C_FIELD(arg0, f32 *, 0x28) = farg0;
    M2C_FIELD(arg0, f32 *, 0x2C) = farg1;
    M2C_FIELD(arg0, f32 *, 0x30) = farg2;
}

void GXInitLightColor(GXLightObjInt *lt_obj, GXColor color) {
    lt_obj->Color = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}

void GXSetNumChans(u8 nChans) {
    extern u8 *gx;
    u32 *reg = (u32 *)(gx + 0x204);
    *reg = (*reg & ~0x70) | (nChans << 4);
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1009;
    GXWGFifo.u32 = nChans;
    *(u32 *)(gx + 0x4F4) |= 4;
}

#pragma push
#pragma peephole off
void __GetImageTileCount(s32 fmt, u16 wd, u16 ht, u32 *rowTiles, u32 *colTiles, u32 *cmpTiles) {
    u32 texRowShift;
    u32 texColShift;

    switch (fmt) {
    case 0x0:
    case 0x8:
    case 0xE:
    case 0x20:
    case 0x30:
        texRowShift = 3;
        texColShift = 3;
        break;
    case 0x1:
    case 0x2:
    case 0x9:
    case 0x11:
    case 0x22:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x39:
    case 0x3A:
        texRowShift = 3;
        texColShift = 2;
        break;
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0xA:
    case 0x13:
    case 0x16:
    case 0x23:
    case 0x2B:
    case 0x2C:
    case 0x3C:
        texRowShift = 2;
        texColShift = 2;
        break;
    default:
        texRowShift = texColShift = 0;
        break;
    }
    if (wd == 0) {
        wd = 1;
    }
    if (ht == 0) {
        ht = 1;
    }
    *rowTiles = (wd + (1 << texRowShift) - 1) >> texRowShift;
    *colTiles = (ht + (1 << texColShift) - 1) >> texColShift;
    *cmpTiles = (fmt == 0x6 || fmt == 0x16) ? 2 : 1;
}
#pragma pop
s32 GXGetTexObjData(void *arg0) {
    return (M2C_FIELD(arg0, s32 *, 0xC) << 5) & 0x03FFFFE0;
}

s32 GXGetTexObjFmt(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x14);
}

void GXLoadTexObj(void *obj, u32 id) {
    typedef struct GXDataTex {
        u8 unk0[0x410];
        void *(*texRegionCallback)(void *obj, u32 id);
    } GXDataTex;
    extern GXDataTex *gx;
    void GXLoadTexObjPreLoaded(void *obj, void *region, u32 id);
    void *r;

    r = gx->texRegionCallback(obj, id);
    GXLoadTexObjPreLoaded(obj, r, id);
}

void GXInitTlutRegion(u32 *region, u32 tmem_addr, u32 tlut_size) {
    u32 *t = region;

    *t = 0;
    *t = (*t & ~0x3FF) | ((tmem_addr - 0x80000) >> 9);
    *t = (*t & ~0x1FFC00) | (tlut_size << 10);
    *t = (*t & ~0xFF000000) | (0x65 << 24);
}

void GXInvalidateTexAll(void) {
    u32 reg0;
    u32 reg1;

    reg0 = 0x66001000;
    reg1 = 0x66001100;
    __GXFlushTextureState();
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg0;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg1;
    __GXFlushTextureState();
}

void __GXFlushTextureState(void);
void *GXSetTexRegionCallback(void *f) {
    extern void *const gx;
    void *oldcb = M2C_FIELD(gx, void **, 0x410);
    M2C_FIELD(gx, void **, 0x410) = f;
    return oldcb;
}

void *GXSetTlutRegionCallback(void *f) {
    extern void *const gx;
    void *oldcb = M2C_FIELD(gx, void **, 0x414);
    M2C_FIELD(gx, void **, 0x414) = f;
    return oldcb;
}

extern volatile PPCWGPipe GXWGPipe : 0xCC008000;
void __SetSURegs(u32 tmap, u32 tcoord) {
    typedef struct GXDataSU {
        u8 unk0[2];
        u16 bpSentNot;
        u8 unk4[0xB4];
        u32 suTs0[8];
        u32 suTs1[8];
        u8 unkF8[0x364];
        u32 tImage0[8];
        u32 tMode0[8];
    } GXDataSU;
    extern GXDataSU *gx;
    u32 w;
    u32 h;
    u8 s_bias;
    u8 t_bias;

    w = (gx->tImage0[tmap] >> 0) & 0x3FF;
    h = (gx->tImage0[tmap] >> 10) & 0x3FF;
    gx->suTs0[tcoord] = (gx->suTs0[tcoord] & ~0xFFFF) | (w << 0);
    gx->suTs1[tcoord] = (gx->suTs1[tcoord] & ~0xFFFF) | (h << 0);
    s_bias = ((gx->tMode0[tmap] >> 0) & 3) == 1;
    t_bias = ((gx->tMode0[tmap] >> 2) & 3) == 1;
    gx->suTs0[tcoord] = (gx->suTs0[tcoord] & ~0x10000) | ((u32)s_bias << 16);
    gx->suTs1[tcoord] = (gx->suTs1[tcoord] & ~0x10000) | ((u32)t_bias << 16);
    GXWGPipe.u8 = 0x61;
    GXWGPipe.u32 = gx->suTs0[tcoord];
    GXWGPipe.u8 = 0x61;
    GXWGPipe.u32 = gx->suTs1[tcoord];
    gx->bpSentNot = 0;
}

void GXSetTevIndirect(s32 tev_stage, s32 ind_stage, s32 format, s32 bias_sel, s32 matrix_sel, s32 wrap_s, s32 wrap_t, u8 add_prev, u8 utc_lod, s32 alpha_sel) {
    extern u8 *gx;
    u32 reg;

    reg = 0;
    reg = (reg & ~0x3) | ind_stage;
    reg = (reg & ~0xC) | (format << 2);
    reg = (reg & ~0x70) | (bias_sel << 4);
    reg = (reg & ~0x180) | (alpha_sel << 7);
    reg = (reg & ~0x1E00) | (matrix_sel << 9);
    reg = (reg & ~0xE000) | (wrap_s << 13);
    reg = (reg & ~0x70000) | (wrap_t << 16);
    reg = (reg & ~0x80000) | (utc_lod << 19);
    reg = (reg & ~0x100000) | (add_prev << 20);
    reg = (reg & ~0xFF000000) | ((tev_stage + 0x10) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetNumIndStages(u8 nIndStages) {
    extern u8 *gx;
    u32 *reg = (u32 *)(gx + 0x204);
    *reg = (*reg & ~0x70000) | (nIndStages << 16);
    *(u32 *)(gx + 0x4F4) |= 6;
}

#pragma push
#pragma scheduling off
void __GXUpdateBPMask(void) {
    typedef struct GXDataBP {
        u8 unk0[2];
        u16 bpSentNot;
        u8 unk4[0x11C];
        u32 iref;
        u32 bpMask;
        u8 unk128[0xDC];
        u32 genMode;
    } GXDataBP;
    extern GXDataBP *gx;
    u32 i;
    u32 tmap;
    u32 new_imask;
    u32 *reg;

    new_imask = 0;
    for (i = 0; i < ((gx->genMode >> 16) & 7); i++) {
        switch (i) {
        case 0:
            tmap = gx->iref & 7;
            break;
        case 1:
            tmap = (gx->iref >> 6) & 7;
            break;
        case 2:
            tmap = (gx->iref >> 12) & 7;
            break;
        case 3:
            tmap = (gx->iref >> 18) & 7;
            break;
        }
        new_imask |= 1 << tmap;
    }
    reg = &gx->bpMask;
    if ((u8)*reg != new_imask) {
        *reg = (*reg & ~0xFF) | new_imask;
        GXWGPipe.u8 = 0x61;
        GXWGPipe.u32 = gx->bpMask;
        gx->bpSentNot = 0;
    }
}
#pragma pop

void __GXFlushTextureState(void) {
    extern u8 *const gx;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *(u32 *)(gx + 0x124);
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevColorIn(s32 stage, s32 a, s32 b, s32 c, s32 d) {
    extern GXDataTev *const gx;
    u32 *reg = &gx->tevc[stage];
    *reg = (*reg & ~0xF000) | (a << 12);
    *reg = (*reg & ~0xF00) | (b << 8);
    *reg = (*reg & ~0xF0) | (c << 4);
    *reg = (*reg & ~0xF) | d;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *reg;
    *(u16 *)((u8 *)gx + 2) = 0;
}

void GXSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d) {
    extern u8 *gx;
    u32 *reg;

    reg = (u32 *)(gx + (stage * 4 + 0x170));
    *reg = (*reg & ~0xE000) | (a << 13);
    *reg = (*reg & ~0x1C00) | (b << 10);
    *reg = (*reg & ~0x380) | (c << 7);
    *reg = (*reg & ~0x70) | (d << 4);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevColorOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 out_reg) {
    extern u8 *gx;
    u32 *reg;

    reg = (u32 *)(gx + (stage * 4 + 0x130));
    *reg = (*reg & ~0x40000) | ((op & 1) << 18);
    if (op <= 1) {
        *reg = (*reg & ~0x300000) | (scale << 20);
        *reg = (*reg & ~0x30000) | (bias << 16);
    } else {
        *reg = (*reg & ~0x300000) | (((op >> 1) & 3) << 20);
        *reg = (*reg & ~0x30000) | (3 << 16);
    }
    *reg = (*reg & ~0x80000) | (clamp << 19);
    *reg = (*reg & ~0xC00000) | (out_reg << 22);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevAlphaOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 out_reg) {
    extern u8 *gx;
    u32 *reg;

    reg = (u32 *)(gx + (stage * 4 + 0x170));
    *reg = (*reg & ~0x40000) | ((op & 1) << 18);
    if (op <= 1) {
        *reg = (*reg & ~0x300000) | (scale << 20);
        *reg = (*reg & ~0x30000) | (bias << 16);
    } else {
        *reg = (*reg & ~0x300000) | (((op >> 1) & 3) << 20);
        *reg = (*reg & ~0x30000) | (3 << 16);
    }
    *reg = (*reg & ~0x80000) | (clamp << 19);
    *reg = (*reg & ~0xC00000) | (out_reg << 22);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevColor(s32 id, GXColor color) {
    extern u8 *gx;
    u32 regRA;
    u32 regBG;

    regRA = 0;
    regRA = (regRA & ~0x7FF) | color.r;
    regRA = (regRA & ~0x7FF000) | (color.a << 12);
    regRA = (regRA & ~0xFF000000) | ((0xE0 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regRA;
    regBG = 0;
    regBG = (regBG & ~0x7FF) | color.b;
    regBG = (regBG & ~0x7FF000) | (color.g << 12);
    regBG = (regBG & ~0xFF000000) | ((0xE1 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevColorS10(s32 id, GXColorS10 color) {
    extern u8 *gx;
    u32 regRA;
    u32 regBG;

    regRA = 0;
    regRA = (regRA & ~0x7FF) | (color.r & 0x7FF);
    regRA = (regRA & ~0x7FF000) | ((color.a & 0x7FF) << 12);
    regRA = (regRA & ~0xFF000000) | ((0xE0 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regRA;
    regBG = 0;
    regBG = (regBG & ~0x7FF) | (color.b & 0x7FF);
    regBG = (regBG & ~0x7FF000) | ((color.g & 0x7FF) << 12);
    regBG = (regBG & ~0xFF000000) | ((0xE1 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevKColor(s32 id, GXColor color) {
    extern u8 *gx;
    u32 regRA;
    u32 regBG;

    regRA = 0;
    regRA = (regRA & ~0xFF) | color.r;
    regRA = (regRA & ~0xFF000) | (color.a << 12);
    regRA = (regRA & ~0xF00000) | (8 << 20);
    regRA = (regRA & ~0xFF000000) | ((0xE0 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regRA;
    regBG = 0;
    regBG = (regBG & ~0xFF) | color.b;
    regBG = (regBG & ~0xFF000) | (color.g << 12);
    regBG = (regBG & ~0xF00000) | (8 << 20);
    regBG = (regBG & ~0xFF000000) | ((0xE1 + id * 2) << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = regBG;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevKColorSel(s32 stage, s32 sel) {
    extern u8 *gx;
    u32 *Kreg;

    Kreg = (u32 *)(gx + ((stage >> 1) * 4 + 0x1B0));
    if (stage & 1) {
        *Kreg = (*Kreg & ~0x7C000) | (sel << 14);
    } else {
        *Kreg = (*Kreg & ~0x1F0) | (sel << 4);
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *Kreg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetTevKAlphaSel(s32 stage, s32 sel) {
    extern GXDataTev *const gx;
    u32 *Kreg;

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        *Kreg = (*Kreg & ~0xF80000) | (sel << 19);
    } else {
        *Kreg = (*Kreg & ~0x3E00) | (sel << 9);
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *Kreg;
    *(u16 *)((u8 *)gx + 2) = 0;
}

void GXSetTevSwapMode(s32 stage, s32 ras_sel, s32 tex_sel) {
    extern GXDataTev *const gx;
    u32 *reg;

    reg = &gx->teva[stage];
    *reg = (*reg & ~0x3) | ras_sel;
    *reg = (*reg & ~0xC) | (tex_sel << 2);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *reg;
    *(u16 *)((u8 *)gx + 2) = 0;
}

void GXSetTevSwapModeTable(s32 table, s32 red, s32 green, s32 blue, s32 alpha) {
    extern GXDataTev *const gx;
    u32 *Kreg;

    Kreg = &gx->tevKsel[table * 2];
    *Kreg = (*Kreg & ~0x3) | red;
    *Kreg = (*Kreg & ~0xC) | (green << 2);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *Kreg;

    Kreg = &gx->tevKsel[table * 2 + 1];
    *Kreg = (*Kreg & ~0x3) | blue;
    *Kreg = (*Kreg & ~0xC) | (alpha << 2);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *Kreg;
    *(u16 *)((u8 *)gx + 2) = 0;
}

void GXSetAlphaCompare(s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1) {
    extern u8 *const gx;
    u32 reg = 0;

    reg = (reg & ~0xFF) | ref0;
    reg = (reg & ~0xFF00) | (ref1 << 8);
    reg = (reg & ~0x70000) | (comp0 << 16);
    reg = (reg & ~0x380000) | (comp1 << 19);
    reg = (reg & ~0xC00000) | (op << 22);
    reg = (reg & ~0xFF000000) | (0xF3 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetZTexture(s32 op, s32 fmt, u32 bias) {
    extern u8 *gx;
    volatile PPCWGPipe *wgPipe = (volatile PPCWGPipe *)0xCC008000;
    u32 zenv0;
    u32 zenv1;
    u32 type;

    zenv0 = 0;
    zenv0 = (zenv0 & ~0xFFFFFF) | bias;
    zenv0 = (zenv0 & 0xFFFFFF) | 0xF4000000;
    zenv1 = 0;
    switch (fmt) {
    case 0x11:
        type = 0;
        break;
    case 0x13:
        type = 1;
        break;
    case 0x16:
        type = 2;
        break;
    default:
        type = 2;
        break;
    }
    zenv1 = (zenv1 & ~3) | type;
    zenv1 = (zenv1 & ~0xC) | (op << 2);
    zenv1 = (zenv1 & 0xFFFFFF) | 0xF5000000;
    wgPipe->u8 = 0x61;
    wgPipe->u32 = zenv0;
    wgPipe->u8 = 0x61;
    wgPipe->u32 = zenv1;
    *(u16 *)(gx + 2) = 0;
}

void GXSetNumTevStages(u8 nStages) {
    extern u8 *gx;
    u32 *reg;

    reg = (u32 *)(gx + 0x204);
    *reg = (*reg & ~0x3C00) | ((u32)(nStages - 1) << 10);
    reg = (u32 *)(gx + 0x4F4);
    *reg |= 4;
}

void GXSetColorUpdate(u8 updateEnable) {
    extern u8 *gx;
    u8 *g;
    u32 *reg;

    g = gx;
    reg = (u32 *)(g + 0x1D0);
    *reg = (*(u32 *)(g + 0x1D0) & ~8) | (updateEnable << 3);
    GXWGFifo.u8 = 0x61;
    g = gx;
    GXWGFifo.u32 = *(u32 *)(g + 0x1D0);
    *(u16 *)(g + 2) = 0;
}

void GXSetAlphaUpdate(u8 updateEnable) {
    extern u8 *gx;
    u32 reg;
    u8 *g;

    g = gx;
    reg = *(u32 *)(g + 0x1D0);
    reg = (reg & ~0x10) | (updateEnable << 4);
    *(u32 *)(g + 0x1D0) = reg;
    GXWGFifo.u8 = 0x61;
    g = gx;
    GXWGFifo.u32 = *(u32 *)(g + 0x1D0);
    *(u16 *)(g + 2) = 0;
}

void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable) {
    extern u8 *gx;
    u8 *g;
    u32 *reg;

    g = gx;
    reg = (u32 *)(g + 0x1D8);
    *reg = (*(u32 *)(g + 0x1D8) & ~1) | compare_enable;
    g = gx;
    reg = (u32 *)(g + 0x1D8);
    *reg = (*(u32 *)(g + 0x1D8) & ~0xE) | (func << 1);
    g = gx;
    reg = (u32 *)(g + 0x1D8);
    *reg = (*(u32 *)(g + 0x1D8) & ~0x10) | (update_enable << 4);
    GXWGFifo.u8 = 0x61;
    g = gx;
    GXWGFifo.u32 = *(u32 *)(g + 0x1D8);
    *(u16 *)(g + 2) = 0;
}

void GXSetZCompLoc(u8 before_tex) {
    extern u8 *gx;
    u8 *g;
    u32 *reg;

    g = gx;
    reg = (u32 *)(g + 0x1DC);
    *reg = (*(u32 *)(g + 0x1DC) & ~0x40) | (before_tex << 6);
    GXWGFifo.u8 = 0x61;
    g = gx;
    GXWGFifo.u32 = *(u32 *)(g + 0x1DC);
    *(u16 *)(g + 2) = 0;
}

void GXSetDither(u8 dither) {
    extern u8 *gx;
    u8 *g;
    u32 *reg;

    g = gx;
    reg = (u32 *)(g + 0x1D0);
    *reg = (*(u32 *)(g + 0x1D0) & ~4) | (dither << 2);
    GXWGFifo.u8 = 0x61;
    g = gx;
    GXWGFifo.u32 = *(u32 *)(g + 0x1D0);
    *(u16 *)(g + 2) = 0;
}

void GXSetDstAlpha(u8 enable, u8 alpha) {
    extern u8 *gx;
    u8 *g;
    u32 *reg;

    reg = (u32 *)(gx + 0x1D4);
    *reg = (*reg & ~0xFF) | alpha;
    g = gx;
    reg = (u32 *)(g + 0x1D4);
    *reg = (*(u32 *)(g + 0x1D4) & ~0x100) | (enable << 8);
    GXWGFifo.u8 = 0x61;
    g = gx;
    GXWGFifo.u32 = *(u32 *)(g + 0x1D4);
    *(u16 *)(g + 2) = 0;
}

void GXSetFieldMask(u8 odd_mask, u8 even_mask) {
    extern u8 *gx;
    u32 reg;

    reg = even_mask;
    reg = (reg & ~2) | (odd_mask << 1);
    reg = (reg & 0xFFFFFF) | 0x44000000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetFieldMode(u8 field_mode, u8 half_aspect_ratio) {
    extern u8 *gx;
    void __GXFlushTextureState(void);
    u32 *reg;

    reg = (u32 *)(gx + 0x7C);
    *reg = (*reg & ~0x400000) | (half_aspect_ratio << 22);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *(u32 *)(gx + 0x7C);
    __GXFlushTextureState();
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = field_mode | 0x68000000;
    __GXFlushTextureState();
}

void __GXSetRange(void) {

}

void GXCallDisplayList(void *list, u32 nbytes) {
    extern u8 *gx;
    void __GXSetDirtyState(void);
    void __GXSendFlushPrim(void);

    if (*(u32 *)(gx + 0x4F4) != 0) {
        __GXSetDirtyState();
    }
    if (*(u32 *)gx == 0) {
        __GXSendFlushPrim();
    }
    GXWGFifo.u8 = 0x40;
    GXWGFifo.u32 = (u32)list;
    GXWGFifo.u32 = nbytes;
}

void GXGetProjectionv(f32 *p) {
    extern void *gx;
    u32 type;

    type = M2C_FIELD(gx, u32 *, 0x420);
    p[0] = (f32)type;
    p[1] = M2C_FIELD(gx, f32 *, 0x424);
    p[2] = M2C_FIELD(gx, f32 *, 0x428);
    p[3] = M2C_FIELD(gx, f32 *, 0x42C);
    p[4] = M2C_FIELD(gx, f32 *, 0x430);
    p[5] = M2C_FIELD(gx, f32 *, 0x434);
    p[6] = M2C_FIELD(gx, f32 *, 0x438);
}

void WriteMTXPS3x3from3x4(f32 mtx[3][4], volatile f32 *dest) {
    *dest = mtx[0][2];
    *dest = mtx[1][2];
    *dest = mtx[2][2];
}

void WriteMTXPS4x3(f32 mtx[3][4], volatile f32 *dest);
void WriteMTXPS3x3from3x4(f32 mtx[3][4], volatile f32 *dest);
void WriteMTXPS4x2(f32 mtx[2][4], volatile f32 *dest) {
    f32 a = mtx[0][0];
    f32 b = mtx[0][1];
    *dest = a;
    *dest = b;
}

void GXSetCurrentMtx(u32 id) {
    extern u8 *gx;
    void __GXSetMatrixIndex(s32 index);
    u32 *reg;

    reg = (u32 *)(gx + 0x80);
    *reg = (*reg & ~0x3F) | id;
    __GXSetMatrixIndex(0);
}

void GXGetViewportv(void *arg0) {
    extern void *gx;
    M2C_FIELD(arg0, f32 *, 0) = (f32) M2C_FIELD(gx, f32 *, 0x43C);
    M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(gx, f32 *, 0x440);
    M2C_FIELD(arg0, f32 *, 8) = (f32) M2C_FIELD(gx, f32 *, 0x444);
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) M2C_FIELD(gx, f32 *, 0x448);
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) M2C_FIELD(gx, f32 *, 0x44C);
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) M2C_FIELD(gx, f32 *, 0x450);
}

void GXSetViewportJitter(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz, u32 field);
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht) {
    extern u8 *gx;
    u32 tp, lf, bm, rt;

    tp = top + 0x156;
    lf = left + 0x156;
    bm = tp + ht - 1;
    rt = lf + wd - 1;
    *(u32 *)(gx + 0xF8) = (*(u32 *)(gx + 0xF8) & ~0x7FF) | tp;
    *(u32 *)(gx + 0xF8) = (*(u32 *)(gx + 0xF8) & ~0x7FF000) | (lf << 12);
    *(u32 *)(gx + 0xFC) = (*(u32 *)(gx + 0xFC) & ~0x7FF) | bm;
    *(u32 *)(gx + 0xFC) = (*(u32 *)(gx + 0xFC) & ~0x7FF000) | (rt << 12);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *(u32 *)(gx + 0xF8);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *(u32 *)(gx + 0xFC);
    *(u16 *)(gx + 2) = 0;
}

void GXSetScissorBoxOffset(s32 x_off, s32 y_off) {
    extern u8 *gx;
    u32 reg = 0;
    u32 hx = (u32)(x_off + 0x156) >> 1;
    u32 hy = (u32)(y_off + 0x156) >> 1;

    reg = (reg & ~0x3FF) | hx;
    reg = (reg & ~0xFFC00) | (hy << 10);
    reg = (reg & ~0xFF000000) | (0x59 << 24);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = reg;
    *(u16 *)(gx + 2) = 0;
}

void GXSetClipMode(s32 mode) {
    extern u8 *const gx;
    GXWGFifo.u8 = 0x10;
    GXWGFifo.u32 = 0x1005;
    GXWGFifo.u32 = mode;
    *(u16 *)(gx + 2) = 1;
}

void __GXSetMatrixIndex(s32 index) {
    extern u8 *gx;
    u32 reg;

    if (index < 5) {
        GXWGFifo.u8 = 8;
        GXWGFifo.u8 = 0x30;
        reg = *(u32 *)(gx + 0x80);
        GXWGFifo.u32 = reg;
        GXWGFifo.u8 = 0x10;
        GXWGFifo.u32 = 0x1018;
        GXWGFifo.u32 = reg;
    } else {
        GXWGFifo.u8 = 8;
        GXWGFifo.u8 = 0x40;
        reg = *(u32 *)(gx + 0x84);
        GXWGFifo.u32 = reg;
        GXWGFifo.u8 = 0x10;
        GXWGFifo.u32 = 0x1019;
        GXWGFifo.u32 = reg;
    }
    *(u16 *)(gx + 2) = 1;
}

void GXClearGPMetric(void) {
    extern void *__cpReg;
    M2C_FIELD(__cpReg, u16 *, 4) = 4;
}

BOOL SIIsChanBusy(s32 chan) {
    BOOL rc = TRUE;
    if (Packet_803096E0.packet[chan].chan == -1 && Si_800D9E70.chan != chan) {
        rc = FALSE;
    }
    return rc;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[3 * chan] = command;
}

void SITransferCommands(void) {
    u32 *regs = (u32 *)__SIRegs;
    regs[14] = 0x80000000;
}

u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;
    enabled = OSDisableInterrupts();
    Si_800D9E70.poll &= ~(0x03FF0000 | 0x0000FF00);
    Si_800D9E70.poll |= poll;
    poll = Si_800D9E70.poll;
    __SIRegs[12] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    s32 enabled;

    if (poll == 0) {
        return Si_800D9E70.poll;
    }
    enabled = OSDisableInterrupts();
    poll >>= 24;
    poll &= 0xF0;
    poll = Si_800D9E70.poll & ~poll;
    __SIRegs[12] = poll;
    Si_800D9E70.poll = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

#pragma push
#pragma use_lmw_stmw on
BOOL SIGetResponse(s32 chan, void *data) {
    BOOL OSDisableInterrupts(void);
    BOOL OSRestoreInterrupts(BOOL level);
    u32 SIGetStatus(s32 chan);
    SIPacketBss *si = &Packet_803096E0;
    BOOL rc;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (SIGetStatus(chan) & 0x20) {
        si->inputBuffer[chan][0] = __SIRegs[3 * chan + 1];
        si->inputBuffer[chan][1] = __SIRegs[3 * chan + 2];
        si->inputBufferValid[chan] = TRUE;
    }
    rc = si->inputBufferValid[chan];
    si->inputBufferValid[chan] = FALSE;
    if (rc) {
        ((u32 *)data)[0] = si->inputBuffer[chan][0];
        ((u32 *)data)[1] = si->inputBuffer[chan][1];
    }
    OSRestoreInterrupts(enabled);
    return rc;
}
#pragma pop

void DefaultCallback(void) {

}

void ResetProc(s32 chan);
s32 SIResetSteeringAsync(s32 chan, void *callback) {
    extern u8 __SISteering[];
    extern void DefaultCallback(void);
    s32 __SISteeringTransfer(s32, s32, s32, void *);
    u8 *steering = __SISteering + chan * 0x28;
    void *cb;
    s32 enabled;
    s32 ret;

    cb = callback ? callback : (void *)DefaultCallback;
    enabled = OSDisableInterrupts();
    if (*(void **)(steering + 0x14) != NULL) {
        ret = -2;
    } else {
        *(void **)(steering + 0x14) = cb;
        ret = 0;
    }
    OSRestoreInterrupts(enabled);
    if (ret != 0) {
        return ret;
    }
    *steering = 0xFF;
    return __SISteeringTransfer(chan, 1, 3, ResetProc);
}

void __SISteeringEnable(s32 chan) {
    s32 SIEnablePolling(u32 poll);
    s32 SIGetResponse(s32 chan, void *data);
    s32 SISetCommand(s32 chan, u32 command);
    extern u32 __SISteeringEnableBits;
    u32 data[2];
    u32 chanBit;

    chanBit = 0x80000000 >> chan;
    if (!(__SISteeringEnableBits & chanBit)) {
        __SISteeringEnableBits |= chanBit;
        SIGetResponse(chan, data);
        SISetCommand(chan, 0x300680);
        SIEnablePolling(__SISteeringEnableBits);
    }
}

f32 fn_8006FB1C(void) {
    M2C_ERROR(/* unknown instruction: ps_merge01 $f2, $f0, $f1 */);
    return M2C_ERROR(/* unknown instruction: ps_merge10 $f1, $f1, $f0 */);
}

s32 fn_8007350C(s32 arg0) {
    extern s32 autoInvalidation_803C6DA8;
    s32 temp_r0;

    temp_r0 = autoInvalidation_803C6DA8;
    autoInvalidation_803C6DA8 = arg0;
    return temp_r0;
}

s32 fn_8007385C(void) {
    return 0x80000000;
}

s32 fn_800777FC(void) {
    return *(s32 *)0xCC006C00 & 1;
}

s32 fn_800778EC(void) {
    return (((u32) *(u32 *)0xCC006C00 >> 6U) & 1) ^ 1;
}

s32 fn_800779D4(void) {
    return ((u32) *(u32 *)0xCC006C00 >> 1U) & 1;
}

u8 fn_80077A00(void) {
    return (u8) *(s32 *)0xCC006C04;
}

u8 fn_80077A2C(void) {
    return (u8) ((u32) *(u32 *)0xCC006C04 >> 8U);
}

void fn_80077BA0(void) {
    extern s32 lbl_803C73F0;
    lbl_803C73F0 = 0;
}

void fn_800780E4(void) {
    extern s32 lbl_803C743C;
    lbl_803C743C = 0;
}

s32 fn_800780F0(void) {
    return 0x4000;
}

s32 fn_800780F8(void) {
    extern s32 lbl_803C7424;
    return lbl_803C7424;
}

void fn_80078B8C(void) {

}

void fn_80078CC4(void) {
    extern s32 lbl_803C7464;
    lbl_803C7464 = 0;
}

void fn_800790F0(void *arg0) {
    extern void *lbl_803C7468;
    M2C_FIELD(arg0, void **, 8) = (void *) lbl_803C7468;
    lbl_803C7468 = arg0;
}

void fn_80079530(void) {
    extern s32 lbl_803C7474;
    extern s32 lbl_803C7470;
    lbl_803C7470 = 0;
    lbl_803C7474 = 0;
}

void fn_800798D4(s32 arg0, s32 arg1) {
    extern s32 lbl_803C7470;
    extern s32 lbl_803C7478;
    lbl_803C7470 = arg0;
    lbl_803C7478 = arg1;
}

void fn_800798E0(s32 arg0, s32 arg1) {
    extern s32 lbl_803C7474;
    extern s32 lbl_803C747C;
    lbl_803C7474 = arg0;
    lbl_803C747C = arg1;
}

s32 fn_800798EC(void) {
    extern s32 lbl_803C74B0;
    return lbl_803C74B0;
}

void fn_8007A004(void) {

}

void fn_8007A1F8(void) {
    extern s32 lbl_803C74D4;
    lbl_803C74D4 = 1;
}

void fn_8007A25C(void) {
    extern s32 lbl_803C74D8;
    lbl_803C74D8 = 1;
}

void fn_8007A6C0(s32 arg0) {
    extern s32 lbl_803C74D0;
    lbl_803C74D0 = arg0;
}

void fn_8007AAF8(void) {

}

s32 fn_8007AB90(void) {
    extern s32 lbl_803C7510;
    return lbl_803C7510;
}

void fn_8007B740(void) {

}

s32 fn_8007BC88(void) {
    return ((u16) *(u16 *)0xCC005000 >> 0xFU) & 1;
}

s32 fn_8007BC98(void) {
    return ((u16) *(u16 *)0xCC005004 >> 0xFU) & 1;
}

s32 fn_8007BD8C(void) {
    extern s32 lbl_803C7528;
    return lbl_803C7528;
}

s32 fn_8007BE1C(void) {
    return *(u16 *)0xCC00500A & 0x200;
}

void fn_8007C838(void) {

}

s32 fn_8007F0C0(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x88);
}

s32 fn_8007F4C4(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x84);
}

s32 fn_80081D8C(void) {
    return 0;
}

void fn_80084304(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
}

void fn_80085DEC(void) {
    *(s8 *)0xCC008000 = 0x48;
}

void fn_800874EC(void *arg0, f32 farg0, f32 farg1, f32 farg2) {
    M2C_FIELD(arg0, f32 *, 0x1C) = farg0;
    M2C_FIELD(arg0, f32 *, 0x20) = farg1;
    M2C_FIELD(arg0, f32 *, 0x24) = farg2;
}

void fn_800875CC(void *arg0, f32 farg0, f32 farg1, f32 farg2) {
    M2C_FIELD(arg0, f32 *, 0x28) = farg0;
    M2C_FIELD(arg0, f32 *, 0x2C) = farg1;
    M2C_FIELD(arg0, f32 *, 0x30) = farg2;
}

s32 fn_8008811C(void *arg0) {
    return (M2C_FIELD(arg0, s32 *, 0xC) << 5) & 0x03FFFFE0;
}

s32 fn_80088128(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x14);
}

void fn_8008A25C(void) {

}

void fn_8008B31C(void) {
    extern void *__cpReg;
    M2C_FIELD(__cpReg, s16 *, 4) = 4;
}

void fn_8008CC84(void) {

}

void TRKDestructEvent(void *arg0) {
    M2C_UNK fn_8008FD50(s32);
    fn_8008FD50(M2C_FIELD(arg0, s32 *, 8));
}

void TRKConstructEvent(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 0) = arg1;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = -1;
}

s32 TRKGetNextEvent(TRKEvent *event) {
    s32 result = 0;

    TRKAcquireMutex(&gTRKEventQueue);
    if (gTRKEventQueue.count > 0) {
        TRK_memcpy(event, &gTRKEventQueue.events[gTRKEventQueue.next], sizeof(TRKEvent));
        gTRKEventQueue.count--;
        if (++gTRKEventQueue.next == 2) {
            gTRKEventQueue.next = 0;
        }
        result = 1;
    }
    TRKReleaseMutex(&gTRKEventQueue);
    return result;
}

s32 TRKInitializeEventQueue(void) {
    TRKInitializeMutex(&gTRKEventQueue);
    TRKAcquireMutex(&gTRKEventQueue);
    gTRKEventQueue.count = 0;
    gTRKEventQueue.next = 0;
    gTRKEventQueue.eventID = 0x100;
    TRKReleaseMutex(&gTRKEventQueue);
    return 0;
}

void TRKNubWelcome(void) {
    TRK_board_display("MetroTRK for GAMECUBE v0.10");
}

void *TRK_memcpy(void *dst, const void *src, u32 n);
s32 TRKTerminateNub(void) {
    TRKTerminateSerialHandler();
    return 0;
}

#pragma push
#pragma use_lmw_stmw on
#pragma push
#pragma opt_propagation off
s32 TRKReadBuffer_ui8(TRKBuffer *b, u8 *data, s32 count) {
    s32 err;
    s32 i;

    for (i = 0, err = 0; err == 0 && i < count; i++) {
        err = TRKReadBuffer1_ui8_inline(b, &data[i]);
    }
    return err;
}
#pragma pop

s32 TRKReadBuffer1_ui16(TRKBuffer *b, u16 *data) {
    s32 err;
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)data;
    } else {
        bigEndianData = swapBuffer;
    }
    err = TRKReadBuffer_inline(b, bigEndianData, sizeof(*data));
    if (!gTRKBigEndian && err == 0) {
        byteData = (u8 *)data;
        byteData[0] = bigEndianData[1];
        byteData[1] = bigEndianData[0];
    }
    return err;
}

#pragma push
#pragma opt_propagation off
s32 TRKReadBuffer1_ui32(TRKBuffer *b, u32 *data) {
    s32 err;
    u32 length;
    u32 bytesLeft;
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)data;
    } else {
        bigEndianData = swapBuffer;
    }
    length = sizeof(*data);
    err = 0;
    bytesLeft = b->length - b->position;
    if (length > bytesLeft) {
        err = 0x302;
        length = bytesLeft;
    }
    TRK_memcpy(bigEndianData, b->data + b->position, length);
    b->position += length;
    if (!gTRKBigEndian && err == 0) {
        byteData = (u8 *)data;
        byteData[0] = bigEndianData[3];
        byteData[1] = bigEndianData[2];
        byteData[2] = bigEndianData[1];
        byteData[3] = bigEndianData[0];
    }
    return err;
}
#pragma pop

#pragma push
#pragma opt_propagation off
s32 TRKReadBuffer1_ui8(TRKBuffer *b, u8 *data) {
    return TRKReadBuffer_inline(b, data, 1);
}
#pragma pop

s32 TRKAppendBuffer_ui32(TRKBuffer *b, const u32 *data, s32 count) {
    s32 err;
    s32 i;

    for (i = 0, err = 0; err == 0 && i < count; i++) {
        err = TRKAppendBuffer1_ui32_inline(b, data[i]);
    }
    return err;
}
#pragma pop

s32 TRKAppendBuffer_ui8(TRKBuffer *b, const u8 *data, s32 count) {
    s32 err;
    s32 i;

    for (i = 0, err = 0; err == 0 && i < count; i++) {
        err = TRKAppendBuffer1_ui8(b, data[i]);
    }
    return err;
}

s32 TRKAppendBuffer1_ui64(TRKBuffer *b, u64 data) {
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)&data;
    } else {
        byteData = (u8 *)&data;
        bigEndianData = swapBuffer;
        bigEndianData[0] = byteData[7];
        bigEndianData[1] = byteData[6];
        bigEndianData[2] = byteData[5];
        bigEndianData[3] = byteData[4];
        bigEndianData[4] = byteData[3];
        bigEndianData[5] = byteData[2];
        bigEndianData[6] = byteData[1];
        bigEndianData[7] = byteData[0];
    }
    return TRKAppendBuffer(b, bigEndianData, sizeof(data));
}

#pragma push
#pragma auto_inline off
s32 TRKAppendBuffer1_ui32(void *b, u32 data) {
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)&data;
    } else {
        byteData = (u8 *)&data;
        bigEndianData = swapBuffer;
        bigEndianData[0] = byteData[3];
        bigEndianData[1] = byteData[2];
        bigEndianData[2] = byteData[1];
        bigEndianData[3] = byteData[0];
    }
    return TRKAppendBuffer(b, bigEndianData, sizeof(data));
}
#pragma pop

#pragma push
#pragma auto_inline off
s32 TRKAppendBuffer1_ui16(void *b, u16 data) {
    u8 *bigEndianData;
    u8 *byteData;
    u8 swapBuffer[sizeof(data)];

    if (gTRKBigEndian) {
        bigEndianData = (u8 *)&data;
    } else {
        byteData = (u8 *)&data;
        bigEndianData = swapBuffer;
        bigEndianData[0] = byteData[1];
        bigEndianData[1] = byteData[0];
    }
    return TRKAppendBuffer(b, bigEndianData, sizeof(data));
}
#pragma pop

s32 TRKReadBuffer(TRKBuffer *b, void *data, u32 length) {
    s32 error = 0;
    u32 bytesLeft;

    if (length == 0) {
        return 0;
    }
    bytesLeft = b->length - b->position;
    if (length > bytesLeft) {
        error = 0x302;
        length = bytesLeft;
    }
    TRK_memcpy(data, b->data + b->position, length);
    b->position += length;
    return error;
}

s32 TRKAppendBuffer(TRKBuffer *b, const void *data, u32 length) {
    s32 error = 0;
    u32 bytesLeft;

    if (length == 0) {
        return 0;
    }
    bytesLeft = 0x880 - b->position;
    if (bytesLeft < length) {
        error = 0x301;
        length = bytesLeft;
    }
    if (length == 1) {
        b->data[b->position] = *(const u8 *)data;
    } else {
        TRK_memcpy(b->data + b->position, data, length);
    }
    b->position += length;
    b->length = b->position;
    return error;
}

s32 TRKSetBufferPosition(void *arg0, u32 arg1) {
    s32 var_r5;

    var_r5 = 0;
    if (arg1 > 0x880U) {
        var_r5 = 0x301;
    } else {
        M2C_FIELD(arg0, u32 *, 0xC) = arg1;
        if (arg1 > (u32) M2C_FIELD(arg0, u32 *, 8)) {
            M2C_FIELD((s32)arg0, u32 *, 8) = arg1;
        }
    }
    return var_r5;
}

#pragma push
#pragma auto_inline off
void TRKResetBuffer(TRKBuffer *b, u8 keepData) {
    b->length = 0;
    b->position = 0;
    if (!keepData) {
        TRK_memset(b->data, 0, 0x880);
    }
}
#pragma pop

void *TRK_memset(void *dst, s32 val, u32 n);
void TRKReleaseBuffer(s32 idx) {
    extern TRKBuffer gTRKMsgBufs[3];
    TRKBuffer *b;

    if (idx == -1) {
        return;
    }
    if (idx >= 0 && idx < 3) {
        b = &gTRKMsgBufs[idx];
        TRKAcquireMutex(b);
        b->isInUse = 0;
        TRKReleaseMutex(b);
    }
}

TRKBuffer *TRKGetBuffer(s32 idx) {
    extern TRKBuffer gTRKMsgBufs[3];
    TRKBuffer *buf = NULL;

    if (idx >= 0 && idx < 3) {
        buf = &gTRKMsgBufs[idx];
    }
    return buf;
}

void usr_puts_serial(const char *msg);
#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
s32 TRKGetFreeBuffer(s32 *msgID, TRKBuffer **outMsg) {
    TRKBuffer *buf;
    s32 error = 0x300;
    s32 i;

    *outMsg = NULL;
    for (i = 0; i < 3; i++) {
        buf = TRKGetBuffer(i);
        TRKAcquireMutex(buf);
        if (!buf->isInUse) {
            buf->length = 0;
            buf->position = 0;
            buf->isInUse = 1;
            error = 0;
            *outMsg = buf;
            *msgID = i;
            i = 3;
        }
        TRKReleaseMutex(buf);
    }
    if (error == 0x300) {
        usr_puts_serial("ERROR : No buffer available\n");
    }
    return error;
}
#pragma pop

s32 TRKInitializeMessageBuffers(void) {
    extern TRKBuffer gTRKMsgBufs[3];
    s32 i;

    for (i = 0; i < 3; i++) {
        TRKInitializeMutex(&gTRKMsgBufs[i]);
        TRKAcquireMutex(&gTRKMsgBufs[i]);
        gTRKMsgBufs[i].isInUse = 0;
        TRKReleaseMutex(&gTRKMsgBufs[i]);
    }
    return 0;
}

s32 TRKTerminateSerialHandler(void) {
    return 0;
}

s32 TRKInitializeSerialHandler(void) {
    extern s32 gTRKFramingState_8030B424[];
    gTRKFramingState_8030B424[0] = -1;
    ((u8 *)gTRKFramingState_8030B424)[8] = 0;
    gTRKFramingState_8030B424[3] = 0;
    return 0;
}

#pragma push
#pragma dont_inline on
void TRKProcessInput(s32 bufferIdx) {
    extern s32 gTRKFramingState_8030B424[];
    TRKEvent event;

    TRKConstructEvent(&event, 2);
    event.msgBufID = bufferIdx;
    gTRKFramingState_8030B424[0] = -1;
    TRKPostEvent(&event);
}
#pragma pop

void fn_8008EF90(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 0) = arg1;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = -1;
}

s32 fn_8008FF1C(void) {
    return 0;
}

void usr_put_initialize(void) {

}

#pragma push
#pragma dont_inline on
void TRKGetInput(void) {
    extern s32 gTRKFramingState_8030B424[];
    TRKBuffer *msgBuffer;
    s32 id;
    u8 command;
    TRKEvent event;

    id = TRKTestForPacket();
    if (id != -1) {
        msgBuffer = TRKGetBuffer(id);
        TRKSetBufferPosition(msgBuffer, 0);
        TRKReadBuffer1_ui8(msgBuffer, &command);
        if (command < 0x80) {
            TRKConstructEvent(&event, 2);
            event.msgBufID = id;
            gTRKFramingState_8030B424[0] = -1;
            TRKPostEvent(&event);
        } else {
            TRKReleaseBuffer(id);
        }
    }
}
#pragma pop

void usr_puts_serial(const char *msg) {
    void OSReport(const char *);
    s32 connect_ = 0;
    char c;
    char buf[2];

    while (!connect_ && (c = *msg++) != '\0') {
        s32 connect = GetTRKConnected();
        buf[0] = c;
        buf[1] = '\0';
        SetTRKConnected(0);
        OSReport(buf);
        SetTRKConnected(connect);
        connect_ = 0;
    }
}

#pragma push
#pragma dont_inline on
s32 TRKDispatchMessage(TRKBuffer *buffer) {
    typedef struct DispatchEntry {
        s32 (*fn)(TRKBuffer *);
    } DispatchEntry;
    extern u32 gTRKDispatchTableSize[];
    extern DispatchEntry gTRKDispatchTable[];
    s32 error;
    u8 command;

    error = 0x500;
    TRKSetBufferPosition(buffer, 0);
    TRKReadBuffer1_ui8(buffer, &command);
    if (command < gTRKDispatchTableSize[0]) {
        error = gTRKDispatchTable[command].fn(buffer);
    }
    return error;
}
#pragma pop
s32 TRKReadBuffer1_ui8(TRKBuffer *b, u8 *data);
TRKBuffer *TRKGetBuffer(s32 idx);
void TRKReleaseBuffer(s32 idx);
s32 TRKTestForPacket(void);
s32 TRKInitializeDispatcher(void) {
    extern u32 gTRKDispatchTableSize[];
    gTRKDispatchTableSize[0] = 0x20;
    return 0;
}

s32 TRKDoReset(TRKBuffer *b) {
    void __TRK_reset(void);
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    __TRK_reset();
    return 0;
}

s32 TRKDoDisconnect(TRKBuffer *b) {
    extern GeneralGameSettings IsTRKConnected_8030B440;
    void TRKConstructEvent(void *arg0, s8 arg1);
    TRKEvent event;
    s32 ackTries;
    s32 err;

    IsTRKConnected_8030B440.difficulty = 0;
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    if (err == 0) {
        TRKConstructEvent(&event, 1);
        TRKPostEvent(&event);
    }
    return err;
}

void TRKDoConnect(TRKBuffer *b) {
    extern GeneralGameSettings IsTRKConnected_8030B440;
    s32 err;
    s32 ackTries;

    IsTRKConnected_8030B440.difficulty = 1;
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
}

void TRKDoUnsupported(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0x10);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
}

void TRKStandardACK(TRKBuffer *b, u8 commandID, u8 replyError) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, commandID);
    TRKAppendBuffer1_ui8(b, replyError);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
}

void SetTRKConnected(s32 connected) {
    extern GeneralGameSettings IsTRKConnected_8030B440;
    IsTRKConnected_8030B440.difficulty = connected;
}

extern GeneralGameSettings IsTRKConnected_8030B440;
s32 GetTRKConnected(void) {
    return IsTRKConnected_8030B440.difficulty;
}

s32 TRKReleaseMutex(void *mutex) {
    return 0;
}

s32 TRKAcquireMutex(void *mutex) {
    return 0;
}

s32 TRKInitializeMutex(void *mutex) {
    return 0;
}

void TRK_fill_mem_80093658(void *dst, int val, u32 n) {
    u32 i;
    u32 v = (u8)val;

    ((u8 *)dst) = ((u8 *)dst) - 1;
    if (n >= 32) {
        i = (~(u32)dst) & 3;
        if (i) {
            n -= i;
            do {
                *++(((u8 *)dst)) = v;
            } while (--i);
        }
        if (v) {
            v |= v << 24 | v << 16 | v << 8;
        }
        ((u32 *)dst) = ((u32 *)(((u8 *)dst) + 1)) - 1;
        i = n >> 5;
        if (i) {
            do {
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
                *++(((u32 *)dst)) = v;
            } while (--i);
        }
        i = (n & 31) >> 2;
        if (i) {
            do {
                *++(((u32 *)dst)) = v;
            } while (--i);
        }
        ((u8 *)dst) = ((u8 *)(((u32 *)dst) + 1)) - 1;
        n &= 3;
    }
    if (n) {
        do {
            *++(((u8 *)dst)) = v;
        } while (--n);
    }
}

void TRKTargetSetInputPendingPtr(void *ptr) {
    extern u8 gTRKState[];
    *(void **)(gTRKState + 0xA0) = ptr;
}

s32 TRKTargetStop(void) {
    extern u8 gTRKState[];
    *(s32 *)(gTRKState + 0x98) = 1;
    return 0;
}

void TRKTargetSetStopped(s32 stopped) {
    extern u8 gTRKState[];
    *(s32 *)(gTRKState + 0x98) = stopped;
}

s32 TRKTargetStopped(void) {
    extern u8 gTRKState[];
    return *(s32 *)(gTRKState + 0x98);
}

s32 TRKTargetFlushCache(u8 space, u32 start, u32 end) {
    void TRK_flush_cache(u32 start, u32 length);

    if (start < end) {
        TRK_flush_cache(start, end - start);
        return 0;
    }
    return 0x700;
}

u32 TRKTargetGetPC(void) {
    extern u8 gTRKCPUState[];
    return *(u32 *)(gTRKCPUState + 0x80);
}

s32 TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, s32 stepOver) {
    s32 err = 0;

    if (stepOver) {
        err = 0x703;
    } else {
        gTRKStepStatus_800DA0B4.type = 1;
        gTRKStepStatus_800DA0B4.rangeStart = rangeStart;
        gTRKStepStatus_800DA0B4.rangeEnd = rangeEnd;
        err = TRKTargetDoStep();
    }
    return err;
}

s32 TRKTargetSingleStep(u32 count, s32 stepOver) {
    s32 err = 0;

    if (stepOver) {
        err = 0x703;
    } else {
        gTRKStepStatus_800DA0B4.type = 0;
        gTRKStepStatus_800DA0B4.count = count;
        err = TRKTargetDoStep();
    }
    return err;
}

s32 TRKTargetAddExceptionInfo(void *b) {
    extern u32 gTRKExceptionStatus_800DA0A4[];
    s32 err;
    u32 inst;
    u32 size;

    err = TRKAppendBuffer1_ui32(b, gTRKExceptionStatus_800DA0A4[0]);
    if (err == 0) {
        size = 4;
        err = TRKTargetAccessMemory(&inst, gTRKExceptionStatus_800DA0A4[0], &size, 0, 1);
        if (err == 0 && size != 4) {
            err = 0x700;
        }
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui32(b, inst);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui16(b, *(u16 *)&gTRKExceptionStatus_800DA0A4[2]);
    }
    return err;
}

#pragma push
#pragma dont_inline on
void TRKPostInterruptEvent(void) {
    extern u8 gTRKState[];
    extern u8 gTRKCPUState[];
    s8 eventType;
    u32 inst;
    u32 size;
    u8 event[0x10];

    if (*(s32 *)(gTRKState + 0x9C)) {
        *(s32 *)(gTRKState + 0x9C) = 0;
    } else {
        switch (*(u32 *)(gTRKCPUState + 0x2F8) & 0xFFFF) {
        case 0xD00:
        case 0x700:
            size = 4;
            TRKTargetAccessMemory(&inst, *(u32 *)(gTRKCPUState + 0x80), &size, 0, 1);
            if (inst == 0x0FE00000) {
                eventType = 5;
            } else {
                eventType = 3;
            }
            break;
        default:
            eventType = 4;
            break;
        }
        TRKConstructEvent(event, eventType);
        TRKPostEvent(event);
    }
}
#pragma pop

__declspec(section ".data") extern s32 gTRKBigEndian;
#pragma push
#pragma dont_inline on
s32 TRKTargetCPUType(DSCPUType *cpuType) {
    cpuType->cpuMajor = 0;
    cpuType->cpuMinor = TRKTargetCPUMinorType();
    cpuType->bigEndian = gTRKBigEndian;
    cpuType->defaultTypeSize = 4;
    cpuType->fpTypeSize = 8;
    cpuType->extended1TypeSize = 4;
    cpuType->extended2TypeSize = 8;
    return 0;
}
#pragma pop

s32 TRKTargetSupportMask(u8 *mask) {
    mask[0] = 0x7A;
    mask[1] = 0;
    mask[2] = 0x4F;
    mask[3] = 7;
    mask[4] = 0;
    mask[5] = 0;
    mask[6] = 0;
    mask[7] = 0;
    mask[8] = 0;
    mask[9] = 0;
    mask[0xA] = 0;
    mask[0xB] = 0;
    mask[0xC] = 0;
    mask[0xD] = 0;
    mask[0xE] = 0;
    mask[0xF] = 0;
    mask[0x10] = 1;
    mask[0x11] = 0;
    mask[0x12] = 3;
    mask[0x13] = 0;
    mask[0x14] = 0;
    mask[0x15] = 0;
    mask[0x16] = 0;
    mask[0x17] = 0;
    mask[0x18] = 0;
    mask[0x19] = 0;
    mask[0x1A] = 3;
    mask[0x1B] = 0;
    mask[0x1C] = 0;
    mask[0x1D] = 0;
    mask[0x1E] = 0;
    mask[0x1F] = 0x80;
    return 0;
}

s32 TRKTargetVersions(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0) = 0;
    M2C_FIELD(arg0, s8 *, 1) = 0xA;
    M2C_FIELD(arg0, s8 *, 2) = 1;
    M2C_FIELD(arg0, s8 *, 3) = 0xA;
    return 0;
}

s32 fn_80093530(void) {
    return 0;
}

s32 fn_80093538(void) {
    return 0;
}

s32 fn_80093540(void) {
    return 0;
}

s32 fn_80094538(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0) = 0;
    M2C_FIELD(arg0, s8 *, 1) = 0xA;
    M2C_FIELD(arg0, s8 *, 2) = 1;
    M2C_FIELD(arg0, s8 *, 3) = 0xA;
    return 0;
}

s32 TRKInitializeTarget(void) {
    extern u8 gTRKState[];
    extern u32 lc_base_8030B9C8[];
    u32 __TRK_get_MSR(void);

    *(s32 *)(gTRKState + 0x98) = 1;
    *(u32 *)(gTRKState + 0x8C) = __TRK_get_MSR();
    lc_base_8030B9C8[0] = 0xE0000000;
    return 0;
}

u32 TRKTargetTranslate(u32 addr) {
    extern u8 gTRKCPUState[];
    extern u32 lc_base_8030B9C8[];

    if (addr >= lc_base_8030B9C8[0] && addr < lc_base_8030B9C8[0] + 0x4000 &&
        (*(u32 *)(gTRKCPUState + 0x238) & 3)) {
        return addr;
    }
    return (addr & 0x3FFFFFFF) | 0x80000000;
}

void EnableMetroTRKInterrupts(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 TRKTargetCPUMinorType(void) {
    return 0x54;
}

void TRK_main(void) {
    s32 TRKInitializeNub(void);
    void TRKNubWelcome(void);
    void TRKNubMainLoop(void);
    s32 TRKTerminateNub(void);
    extern s32 TRK_mainError_8030B9D0[];

    TRK_mainError_8030B9D0[0] = TRKInitializeNub();
    if (TRK_mainError_8030B9D0[0] == 0) {
        TRKNubWelcome();
        TRKNubMainLoop();
    }
    TRK_mainError_8030B9D0[0] = TRKTerminateNub();
}

s32 fn_80095480(void) {
    return 0x54;
}

void TRKUARTInterruptHandler(void) {

}

void TRK_board_display(char *str) {
    void OSReport(const char *, ...);
    OSReport(str);
}

void UnreserveEXI2Port(void) {
    extern void (*gDBCommTable[7])(void);
    gDBCommTable[6]();
}

void ReserveEXI2Port(void) {
    extern void (*gDBCommTable[7])(void);
    gDBCommTable[5]();
}

s32 WriteUART1(u8 c) {
    extern u8 gWriteBuf_8030CAF4[];
    extern s32 gWritePos_8030B9D8[];
    gWriteBuf_8030CAF4[gWritePos_8030B9D8[0]++] = c;
    return 0;
}

void EnableEXI2Interrupts(void) {
    extern void (*gDBCommTable[7])(void);
    gDBCommTable[1]();
}

s32 TRKInitializeIntDrivenUART(u32 param1, u32 param2, u32 param3, volatile u8 **inputPendingPtrRef) {
    extern void (*gDBCommTable[7])(void);
    void TRKEXICallBack(s16 chan, void *context);
    ((void (*)(volatile u8 **, void *))gDBCommTable[0])(inputPendingPtrRef, TRKEXICallBack);
    return 0;
}

void TRKEXICallBack(s16 chan, void *ctx) {
    OSEnableScheduler();
    TRKLoadContext(ctx, 0x500);
}

s32 TRKTargetContinue(void) {
    M2C_UNK ReserveEXI2Port();
    M2C_UNK TRKSwapAndGo();
    M2C_UNK TRKTargetSetStopped(M2C_UNK);
    M2C_UNK UnreserveEXI2Port();
    TRKTargetSetStopped(0);
    UnreserveEXI2Port();
    TRKSwapAndGo();
    ReserveEXI2Port();
    return 0;
}

#pragma push
#pragma auto_inline off
u8 GetUseSerialIO(void) {
    extern u8 bUseSerialIO_8030DC00[];
    return bUseSerialIO_8030DC00[0];
}
#pragma pop

void SetUseSerialIO(u8 sio) {
    extern u8 bUseSerialIO_8030DC00[];
    bUseSerialIO_8030DC00[0] = sio;
}

s32 __close_console(s32 handle) {
    s32 GetTRKConnected();
    u8 fn_80094FC0(s32, s32);

    if (GetTRKConnected() == 0) {
        return 1;
    }
    switch (fn_80094FC0(0xD3, handle)) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

s32 __TRK_write_console(s32 handle, u8 *buffer, u32 *count, void *idle) {
    s32 GetTRKConnected();
    u8 fn_80094FB0(s32, s32, u32 *, u8 *);
    u32 length;
    u8 result;

    if (GetUseSerialIO() == 0) {
        return 1;
    }
    if (GetTRKConnected() == 0) {
        return 1;
    }
    length = *count;
    result = fn_80094FB0(0xD0, 1, &length, buffer);
    *count = length;
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

s32 __read_console(s32 handle, u8 *buffer, u32 *count, void *idle) {
    s32 GetTRKConnected();
    u8 fn_80094FB0(s32, s32, u32 *, u8 *);
    u32 length;
    u8 result;

    if (GetUseSerialIO() == 0) {
        return 1;
    }
    if (GetTRKConnected() == 0) {
        return 1;
    }
    length = *count;
    result = fn_80094FB0(0xD1, 0, &length, buffer);
    *count = length;
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

void *__va_arg(void *ap, s32 type) {
    typedef struct {
        char gpr;
        char fpr;
        char reserved[2];
        char *input_arg_area;
        char *reg_save_area;
    } _va_list_struct;
    char *addr;
    char *reg = &((_va_list_struct *)ap)->gpr;
    s32 g_reg = ((_va_list_struct *)ap)->gpr;
    s32 maxsize = 8;
    s32 size = 4;
    s32 increment = 1;
    s32 even = 0;
    s32 fpr_offset = 0;
    s32 regsize = 4;
    _va_list_struct *list = ap;

    if (type == 3) {
        reg = &list->fpr;
        g_reg = list->fpr;
        size = 8;
        fpr_offset = 32;
        regsize = 8;
    }
    if (type == 2) {
        size = 8;
        maxsize--;
        if (g_reg & 1) {
            even = 1;
        }
        increment = 2;
    }
    if (g_reg < maxsize) {
        g_reg += even;
        addr = list->reg_save_area + fpr_offset + g_reg * regsize;
        *reg = g_reg + increment;
    } else {
        *reg = 8;
        addr = list->input_arg_area;
        addr = (char *)(((u32)addr + (size - 1)) & ~(size - 1));
        list->input_arg_area = addr + size;
    }
    if (type == 0) {
        addr = *(char **)addr;
    }
    return addr;
}

