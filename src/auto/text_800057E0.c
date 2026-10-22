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

void *ThingManagerList_GetByType(void *list, s32 type);
void WildCardRack_Add(void *rack, u8 *item, s16 count);
void Item_Release(void *item);
void MotionState_Draw(void *state);

s32 AIGetStreamPlayState(void);
s32 AIGetDSPSampleRate(void);
s32 AIGetStreamSampleRate(void);
u8 AIGetStreamVolLeft(void);
u8 AIGetStreamVolRight(void);

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

typedef struct GeneralGameSettings {
    s32 difficulty;
    s32 unk4;
    ControllerSettings controllers[18];
    u8 unk14C[4];
} GeneralGameSettings;

extern GeneralGameSettings g_generalGameSettings;

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
s32 TRKInitializeMutex(void *mutex);
s32 TRKAcquireMutex(void *mutex);
s32 TRKReleaseMutex(void *mutex);
void *TRK_memcpy(void *dst, const void *src, u32 n);

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

s32 TRKTargetCPUMinorType(void);

typedef struct SoundSource {
    u8 unk0[0x2C];
    s32 voice;
    s32 looping;
    u8 unk34[0x50 - 0x34];
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
    u32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

void TRKResetBuffer(TRKBuffer *b, u8 keepData);
s32 TRKMessageSend(TRKBuffer *b);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
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
    u8 unk84[0x88 - 0x84];
    void *buffer;
    u8 unk8C[0xBE - 0x8C];
    u16 startBlock;
    u8 unkC0[0xD0 - 0xC0];
    CARDCallback apiCallback;
    u8 unkD4[0xD8 - 0xD4];
    CARDCallback eraseCallback;
    u8 unkDC[0xE0 - 0xDC];
    u8 alarm[0x28];
    u8 unk108[0x110 - 0x108];
} CARDControl;

extern CARDControl __CARDBlock[2];

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
    extern M2C_UNK m_Actions_800DBC80;
    M2C_UNK *var_r4;
    s32 var_ctr;

    var_r4 = &m_Actions_800DBC80;
    var_ctr = 2;
    do {
        M2C_FIELD(var_r4, s32 *, 0) = 0;
        M2C_FIELD(var_r4, s32 *, 4) = 0;
        M2C_FIELD(var_r4, s32 *, 8) = 0;
        M2C_FIELD(var_r4, s32 *, 0xC) = 0;
        M2C_FIELD(var_r4, s32 *, 0x10) = 0;
        M2C_FIELD(var_r4, s32 *, 0x14) = 0;
        M2C_FIELD(var_r4, s32 *, 0x18) = 0;
        M2C_FIELD(var_r4, s32 *, 0x1C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x20) = 0;
        M2C_FIELD(var_r4, s32 *, 0x24) = 0;
        M2C_FIELD(var_r4, s32 *, 0x28) = 0;
        M2C_FIELD(var_r4, s32 *, 0x2C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x30) = 0;
        M2C_FIELD(var_r4, s32 *, 0x34) = 0;
        M2C_FIELD(var_r4, s32 *, 0x38) = 0;
        M2C_FIELD(var_r4, s32 *, 0x3C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x40) = 0;
        M2C_FIELD(var_r4, s32 *, 0x44) = 0;
        M2C_FIELD(var_r4, s32 *, 0x48) = 0;
        M2C_FIELD(var_r4, s32 *, 0x4C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x50) = 0;
        M2C_FIELD(var_r4, s32 *, 0x54) = 0;
        M2C_FIELD(var_r4, s32 *, 0x58) = 0;
        M2C_FIELD(var_r4, s32 *, 0x5C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x60) = 0;
        M2C_FIELD(var_r4, s32 *, 0x64) = 0;
        M2C_FIELD(var_r4, s32 *, 0x68) = 0;
        M2C_FIELD(var_r4, s32 *, 0x6C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x70) = 0;
        M2C_FIELD(var_r4, s32 *, 0x74) = 0;
        M2C_FIELD(var_r4, s32 *, 0x78) = 0;
        M2C_FIELD(var_r4, s32 *, 0x7C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x80) = 0;
        M2C_FIELD(var_r4, s32 *, 0x84) = 0;
        M2C_FIELD(var_r4, s32 *, 0x88) = 0;
        M2C_FIELD(var_r4, s32 *, 0x8C) = 0;
        M2C_FIELD(var_r4, s32 *, 0x90) = 0;
        M2C_FIELD(var_r4, s32 *, 0x94) = 0;
        M2C_FIELD(var_r4, s32 *, 0x98) = 0;
        M2C_FIELD(var_r4, s32 *, 0x9C) = 0;
        var_r4 += 0xA0;
        var_ctr -= 1;
    } while (var_ctr != 0);
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

void SoundListener_SetTotalListeners(u32 arg0) {
    extern s32 g_numListeners;
    if (arg0 != 0U) {
        g_numListeners = arg0 - 1;
    }
}

s8 SoundSource_IsActive(s32 index) {
    extern SoundSource *g_sources_803C6F38;
    return g_sources_803C6F38[index].voice != -1;
}

s8 SoundSource_IsLooping(s32 index) {
    extern SoundSource *g_sources_803C6F38;
    return g_sources_803C6F38[index].looping;
}

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

void CarA_GetInverseLocalInertiaTensor(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

f32 CarA_GetMass(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), f32 *, 0x20);
}

s32 CarA_GetSurfaceScript(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), s32 *, 0x30);
}

s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
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

f32 Debris_GetMass(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0xCC);
}

f32 Destructible_GetMass(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), f32 *, 0x20);
}

s32 Destructible_GetSurfaceScript(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), s32 *, 0x30);
}

s32 Destructible_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
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

u8 IsLoadingThreadRunning(void) {
    extern u8 g_bLoadingThreadRunning;
    return g_bLoadingThreadRunning;
}

s32 FileStreaming_GetNumAvailableQuadPacks(void) {
    return 0xB;
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

void Geometry_Destroy(void) {
    M2C_UNK Geometry_Init(M2C_UNK);
    extern s32 g_pGeometryHandler;
    Geometry_Init(1);
    Deallocate(g_pGeometryHandler);
    g_pGeometryHandler = 0;
}

void Grid_Release(void) {

}

void InterfaceManager_SetRadarBlip(u16 arg0) {
    extern s32 g_radarTextureIndex;
    g_radarTextureIndex = (s32) arg0;
}

void InterfaceManager_SetControllerConfigInputString(void) {

}

s32 ActionInterfacePlay(s32 *arg0) {
    M2C_UNK InterfaceManager_Create(s32, M2C_UNK, M2C_UNK);
    InterfaceManager_Create(*arg0, 0, 0);
    return 1;
}

s32 Item_PreMove(void) {
    return 1;
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

s32 LogHandler_Create(void) {
    return 0;
}

void LogHandler_Destroy(void) {

}

void Looker_Draw(void) {

}

void MatrixCache_Init(void) {

}

extern MatrixCacheData *g_pMatrixCacheData;
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

void Mover_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 Player_InputHeartbeat(void *arg0) {
    return M2C_FIELD(arg0, s8 (**)(), 0x20)() != 0;
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

s32 Player_NULL(void) {
    return 0;
}

void ProcessManager_Create(void) {

}

char *strcpy(char *dst, const char *src);
void FileStreaming_HeartBeat(void);
s32 Playback_GetElement(void);
void Playback_PutElement(s32 element);
void ProcessManager_Init(void) {
    g_processManager.count = 0;
}

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

void ShowStats_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void ShowStats_Release(void) {
    M2C_UNK Gfx_FontManager_ReleaseFont(s32);
    extern s32 debugFont;
    Gfx_FontManager_ReleaseFont(debugFont);
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

void ShowStats(void) {

}

s32 SoundSeq_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void ExitStateGamePlay(s32 arg0) {
    extern s8 g_bExitStateGamePlay;
    extern s32 g_targetGameState;
    g_targetGameState = arg0;
    g_bExitStateGamePlay = 1;
}

void GamePlay_Interface(void *arg0) {
    M2C_UNK GamePlay_Startup();
    extern M2C_UNK g_handleInterfaceGP;
    u16 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u16 *, 4);
    switch (temp_r0) {                        /* irregular */
    case 3:
        GamePlay_Startup();
        if (((M2C_UNK (*)(void *)) M2C_FIELD(&g_handleInterfaceGP, M2C_UNK (**)(void *), 0)) != (short)NULL) {
            M2C_FIELD(&g_handleInterfaceGP, M2C_UNK (**)(void *), 0)(arg0);
            return;
        }
        return;
    case 1:
        if (((M2C_UNK (*)()) M2C_FIELD(&g_handleInterfaceGP, M2C_UNK (**)(), 4)) != NULL) {
            M2C_FIELD(&g_handleInterfaceGP, M2C_UNK (**)(), 4)();
            return;
        }
        break;
    default:
    case 2:
        if (((M2C_UNK (*)(void *)) M2C_FIELD(&g_handleInterfaceGP, M2C_UNK (**)(void *), 0)) != NULL) {
            M2C_FIELD(&g_handleInterfaceGP, M2C_UNK (**)(void *), 0)(arg0);
        }
        break;
    }
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

void Thing_InitAllSlavesNow(s32 arg0) {
    M2C_UNK ThingManagerList_InitAllSlavesNow(s32);
    ThingManagerList_InitAllSlavesNow(arg0 + 0x88);
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

void Thing_CollideWithRay(void) {

}

s32 Thing_GetOrientedCollision(void) {
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

void ThingManager_DARYL_Init(void) {
    M2C_UNK ThingManager_DARYL_Release();
    M2C_UNK MatrixCache_Init();
    ThingManager_DARYL_Release();
    MatrixCache_Init();
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

void Trail_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

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

s32 TriggerLevelStart(void) {
    return 1;
}

s32 TriggerInterfaceEnd(void) {
    return 1;
}

s32 TriggerCreatureKilled(void) {
    return 0;
}

s32 TriggerCreatureDroppedOff(void) {
    return 0;
}

void VertexColorSeqManager_Create(void) {

}

void VertexColorSeqManager_Destroy(void) {
    extern s32 g_iTotalVertexColorSeq;
    extern s32 g_iTotalVertexColorTiles;
    g_iTotalVertexColorSeq = 0;
    g_iTotalVertexColorTiles = 0;
}

void VolumeManager_Create(void) {
    extern s32 g_VolumeHolder;
    g_VolumeHolder = 0;
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

void World_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
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

void PlayerManager_Create(void) {

}

void PlayerManager_Destroy(void) {

}

void PlayerManager_Render_DEBUG(void) {

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

s32 Tracer_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void StateGameManager_Release(void) {
    M2C_UNK FileStreaming_ReleaseAllDataPacks(M2C_UNK, M2C_UNK);
    FileStreaming_ReleaseAllDataPacks(3, 1);
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

void GameRules_ForceRoundEnd(u32 arg0) {
    extern s8 g_bRoundEnd_803C71CC;
    extern u32 g_playerNumThatWon_803C71C8;
    g_bRoundEnd_803C71CC = 1;
    g_playerNumThatWon_803C71C8 = arg0;
    if ((arg0 < 1U) && (arg0 > 4U)) {
        g_playerNumThatWon_803C71C8 = -1U;
    }
}

void GSI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Motion_Release(s32 arg0) {
    MotionState_Release(arg0 + 0xB0);
    Thing_Release(arg0);
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

void OSCreateAlarm(s32 *arg0) {
    *arg0 = 0;
}

s32 OSSetCurrentHeap(s32 arg0) {
    extern s32 __OSCurrHeap;
    s32 temp_r0;

    temp_r0 = __OSCurrHeap;
    __OSCurrHeap = arg0;
    return temp_r0;
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
    M2C_UNK __LCEnable();
    s32 temp_r31;

    temp_r31 = OSDisableInterrupts();
    __LCEnable();
    OSRestoreInterrupts(temp_r31);
}

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

OSSram *__OSLockSram(void) {
    return LockSram(0);
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

void __OSReschedule(void) {
    M2C_UNK SelectThread(M2C_UNK);
    extern s32 RunQueueHint_803C7294;
    if ((s32) RunQueueHint_803C7294 != 0) {
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

f32 PSVECDotProduct(void) {
    M2C_ERROR(/* unknown instruction: ps_mul $f2, $f2, $f3 */);
    M2C_ERROR(/* unknown instruction: ps_madd $f3, $f5, $f4, $f2 */);
    return M2C_ERROR(/* unknown instruction: ps_sum0 $f1, $f3, $f2, $f2 */);
}

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

void SIProbe(void) {
    M2C_UNK SIDecodeType();
    M2C_UNK SIGetType();
    SIGetType();
    SIDecodeType();
}

void SIRefreshSamplingRate(void) {
    M2C_UNK SISetSamplingRate(s32);
    extern s32 SamplingRate_803C75A0;
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

void stateGoToRetry(void) {
    M2C_UNK DVDLowStopMotor(void (*)(u32));
    void cbForStateGoToRetry(u32);
    DVDLowStopMotor(cbForStateGoToRetry);
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

void cbForCancelSync(void) {
    M2C_UNK OSWakeupThread(M2C_UNK *);
    extern M2C_UNK __DVDThreadQueue;
    OSWakeupThread(&__DVDThreadQueue);
}

s32 DVDGetCurrentDiskID(void) {
    return 0x80000000;
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

u8 AIGetStreamVolRight(void) {
    return (u8) ((u32) *(u32 *)0xCC006C04 >> 8U);
}

void AIReset(void) {
    extern s32 __AI_init_flag_803C73F0;
    __AI_init_flag_803C73F0 = 0;
}

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

void __ARQCallbackHack(void) {

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

s32 __AXGetNumVoices(void) {
    extern s32 __AXNumVoices_803C7510;
    return __AXNumVoices_803C7510;
}

void *__AXGetPBs(void) {
    extern u8 __AXPB_802F78A0[];
    return __AXPB_802F78A0;
}

void __AXVPBQuit(void) {

}

s32 __AXGetCurrentProfile(void)
{
  extern u32 __AXCurrentProfile_803C7520;
  extern u32 __AXMaxProfiles_803C751C;
  extern u32 __AXProfileInitialized_803C7524;
  extern s32 __AXProfile_803C7518;
  u32 temp_r3;
  s32 new_var;
  if (((u32) __AXProfileInitialized_803C7524) != 0U)
  {
    temp_r3 = __AXCurrentProfile_803C7520;
    __AXCurrentProfile_803C7520 = temp_r3 + 1;
    __AXCurrentProfile_803C7520 %= __AXMaxProfiles_803C751C;
    return (new_var = __AXProfile_803C7518) + (temp_r3 * 0x38);
  }
  return 0;
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

volatile u16 __DSPRegs[32] : 0xCC005000;
u32 DSPReadMailFromDSP(void) {
    return (((volatile u16 *)0xCC005000)[2] << 16) | ((volatile u16 *)0xCC005000)[3];
}

void DSPSendMailToDSP(u32 mail) {
    volatile u16 *regs = (volatile u16 *)0xCC000000;
    regs[0x2800] = mail >> 16;
    regs[0x2801] = mail;
}

s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag_803C7528;
    return __DSP_init_flag_803C7528;
}

s32 DSPGetDMAStatus(void) {
    return *(u16 *)0xCC00500A & 0x200;
}

void __DSP_debug_printf(char *str, ...) {
}

void __CARDDefaultApiCallback(void) {

}

s32 OnReset_8007DB90(s32 arg0) {
    s32 CARDUnmount(M2C_UNK);
    if ((arg0 == 0) && ((CARDUnmount(0) == -1) || (CARDUnmount(1) == -1))) {
        return 0;
    }
    return 1;
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

s32 __CARDIsPublic(void *arg0) {
    if ((u8) M2C_FIELD(arg0, u8 *, 0) == 0xFF) {
        return -4;
    }
    if (M2C_FIELD(arg0, u8 *, 0x34) & 4) {
        return 0;
    }
    return -0xA;
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

void GXInvalidateVtxCache(void) {
    *(s8 *)0xCC008000 = 0x48;
}

void GXPokeAlphaUpdate(s32 arg0) {
    extern void *__peReg;
    M2C_FIELD(__peReg, u16 *, 2) = (u16) ((M2C_FIELD(__peReg, u16 *, 2) & 0xFFFFFFEF) | ((arg0 * 0x10) & 0xFF0));
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

s32 GXGetTexObjData(void *arg0) {
    return (M2C_FIELD(arg0, s32 *, 0xC) << 5) & 0x03FFFFE0;
}

s32 GXGetTexObjFmt(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x14);
}

void GXInitTlutRegion(s32 *arg0, s32 arg1, s32 arg2) {
    *arg0 = 0;
    *arg0 = (*arg0 & 0xFFFFFC00) | ((u32) (arg1 + 0xFFF80000) >> 9U);
    *arg0 = (*arg0 & 0xFFE003FF) | (arg2 << 0xA);
    *arg0 = (*arg0 & 0xFFFFFF) | 0x65000000;
}

void __GXSetRange(void) {

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

void DefaultCallback(void) {

}

void __SISteeringEnable(s32 arg0)
{
  int new_var2;
  M2C_UNK SIEnablePolling(s32);
  M2C_UNK SIGetResponse(s32, M2C_UNK *);
  int new_var;
  M2C_UNK SISetCommand(s32, M2C_UNK);
  extern s32 __SISteeringEnableBits;
  M2C_UNK spC;
  u32 temp_r4;
  temp_r4 = 0x80000000U >> arg0;
  new_var2 = !(__SISteeringEnableBits & temp_r4);
  if (new_var2)
  {
    __SISteeringEnableBits |= temp_r4;
    SIGetResponse(arg0, &spC);
    new_var = 0x300680;
    SISetCommand(arg0, new_var);
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

s32 TRKTerminateSerialHandler(void) {
    return 0;
}

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

void EnableMetroTRKInterrupts(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 TRKTargetCPUMinorType(void) {
    return 0x54;
}

s32 fn_80095480(void) {
    return 0x54;
}

void TRKUARTInterruptHandler(void) {

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

u8 GetUseSerialIO(void) {
    extern u8 bUseSerialIO_8030DC00[];
    return bUseSerialIO_8030DC00[0];
}

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

