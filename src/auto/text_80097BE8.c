/* Functions from 0x80097BE8 to 0x800CC320, found by decomp-toolkit. */
#include "common.h"

#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

void *GetRenderWindow();
s32 OSSendMessage(void *queue, void *msg, s32 flags);
s32 OSReceiveMessage(void *queue, void **msg, s32 flags);
u32 MemCpyA_(u32 dst, u32 src, u32 len);

typedef struct Vector {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector;

f32 PSVECMag(Vector *v);

typedef union PPCWGPipe {
    u8 u8;
    s8 s8;
    u16 u16;
    s16 s16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

static inline void GXPosition2f32(f32 x, f32 y) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
}

static inline void GXColor3u8(const u8 r, const u8 g, const u8 b) {
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
}

typedef struct CARDFileInfo {
    s32 chan;
    s32 fileNo;
    s32 offset;
    s32 length;
    u16 iBlock;
    u16 pad;
} CARDFileInfo;

s32 CARDMount(s32 chan, void *workArea, void *detachCallback);
s32 CARDUnmount(s32 chan);
s32 CARDDelete(s32 chan, const char *fileName);
s32 CARDClose(CARDFileInfo *fileInfo);
s32 CARDRead(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset);

#define SIGDIGLEN 36

typedef struct decimal {
    char sign;
    char unk1;
    s16 exp;
    struct {
        u8 length;
        u8 text[SIGDIGLEN];
        u8 unk25;
    } sig;
} decimal;

static void __dorounddecup(decimal *d, int digits) {
    u8 *b = d->sig.text;
    u8 *i = b + digits - 1;

    while (1) {
        if (*i < 9) {
            *i += 1;
            break;
        }
        if (i == b) {
            *i = 1;
            d->exp++;
            break;
        }
        *i-- = 0;
    }
}

extern u8 CardWorkArea[0xA000];
extern CARDFileInfo memCardfileInfo[2];
extern s32 memCardBlockSize[2];
extern s32 m_memcardstatus_803C7970[2];

s32 MemCard_Write__FUllPv(u32 id, s32 size, void *data);

typedef struct FileEntry {
    s32 id;
    u8 unk4[0x14 - 0x4];
    s32 position;
    s8 slot;
    u8 unk19[0x1C - 0x19];
} FileEntry;

extern FileEntry g_files[4];
extern char g_appInfo[5][0x40];

typedef struct THPVideoInfo {
    u32 xSize;
    u32 ySize;
} THPVideoInfo;

typedef struct THPPlayer {
    u8 fileInfo[0x3C];
    u8 unk3C[0x44 - 0x3C];
    u32 bufSize;
    u32 audioMaxSamples;
    u8 unk4C[0x50 - 0x4C];
    u32 numFrames;
    u8 unk54[0x58 - 0x54];
    u32 movieDataSize;
    u8 unk5C[0x6C - 0x5C];
    u32 numComponents;
    u8 frameComp[16];
    THPVideoInfo videoInfo;
    u8 unk88[0x10];
    s32 open;
    u8 state;
    u8 unk9D;
    u8 playFlag;
    u8 audioExist;
    void *dispTextureSet;
    void *playAudioBuffer;
    s32 onMemory;
    u8 *movieData;
    u8 unkB0[0xB4 - 0xB0];
    s32 initReadSize;
    s32 initReadFrame;
    u8 unkBC[0xC0 - 0xBC];
    s32 unkC0;
    s32 retraceCount;
    s32 prevCount;
    s32 curCount;
    u8 unkD0[0x198 - 0xD0];
} THPPlayer;

typedef struct THPFileInfo {
    u8 unk0[0x69C];
    u8 *c;
    u32 currByte;
    u32 cnt;
    u8 validHuffmanTabs;
    u8 RST;
    u16 nMCU;
    u16 currMCU;
} THPFileInfo;

extern THPFileInfo *__THPInfo_803C7890;
extern u8 *__THPHuffmanSizeTab_803C7828;
extern u16 *__THPHuffmanCodeTab_803C782C;
extern void *OldVIPostCallback_803C775C;

void *PopUsedTextureSet(void);

typedef struct THPReadBuffer {
    u8 *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct THPAudioBuffer {
    s16 *buffer;
    s16 *curPtr;
    u32 validSample;
    s32 frameNumber;
} THPAudioBuffer;

THPPlayer ActivePlayer;
u8 UsedTextureSetQueue_8036EC78[0x20];
void *UsedTextureSetMessage_8036EC98[3];

extern s32 __float_nan[];
extern s32 __float_huge[];
f64 sin(f64 x);
f64 cos(f64 x);

typedef struct WheelStatus {
    u16 buttons;
    u8 flags;
    u8 unk3[6];
    s8 err;
} WheelStatus;

typedef struct Force {
    u8 playing[4][8];
    u32 effectIds[4][8];
} Force;

typedef struct Wheels {
    WheelStatus status[4];
    s16 nonLinValues[4][256];
    u8 unk828[0x10];
    s32 type[4];
} Wheels;

typedef struct __file_modes {
    u32 open_mode : 2;
    u32 io_mode : 3;
    u32 buffer_mode : 2;
    u32 file_kind : 3;
    u32 file_orientation : 2;
    u32 binary_io : 1;
} __file_modes;

typedef struct __file_state {
    u32 io_state : 3;
    u32 free_buffer : 1;
    u8 eof;
    u8 error;
} __file_state;

typedef struct _FILE {
    u32 handle;
    __file_modes mode;
    __file_state state;
    u8 is_dynamically_allocated;
    u8 unkD[0x18 - 0xD];
    u32 position;
    u8 *buffer;
    u32 buffer_size;
    u8 *buffer_ptr;
    u32 buffer_len;
    u32 buffer_alignment;
    u32 saved_buffer_len;
    u32 buffer_pos;
    void *position_fn;
    void *read_fn;
    s32 (*write_fn)(u32 handle, u8 *buffer, u32 *count, void *ref_con);
    s32 (*close_fn)(u32 handle);
    void *ref_con;
    struct _FILE *next_file_struct;
} FILE;

s32 fclose(FILE *file);

typedef struct SubBlock {
    u32 size_;
    void *bp_;
    struct SubBlock *prev_;
    struct SubBlock *next_;
} SubBlock;

extern u8 __lower_map[256];
s32 fflush(FILE *file);
void free(void *ptr);
s32 unicode_to_UTF8(char *s, u16 wchar);
char *strncpy(char *dst, const char *src, u32 n);

enum __file_kinds { __closed_file, __disk_file, __console_file, __unavailable_file };
enum __file_orientation { __unoriented, __char_oriented, __wide_oriented };

typedef struct VirtualPad {
    u8 unk0[3];
    s8 forceFeedbackActive;
    u32 keyDown;
    u32 keyPress;
    u8 unkC[4];
    u8 keyValue[0x20];
} VirtualPad;

extern VirtualPad g_VirtualPad[4];

typedef struct PADStatus {
    u16 button;
    s8 stickX;
    s8 stickY;
    s8 substickX;
    s8 substickY;
    u8 triggerLeft;
    u8 triggerRight;
    u8 analogA;
    u8 analogB;
    s8 err;
} PADStatus;

extern PADStatus ngcPad[4];
void PADRead(PADStatus *status);
void PADClamp(PADStatus *status);
void PlaySpringForce__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient);
s32 Gfx_CreateView(s32 arg0, s32 width, s32 height, s32 arg3, s32 arg4, s32 arg5);

typedef struct RumbleStatus {
    s32 duration;
    s32 active;
} RumbleStatus;

extern RumbleStatus rumbleStatus[4];

typedef struct JoyBusParam {
    s32 state;
    u8 unk4[0xC];
    s32 result;
    u8 unk14[0x74 - 0x14];
} JoyBusParam;

extern JoyBusParam Param_803C6A40[4];

extern void *wheels;
u8 IsOpened__8LGWheelsFl(void *wheels, s32 chan);
s8 IsConnected__8LGWheelsFl(void *wheels, s32 chan);
void PlayCarAirborne__8LGWheelsFl(void *wheels, s32 chan);
void PlayFrontalCollisionForce__8LGWheelsFlUc(void *wheels, s32 chan, u8 force);
void PlayDamperForce__8LGWheelsFlUcs(void *wheels, s32 chan, u8 coefficient, s16 offset);
void GenerateNonLinValues__8LGWheelsFlUc(void *wheels, s32 chan, u8 coefficient);
typedef struct LGWheels {
    u8 unk0[0x828];
    u8 airborne[4];
    u8 wheels[0x1054 - 0x82C];
    u32 overallGainId[4];
    u8 unk1064[0x10AC - 0x1064];
    u8 force[0x13CC - 0x10AC];
    u8 savedGain;
    u8 unk13CD[0x140E - 0x13CD];
    s16 frontalCollisionParams[4];
} LGWheels;

u8 IsOpened__6WheelsFl(Wheels *wheels, s32 chan);
s32 GetOverallForceGain__8LGWheelsFl(void *wheels, s32 chan);
void SetOverallForceGain__8LGWheelsFll(void *wheels, s32 chan, s32 gain);
void SetOverallForceGain__5ForceFRUll(void *force, u32 *effectId, s32 gain);
s32 GetOverallForceGain__5ForceFRUl(void *force, u32 *effectId);
void InitLGLibrary__8LGWheelsFv(void *wheels);
Wheels *__ct__6WheelsFv(Wheels *wheels);
s32 __ct__5ForceFv(s32 arg0);
s32 __ct__9ConditionFv(s32 arg0);
s32 __ct__8ConstantFv(s32 arg0);
s32 __ct__8PeriodicFv(s32 arg0);
s32 __ct__4RampFv(s32 arg0);

typedef struct RandomNumberGenerator {
    Vector vectors[0x300];
    u32 unk3000;
    u32 unk3004;
    u32 index;
    f32 floats[0x400];
    u32 unk400C;
} RandomNumberGenerator;

extern RandomNumberGenerator g_randomNumberGenerator;
s32 RandomAdd_s32(s32 base, s32 range);
f32 RandomAdd(f32 base, f32 range);
void RandomNumberGenerator_GetRandomVector(Vector *out);
void GenerateQuaternion(Vector *axis, Vector *q, f32 angle);
s32 RandomAdd_s32(s32 base, s32 range);

typedef struct FragmentInfo {
    void *eti_init_info;
    char *TOC;
    s32 active;
} FragmentInfo;

extern FragmentInfo fragmentinfo_8030DC08[1];

typedef struct SFX {
    void *data;
    u8 unk4[0x1C];
} SFX;

extern SFX g_sound_80373560[0xFE];
extern u8 g_soundLst_80373460[0xFE];
extern u32 g_freeSound_803C78D8;

typedef void (*GBACallback)(s32 chan, s32 ret);

typedef struct GBABootInfo {
    s32 paletteColor;
    s32 paletteSpeed;
    u8 *programp;
    s32 length;
    u8 *status;
    GBACallback callback;
    u8 readbuf[4];
    u8 writebuf[4];
    s32 i;
    u32 start;
    s64 begin;
    s32 firstXfer;
    s32 curOffset;
    u32 crc;
    u32 dummyWord[7];
    u32 keyA;
    u32 keyB;
    u32 initialCode;
    s32 realLength;
} GBABootInfo;

typedef struct GBAControl {
    u8 output[5];
    u8 input[5];
    s32 outputBytes;
    s32 inputBytes;
    u8 *status;
    u8 *ptr;
    void *callback;
    s32 ret;
    void *threadQueue[2];
    u8 unk2C[4];
    s64 delay;
    void *proc;
    u8 unk3C[4];
    GBABootInfo bootInfo;
    u8 unkA8[0xF8 - 0xA8];
    void *param;
    u8 unkFC[4];
} GBAControl;

extern GBAControl __GBA[4];

s32 __GBATransfer(s32 chan, s32 w1, s32 r1, void *proc);
void ReadProc(s32 chan);
void WriteProc(s32 chan);
void __GBASyncCallback(s32 chan);
s32 __GBASync(s32 chan);
extern u8 D54_800DAFF8[];

s32 GBAResetAsync(s32 chan, u8 *status, GBACallback callback);
s32 GBAGetStatusAsync(s32 chan, u8 *status, GBACallback callback);
void __GBAX02(s32 chan, u8 *readbuf);
static void F25(s32 chan, s32 ret);
void F27(s32 chan, s32 ret);

typedef struct EffectCondition {
    u8 unk0[2];
    u8 negSaturation;
    u8 posSaturation;
    s16 negCoefficient;
    s16 posCoefficient;
} EffectCondition;

typedef struct EffectState {
    u8 unk0[0xC];
    s32 position;
    u8 unk10[0x54];
    s32 velocity;
} EffectState;

typedef struct Effect {
    u8 unk0[0x24];
    EffectState *state;
    u8 unk28[0x10];
    s32 deadbandLow;
    s32 deadbandHigh;
} Effect;

typedef struct VDevice {
    u8 unk0[0xF0];
    s32 unkF0;
    Effect *activeEffects[64];
    Effect effects[64];
    s32 unk11F4;
} VDevice;

typedef struct LGDevice {
    u8 unk0[4];
    u8 opened : 1;
    u8 started : 1;
    u8 unk4_pad : 6;
    s32 unk8;
    u8 unkC[0x41];
    s8 unk4D;
    u8 unk4E[2];
    VDevice vdev;
} LGDevice;

extern LGDevice g_lgDevices_803771D8[4];
extern s32 g_bGammaInitialized_803C794C;

void SimThread_Init(VDevice *dev, VDevice *owner);
void VDevice_RecalcGammaTable(void);
s32 VDevice_Shutdown(VDevice *dev);
void VDevice_Init(VDevice *dev);
s32 VDevice_StartEffect(VDevice *dev, u32 handle);
u32 VDevice_GetFreeEffect(VDevice *dev);
void Effect_Init(Effect *effect);
s32 Effect_UpdateEffect(Effect *effect, u8 *desc);

extern s16 g_iQuarterSineTable_8037BB7C[0x40];

typedef struct __OutStrCtrl {
    char *CharStr;
    u32 MaxCharCount;
    u32 CharsWritten;
} __OutStrCtrl;

extern s32 __OSCurrHeap;
s32 OSFreeToHeap(s32 heap, s32 ptr);

typedef struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

typedef struct GDLDrawEntry {
    s32 geometry;
    GXColor unk4;
    s32 matrix;
    s32 lightState;
    u8 unk10;
    u8 unk11;
    u8 envmapState;
    u8 pad13;
} GDLDrawEntry;

typedef struct GDLAlphaEntry {
    void (*func)(u16 arg);
    u16 arg;
    u16 next;
} GDLAlphaEntry;

typedef struct GDLSprite {
    u8 unk0[0x20];
    u32 next;
    u8 unk24[0x30 - 0x24];
} GDLSprite;

typedef struct GDLRect {
    u8 unk0[0x83];
    u8 window;
} GDLRect;

typedef struct GDLLine {
    u8 unk0[0x30];
} GDLLine;

typedef struct GDLGroupTri {
    u8 type;
    u8 unk1[0xC - 0x1];
    u32 count;
    u8 unk10[0x20 - 0x10];
} GDLGroupTri;

typedef struct GDL {
    u8 unk0[0x8];
    u32 numSprites;
    u8 unkC[0x14 - 0xC];
    s32 numTris;
    u32 numRects2D;
    u8 unk1C[0x20 - 0x1C];
    u32 numRects;
    u8 unk24[0x34 - 0x24];
    u32 numLines;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 numGroupTris;
    u8 unk48[0x54 - 0x48];
    s32 numDrawEntries;
    u8 unk58[0x6C - 0x58];
    s32 numEnvmapStates;
    u8 unk70[0x74 - 0x70];
    u32 numMatrices;
    u8 unk78[0x80 - 0x78];
    u16 alphaTable[256];
    GDLAlphaEntry alphaEntries[(0x215C0 - 0x280) / 8];
    GDLSprite sprites[0x7CF];
    u8 unk38C90[0x7EA00 - 0x38C90];
    GDLRect rects2D[0x400];
    GDLRect rects[0x20];
    u8 unkA0A80[0xC0040 - 0xA0A80];
    GDLLine lines[0x100];
    u8 unkC3040[0xDFCC0 - 0xC3040];
    GDLDrawEntry drawEntries[(0xF3C00 - 0xDFCC0) / 0x14];
    u8 unkF3BEC[0xF3C00 - 0xDFCC0 - ((0xF3C00 - 0xDFCC0) / 0x14) * 0x14];
    u32 matrices[0x578];
    u8 unkF51E0[0x24CDE4 - 0xF51E0];
    GDLGroupTri *groupTris;
    u8 unk24CDE8[0x24CDF4 - 0x24CDE8];
    u16 spriteHeads[1];
} GDL;

typedef struct RenderStats {
    s32 numTris;
    s32 debugTriCount;
    u8 unk8[0x100 - 0x8];
} RenderStats;

typedef struct Texture {
    u8 unk0[0x24];
    void *data;
    u32 unk28;
    f32 unk2C;
    u16 unk30;
    u16 width;
    u16 height;
    s16 refCount;
    u16 unk38;
    u16 unk3A;
    u16 view;
    s16 wrapMode;
} Texture;

typedef struct FontChar {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    s8 spacingX;
    s8 spacingY;
    u8 unk12[0x14 - 0x12];
} FontChar;

typedef struct Font {
    u8 unk0[0xC];
    u16 texture;
    u8 unkE[0x10 - 0xE];
    s8 inUse;
    u8 unk11;
    u16 refCount;
    FontChar chars[128];
} Font;

typedef struct FontManager {
    s32 numFonts;
    Font fonts[16];
} FontManager;

typedef struct DLSkinGroup {
    u8 unk0[0x14];
} DLSkinGroup;

typedef struct DisplayList {
    u8 unk0[0x10];
    u16 numSkinGroups;
    u8 unk12[0x109E0 - 0x12];
    DLSkinGroup skinGroups[0x100];
} DisplayList;

typedef struct GfxPointLight {
    Vector position;
    f32 radius;
    GXColor color;
    u8 unk18[0x20 - 0x18];
} GfxPointLight;

typedef struct GfxState {
    GfxPointLight pointLights[3];
    Vector lightDir;
    GXColor lightColor;
    GXColor unk74;
    GXColor ambientColor;
    u32 unk7C;
    f32 unk80;
    u8 unk84[0x88 - 0x84];
    s32 unk88;
    s16 debugFont;
    u8 lightChanged;
    s8 unk8F;
    u8 numPointLights;
    u8 unk91;
    u8 unk92;
    u8 unk93;
    u8 enviroMapMode;
    u8 unk95[0x98 - 0x95];
    f32 farClippingPlane;
    u8 unk9C[0xA0 - 0x9C];
} GfxState;

static inline void SetVector(Vector *v, f32 x, f32 y, f32 z, f32 w) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
}

static inline void IdentityMatrix(Vector *m) {
    SetVector(&m[0], 1.0f, 0.0f, 0.0f, 0.0f);
    SetVector(&m[1], 0.0f, 1.0f, 0.0f, 0.0f);
    SetVector(&m[2], 0.0f, 0.0f, 1.0f, 0.0f);
    SetVector(&m[3], 0.0f, 0.0f, 0.0f, 1.0f);
}

void OSReport(const char *msg, ...);
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void OSSetArenaLo(void *newLo);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, s32 maxHeaps);
s32 OSCreateHeap(void *start, void *end);
s32 OSSetCurrentHeap(s32 heap);
s32 raise(s32 sig);
int tolower(int c);

typedef void (*__signal_func_ptr)(s32);
extern __signal_func_ptr signal_funcs_8030E050[6];
void exit(s32 status);
void _ExitProcess(void);
extern s32 __aborting;
extern s32 __atexit_curr_func_803C75DC;
extern void (*__atexit_funcs_8030DC18[64])(void);
extern void (*__console_exit)(void);
void __unregister_fragment(s32 fragmentID) {
    FragmentInfo *f;

    if (fragmentID >= 0 && fragmentID < 1) {
        f = &fragmentinfo_8030DC08[fragmentID];
        f->eti_init_info = 0;
        f->TOC = 0;
        f->active = 0;
    }
}

s32 __register_fragment(void *info, char *TOC) {
    FragmentInfo *f;
    s32 i;

    for (i = 0, f = fragmentinfo_8030DC08; i < 1; ++i, ++f) {
        if (f->active == 0) {
            f->eti_init_info = info;
            f->TOC = TOC;
            f->active = 1;
            return i;
        }
    }
    return -1;
}

static inline void InitDefaultHeap(void) {
    void *arenaLo;
    void *arenaHi;

    OSReport("GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n");
    OSReport("Metrowerks CW runtime library initializing default heap\n");
    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
    OSSetArenaLo(arenaLo);
    arenaLo = (void *)(((u32)arenaLo + 0x1F) & ~0x1F);
    arenaHi = (void *)((u32)arenaHi & ~0x1F);
    OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
    OSSetArenaLo(arenaLo = arenaHi);
}

void *what__Q23std13bad_exceptionCFv(void) {
    extern u8 g_cameraManager[];
    return g_cameraManager;
}

void __sys_free(void *ptr) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    OSFreeToHeap(__OSCurrHeap, (s32)ptr);
}

s32 OSAllocFromHeap(s32 heap, s32 ptr);
void __sys_alloc(void *ptr) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    OSAllocFromHeap(__OSCurrHeap, (s32)ptr);
}

void abort(void) {
    raise(1);
    __aborting = 1;
    while (__atexit_curr_func_803C75DC > 0) {
        (*__atexit_funcs_8030DC18[--__atexit_curr_func_803C75DC])();
    }
    if (__console_exit) {
        (*__console_exit)();
        __console_exit = 0;
    }
    _ExitProcess();
}

void *malloc(u32 size) {
    void *allocate_from_fixed_pools(void *pool, u32 size);
    void *allocate_from_var_pools(void *pool, u32 size);
    extern u8 protopool_8030DD18[0x34];
    extern u8 init_803C75E8;
    void *pool;

    if (!init_803C75E8) {
        memset(protopool_8030DD18, 0, sizeof(protopool_8030DD18));
        init_803C75E8 = 1;
    }
    pool = protopool_8030DD18;
    if (size == 0) {
        return NULL;
    }
    if (size > (u32)-0x31) {
        return NULL;
    }
    if (size <= 0x44) {
        return allocate_from_fixed_pools(pool, size);
    }
    return allocate_from_var_pools(pool, size);
}

void SubBlock_merge_next(SubBlock *ths, SubBlock **start) {
    u32 this_cur_size;
    SubBlock *next_sub_block;
    SubBlock *after;

    next_sub_block = (SubBlock *)((u8 *)ths + (ths->size_ & 0xFFFFFFF8));
    if (!(next_sub_block->size_ & 2)) {
        this_cur_size = (ths->size_ & 0xFFFFFFF8) + (next_sub_block->size_ & 0xFFFFFFF8);
        ths->size_ &= ~0xFFFFFFF8;
        ths->size_ |= this_cur_size & 0xFFFFFFF8;
        if (!(ths->size_ & 2)) {
            *(u32 *)((u8 *)ths + this_cur_size - sizeof(u32)) = this_cur_size;
        }
        after = (SubBlock *)((u8 *)ths + this_cur_size);
        if (!(ths->size_ & 2)) {
            after->size_ &= ~0x04L;
        } else {
            after->size_ |= 0x04L;
        }
        if (*start == next_sub_block) {
            *start = (*start)->next_;
        }
        if (*start == next_sub_block) {
            *start = NULL;
        }
        next_sub_block->next_->prev_ = next_sub_block->prev_;
        next_sub_block->prev_->next_ = next_sub_block->next_;
    }
}

s32 __flush_all(void) {
    extern FILE __files[];
    s32 retval = 0;
    FILE *stream = &__files[0];

    while (stream) {
        if (stream->mode.file_kind && fflush(stream)) {
            retval = -1;
        }
        stream = stream->next_file_struct;
    }
    return retval;
}

void __close_all(void) {
    extern FILE __files[];
    FILE *p = &__files[0];
    FILE *plast;

    while (p) {
        if (p->mode.file_kind != 0) {
            fclose(p);
        }

        plast = p;
        p = p->next_file_struct;

        if (plast->is_dynamically_allocated) {
            free(plast);
        } else {
            plast->mode.file_kind = 3;
            if (p && p->is_dynamically_allocated) {
                plast->next_file_struct = NULL;
            }
        }
    }
}

void __timesdec(decimal *result, const decimal *x, const decimal *y) {
    u32 accumulator = 0;
    u8 mantissa[SIGDIGLEN * 2];
    int i = x->sig.length + y->sig.length - 1;
    u8 *pDigit;
    u8 *ip;
    u8 *jp;
    u8 *end;

    result->sign = 0;
    end = pDigit = mantissa + i + 1;

    while (i > 0) {
        int k = y->sig.length - 1;
        int j = i - k - 1;
        int l;
        int t;

        if (j < 0) {
            j = 0;
            k = i - 1;
        }
        ip = (u8 *)&x->sig.text[j];
        jp = (u8 *)&y->sig.text[k];
        l = k + 1;
        t = x->sig.length - j;
        if (l > t) {
            l = t;
        }
        for (; l > 0; --l, ++ip, --jp) {
            accumulator += *ip * *jp;
        }
        *--pDigit = accumulator % 10;
        accumulator /= 10;
        --i;
    }

    result->exp = x->exp + y->exp;

    if (accumulator > 0) {
        *--pDigit = accumulator;
        result->exp++;
    }

    for (i = 0; i < SIGDIGLEN && pDigit < end; ++i, ++pDigit) {
        result->sig.text[i] = *pDigit;
    }
    result->sig.length = i;

    if (pDigit < end && *pDigit >= 5) {
        if (*pDigit == 5) {
            u8 *q;
            for (q = pDigit + 1; q < end; ++q) {
                if (*q != 0) {
                    goto round;
                }
            }
            if ((*(pDigit - 1) & 1) == 0) {
                return;
            }
        }
    round:
        __dorounddecup(result, result->sig.length);
    }
}

s32 __count_trailing_zerol(u32 x) {
    s32 result = 0;
    s32 bits_not_checked = sizeof(u32) * 8;
    s32 n = bits_not_checked / 2;
    s32 mask_size = n;
    u32 mask = (~0UL) >> (bits_not_checked - n);

    while (bits_not_checked) {
        if (!(x & mask)) {
            result += mask_size;
            x >>= mask_size;
            bits_not_checked -= mask_size;
        } else if (mask == 1) {
            break;
        }
        if (n > 1) {
            n /= 2;
        }
        if (mask > 1) {
            mask >>= n;
            mask_size -= n;
        }
    }
    return result;
}

s32 __flush_buffer(FILE *file, u32 *bytes_flushed) {
    u32 buffer_len;
    s32 ioresult;

    buffer_len = file->buffer_ptr - file->buffer;
    if (buffer_len) {
        file->buffer_len = buffer_len;
        ioresult = file->write_fn(file->handle, file->buffer, &file->buffer_len, file->ref_con);
        if (bytes_flushed) {
            *bytes_flushed = file->buffer_len;
        }
        if (ioresult) {
            return ioresult;
        }
        file->position += file->buffer_len;
    }
    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len -= file->position & file->buffer_alignment;
    file->buffer_pos = file->position;
    return 0;
}

void __prep_buffer(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x24) = (s32) M2C_FIELD(arg0, s32 *, 0x1C);
    M2C_FIELD(arg0, s32 *, 0x28) = (s32) M2C_FIELD(arg0, s32 *, 0x20);
    M2C_FIELD(arg0, s32 *, 0x28) = (s32) (M2C_FIELD(arg0, s32 *, 0x28) - (M2C_FIELD(arg0, s32 *, 0x18) & M2C_FIELD(arg0, s32 *, 0x2C)));
    M2C_FIELD(arg0, s32 *, 0x34) = (s32) M2C_FIELD(arg0, s32 *, 0x18);
}

#pragma push
#pragma dont_inline on
int tolower(int c) {
    if (c == -1) {
        return -1;
    }
    return __lower_map[c & 0xFF];
}
#pragma pop

u32 fwrite(const void *ptr, u32 memb_size, u32 num_memb, void *file) {
    u32 __fwrite(const void *, u32, u32, void *);
    return __fwrite(ptr, memb_size, num_memb, file);
}

s32 fclose(FILE *file) {
    s32 flush_result, close_result;

    if (file == NULL) {
        return -1;
    }
    if (file->mode.file_kind == __closed_file) {
        return 0;
    }

    flush_result = fflush(file);
    close_result = (*file->close_fn)(file->handle);
    file->mode.file_kind = __closed_file;
    file->handle = 0;

    if (file->state.free_buffer) {
        free(file->buffer);
    }

    return ((flush_result || close_result) ? -1 : 0);
}

void fseek(void) {
    M2C_UNK fn_8009A088();
    fn_8009A088();
}

s32 ftell(void *arg0) {
    extern s32 errno;
    s32 temp_r3;
    u32 temp_r0;
    u32 temp_r5;

    temp_r0 = ((u16) M2C_FIELD(arg0, u16 *, 4) >> 6U) & 7;
    if (((temp_r0 != 1) && (temp_r0 != 2)) || ((u8) M2C_FIELD(arg0, u8 *, 0xA) != 0)) {
        errno = 0x28;
        return -1;
    }
    temp_r5 = ((u8) M2C_FIELD(arg0, u8 *, 8) >> 5U) & 7;
    if (temp_r5 == 0) {
        return M2C_FIELD(arg0, s32 *, 0x18);
    }
    temp_r3 = M2C_FIELD(arg0, s32 *, 0x34) + (M2C_FIELD(arg0, s32 *, 0x24) - M2C_FIELD(arg0, s32 *, 0x1C));
    if (temp_r5 >= 3U) {
        return temp_r3 - (temp_r5 - 2);
    }
    return temp_r3;
}

#pragma push
#pragma use_lmw_stmw on
u32 wcstombs(char *s, const u16 *pwcs, u32 n) {
    s32 result;
    s32 chars_written = 0;
    char temp[3];
    u16 *source;

    if (!s || !pwcs) {
        return 0;
    }

    source = (u16 *)pwcs;
    while (chars_written <= n) {
        if (!*source) {
            *(s + chars_written) = '\0';
            break;
        } else {
            result = unicode_to_UTF8(temp, *source++);
            if ((chars_written + result) <= n) {
                strncpy(s + chars_written, temp, result);
                chars_written += result;
            } else {
                break;
            }
        }
    }

    return chars_written;
}

s32 unicode_to_UTF8(char *s, u16 wchar) {
    int number_of_bytes;
    u16 wide_char;
    char *target_ptr;
    char first_byte_mark[4] = {0x00, 0x00, 0xC0, 0xE0};

    if (!s) {
        return 0;
    }

    wide_char = wchar;
    if (wide_char < 0x0080) {
        number_of_bytes = 1;
    } else if (wide_char < 0x0800) {
        number_of_bytes = 2;
    } else {
        number_of_bytes = 3;
    }

    target_ptr = s + number_of_bytes;

    switch (number_of_bytes) {
    case 3:
        *--target_ptr = (wide_char & 0x003F) | 0x80;
        wide_char >>= 6;
    case 2:
        *--target_ptr = (wide_char & 0x003F) | 0x80;
        wide_char >>= 6;
    case 1:
        *--target_ptr = wide_char | first_byte_mark[number_of_bytes];
    }

    return number_of_bytes;
}

int memcmp(const void *src1, const void *src2, unsigned long n) {
    const unsigned char *p1;
    const unsigned char *p2;

    for (p1 = (const unsigned char *)src1 - 1, p2 = (const unsigned char *)src2 - 1, n++; --n;) {
        if (*++p1 != *++p2) {
            return (*p1 < *p2) ? -1 : +1;
        }
    }

    return 0;
}

void *__memrchr(const void *src, int val, unsigned long n) {
    const unsigned char *p;
    unsigned long v = (val & 0xff);

    for (p = (unsigned char *)src + n, n++; --n;) {
        if (*--p == v) {
            return (void *)p;
        }
    }

    return 0;
}

void *memchr(const void *src, int val, unsigned long n) {
    const unsigned char *p;
    unsigned long v = (val & 0xff);

    for (p = (unsigned char *)src - 1, n++; --n;) {
        if ((*++p & 0xff) == v) {
            return (void *)p;
        }
    }

    return 0;
}

void __copy_longs_rev_unaligned(void *dst, const void *src, unsigned long n) {
    unsigned long src_offset, left_shift, right_shift, v1, v2;
    unsigned char *cps;
    unsigned char *cpd;
    unsigned long i;

    cps = ((unsigned char *)src) + n;
    cpd = ((unsigned char *)dst) + n;

    i = ((unsigned long)cpd) & 3;

    if (i) {
        n -= i;

        do
            *--cpd = *--cps;
        while (--i);
    }

    src_offset = ((unsigned long)cps) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    cps += 4 - src_offset;

    i = n >> 3;

    v1 = *--((unsigned long *)cps);

    do {
        v2 = *--((unsigned long *)cps);
        *--((unsigned long *)cpd) = (v2 << left_shift) | (v1 >> right_shift);
        v1 = *--((unsigned long *)cps);
        *--((unsigned long *)cpd) = (v1 << left_shift) | (v2 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = *--((unsigned long *)cps);
        *--((unsigned long *)cpd) = (v2 << left_shift) | (v1 >> right_shift);
    }

    n &= sizeof(unsigned long) - 1;

    if (n) {
        cps += src_offset;
        do
            *--cpd = *--cps;
        while (--n);
    }
}

void __copy_longs_unaligned(void *dst, const void *src, u32 n) {
    u32 i, v1, v2;
    u32 src_offset, left_shift, right_shift;

#define cps ((u8 *)src)
#define cpd ((u8 *)dst)
#define lps ((u32 *)src)
#define lpd ((u32 *)dst)
#define deref_auto_inc(p) *++(p)

    i = (-(u32)dst) & 3;

    cps = ((u8 *)src) - 1;
    cpd = ((u8 *)dst) - 1;

    if (i) {
        n -= i;
        do {
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        } while (--i);
    }

    src_offset = ((u32)(cps + 1)) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    cps -= src_offset;

    lps = ((u32 *)(cps + 1)) - 1;
    lpd = ((u32 *)(cpd + 1)) - 1;

    i = n >> 3;

    v1 = deref_auto_inc(lps);

    do {
        v2 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v1 << left_shift) | (v2 >> right_shift);
        v1 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v2 << left_shift) | (v1 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v1 << left_shift) | (v2 >> right_shift);
    }

    cps = ((u8 *)(lps + 1)) - 1;
    cpd = ((u8 *)(lpd + 1)) - 1;

    if (n &= 3) {
        cps -= 4 - src_offset;
        do {
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        } while (--n);
    }

#undef cps
#undef cpd
#undef lps
#undef lpd
#undef deref_auto_inc
}

void __copy_longs_rev_aligned(void *dst, const void *src, unsigned long n) {
    unsigned char *cps;
    unsigned char *cpd;
    unsigned long *lps;
    unsigned long *lpd;
    unsigned long i;

    cps = ((unsigned char *)src) + n;
    cpd = ((unsigned char *)dst) + n;

    i = ((unsigned long)cpd) & 3;

    if (i) {
        n -= i;

        do
            *--cpd = *--cps;
        while (--i);
    }

    lps = (unsigned long *)cps;
    lpd = (unsigned long *)cpd;

    i = n >> 5;

    if (i)
        do {
            *--lpd = *--lps;
            *--lpd = *--lps;
            *--lpd = *--lps;
            *--lpd = *--lps;
            *--lpd = *--lps;
            *--lpd = *--lps;
            *--lpd = *--lps;
            *--lpd = *--lps;
        } while (--i);

    i = (n & 31) >> 2;

    if (i)
        do
            *--lpd = *--lps;
        while (--i);

    cps = (unsigned char *)lps;
    cpd = (unsigned char *)lpd;

    n &= sizeof(unsigned long) - 1;

    if (n)
        do
            *--cpd = *--cps;
        while (--n);

    return;
}

void __copy_longs_aligned(void *dst, const void *src, unsigned long n) {
    unsigned char *cps;
    unsigned char *cpd;
    unsigned long i;

    cps = ((unsigned char *)src) - 1;
    cpd = ((unsigned char *)dst) - 1;

    i = (-(unsigned long)dst) & 3;

    if (i) {
        n -= i;

        do
            *++cpd = *++cps;
        while (--i);
    }

    cps -= 3;
    cpd -= 3;

    i = n >> 5;

    if (i)
        do {
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
        } while (--i);

    i = (n & 31) >> 2;

    if (i)
        do
            *++((unsigned long *)cpd) = *++((unsigned long *)cps);
        while (--i);

    cps += 3;
    cpd += 3;

    n &= 3;

    if (n)
        do
            *++cpd = *++cps;
        while (--n);

    return;
}

#pragma pop
void __stdio_atexit(void) {
    extern void __close_all(void);
    extern void (*__stdio_exit)(void);
    __stdio_exit = __close_all;
}

int vprintf(const char *format, char *arg) {
    extern u8 __files[];
    extern void *__FileWrite(void *, const char *, u32);
    int fwide(void *, int);
    int __pformatter(void *(*)(void *, const char *, u32), void *, const char *, char *);
    void *stream = __files + 0x50;

    if (fwide(stream, -1) >= 0) {
        return -1;
    }
    return __pformatter(&__FileWrite, stream, format, arg);
}

void *__StringWrite(void *osc, const char *Buffer, u32 NumChars) {
    u32 CharsToBeWritten;
    __OutStrCtrl *Oscp = (__OutStrCtrl *)osc;

    CharsToBeWritten = ((Oscp->CharsWritten + NumChars) <= Oscp->MaxCharCount) ? NumChars : Oscp->MaxCharCount - Oscp->CharsWritten;
    memcpy(Oscp->CharStr + Oscp->CharsWritten, Buffer, CharsToBeWritten);
    Oscp->CharsWritten += CharsToBeWritten;
    return (void *)1;
}

void *__FileWrite(void *File, const char *Buffer, u32 NumChars) {
    return (fwrite(Buffer, 1, NumChars, File) == NumChars ? File : 0);
}

void srand(s32 arg0) {
    extern s32 next_803C6EA0;
    next_803C6EA0 = arg0;
}

s32 rand(void) {
    extern s32 next_803C6EA0;
    next_803C6EA0 = next_803C6EA0 * 1103515245 + 12345;
    return (next_803C6EA0 >> 16) & 0x7FFF;
}

s32 raise(s32 sig) {
    __signal_func_ptr temp_r31;

    if (sig < 1 || sig > 6) {
        return -1;
    }

    temp_r31 = signal_funcs_8030E050[sig - 1];

    if (temp_r31 != (__signal_func_ptr)1) {
        signal_funcs_8030E050[sig - 1] = (__signal_func_ptr)0;
    }

    if (temp_r31 == (__signal_func_ptr)1 || (temp_r31 == (__signal_func_ptr)0 && sig == 1)) {
        return 0;
    }

    if (temp_r31 == (__signal_func_ptr)0) {
        exit(0);
    }

    (*temp_r31)(sig);
    return 0;
}

typedef void (*__signal_func_ptr)(s32);
extern __signal_func_ptr signal_funcs_8030E050[6];
void exit(s32 status);
char *strstr(const char *str, const char *pat) {
    const u8 *s1 = (const u8 *)str - 1;
    const u8 *p1 = (const u8 *)pat - 1;
    u32 firstc, c1, c2;

    if ((pat == 0) || (!(firstc = *++p1))) {
        return (char *)str;
    }

    while (c1 = *++s1) {
        if (c1 == firstc) {
            const u8 *s2 = s1 - 1;
            const u8 *p2 = p1 - 1;

            while ((c1 = *++s2) == (c2 = *++p2) && c1)
                ;

            if (!c2) {
                return (char *)s1;
            }
        }
    }

    return 0;
}

char *strrchr(const char *str, s32 chr) {
    const u8 *p = (const u8 *)str - 1;
    const u8 *q = 0;
    u32 c = (chr & 0xFF);
    u32 ch;

    while (ch = *++p) {
        if (ch == c) {
            q = p;
        }
    }

    if (q) {
        return (char *)q;
    }

    return c ? 0 : (char *)p;
}

char *strchr(const char *str, int chr) {
    const unsigned char *p = (unsigned char *)str - 1;
    unsigned long c = (chr & 0xff);
    unsigned long ch;

    while (ch = *++p) {
        if (ch == c) {
            return (char *)p;
        }
    }

    return c ? 0 : (char *)p;
}

char *strcat(char *dst, const char *src) {
    const unsigned char *p = (unsigned char *)src - 1;
    unsigned char *q = (unsigned char *)dst - 1;

    while (*++q)
        ;

    q--;

    while (*++q = *++p)
        ;

    return dst;
}

char *strncpy(char *dst, const char *src, u32 n) {
    const u8 *p = (const u8 *)src - 1;
    u8 *q = (u8 *)dst - 1;

    n++;
    while (--n) {
        if (!(*++q = *++p)) {
            while (--n) {
                *++q = 0;
            }
            break;
        }
    }

    return dst;
}

char *strcpy(char *dst, const char *src) {
    register u8 *destb, *fromb;
    register u32 w, t, align;

    fromb = (u8 *)src;
    destb = (u8 *)dst;

    if ((align = ((s32)fromb & 3)) != ((s32)destb & 3)) {
        goto bytecopy;
    }

    if (align) {
        if ((*destb = *fromb) == 0) {
            return dst;
        }
        for (align = 3 - align; align; align--) {
            if ((*(++destb) = *(++fromb)) == 0) {
                return dst;
            }
        }
        ++destb;
        ++fromb;
    }

    w = *((u32 *)fromb);
    t = w + 0xFEFEFEFF;
    t &= 0x80808080;
    if (t) {
        goto bytecopy;
    }
    --((u32 *)destb);

    do {
        *(++((u32 *)destb)) = w;
        w = *(++((u32 *)fromb));
        t = w + 0xFEFEFEFF;
        t &= 0x80808080;
        if (t) {
            goto adjust;
        }
    } while (1);

adjust:
    ++((u32 *)destb);

bytecopy:
    if ((*destb = *fromb) == 0) {
        return dst;
    }

    do {
        if ((*(++destb) = *(++fromb)) == 0) {
            return dst;
        }
    } while (1);

    return dst;
}

u32 strlen(const char *str) {
    u32 len = -1;
    u8 *p = (u8 *)str - 1;

    do {
        len++;
    } while (*++p);

    return len;
}

void time(s32 *arg0) {
    s32 __get_time();
    s32 temp_r3;

    temp_r3 = __get_time();
    if (arg0 != NULL) {
        *arg0 = temp_r3;
    }
}

s32 InitializeUART(u32 baudRate);
s32 WriteUARTN(const void *buf, u32 len);
void __TRK_write_console(s32 handle, u8 *buffer, u32 *count, void *idle_proc);
int __write_console(s32 handle, u8 *buffer, u32 *count, void *idle_proc) {
    static int initialized;
    s32 status = 0;

    if (initialized == 0) {
        status = InitializeUART(0xE100);
        if (status == 0) {
            initialized = 1;
        }
    }
    if (status != 0) {
        return 1;
    }
    if (WriteUARTN(buffer, *count) != 0) {
        *count = 0;
        return 1;
    }
    __TRK_write_console(handle, buffer, count, idle_proc);
    return 0;
}

s32 fwide(FILE *stream, s32 mode) {
    s32 orientation;
    s32 result;

    if ((stream == NULL) || (stream->mode.file_kind == __closed_file)) {
        return 0;
    }

    orientation = stream->mode.file_orientation;
    switch (orientation) {
    case __unoriented:
        if (mode > 0) {
            stream->mode.file_orientation = __wide_oriented;
        } else if (mode < 0) {
            stream->mode.file_orientation = __char_oriented;
        }
        result = mode;
        break;
    case __wide_oriented:
        result = 1;
        break;
    case __char_oriented:
        result = -1;
        break;
    }

    return result;
}

double __kernel_sin(double x, double y, int iy) {
    double z, r, v;
    int ix;
    ix = __HI(x) & 0x7fffffff;
    if (ix < 0x3e400000) {
        if ((int)x == 0) {
            return x;
        }
    }
    z = x * x;
    v = z * x;
    r = 8.33333333332248946124e-03 + z * (-1.98412698298579493134e-04 + z * (2.75573137070700676789e-06 + z * (-2.50507602534068634195e-08 + z * 1.58969099521155010221e-10)));
    if (iy == 0) {
        return x + v * (-1.66666666666666324348e-01 + z * r);
    } else {
        return x - ((z * (5.0e-1 * y - v * r) - y) - v * -1.66666666666666324348e-01);
    }
}

double __kernel_tan(double x, double y, int iy);
int __ieee754_rem_pio2(double x, double *y);
double copysign(double x, double y) {
    __HI(x) = (__HI(x) & 0x7fffffff) | (__HI(y) & 0x80000000);
    return x;
}

double frexp(double x, int *eptr) {
    int hx, ix, lx;
    hx = __HI(x);
    ix = 0x7fffffff & hx;
    lx = __LO(x);
    *eptr = 0;
    if (ix >= 0x7ff00000 || ((ix | lx) == 0)) {
        return x;
    }
    if (ix < 0x00100000) {
        x *= 1.80143985094819840000e+16;
        hx = __HI(x);
        ix = hx & 0x7fffffff;
        *eptr = -54;
    }
    *eptr += (ix >> 20) - 1022;
    hx = (hx & 0x800fffff) | 0x3fe00000;
    __HI(x) = hx;
    return x;
}

double tan(double x) {
    double y[2], z = 0.0;
    int n, ix;
    ix = __HI(x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_tan(x, z, 1);
    } else if (ix >= 0x7ff00000) {
        return x - x;
    } else {
        n = __ieee754_rem_pio2(x, y);
        return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
    }
}

double acos(double x) {
    double fn_800957D4(double);
    return fn_800957D4(x);
}

void atan2(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void pow(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

f32 tanf(void) {
    f64 tan();
    return (f32) tan();
}

f32 sinf(void) {
    f64 sin();
    return (f32) sin();
}

f32 cosf(void) {
    f64 cos();
    return (f32) cos();
}

void fn_8009C888(s32 arg0) {
    extern s32 lbl_803C6EA0;
    lbl_803C6EA0 = arg0;
}

void fn_8009FAD8(void) {
    M2C_UNK fn_8009CE30();
    fn_8009CE30();
}

void fn_8009FAF8(void) {
    M2C_UNK fn_8009D140();
    fn_8009D140();
}

void fn_8009FB18(void) {
    M2C_UNK fn_8009D3D0();
    fn_8009D3D0();
}

f32 fn_8009FB38(void) {
    f64 fn_8009FA60();
    return (f32) fn_8009FA60();
}

f32 fn_8009FB5C(void) {
    f64 fn_8009F988();
    return (f32) fn_8009F988();
}

f32 fn_8009FB80(void) {
    f64 fn_8009F420();
    return (f32) fn_8009F420();
}

void EXI2_Init(void) {

}

void EXI2_EnableInterrupts(void) {

}

s32 EXI2_Poll(void) {
    return 0;
}

s32 EXI2_ReadN(void) {
    return 0;
}

s32 EXI2_WriteN(void) {
    return 0;
}

void EXI2_Reserve(void) {

}

void EXI2_Unreserve(void) {

}

s32 AMC_IsStub(void) {
    return 1;
}

s32 Hu_IsStub(void) {
    return 0;
}

void DBClose(void) {

}

void DBOpen(void) {

}

void DBGHandler(s16 interrupt) {
    extern void (*DBGCallback_803C7604)(s16);

    *(u16 *)0xCC003000 = 0x1000;
    if (DBGCallback_803C7604) {
        DBGCallback_803C7604(interrupt);
    }
}

void MWCallback(void) {
    extern u8 EXIInputFlag_803C7614;
    extern void (*MTRCallback_803C7600)(s32);

    EXIInputFlag_803C7614 = 1;
    if (MTRCallback_803C7600) {
        MTRCallback_803C7600(0);
    }
}

BOOL DBGReadStatus(u32 *status) {
    BOOL DBGEXIImm(void *buffer, s32 bytecounter, u32 write);
    BOOL err;
    u32 cmd;

    *(volatile u32 *)0xCC006828 = (*(volatile u32 *)0xCC006828 & 0x405) | 0xC0;
    cmd = 0x40000000;
    err = !DBGEXIImm(&cmd, 2, 1);
    while (*(volatile u32 *)0xCC006834 & 1) {
    }
    err |= !DBGEXIImm(status, 4, 0);
    while (*(volatile u32 *)0xCC006834 & 1) {
    }
    *(volatile u32 *)0xCC006828 &= 0x405;
    return !err;
}

s32 DBGReadMailbox(u32 *mailbox) {
    s32 DBGEXIImm(void *buf, s32 len, u32 mode);
    s32 err;
    u32 cmd;

    *(volatile u32 *)0xCC006828 = (*(volatile u32 *)0xCC006828 & 0x405) | 0xC0;
    cmd = 0x60000000;
    err = !DBGEXIImm(&cmd, 2, 1);
    while (*(volatile u32 *)0xCC006834 & 1) {
    }
    err |= !DBGEXIImm(mailbox, 4, 0);
    while (*(volatile u32 *)0xCC006834 & 1) {
    }
    *(volatile u32 *)0xCC006828 &= 0x405;
    return !err;
}

f32 MagnitudeVector(Vector *v) {
    f32 zero = 0.0f;

    if (v->x != zero || v->y != zero || v->z != zero) {
        return PSVECMag(v);
    }
    return zero;
}

void CopyVector(Vector *dst, Vector *src) {
    *dst = *src;
}

void CopyMatrix(void *dst, void *src) {
    memcpy(dst, src, 0x40);
}

void Init_Math3D(void) {
    extern f32 g_identity[];

    g_identity[0] = 1.0f;
    g_identity[1] = 0.0f;
    g_identity[2] = 0.0f;
    g_identity[3] = 0.0f;
    g_identity[4] = 0.0f;
    g_identity[5] = 1.0f;
    g_identity[6] = 0.0f;
    g_identity[7] = 0.0f;
    g_identity[8] = 0.0f;
    g_identity[9] = 0.0f;
    g_identity[10] = 1.0f;
    g_identity[11] = 0.0f;
    g_identity[12] = 0.0f;
    g_identity[13] = 0.0f;
    g_identity[14] = 0.0f;
    g_identity[15] = 1.0f;
    g_identity[16] = 0.0f;
    g_identity[17] = 0.0f;
    g_identity[18] = 0.0f;
    g_identity[19] = 1.0f;
    g_identity[20] = 1.0f;
    g_identity[21] = 0.0f;
    g_identity[22] = 0.0f;
    g_identity[23] = 1.0f;
    g_identity[24] = 0.0f;
    g_identity[25] = 1.0f;
    g_identity[26] = 0.0f;
    g_identity[27] = 1.0f;
    g_identity[28] = 0.0f;
    g_identity[29] = 0.0f;
    g_identity[30] = 1.0f;
    g_identity[31] = 1.0f;
}

void SinCos(f32 *s, f32 *c, f32 angle) {
    f32 r;

    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    r = 0.017453292f * angle;
    *s = sin(r);
    *c = cos(r);
}

void SinCosRad(f32 *s, f32 *c, f32 rad) {
    f32 angle = rad * 57.29578f;
    f32 r;

    while (angle < 0.0f) {
        angle += 360.0f;
    }
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    r = 0.017453292f * angle;
    *s = sin(r);
    *c = cos(r);
}

f32 ArcTan(void) {
    f64 atan2();
    return (f32) atan2();
}

f32 Sqrt(f32 x) {
    f64 d = x;
    f64 result;

    if (d > 0.0) {
        f64 guess = __frsqrte(d);
        guess = 0.5 * guess * (3.0 - guess * guess * d);
        guess = 0.5 * guess * (3.0 - guess * guess * d);
        guess = 0.5 * guess * (3.0 - guess * guess * d);
        guess = 0.5 * guess * (3.0 - guess * guess * d);
        result = d * guess;
    } else if (d == 0.0) {
        result = 0.0;
    } else if (d) {
        result = *(f32 *)__float_nan;
    } else {
        result = *(f32 *)__float_huge;
    }
    return result;
}

f32 _fabs(f32 x) {
    f64 result = __fabs((f64)x);
    return result;
}

void AccumulateVector(void *arg0, void *arg1, void *arg2, f32 farg0) {
    M2C_FIELD(arg2, f32 *, 0) = (f32) ((M2C_FIELD(arg0, f32 *, 0) * farg0) + M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg2, f32 *, 4) = (f32) ((M2C_FIELD(arg0, f32 *, 4) * farg0) + M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg2, f32 *, 8) = (f32) ((M2C_FIELD(arg0, f32 *, 8) * farg0) + M2C_FIELD(arg1, f32 *, 8));
}

void ProjectVectorTangential(Vector *v, Vector *n, Vector *out) {
    f32 PSVECDotProduct();
    f32 dot;
    f32 scale;

    dot = PSVECDotProduct(v, n);
    scale = -dot;
    out->x = v->x + n->x * scale;
    out->y = v->y + n->y * scale;
    out->z = v->z + n->z * scale;
}

void DistanceXYZ(Vector *a, Vector *b, f32 *dist) {
    f32 dx = a->x - b->x;
    f32 dy = a->y - b->y;
    f32 dz = a->z - b->z;

    *dist = Sqrt(dx * dx + dy * dy + dz * dz);
}

s32 CompareDistances(void *arg0, void *arg1, f32 farg0) {
    f32 temp_f0;
    f32 temp_f1;
    f32 temp_f3;
    f32 temp_f4;
    f32 temp_f5;

    temp_f5 = farg0 * farg0;
    temp_f4 = M2C_FIELD(arg0, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
    temp_f3 = M2C_FIELD(arg0, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0);
    temp_f1 = M2C_FIELD(arg0, f32 *, 8) - M2C_FIELD(arg1, f32 *, 8);
    temp_f0 = (temp_f1 * temp_f1) + ((temp_f3 * temp_f3) + (temp_f4 * temp_f4));
    if (temp_f0 > temp_f5) {
        return 1;
    }
    if (temp_f0 < temp_f5) {
        return -1;
    }
    return 0;
}

void XformVec(Vector *m, Vector *v, Vector *out) {
    f32 x = v->x;
    f32 y = v->y;
    f32 z = v->z;
    f32 rx = m[0].x * x + m[1].x * y + m[2].x * z + m[3].x;
    f32 ry = m[0].y * x + m[1].y * y + m[2].y * z + m[3].y;
    f32 rz = m[0].z * x + m[1].z * y + m[2].z * z + m[3].z;
    out->x = rx;
    out->y = ry;
    out->z = rz;
}

void XformVec4x4(Vector *m, Vector *v, Vector *out) {
    f32 x = v->x;
    f32 y = v->y;
    f32 z = v->z;
    f32 rx = m[0].x * x + m[1].x * y + m[2].x * z + m[3].x;
    f32 ry = m[0].y * x + m[1].y * y + m[2].y * z + m[3].y;
    f32 rz = m[0].z * x + m[1].z * y + m[2].z * z + m[3].z;
    out->x = rx;
    out->y = ry;
    out->z = rz;
}

f32 PSVECDotProduct(Vector *a, Vector *b);
void InverseXformVec(Vector *m, Vector *v, Vector *out) {
    f32 x = v->x - m[3].x;
    f32 y = v->y - m[3].y;
    f32 z = v->z - m[3].z;
    f32 rx = m[0].x * x + m[0].y * y + m[0].z * z;
    f32 ry = m[1].x * x + m[1].y * y + m[1].z * z;
    f32 rz = m[2].x * x + m[2].y * y + m[2].z * z;
    out->x = rx;
    out->y = ry;
    out->z = rz;
}

void XformVecOrientation(Vector *m, Vector *v, Vector *out) {
    f32 x = v->x;
    f32 y = v->y;
    f32 z = v->z;
    f32 rx = m[0].x * x + m[1].x * y + m[2].x * z;
    f32 ry = m[0].y * x + m[1].y * y + m[2].y * z;
    f32 rz = m[0].z * x + m[1].z * y + m[2].z * z;
    out->x = rx;
    out->y = ry;
    out->z = rz;
}

void InverseXformVecOrientation(Vector *m, Vector *v, Vector *out) {
    f32 x = v->x;
    f32 y = v->y;
    f32 z = v->z;
    f32 rx = m[0].x * x + m[0].y * y + m[0].z * z;
    f32 ry = m[1].x * x + m[1].y * y + m[1].z * z;
    f32 rz = m[2].x * x + m[2].y * y + m[2].z * z;
    out->x = rx;
    out->y = ry;
    out->z = rz;
}

void TransposeSubMatrix(void *arg0) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;

    temp_f1 = M2C_FIELD(arg0, f32 *, 4);
    M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x10);
    M2C_FIELD(arg0, f32 *, 0x10) = temp_f1;
    temp_f1_2 = M2C_FIELD(arg0, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x20);
    M2C_FIELD(arg0, f32 *, 0x20) = temp_f1_2;
    temp_f1_3 = M2C_FIELD(arg0, f32 *, 0x18);
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) M2C_FIELD(arg0, f32 *, 0x24);
    M2C_FIELD(arg0, f32 *, 0x24) = temp_f1_3;
}

s32 FloatsEqual(f32 a, f32 b, f32 epsilon) {
    f32 diff = a - b;
    if (diff < 0.0f) {
        diff *= -1.0f;
    }
    return diff < epsilon;
}

void GenerateQuaternion(Vector *axis, Vector *q, f32 angle) {
    f32 s;
    if (angle) {
        angle *= 0.5f;
        q->w = cos(angle);
        s = sin(angle);
        q->x = axis->x * s;
        q->y = axis->y * s;
        q->z = axis->z * s;
    } else {
        q->w = 1.0f;
        q->x = q->y = q->z = 0.0f;
    }
}

void GenerateSpinAngleFromQuaternion(Vector *q, Vector *axis, f32 *angle) {
    f32 s;
    *angle = acos(q->w);
    s = sin(*angle);
    if (0.0f == s) {
        axis->x = 1.0f;
        axis->y = 0.0f;
        axis->z = 0.0f;
    } else {
        axis->x = q->x / s;
        axis->y = q->y / s;
        axis->z = q->z / s;
        *angle *= 2.0f;
    }
}

void QuaternionMult(Vector *a, Vector *b, Vector *out) {
    out->w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    out->x = a->y * b->z - a->z * b->y + a->w * b->x + a->x * b->w;
    out->y = a->z * b->x - a->x * b->z + a->w * b->y + a->y * b->w;
    out->z = a->x * b->y - a->y * b->x + a->w * b->z + a->z * b->w;
}

void GetComponentOfVector(void *arg0, void *arg1) {
    f32 PSVECDotProduct();
    f32 temp_f1;

    temp_f1 = PSVECDotProduct();
    M2C_FIELD(arg1, f32 *, 0) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 4) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 4));
    M2C_FIELD(arg1, f32 *, 8) = (f32) (temp_f1 * M2C_FIELD(arg0, f32 *, 8));
}

void ProjectVectorToNormal(Vector *n, Vector *v) {
    f32 PSVECDotProduct();
    f32 scale;

    scale = -PSVECDotProduct(n, v);
    v->x = scale * n->x + v->x;
    v->y = scale * n->y + v->y;
    v->z = scale * n->z + v->z;
}

void MinMaxVec(Vector *min, Vector *max, Vector *v) {
    if (min->x > v->x) {
        min->x = v->x;
    }
    if (min->y > v->y) {
        min->y = v->y;
    }
    if (min->z > v->z) {
        min->z = v->z;
    }
    if (min->w > v->w) {
        min->w = v->w;
    }
    if (max->x < v->x) {
        max->x = v->x;
    }
    if (max->y < v->y) {
        max->y = v->y;
    }
    if (max->z < v->z) {
        max->z = v->z;
    }
    if (max->w < v->w) {
        max->w = v->w;
    }
}

void ScaleMatrix(void *arg0, void *arg1) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg0, f32 *, 0) * M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg0, f32 *, 4) * M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg0, f32 *, 8) * M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) (M2C_FIELD(arg0, f32 *, 0xC) * M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (M2C_FIELD(arg0, f32 *, 0x10) * M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (M2C_FIELD(arg0, f32 *, 0x14) * M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) (M2C_FIELD(arg0, f32 *, 0x18) * M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 0x1C) = (f32) (M2C_FIELD(arg0, f32 *, 0x1C) * M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) (M2C_FIELD(arg0, f32 *, 0x20) * M2C_FIELD(arg1, f32 *, 8));
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) (M2C_FIELD(arg0, f32 *, 0x24) * M2C_FIELD(arg1, f32 *, 8));
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (M2C_FIELD(arg0, f32 *, 0x28) * M2C_FIELD(arg1, f32 *, 8));
    M2C_FIELD(arg0, f32 *, 0x2C) = (f32) (M2C_FIELD(arg0, f32 *, 0x2C) * M2C_FIELD(arg1, f32 *, 8));
    M2C_FIELD(arg0, f32 *, 0x30) = (f32) (M2C_FIELD(arg0, f32 *, 0x30) * M2C_FIELD(arg1, f32 *, 0xC));
    M2C_FIELD(arg0, f32 *, 0x34) = (f32) (M2C_FIELD(arg0, f32 *, 0x34) * M2C_FIELD(arg1, f32 *, 0xC));
    M2C_FIELD(arg0, f32 *, 0x38) = (f32) (M2C_FIELD(arg0, f32 *, 0x38) * M2C_FIELD(arg1, f32 *, 0xC));
    M2C_FIELD(arg0, f32 *, 0x3C) = (f32) (M2C_FIELD(arg0, f32 *, 0x3C) * M2C_FIELD(arg1, f32 *, 0xC));
}

void Dprintf(char *str, ...) {
}

void ForceIPLReset(void) {
    extern s32 m_ilpreset_803C7620;
    extern s32 m_reset;
    m_ilpreset_803C7620 = 1;
    m_reset = 1;
}

s32 Init_Display(void) {
    extern s32 g_viewID;

    g_viewID = Gfx_CreateView(0, 640, 480, 16, 0, 0);
    return g_viewID >= 0;
}

void Main_Shutdown_NGC(void) {

}

s32 RunThread(void *(*func)(void *)) {
    s32 OSCreateThread(void *thread, void *(*func)(void *), void *param, void *stack, u32 stackSize, s32 priority, u16 attr);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    s32 OSResumeThread(void *thread);
    extern u8 Thread_8030E0F0[0x310];
    extern u8 ThreadStack_8030E400[0x5000];
    extern s8 NGC_Thread_On;
    s32 enabled;

    OSCreateThread(Thread_8030E0F0, func, 0, ThreadStack_8030E400 + 0x5000, 0x5000, 0x10, 1);
    enabled = OSDisableInterrupts();
    OSResumeThread(Thread_8030E0F0);
    OSRestoreInterrupts(enabled);
    NGC_Thread_On = 1;
    return 1;
}

s8 isInThread(void) {
    void *OSGetCurrentThread(void);
    extern u8 Thread_8030E0F0[0x310];

    return (s8)(Thread_8030E0F0 == OSGetCurrentThread());
}

void EndThread(void) {
    M2C_UNK OSExitThread(M2C_UNK *);
    extern s8 NGC_Thread_On;
    M2C_UNK sp8;

    OSExitThread(&sp8);
    NGC_Thread_On = 0;
}

void TextureManager_ReleaseCache(void) {

}

void InitVI(void) {
    M2C_UNK VIFlush();
    M2C_UNK VISetNextFrameBuffer(s32);
    M2C_UNK VIWaitForRetrace();
    extern s32 g_currentBuffer;
    extern s32 g_frameBuffer1;
    extern s32 g_frameBuffer2;
    extern s32 *rmode;
    VISetNextFrameBuffer(g_frameBuffer1);
    g_currentBuffer = g_frameBuffer2;
    VIFlush();
    VIWaitForRetrace();
    if (*rmode & 1) {
        VIWaitForRetrace();
    }
}

#pragma push
#pragma use_lmw_stmw on
void GetSystemDate(s32 *year, s32 *month, s32 *day, s32 *hour, s32 *minute) {
    s64 OSGetTime(void);
    void OSTicksToCalendarTime(s64 ticks, void *td);
    s32 td[10];

    OSTicksToCalendarTime(OSGetTime(), td);
    *year = td[5];
    *month = td[4] + 1;
    *day = td[3];
    *hour = td[2];
    *minute = td[1];
}
#pragma pop

void *Allocate(u32 size, u32 flags, const char *name) {
    void *OSAllocFromHeap(s32 heap, u32 size);
    void Dprintf(const char *fmt, ...);
    void *ptr;

    if (size == 0) {
        return 0;
    }
    ptr = OSAllocFromHeap(__OSCurrHeap, size);
    if (ptr == 0) {
        Dprintf("ALLOCATE ERROR Could not allocate the requested memory, going to crash!!\n");
    }
    return ptr;
}

void Deallocate(s32 ptr) {
    if (ptr != 0) {
        OSFreeToHeap(__OSCurrHeap, ptr);
    }
}

void MemCpy32(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void MemClr32(void *dst, u32 size) {
    memset(dst, 0, size);
}

void HeapDump(void) {

}

void Create_Plane3(Vector *plane, Vector *normal, Vector *point) {
    plane->x = normal->x;
    plane->y = normal->y;
    plane->z = normal->z;
    plane->w = -1.0f * (normal->z * point->z + (normal->x * point->x + normal->y * point->y));
}

void Reverse_Plane(Vector *plane) {
    plane->x *= -1.0f;
    plane->y *= -1.0f;
    plane->z *= -1.0f;
    plane->w *= -1.0f;
}

f32 Distance_Plane(void *arg0, void *arg1) {
    return M2C_FIELD(arg0, f32 *, 0xC) + ((M2C_FIELD(arg0, f32 *, 8) * M2C_FIELD(arg1, f32 *, 8)) + ((M2C_FIELD(arg0, f32 *, 0) * M2C_FIELD(arg1, f32 *, 0)) + (M2C_FIELD(arg0, f32 *, 4) * M2C_FIELD(arg1, f32 *, 4))));
}

#pragma push
#pragma dont_inline on
void RandomNumberGenerator_GetRandomQuaternion(Vector *q) {
    Vector axis __attribute__((aligned(16)));
    f32 angle;

    angle = RandomAdd(0.0f, 6.2831855f);
    RandomNumberGenerator_GetRandomVector(&axis);
    GenerateQuaternion(&axis, q, angle);
}
#pragma pop
void RandomNumberGenerator_GetRandomSubMatrix(Vector *m) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    s32 i = rng->unk3000 * 3;

    m[0].x = rng->vectors[i].x;
    m[0].y = rng->vectors[i].y;
    m[0].z = rng->vectors[i++].z;
    m[1].x = rng->vectors[i].x;
    m[1].y = rng->vectors[i].y;
    m[1].z = rng->vectors[i++].z;
    m[2].x = rng->vectors[i].x;
    m[2].y = rng->vectors[i].y;
    m[2].z = rng->vectors[i].z;
    if (++rng->unk3000 >= 0x100) {
        rng->unk3000 = 0;
    }
}

void RandomNumberGenerator_GetRandomVector(Vector *out) {
    *out = g_randomNumberGenerator.vectors[RandomAdd_s32(0, 0x300)];
}

s32 RandomNumberGenerator_GetRandomInteger(s32 min, s32 max) {
    return RandomAdd_s32(min, max - min);
}

s32 RandomAdd_s32(s32 base, s32 range) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * range + base;
}

f32 RandomDirVar(f32 base, f32 var) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * (var * 2.0f) + (base - var);
}

void TimeHandler_SetSlowMotion(void) {
    extern u8 g_iTimeShift;
    if (g_iTimeShift) {
        g_iTimeShift = 0;
    } else {
        g_iTimeShift = 1;
    }
}

void Timer_Heartbeat(void) {
    extern s32 g_timer300;
    extern s32 g_timer163;
    extern s32 g_timer78;
    extern s32 g_iDeltaTimeMS;

    if (g_timer300 < 0) {
        g_timer300 += 300;
    }
    g_timer300 -= g_iDeltaTimeMS;
    if (g_timer163 < 0) {
        g_timer163 += 163;
    }
    g_timer163 -= g_iDeltaTimeMS;
    if (g_timer78 < 0) {
        g_timer78 += 78;
    }
    g_timer78 -= g_iDeltaTimeMS;
}

f32 RandomNumberGenerator_GetRandomFloat(f32 min, f32 max) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * (max - min) + min;
}

f32 RandomAdd(f32 base, f32 range) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * range + base;
}

s8 Timer_Interrupt300(void) {
    extern u32 g_timer300;
    return (s8) ((u32) g_timer300 >> 0x1FU);
}

void TimeHandler_Init(void) {
    u32 OSGetTick(void);
    s64 OSGetTime(void);
    void OSTicksToCalendarTime(s64 ticks, void *td);
    extern u8 g_todaysDate[];
    extern s32 m_iDeltaTimeUs;
    extern u32 m_iLastTimeUs;

    m_iLastTimeUs = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
    m_iDeltaTimeUs = 1;
    OSTicksToCalendarTime(OSGetTime(), g_todaysDate);
}

void TimeHandler_SetTimeElapsedSec_f32(f32 sec)
{
  extern f32 g_fDeltaTime60th;
  extern f32 g_fDeltaTimeSec;
  extern f32 g_fDeltaTimeSec_inv;
  extern u32 g_iDeltaTimeMS;
  u32 ms;
  ms = (u32) (1000.0f * sec);
  g_fDeltaTime60th = ((f32) ms) / (1000.0f / 60.0f);
  g_iDeltaTimeMS = ms;
  g_fDeltaTimeSec = sec;
  g_fDeltaTimeSec_inv = 1.0f / sec;
}

void Performance_Start(void) {

}

void Performance_End(void) {

}

void Release_Performance_Get(u32 *arg0) {
    u32 OSGetTick(void);
    *arg0 = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

void Release_Performance_Diff(u32 *arg0) {
    u32 OSGetTick(void);
    extern u32 max_time;
    u32 now;

    now = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
    if (now < *arg0) {
        now += max_time;
    }
    *arg0 = now - *arg0;
}

void AnimExtractKeyFrame_NonCompound(void) {

}

void AnimExtractKeyFrame(void *anim, s32 frame) {
    void AnimExtractKeyFrame_Compound(void *anim, s32 frame);
    void AnimExtractKeyFrame_CompoundHybrid(void *anim, s32 frame);
    void AnimExtractKeyFrame_Hybrid(void *anim, s32 frame);
    void AnimExtractKeyFrame_NonCompound(void *anim, s32 frame);
    s32 flags = *(s32 *)((u8 *)anim + 4);

    if (flags & 2) {
        if (flags & 8) {
            AnimExtractKeyFrame_CompoundHybrid(anim, frame);
        } else {
            AnimExtractKeyFrame_Compound(anim, frame);
        }
    } else if (flags & 8) {
        AnimExtractKeyFrame_Hybrid(anim, frame);
    } else {
        AnimExtractKeyFrame_NonCompound(anim, frame);
    }
}

void FontManager_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void FontManager_Destroy(void) {

}

void Gfx_FontManager_Init(void) {
    extern FontManager g_fontManager;
    extern GfxState g_state;
    u32 i;

    g_state.debugFont = 0;
    g_fontManager.numFonts = 0;
    for (i = 0; i < 16; i++) {
        g_fontManager.fonts[i].inUse = 0;
    }
}

void Gfx_SetDebugFont(s16 font) {
    extern GfxState g_state;
    g_state.debugFont = font;
}

s32 Gfx_FontManager_ReleaseFont(s32 id) {
    extern FontManager g_fontManager;
    Font *font = &g_fontManager.fonts[id];
    s32 ret = 0;

    if (font->inUse == 0) {
        return 1;
    }
    if (--font->refCount == 0) {
        font->inUse = 0;
        g_fontManager.numFonts--;
        ret = 1;
    }
    return ret;
}

void Gfx_FontManager_SetCharacterSpacing(s32 id, u8 c, f32 x, f32 y) {
    extern FontManager g_fontManager;
    extern u8 *m_texture;
    Font *font = &g_fontManager.fonts[id];
    Texture *tex = &((Texture *)m_texture)[font->texture];
    u16 width = tex->width;

    font->chars[c].spacingX = x;
    font->chars[c].spacingY = y;
    font->chars[c].unk8 = font->chars[c].unk0 + x / width;
}

void Gfx_AnimExtractKeyFrame(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_AnimTraverseHeirarchyToMatrices(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

#pragma push
#pragma dont_inline on
void Gfx_DisplayList_Destroy(void) {
    extern s32 g_pDL;
    extern s32 g_pGDL;
    Deallocate(g_pGDL);
    Deallocate(g_pDL);
}

GDLLine *Gfx_Add3DLineFX(void) {
    extern GDL *g_pGDL;

    if (g_pGDL->numLines >= 0x100) {
        g_pGDL->numLines = 0;
    }
    return &g_pGDL->lines[g_pGDL->numLines++];
}

GDLSprite *Gfx_AddBillBoardSprite(u16 tex) {
    extern GDL *g_pGDL;
    GDLSprite *sprite;

    if (g_pGDL->numSprites == 0x7CF) {
        g_pGDL->numSprites = 0;
    }
    sprite = &g_pGDL->sprites[g_pGDL->numSprites];
    sprite->next = g_pGDL->spriteHeads[tex];
    g_pGDL->spriteHeads[tex] = g_pGDL->numSprites++;
    return sprite;
}

void Gfx_End_GroupTri(s32 n) {
    extern GDL *g_pGDL;

    g_pGDL->groupTris[g_pGDL->numGroupTris].count += n;
    if (g_pGDL->groupTris[g_pGDL->numGroupTris].count != 0) {
        switch (g_pGDL->groupTris[g_pGDL->numGroupTris].type) {
        case 0:
            g_pGDL->unk38 += n;
            break;
        case 1:
            g_pGDL->unk3C += n;
            break;
        case 2:
            break;
        case 4:
            g_pGDL->unk40 += n;
            break;
        }
        g_pGDL->numGroupTris++;
    }
}

GDLRect *Gfx_Add2DRectangleRenderWindow(void) {
    extern GDL *g_pGDL;
    extern s32 g_currentWindow;

    if (g_pGDL->numRects >= 0x20) {
        g_pGDL->numRects = 0;
    }
    g_pGDL->rects[g_pGDL->numRects].window = g_currentWindow;
    return &g_pGDL->rects[g_pGDL->numRects++];
}

GDLRect *Gfx_Add2DRectangle(void) {
    extern GDL *g_pGDL;

    if (g_pGDL->numRects2D >= 0x400) {
        g_pGDL->numRects2D = 0;
    }
    return &g_pGDL->rects2D[g_pGDL->numRects2D++];
}

#pragma pop

void Gfx_Add2DRectangleBack(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void DrawAlphaTable(void) {
    extern GDL *g_pGDL;
    u16 i;
    u16 idx;

    for (i = 0; i < 0x100; i++) {
        for (idx = g_pGDL->alphaTable[i]; idx != 0; idx = g_pGDL->alphaEntries[idx].next) {
            g_pGDL->alphaEntries[idx].func(g_pGDL->alphaEntries[idx].arg);
        }
    }
}

s32 Gfx_GetEnvmapState(void) {
    extern GDL *g_pGDL;
    extern GfxState g_state;

    if (g_state.unk8F) {
        return 0;
    }
    return g_pGDL->numEnvmapStates - 1;
}

s32 Gfx_AddWorldMatrixState_NonCached(u32 matrix) {
    extern GDL *g_pGDL;

    if (matrix == 0) {
        return 0xFFFE;
    }
    if (g_pGDL->numMatrices >= 0x578) {
        g_pGDL->numMatrices = 0;
    }
    g_pGDL->matrices[g_pGDL->numMatrices] = matrix;
    return g_pGDL->numMatrices++;
}

void *Gfx_GetWorldMatrixState(u32 index) {
    extern GDL *g_pGDL;
    extern f32 g_identity[];

    if (index == 0xFFFE) {
        return g_identity;
    }
    if (index == 0xFFFF) {
        return (u8 *)GetRenderWindow() + 0x140;
    }
    return (void *)g_pGDL->matrices[index];
}

void Gfx_GetCameraPosition(Vector *pos) {
    *pos = *(Vector *)((u8 *)GetRenderWindow() + 0x180);
}

void Gfx_GetCameraVector(Vector *right, Vector *up, Vector *at) {
    *right = *(Vector *)((u8 *)GetRenderWindow() + 0x190);
    *up = *(Vector *)((u8 *)GetRenderWindow() + 0x1A0);
    *at = *(Vector *)((u8 *)GetRenderWindow() + 0x1B0);
}

DLSkinGroup *DisplayList_AddGroup_Skin(void) {
    extern DisplayList *g_pDL;
    DisplayList *dl = g_pDL;

    if (dl->numSkinGroups == 0x100) {
        return NULL;
    }
    return &dl->skinGroups[dl->numSkinGroups++];
}

s32 Gfx_FrustrumCullSphereTestAllWindows(Vector *center, f32 radius) {
    extern u8 g_CurrentViews;
    void Gfx_SetRenderWindow(u32 view);
    s32 Gfx_FrustrumCullSphereTest(Vector *center, f32 radius);
    u32 i;
    s32 ret;

    if (g_CurrentViews != 1) {
        for (i = 1; i < 5; i++) {
            if (g_CurrentViews & (1 << i)) {
                Gfx_SetRenderWindow(i);
                ret = Gfx_FrustrumCullSphereTest(center, radius);
                if (ret) {
                    return ret;
                }
            }
        }
        return ret;
    }
    return Gfx_FrustrumCullSphereTest(center, radius);
}

s32 Gfx_AddWorldMatrixState_CacheMatrix(s32 matrix);
s32 Gfx_GetLightState(void);
#pragma push
#pragma dont_inline on
void Gfx_DrawGeometry(s32 geometry, s32 matrix) {
    extern GDL *g_pGDL;
    extern GfxState g_state;

    g_pGDL->drawEntries[g_pGDL->numDrawEntries].geometry = geometry;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].matrix = Gfx_AddWorldMatrixState_CacheMatrix(matrix);
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk10 = (g_state.unk7C >> 4) & 1;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk4 = g_state.unk74;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].lightState = Gfx_GetLightState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].envmapState = Gfx_GetEnvmapState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk11 = 0;
    g_pGDL->numDrawEntries++;
}

void Gfx_DrawGeometry_MatrixNotCached(s32 geometry, s32 matrix) {
    extern GDL *g_pGDL;
    extern GfxState g_state;

    g_pGDL->drawEntries[g_pGDL->numDrawEntries].geometry = geometry;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].matrix = Gfx_AddWorldMatrixState_NonCached(matrix);
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk10 = (g_state.unk7C >> 4) & 1;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk4 = g_state.unk74;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].lightState = Gfx_GetLightState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].envmapState = Gfx_GetEnvmapState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk11 = 0;
    g_pGDL->numDrawEntries++;
}

s32 Gfx_Create(void) {
    void Init_Math3D(void);
    void RenderState_Init(void);
    void Render_Create(void);
    void TimeHandler_Init(void);
    void FontManager_Create(void);
    s32 Gfx_TextureHandler_Create(void);
    s32 Gfx_DisplayList_Create(void);
    extern GXColor g_textcolor;
    extern s8 g_screenTextY;
    extern s8 g_screenTextX;
    extern u8 g_CurrentViews;
    extern u8 g_screenText[0xA00];

    Init_Math3D();
    RenderState_Init();
    Render_Create();
    TimeHandler_Init();
    FontManager_Create();
    g_textcolor.a = 0xFF;
    g_textcolor.b = 0xFF;
    g_textcolor.g = 0xFF;
    g_textcolor.r = 0xFF;
    g_screenTextY = 0;
    g_screenTextX = 0;
    g_CurrentViews = 1;
    memset(g_screenText, 0, sizeof(g_screenText));
    if (Gfx_TextureHandler_Create() == 0) {
        return 0;
    } else if (Gfx_DisplayList_Create() == 0) {
        return 0;
    }
    return 1;
}

#pragma pop

void Gfx_Destroy(void) {
    M2C_UNK Gfx_DisplayList_Destroy();
    M2C_UNK Gfx_TextureHandler_Destroy();
    M2C_UNK Render_Destroy();
    M2C_UNK FontManager_Destroy();
    FontManager_Destroy();
    Gfx_DisplayList_Destroy();
    Gfx_TextureHandler_Destroy();
    Render_Destroy();
}

s32 Gfx_CreateView(s32 arg0, s32 width, s32 height, s32 arg3, s32 arg4, s32 arg5) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_BeginScene(void) {
    void Render_BeginScene(void);
    void Render_InitDisplayList(void);
    void Gfx_SetProjectionMatrix(f32 fov);
    void Gfx_SetWorldMatrix(void *matrix);
    extern GfxState g_state;
    extern u8 g_renderStats[0x100];

    g_state.unk7C = 0;
    Render_BeginScene();
    Render_InitDisplayList();
    memset(g_renderStats, 0, sizeof(g_renderStats));
    Gfx_SetProjectionMatrix(*(f32 *)((u8 *)GetRenderWindow() + 0x270));
    Gfx_SetWorldMatrix(0);
}

void Gfx_EndScene(void) {
    M2C_UNK Render_EndScene();
    extern s8 g_screenTextX;
    extern s8 g_screenTextY;
    Render_EndScene();
    g_screenTextY = 0;
    g_screenTextX = 0;
}

void Gfx_FlipBuffers(void) {
    void Render_FlipBuffers(void);
    extern u8 g_screenText[0xA00];

    Render_FlipBuffers();
    memset(g_screenText, 0, sizeof(g_screenText));
}

void RenderState_Init(void) {
    extern GfxState g_state;
    g_state.unk7C = 1;
    g_state.numPointLights = 0;
    g_state.unk80 = 1.0f;
    g_state.unk91 = 0xFF;
    g_state.lightChanged = 1;
    g_state.unk8F = 1;
    g_state.unk92 = 0xFF;
    g_state.enviroMapMode = 0;
    g_state.unk88 = 0;
    g_state.farClippingPlane = 10000.0f;
}

void Gfx_EnableState(u32 state) {
    extern GfxState g_state;
    g_state.unk7C |= state;
}

void Gfx_DisableState(u32 state) {
    extern GfxState g_state;
    g_state.unk7C &= ~state;
}

void Gfx_SetEnviroMapMode(u8 mode) {
    extern GfxState g_state;
    g_state.enviroMapMode = mode;
}

void Gfx_SetFarClippingPlane(f32 farPlane) {
    extern GfxState g_state;
    g_state.farClippingPlane = farPlane;
}

void Gfx_RemovePointLights(void) {
    extern GfxState g_state;
    if (g_state.numPointLights != 0) {
        g_state.numPointLights = 0;
        g_state.lightChanged = 1;
    }
}

void Gfx_AddPointLight(GXColor *color, Vector *position, f32 radius) {
    extern GfxState g_state;

    if (g_state.numPointLights >= 3) {
        return;
    }
    g_state.pointLights[g_state.numPointLights].color = *color;
    g_state.pointLights[g_state.numPointLights].position = *position;
    g_state.pointLights[g_state.numPointLights].radius = radius;
    g_state.lightChanged = 1;
    g_state.numPointLights++;
}

void Gfx_SetParallelLight(GXColor *color, Vector *dir) {
    extern GfxState g_state;
    f32 PSVECSquareMag(Vector *v);
    void PSVECNormalize(Vector *src, Vector *dst);
    Vector *lightDir;

    g_state.lightColor = *color;
    if (dir != NULL) {
        lightDir = &g_state.lightDir;
        *lightDir = *dir;
        if (PSVECSquareMag(lightDir)) {
            PSVECNormalize(lightDir, lightDir);
        }
    }
    g_state.lightChanged = 1;
}

void Gfx_SetAmbientColor(GXColor *color) {
    extern GfxState g_state;
    g_state.ambientColor = *color;
}

void Gfx_SetBaseColor(GXColor *color) {
    extern GfxState g_state;
    g_state.unk74 = *color;
}

void Gfx_SetRenderWindow(s32 window) {
    extern s32 g_currentWindow;
    extern void *g_pWindow;
    extern u8 g_window[][0x2D0];
    g_currentWindow = window;
    g_pWindow = g_window[window];
}

s32 Gfx_GetRenderWindowIndex(void) {
    extern s32 g_currentWindow;
    return g_currentWindow;
}

void *GetRenderWindow() {
    extern s32 g_currentWindow;
    extern u8 g_window[][0x2D0];
    return g_window[g_currentWindow];
}

s32 Gfx_GetCameraPositions(Vector *out) {
    extern u8 g_CurrentViews;
    extern u8 g_window[][0x2D0];
    u32 i;
    s32 count = 0;

    for (i = 0; i < 5; i++) {
        if (g_CurrentViews & (1 << i)) {
            out[count++] = *(Vector *)(g_window[i] + 0x180);
        }
    }
    return count;
}

f32 Gfx_GetMinCameraDistSquared(Vector *pos) {
    extern u8 g_CurrentViews;
    extern u8 g_window[][0x2D0];
    f32 PSVECSquareDistance(Vector *a, Vector *b);
    f32 minDist;
    f32 dist;
    u32 i;

    if (g_CurrentViews != 1) {
        minDist = 99999.9f;
        for (i = 1; i < 5; i++) {
            if (g_CurrentViews & (1 << i)) {
                dist = PSVECSquareDistance(pos, (Vector *)(g_window[i] + 0x180));
                if (minDist > dist) {
                    minDist = dist;
                }
            }
        }
        return minDist;
    }
    return PSVECSquareDistance(pos, (Vector *)(g_window[0] + 0x180));
}

void Gfx_ClearScreen(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

#pragma push
#pragma dont_inline on
s32 Gfx_TextureHandler_Create(void) {
    extern u8 *m_texture;
    extern u32 m_totalTextures;

    m_totalTextures = 0x1FF;
    m_texture = Allocate(0x7FC0, 0, "");
    memset(m_texture, 0, m_totalTextures << 6);
    return 1;
}
#pragma pop

void Gfx_TextureHandler_Destroy(void) {
    extern u8 *m_texture;
    extern u32 m_totalTextures;
    void Render_DestroyTextureSlot(u32 id);
    void Deallocate(void *p);
    u32 i;

    for (i = 1; i < m_totalTextures; i++) {
        Render_DestroyTextureSlot(i);
        if (*(void **)(m_texture + i * 0x40 + 0x24) != NULL && *(u16 *)(m_texture + i * 0x40 + 0x3A) == 0) {
            Deallocate(*(void **)(m_texture + i * 0x40 + 0x24));
        }
    }
    Deallocate(m_texture);
    m_texture = NULL;
}

#pragma push
#pragma readonly_strings on
#pragma dont_inline on
u16 Gfx_LoadTexture(char *name) {
    extern u8 *m_texture;
    extern u32 m_totalTextures;
    extern s32 m_currentView;
    s32 LoadBTF(char *name, u32 id);
    s32 LoadTGA(char *name, u32 id);
    s32 stricmp(const char *s1, const char *s2);
    char buf[260];
    char *ext;
    Texture *tex;
    u16 i;

    if (strcmp(name, "_NONE_") == 0) {
        return 0xFFFF;
    }
    strcpy(buf, name);
    if ((ext = strstr(buf, ".tga")) != NULL) {
        *ext = 0;
    } else if ((ext = strstr(buf, ".TGA")) != NULL) {
        *ext = 0;
    } else if ((ext = strstr(buf, ".btf")) != NULL) {
        *ext = 0;
    }
    if (strlen(buf) > 34) {
        buf[34] = 0;
    }

    for (i = 1; i < m_totalTextures; i++) {
        if (stricmp(buf, (char *)((Texture *)m_texture)[i].unk0) == 0 && m_currentView == *(u16 *)(m_texture + (i * sizeof(Texture) + 0x3C))) {
            tex = &((Texture *)m_texture)[i];
            tex->refCount++;
            return i;
        }
    }

    for (i = 1; i < m_totalTextures; i++) {
        if (((Texture *)m_texture)[i].data == NULL) {
            break;
        }
    }
    if (i >= m_totalTextures) {
        i = m_totalTextures - 1;
    }

    tex = &((Texture *)m_texture)[i];
    tex->wrapMode = 0;
    if (LoadBTF(buf, i) == 0 && LoadTGA(buf, i) == 0) {
        return 0xFFFF;
    }
    tex = &((Texture *)m_texture)[i];
    strncpy((char *)tex->unk0, buf, 0x24);
    tex = &((Texture *)m_texture)[i];
    tex->refCount++;
    ((Texture *)m_texture)[i].view = (s8)m_currentView;
    tex = &((Texture *)m_texture)[i];
    tex->unk2C = 0.25f;
    tex = &((Texture *)m_texture)[i];
    tex->unk3A = 0;
    return i;
}
#pragma pop

u8 *LoadBTF_RAM(u8 *data, u32 id) {
    extern u8 *m_texture;
    void Render_CreateTextureSlot(u32 id, s32 arg1, void *texture);
    Texture *tex;

    tex = &((Texture *)m_texture)[id];
    tex->width = ((*(u16 *)(data + 4) & 0xFF00) / 256) | (*(u16 *)(data + 4) * 256);
    tex = &((Texture *)m_texture)[id];
    __sthbrx(*(u16 *)(data + 2), &tex->unk30, 0);
    tex = &((Texture *)m_texture)[id];
    __sthbrx(*(u16 *)(data + 6), &tex->height, 0);
    tex = &((Texture *)m_texture)[id];
    tex->data = *(void **)(data + 8);
    tex = &((Texture *)m_texture)[id];
    tex->unk28 = *(u32 *)(data + 0xC);
    tex = &((Texture *)m_texture)[id];
    __sthbrx(*(u16 *)(data + 0), &tex->unk38, 0);
    Render_CreateTextureSlot(id, 1, tex);
    return data + 0x10;
}

void Gfx_SetTextureWrapMode(u16 id, s16 mode) {
    extern u8 *m_texture;
    void Render_CreateTextureSlot(u32 id, s32 arg1, void *texture);
    u8 *tex = m_texture + id * 0x40;

    *(s16 *)(tex + 0x3E) = mode;
    Render_CreateTextureSlot(id, 1, tex);
}

void Gfx_LoadPalettes_RAM(void) {

}

void MovieManager_Destroy(void) {

}

s32 MovieManager_GetTextureIndex(void) {
    return 0;
}

void Render_DestroyTextureSlot(u32 id) {
    void GXInitTexObj(void *obj, void *image, u16 width, u16 height, s32 format, s32 wrapS, s32 wrapT, u8 mipmap);
    extern u8 m_gxTexObjArray_80322F30[][0x20];

    GXInitTexObj(m_gxTexObjArray_80322F30[id], NULL, 0x20, 0x20, 6, 0, 0, 0);
}

void Render_SetTextureHandle(u16 id) {
    void GXSetNumTexGens(u8 n);
    void GXSetTevOrder(s32 stage, s32 coord, s32 map, s32 color);
    void GXLoadTexObj(void *obj, s32 map);
    extern u32 m_curTexture;
    extern u8 m_gxTexObjArray_80322F30[][0x20];

    if (id == 0xFFFF) {
        GXSetNumTexGens(0);
        GXSetTevOp(0, 4);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        m_curTexture = 0xFFFF;
    } else if (m_curTexture != id) {
        GXSetNumTexGens(1);
        GXSetTevOp(0, 0);
        GXSetTevOrder(0, 0, 0, 4);
        GXLoadTexObj(m_gxTexObjArray_80322F30[id], 0);
        m_curTexture = id;
    }
}

void Render_FlipBuffers(void) {
    void PrintDVDMessage(void);
    void GXSetColorUpdate(u8 enable);
    void GXCopyDisp(void *dest, u8 clear);
    void GXDrawDone(void);
    void VISetNextFrameBuffer(void *fb);
    void VISetBlack(s32 black);
    void VIFlush(void);
    void VIWaitForRetrace(void);
    extern void *g_currentBuffer;
    extern void *g_frameBuffer1;
    extern void *g_frameBuffer2;
    extern u8 m_firstFrame_803C6ED8;

    PrintDVDMessage();
    GXSetColorUpdate(1);
    GXCopyDisp(g_currentBuffer, 1);
    GXDrawDone();
    VISetNextFrameBuffer(g_currentBuffer);
    if (m_firstFrame_803C6ED8) {
        VISetBlack(0);
        m_firstFrame_803C6ED8 = 0;
    }
    VIFlush();
    VIWaitForRetrace();
    if (g_currentBuffer == g_frameBuffer1) {
        g_currentBuffer = g_frameBuffer2;
    } else {
        g_currentBuffer = g_frameBuffer1;
    }
}

#pragma push
#pragma dont_inline on
s32 DisplayList_Create(void) {
    void DisplayList_Init(void);
    extern void *g_pDL;

    g_pDL = Allocate(0x125DC, 0, "");
    if (g_pDL != 0) {
        DisplayList_Init();
        return 1;
    }
    return 0;
}
#pragma pop

void DisplayList_Init(void) {
    extern void *g_pDL;

    memset(g_pDL, 0, 0x18);
}

void Render_StateBlendMode(u32 mode) {
    void GXSetBlendMode(s32 type, s32 src, s32 dst, s32 op);
    extern s32 m_srcBlend_803C76D8;
    extern s32 m_dstBlend_803C76D4;

    switch (mode) {
    case 0:
    case 1:
        m_srcBlend_803C76D8 = 1;
        m_dstBlend_803C76D4 = 0;
        break;
    case 2:
        m_srcBlend_803C76D8 = 1;
        m_dstBlend_803C76D4 = 1;
        break;
    case 3:
    case 8:
    case 10:
        m_srcBlend_803C76D8 = 0;
        m_dstBlend_803C76D4 = 3;
        break;
    case 5:
    case 6:
    case 7:
        m_srcBlend_803C76D8 = 4;
        m_dstBlend_803C76D4 = 5;
        break;
    default:
        m_srcBlend_803C76D8 = 1;
        m_dstBlend_803C76D4 = 0;
        break;
    }
    GXSetBlendMode(1, m_srcBlend_803C76D8, m_dstBlend_803C76D4, 5);
}

void Gfx_StateTextureBlendMode(s32 arg0) {
    M2C_UNK GXSetTevOp(M2C_UNK, M2C_UNK);
    extern s32 m_blendMode_803C76D0;
    m_blendMode_803C76D0 = arg0;
    switch (arg0) {                                 /* irregular */
    case 1:
        GXSetTevOp(0, 1);
        return;
    case 2:
        GXSetTevOp(0, 0);
        return;
    case 3:
        GXSetTevOp(0, 3);
        return;
    case 4:
        GXSetTevOp(0, 2);
        return;
    }
}

void Render_StateAlphaBlendEnable(s32 enable) {
    void GXSetBlendMode(s32 type, s32 src, s32 dst, s32 op);
    void GXSetFog(s32 type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color);
    void Render_StateFogEnable(s32 enable);
    extern s32 m_fogMode_803C76CC;
    extern GXColor m_fogColor_803C76C8;
    extern s32 m_srcBlend_803C76D8;
    s32 fogMode = m_fogMode_803C76CC;

    if (enable == 1) {
        GXSetBlendMode(1, m_srcBlend_803C76D8, m_srcBlend_803C76D8, 5);
        GXSetFog(0, 1.0f, 10.0f, 1.0f, 10.0f, m_fogColor_803C76C8);
    } else {
        GXSetBlendMode(0, 4, 5, 5);
        m_fogMode_803C76CC = !m_fogMode_803C76CC;
        Render_StateFogEnable(fogMode);
    }
}

void Render_StateCullMode(s32 arg0) {
    M2C_UNK GXSetCullMode(M2C_UNK);
    extern s32 m_cullMode;
    switch (arg0) {                                 /* irregular */
    case 0:
        GXSetCullMode(0);
        break;
    case 1:
        GXSetCullMode(1);
        break;
    case 2:
        GXSetCullMode(2);
        break;
    }
    m_cullMode = arg0;
}

void Render_StateZEnable(s32 enable) {
    void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable);
    extern s32 m_zmode_803C76B8;
    extern s32 m_zstate_803C76B4;
    extern s32 m_zwrite_803C76B0;

    GXSetZMode(enable, m_zmode_803C76B8, m_zwrite_803C76B0);
    m_zstate_803C76B4 = enable;
}

void Render_StateZWriteEnable(s32 enable) {
    void GXSetZMode(u8 compare_enable, s32 func, u8 update_enable);
    extern s32 m_zmode_803C76B8;
    extern s32 m_zstate_803C76B4;
    extern s32 m_zwrite_803C76B0;

    GXSetZMode(m_zstate_803C76B4, m_zmode_803C76B8, enable);
    m_zwrite_803C76B0 = enable;
}

void Render_StateFogEnable(s32 enable) {
    void GXSetFog(s32 type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color);
    extern s32 m_fogMode_803C76CC;
    extern GXColor m_fogColor_803C76C8;
    extern u8 m_fogType_803C76C4;
    extern f32 m_fogRange1_803C76C0;
    extern f32 m_fogRange2_803C76BC;

    if (m_fogMode_803C76CC != enable) {
        if (enable) {
            GXSetFog(m_fogType_803C76C4, m_fogRange1_803C76C0, m_fogRange2_803C76BC, 1.0f, 15000.0f, m_fogColor_803C76C8);
        } else {
            GXSetFog(0, 1.0f, 10.0f, 1.0f, 10.0f, m_fogColor_803C76C8);
        }
        m_fogMode_803C76CC = enable;
    }
}

void Gfx_GetRenderStats(RenderStats *stats) {
    extern GDL *g_pGDL;
    extern RenderStats g_renderStats;
    extern s32 g_debugTriCount;

    g_renderStats.numTris = g_pGDL->numTris;
    g_renderStats.debugTriCount = g_debugTriCount;
    *stats = g_renderStats;
}

void Render_ClearScreen(void) {

}

void Set2DRectVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 2U) {
        m_VtxDescFlag = 2U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void Set2DRectFlatVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x80U) {
        m_VtxDescFlag = 0x80U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    }
}

void Set3DRectVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x40U) {
        m_VtxDescFlag = 0x40U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriFlatVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x20U) {
        m_VtxDescFlag = 0x20U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    }
}

void Render_Destroy(void) {

}

void Gfx_GetViewport(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0x280;
    *arg3 = 0x1E0;
}

void Render_UseLighting(u8 arg0) {
    M2C_UNK GXSetChanCtrl(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, s32, M2C_UNK, M2C_UNK);
    extern s32 m_pointLightMask_803C771C;
    if (arg0 != 0) {
        GXSetChanCtrl(4, 1, 0, 0, m_pointLightMask_803C771C | 1, 2, 2);
        return;
    }
    GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
}

void SetVertexBuffer(u32 *vb) {
    void GXSetArray(s32 attr, void *data, u8 stride);
    extern u32 *m_vertexBuffer;
    u8 *data;

    if (vb != m_vertexBuffer) {
        data = (u8 *)(vb + 4);
        m_vertexBuffer = vb;
        GXSetArray(9, data, 0xC);
        GXSetArray(0xD, data + vb[0], 8);
        if (vb[1] != 0) {
            GXSetArray(0xB, data + vb[1], 4);
        }
        if (vb[2] != 0) {
            GXSetArray(0xA, data + vb[2], 0xC);
        }
        DCFlushRange(vb, vb[3]);
    }
}

void SetTriVtxDescIdx(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x100U) {
        m_VtxDescFlag = 0x100U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriVtxDescIdxAlpha(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x400U) {
        m_VtxDescFlag = 0x400U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriNormVtxDescIdx(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x200U) {
        m_VtxDescFlag = 0x200U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xA, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xA, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriEnvVtxDescIdx(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 0x800U) {
        m_VtxDescFlag = 0x800U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xA, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xA, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    }
}

void SetTriVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 8U) {
        m_VtxDescFlag = 8U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void Render_SetConstColor(s32 *arg0) {
    M2C_UNK GXSetChanAmbColor(M2C_UNK, s32 *);
    M2C_UNK GXSetChanMatColor(M2C_UNK, s32 *);
    extern s32 black_803C6EE4;
    extern s32 white_803C6EE0;
    s32 sp14;
    s32 sp10;
    s32 spC;
    s32 sp8;

    if (arg0 != NULL) {
        sp14 = *arg0;
        GXSetChanMatColor(4, &sp14);
        sp10 = white_803C6EE0;
        GXSetChanAmbColor(4, &sp10);
        GXSetChanCtrl(4, 1, 0, 0, 1, 2, 2);
        return;
    }
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    spC = black_803C6EE4;
    GXSetChanAmbColor(4, &spC);
    sp8 = white_803C6EE0;
    GXSetChanMatColor(4, &sp8);
}

void Render_DrawLine2D(void *line) {
    typedef struct Line2D {
        Vector p0;
        Vector p1;
        u8 r;
        u8 g;
        u8 b;
    } Line2D;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void Set2DLineVtxDesc(void);
    Line2D *l = line;

    Set2DLineVtxDesc();
    GXBegin(0xA8, 0, 2);
    GXPosition2f32(l->p0.x, l->p0.y);
    GXColor3u8(l->r, l->g, l->b);
    GXPosition2f32(l->p1.x, l->p1.y);
    GXColor3u8(l->r, l->g, l->b);
}

void Set2DLineVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 1U) {
        m_VtxDescFlag = 1U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
    }
}

void Set3DLineVtxDesc(void) {
    M2C_UNK GXClearVtxDesc();
    M2C_UNK GXSetVtxAttrFmt(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetVtxDesc(M2C_UNK, M2C_UNK);
    extern u32 m_VtxDescFlag;
    if ((u32) m_VtxDescFlag != 4U) {
        m_VtxDescFlag = 4U;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
    }
}

f32 Gfx_GetProjectionMatrix(void) {
    void *GetRenderWindow();
    return M2C_FIELD(GetRenderWindow(), f32 *, 0x270);
}

#pragma push
#pragma dont_inline on
void Gfx_SetOrthoMatrix(void) {
    void C_MTXOrtho(f32 m[3][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
    void GXSetProjection(f32 m[3][4], s32 type);
    void PSMTXIdentity(f32 m[3][4]);
    void GXLoadPosMtxImm(f32 m[3][4], u32 id);
    void GXSetCurrentMtx(u32 id);
    f32 mtx[3][4];
    f32 proj[3][4];
    f32 width;

    width = *(u32 *)((u8 *)GetRenderWindow() + 0x280);
    C_MTXOrtho(proj, 0.0f, *(u32 *)((u8 *)GetRenderWindow() + 0x284), 0.0f, width, 1.0f, -1.0f);
    GXSetProjection(proj, 1);
    PSMTXIdentity(mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
}

void Render_SaveCamera(void) {
    extern Vector GXCameraSpace_8036CF80[7];

    GXCameraSpace_8036CF80[3] = *(Vector *)((u8 *)GetRenderWindow() + 0x180);
    GXCameraSpace_8036CF80[4] = *(Vector *)((u8 *)GetRenderWindow() + 0x190);
    GXCameraSpace_8036CF80[5] = *(Vector *)((u8 *)GetRenderWindow() + 0x1A0);
    GXCameraSpace_8036CF80[6] = *(Vector *)((u8 *)GetRenderWindow() + 0x1B0);
}
#pragma pop

void MultiplyMatrix4x4(void *arg0, void *arg1, void *arg2) {
    M2C_FIELD(arg2, f32 *, 0) = (f32) ((M2C_FIELD(arg1, f32 *, 0x30) * M2C_FIELD(arg0, f32 *, 0xC)) + ((M2C_FIELD(arg1, f32 *, 0x20) * M2C_FIELD(arg0, f32 *, 8)) + ((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg0, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 0x10) * M2C_FIELD(arg0, f32 *, 4)))));
    M2C_FIELD(arg2, f32 *, 4) = (f32) ((M2C_FIELD(arg1, f32 *, 0x34) * M2C_FIELD(arg0, f32 *, 0xC)) + ((M2C_FIELD(arg1, f32 *, 0x24) * M2C_FIELD(arg0, f32 *, 8)) + ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg0, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 0x14) * M2C_FIELD(arg0, f32 *, 4)))));
    M2C_FIELD(arg2, f32 *, 8) = (f32) ((M2C_FIELD(arg1, f32 *, 0x38) * M2C_FIELD(arg0, f32 *, 0xC)) + ((M2C_FIELD(arg1, f32 *, 0x28) * M2C_FIELD(arg0, f32 *, 8)) + ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg0, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 0x18) * M2C_FIELD(arg0, f32 *, 4)))));
    M2C_FIELD(arg2, f32 *, 0xC) = (f32) ((M2C_FIELD(arg1, f32 *, 0x3C) * M2C_FIELD(arg0, f32 *, 0xC)) + ((M2C_FIELD(arg1, f32 *, 0x2C) * M2C_FIELD(arg0, f32 *, 8)) + ((M2C_FIELD(arg1, f32 *, 0xC) * M2C_FIELD(arg0, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 0x1C) * M2C_FIELD(arg0, f32 *, 4)))));
    M2C_FIELD(arg2, f32 *, 0x10) = (f32) ((M2C_FIELD(arg1, f32 *, 0x30) * M2C_FIELD(arg0, f32 *, 0x1C)) + ((M2C_FIELD(arg1, f32 *, 0x20) * M2C_FIELD(arg0, f32 *, 0x18)) + ((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg0, f32 *, 0x10)) + (M2C_FIELD(arg1, f32 *, 0x10) * M2C_FIELD(arg0, f32 *, 0x14)))));
    M2C_FIELD(arg2, f32 *, 0x14) = (f32) ((M2C_FIELD(arg1, f32 *, 0x34) * M2C_FIELD(arg0, f32 *, 0x1C)) + ((M2C_FIELD(arg1, f32 *, 0x24) * M2C_FIELD(arg0, f32 *, 0x18)) + ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg0, f32 *, 0x10)) + (M2C_FIELD(arg1, f32 *, 0x14) * M2C_FIELD(arg0, f32 *, 0x14)))));
    M2C_FIELD(arg2, f32 *, 0x18) = (f32) ((M2C_FIELD(arg1, f32 *, 0x38) * M2C_FIELD(arg0, f32 *, 0x1C)) + ((M2C_FIELD(arg1, f32 *, 0x28) * M2C_FIELD(arg0, f32 *, 0x18)) + ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg0, f32 *, 0x10)) + (M2C_FIELD(arg1, f32 *, 0x18) * M2C_FIELD(arg0, f32 *, 0x14)))));
    M2C_FIELD(arg2, f32 *, 0x1C) = (f32) ((M2C_FIELD(arg1, f32 *, 0x3C) * M2C_FIELD(arg0, f32 *, 0x1C)) + ((M2C_FIELD(arg1, f32 *, 0x2C) * M2C_FIELD(arg0, f32 *, 0x18)) + ((M2C_FIELD(arg1, f32 *, 0xC) * M2C_FIELD(arg0, f32 *, 0x10)) + (M2C_FIELD(arg1, f32 *, 0x1C) * M2C_FIELD(arg0, f32 *, 0x14)))));
    M2C_FIELD(arg2, f32 *, 0x20) = (f32) ((M2C_FIELD(arg1, f32 *, 0x30) * M2C_FIELD(arg0, f32 *, 0x2C)) + ((M2C_FIELD(arg1, f32 *, 0x20) * M2C_FIELD(arg0, f32 *, 0x28)) + ((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg0, f32 *, 0x20)) + (M2C_FIELD(arg1, f32 *, 0x10) * M2C_FIELD(arg0, f32 *, 0x24)))));
    M2C_FIELD(arg2, f32 *, 0x24) = (f32) ((M2C_FIELD(arg1, f32 *, 0x34) * M2C_FIELD(arg0, f32 *, 0x2C)) + ((M2C_FIELD(arg1, f32 *, 0x24) * M2C_FIELD(arg0, f32 *, 0x28)) + ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg0, f32 *, 0x20)) + (M2C_FIELD(arg1, f32 *, 0x14) * M2C_FIELD(arg0, f32 *, 0x24)))));
    M2C_FIELD(arg2, f32 *, 0x28) = (f32) ((M2C_FIELD(arg1, f32 *, 0x38) * M2C_FIELD(arg0, f32 *, 0x2C)) + ((M2C_FIELD(arg1, f32 *, 0x28) * M2C_FIELD(arg0, f32 *, 0x28)) + ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg0, f32 *, 0x20)) + (M2C_FIELD(arg1, f32 *, 0x18) * M2C_FIELD(arg0, f32 *, 0x24)))));
    M2C_FIELD(arg2, f32 *, 0x2C) = (f32) ((M2C_FIELD(arg1, f32 *, 0x3C) * M2C_FIELD(arg0, f32 *, 0x2C)) + ((M2C_FIELD(arg1, f32 *, 0x2C) * M2C_FIELD(arg0, f32 *, 0x28)) + ((M2C_FIELD(arg1, f32 *, 0xC) * M2C_FIELD(arg0, f32 *, 0x20)) + (M2C_FIELD(arg1, f32 *, 0x1C) * M2C_FIELD(arg0, f32 *, 0x24)))));
    M2C_FIELD(arg2, f32 *, 0x30) = (f32) ((M2C_FIELD(arg1, f32 *, 0x30) * M2C_FIELD(arg0, f32 *, 0x3C)) + ((M2C_FIELD(arg1, f32 *, 0x20) * M2C_FIELD(arg0, f32 *, 0x38)) + ((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg0, f32 *, 0x30)) + (M2C_FIELD(arg1, f32 *, 0x10) * M2C_FIELD(arg0, f32 *, 0x34)))));
    M2C_FIELD(arg2, f32 *, 0x34) = (f32) ((M2C_FIELD(arg1, f32 *, 0x34) * M2C_FIELD(arg0, f32 *, 0x3C)) + ((M2C_FIELD(arg1, f32 *, 0x24) * M2C_FIELD(arg0, f32 *, 0x38)) + ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg0, f32 *, 0x30)) + (M2C_FIELD(arg1, f32 *, 0x14) * M2C_FIELD(arg0, f32 *, 0x34)))));
    M2C_FIELD(arg2, f32 *, 0x38) = (f32) ((M2C_FIELD(arg1, f32 *, 0x38) * M2C_FIELD(arg0, f32 *, 0x3C)) + ((M2C_FIELD(arg1, f32 *, 0x28) * M2C_FIELD(arg0, f32 *, 0x38)) + ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg0, f32 *, 0x30)) + (M2C_FIELD(arg1, f32 *, 0x18) * M2C_FIELD(arg0, f32 *, 0x34)))));
    M2C_FIELD(arg2, f32 *, 0x3C) = (f32) ((M2C_FIELD(arg1, f32 *, 0x3C) * M2C_FIELD(arg0, f32 *, 0x3C)) + ((M2C_FIELD(arg1, f32 *, 0x2C) * M2C_FIELD(arg0, f32 *, 0x38)) + ((M2C_FIELD(arg1, f32 *, 0xC) * M2C_FIELD(arg0, f32 *, 0x30)) + (M2C_FIELD(arg1, f32 *, 0x1C) * M2C_FIELD(arg0, f32 *, 0x34)))));
}

void MatrixToMtxTranspose(void *arg0, void *arg1) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) M2C_FIELD(arg1, f32 *, 0);
    M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(arg1, f32 *, 0x10);
    M2C_FIELD(arg0, f32 *, 8) = (f32) M2C_FIELD(arg1, f32 *, 0x20);
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) M2C_FIELD(arg1, f32 *, 0x30);
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) M2C_FIELD(arg1, f32 *, 4);
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) M2C_FIELD(arg1, f32 *, 0x14);
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) M2C_FIELD(arg1, f32 *, 0x24);
    M2C_FIELD(arg0, f32 *, 0x1C) = (f32) M2C_FIELD(arg1, f32 *, 0x34);
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) M2C_FIELD(arg1, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) M2C_FIELD(arg1, f32 *, 0x18);
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) M2C_FIELD(arg1, f32 *, 0x28);
    M2C_FIELD(arg0, f32 *, 0x2C) = (f32) M2C_FIELD(arg1, f32 *, 0x38);
}

#pragma push
#pragma dont_inline on
s32 isInWindow(void *arg0) {
    f32 temp_f1;
    u32 temp_r3;

    if (((f32) M2C_FIELD(GetRenderWindow(), u32 *, 0x2B0) > M2C_FIELD(arg0, f32 *, 0)) || ((f32) M2C_FIELD(GetRenderWindow(), u32 *, 0x2B4) < M2C_FIELD(arg0, f32 *, 0))) {
        return 0;
    }
    if (((f32) M2C_FIELD(GetRenderWindow(), u32 *, 0x2B8) > M2C_FIELD(arg0, f32 *, 4)) || ((f32) M2C_FIELD(GetRenderWindow(), u32 *, 0x2BC) < M2C_FIELD(arg0, f32 *, 4))) {
        return 0;
    }
    temp_r3 = M2C_FIELD(GetRenderWindow(), u32 *, 0x2B0);
    temp_f1 = (f32) temp_r3;
    M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg0, f32 *, 0) - temp_f1);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg0, f32 *, 4) - (f32) M2C_FIELD(GetRenderWindow(temp_r3, temp_f1), u32 *, 0x2B8));
    return 1;
}
#pragma pop

void Gfx_SetTextColor(s8 arg0, s8 arg1, s8 arg2, s8 arg3) {
    extern M2C_UNK g_textcolor;
    M2C_FIELD(&g_textcolor, s8 *, 0) = arg0;
    M2C_FIELD(&g_textcolor, s8 *, 1) = arg1;
    M2C_FIELD(&g_textcolor, s8 *, 2) = arg2;
    M2C_FIELD(&g_textcolor, s8 *, 3) = arg3;
}

void VideoDecodeThreadStart(void) {
    extern s32 VideoDecodeThreadCreated_803C7728;
    extern u8 VideoDecodeThread_8036CFF0[];
    if (VideoDecodeThreadCreated_803C7728) {
        OSResumeThread(VideoDecodeThread_8036CFF0);
    }
}

void VideoDecodeThreadCancel(void) {
    extern s32 VideoDecodeThreadCreated_803C7728;
    extern u8 VideoDecodeThread_8036CFF0[];
    if (VideoDecodeThreadCreated_803C7728) {
        OSCancelThread(VideoDecodeThread_8036CFF0);
        VideoDecodeThreadCreated_803C7728 = 0;
    }
}

void *PopFreeTextureSet(void) {
    extern u8 FreeTextureSetQueue_8036E300[];
    void *msg;

    OSReceiveMessage(FreeTextureSetQueue_8036E300, &msg, 1);
    return msg;
}

void PushFreeTextureSet(void *arg0) {
    extern u8 FreeTextureSetQueue_8036E300[];
    OSSendMessage(FreeTextureSetQueue_8036E300, arg0, 0);
}

void *PopDecodedTextureSet(s32 arg0) {
    extern u8 DecodedTextureSetQueue_8036E32C[];
    void *msg;

    if (OSReceiveMessage(DecodedTextureSetQueue_8036E32C, &msg, arg0) == 1) {
        return msg;
    }
    return NULL;
}

void PushDecodedTextureSet(void *arg0) {
    extern u8 DecodedTextureSetQueue_8036E32C[];
    OSSendMessage(DecodedTextureSetQueue_8036E32C, arg0, 1);
}

void THPGXRestore(void) {
    M2C_UNK GXSetBlendMode(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetNumChans(M2C_UNK);
    M2C_UNK GXSetNumTevStages(M2C_UNK);
    M2C_UNK GXSetNumTexGens(M2C_UNK);
    M2C_UNK GXSetTevOrder(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetTevSwapMode(M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetTevSwapModeTable(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK);
    M2C_UNK GXSetZMode(M2C_UNK, M2C_UNK, M2C_UNK);
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0xF);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
}

void Mpeg_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void AudioInit(void);
void Mpeg_Init(void) {
    void GXSetCopyClear(GXColor, u32);
    extern s32 m_rendermode_803C7754;
    extern s32 rmode;
    GXColor black;

    black.r = 0;
    black.g = 0;
    black.b = 0;
    black.a = 0xFF;
    GXSetDispCopyGamma(0);
    GXSetCopyFilter(0, 0, 0, 0);
    GXSetCopyClear(black, 0xFFFFFF);
    GXSetCullMode(0);
    THPPlayerInit();
    AudioInit();
    m_rendermode_803C7754 = rmode;
}

u32 Mpeg_Heartbeat(void) {
    M2C_UNK GXInvalidateVtxCache();
    M2C_UNK THPPlayerClose();
    M2C_UNK THPPlayerDrawCurrentFrame(s32, s32, s32, s32, s32);
    M2C_UNK THPPlayerDrawDone();
    s32 THPPlayerGetState();
    M2C_UNK THPPlayerStop();
    extern u8 badMojo_803C7734;
    extern s32 m_movie_x_803C7740;
    extern s32 m_movie_y_803C773C;
    extern s32 m_rendermode_803C7754;
    extern M2C_UNK m_videoInfo_803C7744;
    s32 temp_r3;

    if ((u8) badMojo_803C7734 != 0) {
        return 0U;
    }
    GXInvalidateVtxCache();
    THPPlayerDrawCurrentFrame(m_rendermode_803C7754, m_movie_x_803C7740, m_movie_y_803C773C, M2C_FIELD(&m_videoInfo_803C7744, s32 *, 0), M2C_FIELD(&m_videoInfo_803C7744, s32 *, 4));
    THPPlayerDrawDone();
    if (THPPlayerGetState() == 5) {
        THPPlayerStop();
        THPPlayerClose();
        badMojo_803C7734 = 1;
        return 0U;
    }
    temp_r3 = THPPlayerGetState();
    return (u32) ((3 - temp_r3) | (temp_r3 - 3)) >> 0x1FU;
}

void Mpeg_Render(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Mpeg_Destroy(void) {

}

void *AIRegisterDMACallback(void (*callback)(void));
void AudioInit(void) {
    extern void AudioCallback(void);
    extern s32 SoundBufferNumber_803C7730;
    extern u8 SoundBuffer_8036E360[][0x3C0];
    extern void *audio_restore_cb;
    audio_restore_cb = AIRegisterDMACallback(AudioCallback);
    memset(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0, 0x3C0);
    DCFlushRange(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    AIInitDMA(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    AIStartDMA();
}

void AudioCallback(void) {
    s32 OSEnableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    void THPPlayerAudioOutput(void *buf, s32 samples, s32 vol);
    extern s32 SoundBufferNumber_803C7730;
    extern u8 SoundBuffer_8036E360[][0x3C0];
    extern s32 m_movieVolume_803C7738;
    s32 old;

    old = OSEnableInterrupts();
    SoundBufferNumber_803C7730 ^= 1;
    memset(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0, 0x3C0);
    THPPlayerAudioOutput(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0xF0, m_movieVolume_803C7738);
    DCFlushRange(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    AIInitDMA(SoundBuffer_8036E360[SoundBufferNumber_803C7730], 0x3C0);
    OSRestoreInterrupts(old);
}

BOOL THPPlayerInit(void) {
    void LCEnable(void);
    BOOL THPInit(void);
    void OSInitMessageQueue(void *queue, void *msgArray, s32 msgCount);
    extern s32 Initialized_803C7758;

    memset(&ActivePlayer, 0, sizeof(THPPlayer));
    LCEnable();
    OSInitMessageQueue(UsedTextureSetQueue_8036EC78, UsedTextureSetMessage_8036EC98, 3);
    if (THPInit()) {
        Initialized_803C7758 = TRUE;
        return TRUE;
    }
    return FALSE;
}

void THPPlayerQuit(void) {
    extern s32 Initialized_803C7758;
    LCDisable();
    Initialized_803C7758 = 0;
}

BOOL THPPlayerClose(void) {
    s32 DVDClose(void *fileInfo);
    if (ActivePlayer.open && ActivePlayer.state == 0) {
        ActivePlayer.open = FALSE;
        DVDClose(ActivePlayer.fileInfo);
        return TRUE;
    }
    return FALSE;
}

u32 THPPlayerCalcNeedMemory(void) {
    u32 size;

    if (ActivePlayer.open) {
        if (ActivePlayer.onMemory) {
            size = (ActivePlayer.movieDataSize + 0x1F) & ~0x1F;
        } else {
            size = ((ActivePlayer.bufSize + 0x1F) & ~0x1F) * 10;
        }
        size += ((ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize + 0x1F) & ~0x1F) * 3;
        size += ((ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4 + 0x1F) & ~0x1F) * 3;
        size += ((ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4 + 0x1F) & ~0x1F) * 3;
        if (ActivePlayer.audioExist) {
            size += ((ActivePlayer.audioMaxSamples * 4 + 0x1F) & ~0x1F) * 3;
        }
        size += 0x1000;
        return size;
    }
    return 0;
}

void PrepareReady(void *arg0) {
    extern u8 PrepareReadyQueue_8036ED00[];
    OSSendMessage(PrepareReadyQueue_8036ED00, arg0, 1);
}

BOOL THPPlayerPlay(void) {
    if (ActivePlayer.open && (ActivePlayer.state == 1 || ActivePlayer.state == 4)) {
        ActivePlayer.state = 2;
        ActivePlayer.prevCount = 0;
        ActivePlayer.curCount = 0;
        ActivePlayer.retraceCount = -1;
        ActivePlayer.unkC0 = -1;
        return TRUE;
    }
    return FALSE;
}

void THPPlayerStop(void) {
    void AudioDecodeThreadCancel(void);
    BOOL DVDCancel(void *block);
    void ReadThreadCancel(void);
    void *VISetPostRetraceCallback(void *cb);
    void VideoDecodeThreadCancel(void);

    if (ActivePlayer.open && ActivePlayer.state) {
        ActivePlayer.unk9D = 0;
        ActivePlayer.state = 0;
        VISetPostRetraceCallback(OldVIPostCallback_803C775C);
        if (!ActivePlayer.onMemory) {
            DVDCancel(ActivePlayer.fileInfo);
            ReadThreadCancel();
        }
        VideoDecodeThreadCancel();
        if (ActivePlayer.audioExist) {
            AudioDecodeThreadCancel();
        }
        while (PopUsedTextureSet()) {
        }
        ActivePlayer.dispTextureSet = NULL;
        ActivePlayer.playAudioBuffer = NULL;
    }
}

BOOL ProperTimingForStart(void) {
    u32 VIGetNextField(void);

    if (ActivePlayer.playFlag & 2) {
        if (VIGetNextField() == 0) {
            return TRUE;
        }
    } else if (ActivePlayer.playFlag & 4) {
        if (VIGetNextField() == 1) {
            return TRUE;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}

BOOL THPPlayerGetVideoInfo(THPVideoInfo *videoInfo) {
    if (ActivePlayer.open) {
        memcpy(videoInfo, &ActivePlayer.videoInfo, sizeof(THPVideoInfo));
        return TRUE;
    }
    return FALSE;
}

u8 THPPlayerGetState(void) {
    return ActivePlayer.state;
}

void PushUsedTextureSet(void *arg0) {
    OSSendMessage(UsedTextureSetQueue_8036EC78, arg0, 0);
}

void *PopUsedTextureSet(void) {
    void *msg;

    if (OSReceiveMessage(UsedTextureSetQueue_8036EC78, &msg, 0) == 1) {
        return msg;
    }
    return NULL;
}

#pragma push
#pragma dont_inline on
void THPPlayerDrawDone(void) {
    extern s32 Initialized_803C7758;
    void *msg;
    void *textureSet;

    GXDrawDone();
    if (Initialized_803C7758) {
        while (TRUE) {
            if (OSReceiveMessage(UsedTextureSetQueue_8036EC78, &msg, 0) == 1) {
                textureSet = msg;
            } else {
                textureSet = NULL;
            }
            if (textureSet == NULL) {
                break;
            }
            PushFreeTextureSet(textureSet);
        }
    }
}

s32 CreateReadThread(s32 priority) {
    s32 OSCreateThread(void *thread, void *func, void *param, void *stack, u32 stackSize, s32 priority, u16 attr);
    void OSInitMessageQueue(void *queue, void *msgArray, s32 msgCount);
    void *Reader(void *param);
    extern s32 ReadThreadCreated_803C7768;
    extern u8 ReadThread_8036ED20[];
    struct ReadThreadData {
        u8 thread[0x310];
        u8 stack[0x1000];
        u8 freeQueue[0x20];
        void *freeMessage[10];
        u8 readedQueue[0x20];
        void *readedMessage[10];
        u8 readedQueue2[0x20];
        void *readedMessage2[10];
    } *rt = (struct ReadThreadData *)ReadThread_8036ED20;

    if (OSCreateThread(rt->thread, Reader, NULL, rt->stack + 0x1000, 0x1000, priority, 1) == 0) {
        return 0;
    }
    OSInitMessageQueue(rt->freeQueue, rt->freeMessage, 10);
    OSInitMessageQueue(rt->readedQueue, rt->readedMessage, 10);
    OSInitMessageQueue(rt->readedQueue2, rt->readedMessage2, 10);
    ReadThreadCreated_803C7768 = 1;
    return 1;
}

#pragma pop
void ReadThreadStart(void) {
    extern s32 ReadThreadCreated_803C7768;
    extern u8 ReadThread_8036ED20[];
    if (ReadThreadCreated_803C7768) {
        OSResumeThread(ReadThread_8036ED20);
    }
}

void ReadThreadCancel(void) {
    extern s32 ReadThreadCreated_803C7768;
    extern u8 ReadThread_8036ED20[];
    if (ReadThreadCreated_803C7768) {
        OSCancelThread(ReadThread_8036ED20);
        ReadThreadCreated_803C7768 = 0;
    }
}

void *PopReadedBuffer(void) {
    extern u8 ReadedBufferQueue_80370078[];
    void *msg;

    OSReceiveMessage(ReadedBufferQueue_80370078, &msg, 1);
    return msg;
}

void PushReadedBuffer(void *arg0) {
    extern u8 ReadedBufferQueue_80370078[];
    OSSendMessage(ReadedBufferQueue_80370078, arg0, 1);
}

void *PopFreeReadBuffer(void) {
    extern u8 FreeReadBufferQueue_80370030[];
    void *msg;

    OSReceiveMessage(FreeReadBufferQueue_80370030, &msg, 1);
    return msg;
}

void PushFreeReadBuffer(void *arg0) {
    extern u8 FreeReadBufferQueue_80370030[];
    OSSendMessage(FreeReadBufferQueue_80370030, arg0, 1);
}

void *PopReadedBuffer2(void) {
    extern u8 ReadedBufferQueue2_803700C0[];
    void *msg;

    OSReceiveMessage(ReadedBufferQueue2_803700C0, &msg, 1);
    return msg;
}

void PushReadedBuffer2(void *arg0) {
    extern u8 ReadedBufferQueue2_803700C0[];
    OSSendMessage(ReadedBufferQueue2_803700C0, arg0, 1);
}

void AudioDecodeThreadStart(void) {
    extern s32 AudioDecodeThreadCreated_803C7770;
    extern u8 AudioDecodeThread_80370108[];
    if (AudioDecodeThreadCreated_803C7770) {
        OSResumeThread(AudioDecodeThread_80370108);
    }
}

void AudioDecodeThreadCancel(void) {
    extern s32 AudioDecodeThreadCreated_803C7770;
    extern u8 AudioDecodeThread_80370108[];
    if (AudioDecodeThreadCreated_803C7770) {
        OSCancelThread(AudioDecodeThread_80370108);
        AudioDecodeThreadCreated_803C7770 = 0;
    }
}

void AudioDecoder(void) {
    M2C_UNK AudioDecode();
    s32 PopReadedBuffer();
    M2C_UNK PushReadedBuffer2(s32);
    s32 temp_r31;

loop_1:
    temp_r31 = PopReadedBuffer();
    AudioDecode();
    PushReadedBuffer2(temp_r31);
    goto loop_1;
}

void AudioDecoderForOnMemory(void *bufAddr) {
    void AudioDecode(THPReadBuffer *readBuffer);
    s32 OSSuspendThread(void *thread);
    extern u8 AudioDecodeThread_80370108[];
    THPReadBuffer readBuffer;
    s32 tmp;
    s32 size;
    s32 i;

    i = 0;
    size = ActivePlayer.initReadSize;
    readBuffer.ptr = bufAddr;
    while (1) {
        readBuffer.frameNumber = i;
        AudioDecode(&readBuffer);
        if (ActivePlayer.numFrames - 1 == (i + ActivePlayer.initReadFrame) % ActivePlayer.numFrames) {
            if (ActivePlayer.playFlag & 1) {
                size = *(s32 *)readBuffer.ptr;
                readBuffer.ptr = ActivePlayer.movieData;
            } else {
                OSSuspendThread(AudioDecodeThread_80370108);
            }
        } else {
            tmp = *(s32 *)readBuffer.ptr;
            readBuffer.ptr += size;
            size = tmp;
        }
        i++;
    }
}

#pragma push
#pragma use_lmw_stmw on
void AudioDecode(THPReadBuffer *readBuffer) {
    THPAudioBuffer *PopFreeAudioBuffer(void);
    void PushDecodedAudioBuffer(THPAudioBuffer *buffer);
    u32 THPAudioDecode(s16 *buffer, u8 *audioFrame, s32 flag);
    THPAudioBuffer *audioBuf;
    u32 i;
    u32 *compSizePtr;
    u8 *ptr;

    ptr = readBuffer->ptr + ActivePlayer.numComponents * 4 + 8;
    compSizePtr = (u32 *)(readBuffer->ptr + 8);
    audioBuf = PopFreeAudioBuffer();
    for (i = 0; i < ActivePlayer.numComponents; i++) {
        switch (ActivePlayer.frameComp[i]) {
        case 1:
            audioBuf->validSample = THPAudioDecode(audioBuf->buffer, ptr, 0);
            audioBuf->curPtr = audioBuf->buffer;
            audioBuf->frameNumber = readBuffer->frameNumber;
            PushDecodedAudioBuffer(audioBuf);
            break;
        }
        ptr += *compSizePtr;
        compSizePtr++;
    }
}
#pragma pop
void *PopFreeAudioBuffer(void) {
    extern u8 FreeAudioBufferQueue_80371418[];
    void *msg;

    OSReceiveMessage(FreeAudioBufferQueue_80371418, &msg, 1);
    return msg;
}

void PushFreeAudioBuffer(void *arg0) {
    extern u8 FreeAudioBufferQueue_80371418[];
    OSSendMessage(FreeAudioBufferQueue_80371418, arg0, 0);
}

void *PopDecodedAudioBuffer(s32 arg0) {
    extern u8 DecodedAudioBufferQueue_80371444[];
    void *msg;

    if (OSReceiveMessage(DecodedAudioBufferQueue_80371444, &msg, arg0) == 1) {
        return msg;
    }
    return NULL;
}

void PushDecodedAudioBuffer(void *arg0) {
    extern u8 DecodedAudioBufferQueue_80371444[];
    OSSendMessage(DecodedAudioBufferQueue_80371444, arg0, 1);
}

void *PopUsedTextureSet(void);
void __THPHuffGenerateCodeTable(void) {
    u8 *sizeTab;
    u16 *codeTab;
    u8 si;
    u16 p;
    u16 code;

    sizeTab = __THPHuffmanSizeTab_803C7828;
    codeTab = __THPHuffmanCodeTab_803C782C;
    p = 0;
    code = 0;
    si = sizeTab[0];
    while (sizeTab[p]) {
        while (sizeTab[p] == si) {
            codeTab[p++] = code;
            code++;
        }
        code <<= 1;
        si++;
    }
}

void __THPRestartDefinition(void) {
    __THPInfo_803C7890->RST = TRUE;
    __THPInfo_803C7890->c += 2;
    __THPInfo_803C7890->nMCU = (u16)((__THPInfo_803C7890->c[0] << 8) | __THPInfo_803C7890->c[1]);
    __THPInfo_803C7890->c += 2;
    __THPInfo_803C7890->currMCU = __THPInfo_803C7890->nMCU;
}

s32 __THPAudioGetNewSample(u32 *info) {
    struct THPAudioDecodeInfo {
        u8 *encodeData;
        u32 offsetNibbles;
        u8 predictor;
        u8 scale;
    } *p = (struct THPAudioDecodeInfo *)info;
    s32 sample;

    if (!(p->offsetNibbles & 0xF)) {
        p->predictor = (u8)((*p->encodeData >> 4) & 7);
        p->scale = (u8)(*p->encodeData & 0xF);
        p->encodeData++;
        p->offsetNibbles += 2;
    }
    if (p->offsetNibbles & 1) {
        sample = (s32)((*p->encodeData & 0xF) << 28) >> 28;
        p->encodeData++;
    } else {
        sample = (s32)((*p->encodeData & 0xF0) << 24) >> 28;
    }
    p->offsetNibbles++;
    return sample;
}

void __THPAudioInitialize(void *arg0, u8 *arg1) {
    M2C_FIELD(arg0, u8 **, 0) = arg1;
    M2C_FIELD(arg0, s32 *, 4) = 2;
    M2C_FIELD(arg0, s8 *, 8) = (s8) (((u8) *M2C_FIELD(arg0, u8 **, 0) >> 4U) & 7);
    M2C_FIELD(arg0, s8 *, 9) = (s8) (*M2C_FIELD(arg0, u8 **, 0) & 0xF);
    M2C_FIELD(arg0, u8 **, 0) = (u8 *) (M2C_FIELD(arg0, u8 **, 0) + 1);
}

#pragma push
#pragma optimization_level 1
s32 AudioVoice_Create_(void) {
    extern u8 g_voice[0x20][0x70];
    extern s32 m_InitOnce_803C7898;
    u32 i;

    for (i = 0; i < 0x20; i++) {
        *(s32 *)&g_voice[i][0x50] = 0;
        *(u32 *)&g_voice[i][0] = i;
        g_voice[i][1] = 0xFF;
    }
    m_InitOnce_803C7898 = 1;
    return 1;
}
#pragma pop

#pragma push
#pragma optimization_level 1
void AudioVoice_Destroy_(void) {
    void AXRegisterAuxBCallback(void *callback, void *context);
    void AXRegisterAuxACallback(void *callback, void *context);
    s32 AXFXDelayShutdown(void *delay);
    extern u8 g_reverb_803715B0[2][0x60];

    AXRegisterAuxBCallback(NULL, NULL);
    AXRegisterAuxACallback(NULL, NULL);
    AXFXDelayShutdown(g_reverb_803715B0[1]);
    AXFXDelayShutdown(g_reverb_803715B0[0]);
}

u32 CreateSFX_(u32 *sfx) {
    u32 MAllocA_(u32 size);
    u32 size;
    u32 data;

    size = (sfx[1] + 0x1F) & ~0x1F;
    data = MAllocA_(size);
    if (data == 0) {
        return 0;
    }
    DCFlushRange((void *)sfx[0], size);
    MemCpyA_(data, sfx[0], size);
    if (((u8 *)sfx)[0x11] == 0x10) {
        sfx[1] >>= 1;
        ((u32 *)&data)[0] >>= 1;
        ((s32 *)sfx)[2] >>= 1;
    }
    sfx[0] = data;
    return data;
}

#pragma push
#pragma auto_inline off
void SetVoiceState_(void *voice, s32 state) {
    extern void *g_pFree_803C78A4;
    s32 OSDisableInterrupts();
    void OSRestoreInterrupts(s32);
    void AXFreeVoice(void *);
    s32 level;
    s32 level2;

    switch (state) {
    case 3:
        level = OSDisableInterrupts();
        M2C_FIELD(voice, u8 *, 0x15) = 0;
        M2C_FIELD(voice, u8 *, 1) = 0xFE;
        if (M2C_FIELD(voice, void **, 0x50) != NULL) {
            level2 = OSDisableInterrupts();
            if (M2C_FIELD(M2C_FIELD(voice, void **, 0x50), void **, 0xC) != NULL) {
                AXFreeVoice(M2C_FIELD(voice, void **, 0x50));
            }
            M2C_FIELD(voice, void **, 0x50) = NULL;
            OSRestoreInterrupts(level2);
        }
        M2C_FIELD(voice, void **, 4) = g_pFree_803C78A4;
        g_pFree_803C78A4 = voice;
        OSRestoreInterrupts(level);
        break;
    case 2:
        M2C_FIELD(voice, u8 *, 0x15) = 2;
        M2C_FIELD(M2C_FIELD(voice, void **, 0x50), u16 *, 0x146) = 0;
        M2C_FIELD(M2C_FIELD(voice, void **, 0x50), u32 *, 0x1C) |= 4;
        break;
    case 1:
        M2C_FIELD(voice, u8 *, 0x15) = 1;
        M2C_FIELD(M2C_FIELD(voice, void **, 0x50), u16 *, 0x146) = 1;
        M2C_FIELD(M2C_FIELD(voice, void **, 0x50), u32 *, 0x1C) |= 4;
        break;
    }
}
#pragma pop

#pragma pop

void SetVoicePriority_(void *arg0) {
    M2C_UNK AXSetVoicePriority(s32, u8);
    s32 OSDisableInterrupts();
    M2C_UNK OSRestoreInterrupts(s32);
    s32 temp_r31;

    temp_r31 = OSDisableInterrupts();
    AXSetVoicePriority(M2C_FIELD(arg0, s32 *, 0x50), M2C_FIELD(arg0, u8 *, 0x14));
    OSRestoreInterrupts(temp_r31);
}

#pragma push
#pragma optimization_level 1
void DestroyVoice_(void *arg0) {
    extern void (*DestroySongVoice)(void *);
    extern void *g_pFree_803C78A4;
    s32 OSDisableInterrupts();
    void OSRestoreInterrupts(s32);
    void *voice;
    s32 level;

    level = OSDisableInterrupts();
    voice = M2C_FIELD(arg0, void **, 0x14);
    if (M2C_FIELD(voice, u8 *, 0) == 0) {
        DestroySongVoice(voice);
    }
    M2C_FIELD(voice, u8 *, 0x15) = 0;
    M2C_FIELD(voice, u8 *, 1) = 0xFD;
    M2C_FIELD(voice, void **, 0x50) = 0;
    M2C_FIELD(voice, void **, 4) = g_pFree_803C78A4;
    g_pFree_803C78A4 = voice;
    OSRestoreInterrupts(level);
}

#pragma push
#pragma optimization_level 1
void ProcessVoice_(void) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_active_803C78A0;
    extern void (*DestroySongVoice)(void *);
    extern void *g_pFree_803C78A4;
    extern const u16 g_volTab[64];
    s32 OSDisableInterrupts();
    void OSRestoreInterrupts(s32);
    void AXFreeVoice(void *);
    u8 *voice;
    void *axv;
    s32 i;
    s32 level2;
    s32 level;
    s32 steps;

    g_active_803C78A0 = -1;
    for (voice = g_voice[0], i = 0x20; i != 0; i--) {
        u8 state = voice[0x15];
        voice += 0x70;
        if (state == 1) {
            g_active_803C78A0++;
        }
    }

    for (voice = g_voice[0], i = 0x20; i != 0; voice += 0x70, i--) {
        if (voice[0x15] != 1) {
            continue;
        }
        axv = M2C_FIELD(voice, void **, 0x50);
        if (M2C_FIELD(axv, u16 *, 0x146) == 0) {
            if (voice[0] == 0) {
                DestroySongVoice(voice);
            }
            level = OSDisableInterrupts();
            voice[0x15] = 0;
            voice[1] = 0xFE;
            if (M2C_FIELD(voice, void **, 0x50) != NULL) {
                level2 = OSDisableInterrupts();
                if (M2C_FIELD(M2C_FIELD(voice, void **, 0x50), void **, 0xC) != NULL) {
                    AXFreeVoice(M2C_FIELD(voice, void **, 0x50));
                }
                M2C_FIELD(voice, void **, 0x50) = NULL;
                OSRestoreInterrupts(level2);
            }
            M2C_FIELD(voice, void **, 4) = g_pFree_803C78A4;
            g_pFree_803C78A4 = voice;
            OSRestoreInterrupts(level);
            continue;
        }

        M2C_FIELD(axv, u32 *, 0x1C) |= 0x200;
        if (M2C_FIELD(voice, s16 *, 0x5A) < 0) {
            M2C_FIELD(voice, s16 *, 0x5A) = (M2C_FIELD(voice, s32 *, 0x38) * g_volTab[g_active_803C78A0]) >> 15;
            M2C_FIELD(axv, s16 *, 0x19C) = M2C_FIELD(voice, s16 *, 0x5A);
            M2C_FIELD(axv, s16 *, 0x19E) = 0;
        } else {
            steps = (((M2C_FIELD(voice, s32 *, 0x38) * g_volTab[g_active_803C78A0]) >> 15) - M2C_FIELD(voice, s16 *, 0x5A)) / 160;
            if (steps != 0) {
                M2C_FIELD(axv, s16 *, 0x19C) = M2C_FIELD(voice, s16 *, 0x5A);
                M2C_FIELD(axv, s16 *, 0x19E) = (u16)steps;
                M2C_FIELD(voice, s16 *, 0x5A) += steps * 160;
            } else {
                M2C_FIELD(voice, s16 *, 0x5A) = (M2C_FIELD(voice, s32 *, 0x38) * g_volTab[g_active_803C78A0]) >> 15;
                M2C_FIELD(axv, s16 *, 0x19C) = M2C_FIELD(voice, s16 *, 0x5A);
                M2C_FIELD(axv, s16 *, 0x19E) = 0;
            }
        }

        if (voice[0x56] != 0) {
            voice[0x56] = 0;
            M2C_FIELD(voice, s16 *, 0x64) = M2C_FIELD(voice, s16 *, 0x60);
            M2C_FIELD(voice, s16 *, 0x66) = M2C_FIELD(voice, s16 *, 0x62);
            M2C_FIELD(axv, s16 *, 0x14A) = M2C_FIELD(voice, s16 *, 0x60);
            M2C_FIELD(axv, s16 *, 0x14E) = M2C_FIELD(voice, s16 *, 0x62);
            M2C_FIELD(axv, s16 *, 0x14C) = 0;
            M2C_FIELD(axv, s16 *, 0x150) = 0;
            M2C_FIELD(axv, u16 *, 0x144) &= ~0x8;
            M2C_FIELD(axv, u32 *, 0x1C) |= 0x12;
        }

        if (voice[0x57] != 0) {
            voice[0x57] = 0;
            M2C_FIELD(voice, s16 *, 0x6C) = M2C_FIELD(voice, s16 *, 0x68);
            M2C_FIELD(voice, s16 *, 0x6E) = M2C_FIELD(voice, s16 *, 0x6A);
            if (voice[0] != 0) {
                M2C_FIELD(axv, s16 *, 0x15A) = M2C_FIELD(voice, s16 *, 0x68);
                M2C_FIELD(axv, s16 *, 0x15E) = M2C_FIELD(voice, s16 *, 0x6A);
                M2C_FIELD(axv, s16 *, 0x15C) = 0;
                M2C_FIELD(axv, s16 *, 0x160) = 0;
                M2C_FIELD(axv, u16 *, 0x144) &= ~0x800;
            } else {
                M2C_FIELD(axv, s16 *, 0x152) = M2C_FIELD(voice, s16 *, 0x68);
                M2C_FIELD(axv, s16 *, 0x156) = M2C_FIELD(voice, s16 *, 0x6A);
                M2C_FIELD(axv, s16 *, 0x154) = 0;
                M2C_FIELD(axv, s16 *, 0x158) = 0;
                M2C_FIELD(axv, u16 *, 0x144) &= ~0x40;
            }
            M2C_FIELD(axv, u32 *, 0x1C) |= 0x12;
        }
    }
}
#pragma pop

#pragma pop

#pragma push
#pragma optimization_level 1
s8 SetVoiceRate_(void *voice) {
    u32 rate = (1.0f / 48000.0f) * ((f32)M2C_FIELD(voice, u32 *, 0x30) * (f32)M2C_FIELD(voice, u32 *, 0x34));

    if (rate <= 0x2EE00 && rate >= 0x60) {
        M2C_FIELD(M2C_FIELD(voice, void **, 0x50), u32 *, 0x1DE) = rate;
        M2C_FIELD(M2C_FIELD(voice, void **, 0x50), u32 *, 0x1C) |= 0x80000;
        return 1;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimization_level 1
s8 SetVoiceOffset_(void *voice, u32 offset) {
    void *ax;

    if (offset < M2C_FIELD(voice, u32 *, 0x28)) {
        ax = M2C_FIELD(voice, void **, 0x50);
        M2C_FIELD(ax, u32 *, 0x1B2) = M2C_FIELD(voice, u32 *, 0x20) + offset;
        M2C_FIELD(ax, u32 *, 0x1C) |= 0x1000;
        return 1;
    }
    return 0;
}
#pragma pop

void DestroyIT_(void *data) {
    void FreeA_(u32 addr);
    void OSFreeToHeap(s32 heap, void *ptr);
    extern s32 __OSCurrHeap;
    extern u32 g_aheap;

    OSFreeToHeap(__OSCurrHeap, data);
    FreeA_(g_aheap);
    g_aheap = 0;
}

u32 GetITGVol_(u8 *trk, u8 *ch) {
    extern u32 g_musicVol;
    u8 **pt = &trk;
    u8 **pc = &ch;

    return ((*(u32 *)(*pt + 0x18) * g_musicVol >> 7) * (*(u32 *)(*pc + 0x1C) * *(u32 *)(*pc + 0x28) >> 6)) >> 15;
}

void DestroyMIDI_(s32 arg0) {
    M2C_UNK FreeA_(s32);
    M2C_UNK OSFreeToHeap(s32, s32);
    extern s32 __OSCurrHeap;
    extern s32 g_aheap;
    OSFreeToHeap(__OSCurrHeap, arg0);
    FreeA_(g_aheap);
}

u32 ReadVar_(u8 **pp) {
    u8 ***ppp = &pp;
    u32 value = 0;
    u8 c;

    do {
        c = *(**ppp)++;
        value = (value << 7) | (c & 0x7F);
    } while ((c & 0x7F) != c);
    return value;
}

u32 GetPitchWheel_(u8 note, u8 count) {
    extern const u32 g_pitchTab[];
    u8 *pn = &note;
    u8 *pc = &count;
    u32 pitch = g_pitchTab[*pn];
    u32 base;
    s32 i = *pc - 1;

    if (i != 0) {
        base = pitch;
        do {
            pitch = (u32)(((u64)pitch * base) >> 30);
        } while (--i > 0);
    }
    return pitch >> 14;
}

#pragma push
#pragma optimization_level 1
u32 GetNoteRate_(u32 rate, u8 note) {
    extern const f32 g_noteTab[128];

    return rate * g_noteTab[note];
}
#pragma pop

s32 GetID_(u8 *chunk) {
    s32 id = 0;
    s32 i;
    for (i = 0; i < 4; i++) {
        id = (id << 8) | chunk[i];
    }
    return id;
}

s32 GetType_(u8 *chunk) {
    u8 **p = &chunk;
    return (((((*p)[8] << 8) | (*p)[9]) << 8 | (*p)[10]) << 8) | (*p)[11];
}

s32 GetID_(u8 *chunk);
s8 CmpID_(u8 *chunk, s32 id) {
    return GetID_(chunk) == id;
}

s32 Audio_Create_(void) {
    void Audio_Update_(void);
    ARInit(0, 0);
    ARQInit();
    AIInit(0);
    AXInit();
    AISetDSPSampleRate(1);
    AXRegisterCallback(Audio_Update_);
    return 1;
}

void Audio_Destroy_(void) {
    M2C_UNK AIReset();
    M2C_UNK ARQReset();
    M2C_UNK ARReset();
    M2C_UNK AXQuit();
    AXQuit();
    AIReset();
    ARQReset();
    ARReset();
}

void Audio_Update_(void) {
    M2C_UNK Audio_Update(M2C_UNK);
    Audio_Update(0xD05);
}

#pragma push
#pragma optimization_level 1
u32 HAllocA_(s32 size) {
    typedef struct MemBlk {
        u32 addr;
        s32 size;
        u32 unk8;
        u32 unkC;
    } MemBlk;
    extern MemBlk g_memblk_80371670[];
    extern s32 g_last_803C78B4;
    extern u32 g_heap_803C78C4;

    size = (size + 0x1F) & ~0x1F;
    if (g_memblk_80371670[g_last_803C78B4].size < 0 || g_heap_803C78C4 != g_memblk_80371670[g_last_803C78B4].addr ||
        g_memblk_80371670[g_last_803C78B4].size < size) {
        return 0;
    }
    g_memblk_80371670[g_last_803C78B4].addr += size;
    g_memblk_80371670[g_last_803C78B4].size -= size;
    return g_heap_803C78C4;
}
#pragma pop

void MemCpyDone(void) {
    extern s32 g_arqWait_803C78B0;
    g_arqWait_803C78B0 = 0;
}

u32 MemCpyA_(u32 dst, u32 src, u32 len) {
    extern u32 g_arqWait_803C78B0;
    void ARQPostRequest(void *req, u32 owner, u32 type, u32 prio, u32 src, u32 dst, u32 len, void (*cb)(void));
    void PPCSync(void);
    void MemCpyDone(void);
    u8 req[0x20];

    g_arqWait_803C78B0 = 1;
    if (src & 0x80000000) {
        ARQPostRequest(req, 0, 0, 1, src, dst, len, MemCpyDone);
    } else {
        ARQPostRequest(req, 0, 1, 1, src, dst, len, MemCpyDone);
    }
    while (g_arqWait_803C78B0 != 0) {
        PPCSync();
    }
    return dst + len;
}

s32 Mul16_(u32 a, u32 b) {
    u32 hi = __mulhwu(a, b);
    u32 lo = a * b;
    return (lo >> 16) | (hi << 16);
}

s8 AudioVoice_Create(void) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_sfxvol_803C78D4;
    extern s8 g_speakers;
    s8 AudioVoice_Create_(void *voices);
    memset(g_voice, 0, sizeof(g_voice));
    g_sfxvol_803C78D4 = 0x7FFF;
    g_speakers = 1;
    return AudioVoice_Create_(g_voice);
}

#pragma push
#pragma optimization_level 1
s32 AudioVoice_Init(void) {
    void AudioVoice_Init_(void);
    extern void (*VoiceUpdate)(void);
    void ProcessVoice(void);
    u32 i;

    for (i = 0; i < 0xFE; i++) {
        g_soundLst_80373460[i] = i + 1;
        g_sound_80373560[i].data = 0;
    }
    g_soundLst_80373460[0xFD] = 0xFF;
    g_freeSound_803C78D8 = 0;
    AudioVoice_Init_();
    VoiceUpdate = ProcessVoice;
    return 1;
}
#pragma pop
void AudioVoice_Destroy(void) {
    M2C_UNK AudioVoice_Destroy_();
    extern s32 VoiceUpdate;
    VoiceUpdate = 0;
    AudioVoice_Destroy_();
}

void SetSpeakerMode(s8 arg0) {
    extern s8 g_speakers;
    g_speakers = arg0;
}

#pragma push
#pragma optimization_level 1
void SetReverb(s32 arg0, s32 arg1) {
    void SetReverb_(s32, s32, u16, u8);
    SetReverb_(arg0, arg1, 0x666, 0x28);
}
#pragma pop
void ProcessVoice(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

#pragma push
#pragma optimization_level 1
u32 CreateSFX(void *desc) {
    u32 CreateSFX_(SFX *);
    u32 idx;
    SFX *sfx;

    if (g_freeSound_803C78D8 == 0xFFFFFFFF) {
        return 0;
    }
    sfx = &g_sound_80373560[g_freeSound_803C78D8];
    memcpy(sfx, desc, sizeof(SFX));
    if (CreateSFX_(sfx) == 0) {
        return 0;
    }
    idx = g_freeSound_803C78D8;
    g_freeSound_803C78D8 = g_soundLst_80373460[g_freeSound_803C78D8];
    g_soundLst_80373460[idx++] = 0xFF;
    return idx;
}
#pragma pop

#pragma push
#pragma optimization_level 1
s8 DestroySFX(u32 handle) {
    s32 FreeA_(void *);
    void *data;
    SFX *sfx;
    u32 next;
    u32 idx = handle - 1;

    if (idx < 0xFE && g_sound_80373560[idx].data != 0) {
        next = g_freeSound_803C78D8;
        g_freeSound_803C78D8 = idx;
        sfx = &g_sound_80373560[idx];
        g_soundLst_80373460[idx] = next;
        data = sfx->data;
        sfx->data = 0;
        return FreeA_(data);
    }
    return 0;
}

void SetSFXVol(s32 vol) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_sfxvol_803C78D4;
    void SetVoiceVol_(void *voice, s32 vol);
    u8 *voice;
    u32 i;

    g_sfxvol_803C78D4 = vol;
    for (voice = g_voice[0], i = 0x20; i != 0; voice += 0x70, i--) {
        if (voice[0] == 1 && voice[0x15] == 1) {
            SetVoiceVol_(voice, g_sfxvol_803C78D4);
        }
    }
}

#pragma push
#pragma optimization_level 1
void ReleaseVoice(u32 id) {
    extern u8 g_voice[0x20][0x70];

    if (id == *(u32 *)g_voice[(u8)id]) {
        SetVoiceState_(g_voice[(u8)id], 3);
    }
}

s32 SetVoiceVol(u32 id, s32 vol) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_sfxvol_803C78D4;
    void SetVoiceVol_(void *voice, s32 vol);
    u8 *voice = g_voice[(u8)id];

    if (*(u32 *)voice == id) {
        *(s32 *)(voice + 0x38) = vol;
        if (voice[0x15] == 1) {
            SetVoiceVol_(voice, g_sfxvol_803C78D4);
        }
        return 1;
    }
    return 0;
}

s32 SetVoicePos(u32 id, s16 x, s16 y) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_sfxvol_803C78D4;
    void SetVoiceVol_(void *voice, s32 vol);
    u8 *voice = g_voice[(u8)id];

    if (*(u32 *)voice == id) {
        *(s32 *)(voice + 0x40) = x;
        *(s32 *)(voice + 0x44) = y;
        if (voice[0x15] == 1) {
            SetVoiceVol_(voice, g_sfxvol_803C78D4);
        }
        return 1;
    }
    return 0;
}

#pragma pop

#pragma pop

s32 SetVoicePitch(u32 id, s32 pitch) {
    s8 SetVoiceRate_(void *voice);
    extern u8 g_voice[0x20][0x70];
    u8 *voice = g_voice[(u8)id];

    if (*(u32 *)voice == id) {
        *(s32 *)(voice + 0x34) = pitch;
        if (SetVoiceRate_(voice)) {
            return 1;
        }
        SetVoiceState_(voice, 3);
    }
    return 0;
}

s32 AudioIT_Create(void) {
    extern void *g_pHdr_803C78E8;
    extern u8 g_song_80375520[0x30];
    extern u8 g_trk_80375550[0x20][0x70];
    u8 i;

    memset(g_song_80375520, 0, 0x30);
    memset(g_trk_80375550, 0, 0xE00);
    i = 0;
    while (i < 0x20) {
        u8 id = i++;
        u8 *trk = g_trk_80375550[id];
        trk[0x10] = id;
    }
    g_pHdr_803C78E8 = NULL;
    return 1;
}

void DestroyIT(void);
s32 AudioIT_Init(void) {
    extern void *g_pHdr_803C78E8;
    extern u8 g_song_80375520[0x30];
    extern u8 g_trk_80375550[0x20][0x70];
    u32 *trk;
    u32 i;

    if (g_pHdr_803C78E8 != NULL) {
        DestroyIT();
    }
    *(s32 *)(g_song_80375520 + 0x1C) = 0x10000;
    trk = (u32 *)g_trk_80375550;
    for (i = 0x20; i > 0; i--) {
        trk[0] = 0;
        trk[1] = 0;
    }
    return 1;
}

void AudioIT_Destroy(void) {

}

void DestroyIT(void) {
    void SetITState(s32 state);
    void DestroyIT_(void *hdr);
    extern u8 g_musicTitle[0x20];
    extern void *g_pHdr_803C78E8;
    if (g_pHdr_803C78E8 != NULL) {
        SetITState(3);
        DestroyIT_(g_pHdr_803C78E8);
        g_pHdr_803C78E8 = NULL;
        memset(g_musicTitle, 0, 0x20);
    }
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void SetITVol(void) {
    void SetVoiceVol_(void *voice, s32 vol);
    extern u8 g_song_80375520[0x30];
    extern u8 g_trk_80375550[0x20][0x70];
    u8 *trk;
    u8 *voice;
    s32 i;
    s32 gvol;

    for (trk = g_trk_80375550[0], i = 0x20; i != 0; trk += 0x70, i--) {
        voice = *(u8 **)trk;
        gvol = (u16)GetITGVol_(g_song_80375520, trk);
        *(u16 *)(trk + 0x1A) = gvol;
        while (voice != NULL) {
            SetVoiceVol_(voice, gvol);
            voice = *(u8 **)(voice + 4);
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetITPitch(s32 pitch) {
    s8 SetVoiceRate_(void *voice);
    extern u8 g_voice[0x20][0x70];
    extern u8 g_song_80375520[0x30];
    u8 *voice;
    s32 i;

    *(s32 *)(g_song_80375520 + 0x1C) = pitch;
    for (voice = g_voice[0], i = 0x20; i != 0; voice += 0x70, i--) {
        if (voice[0x15] != 0 && voice[0] == 0) {
            *(s32 *)(voice + 0x34) = pitch;
            SetVoiceRate_(voice);
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetITTempo(u32 tempo) {
    extern u8 g_song_80375520[0x30];

    if (tempo > 0x1FFFF) {
        tempo = 0x1FFFF;
    }
    *(u32 *)(g_song_80375520 + 0x28) = tempo;
    *(u32 *)g_song_80375520 = (*(u32 *)(g_song_80375520 + 0x28) << 15) / (30000000 / (*(u32 *)(g_song_80375520 + 0x24) * 24));
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetITOrder(u8 order, s8 keep) {
    extern void *g_pHdr_803C78E8;
    extern u8 g_song_80375520[0x30];

    if (g_pHdr_803C78E8 != NULL && order < *(u16 *)((u8 *)g_pHdr_803C78E8 + 0x20)) {
        g_song_80375520[0x15] = order;
        if (keep == 0) {
            *(s32 *)(g_song_80375520 + 0x10) = 1;
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetITTrackPriority(s32 id, s32 prio) {
    void SetVoicePriority_(void *voice);
    struct Trk {
        u8 *head;
        u8 *active;
        u8 pad[0xA];
        u8 prio;
        u8 pad2[0x5D];
    };
    extern struct Trk g_trk_80375550[0x20];
    u8 *voice;
    u8 *next;

    prio *= 2;
    g_trk_80375550[id].prio = prio;
    voice = g_trk_80375550[id].active;
    if (voice != NULL) {
        voice[0x14] = prio;
        SetVoicePriority_(voice);
        voice = ((u8 **)voice)[2];
    } else {
        voice = g_trk_80375550[id].head;
    }
    prio--;
    while (voice != NULL) {
        next = ((u8 **)voice)[1];
        voice[0x14] = prio;
        SetVoicePriority_(voice);
        voice = next;
    }
}

#pragma push
#pragma optimization_level 1
void SetITTrackOn(u32 mask, s8 deferred) {
    extern u32 g_trkOn;
    extern u8 g_trk_80375550[0x20][0x70];
    u8 *trk;

    if (deferred) {
        g_trkOn |= mask;
        return;
    }
    for (trk = g_trk_80375550[0]; mask != 0; trk += 0x70) {
        trk[0x11] |= mask & 1;
        mask >>= 1;
    }
}

#pragma push
#pragma optimization_level 1
void SetITTrackOff(u32 mask, s8 deferred) {
    extern u32 g_trkOff;
    extern u8 g_trk_80375550[0x20][0x70];
    u8 *trk;
    u8 *voice;
    u8 *next;

    if (deferred) {
        g_trkOff |= mask;
        return;
    }
    for (trk = g_trk_80375550[0]; mask != 0; trk += 0x70) {
        if (mask & 1) {
            trk[0x11] = 0;
            voice = *(u8 **)trk;
            *(u8 **)trk = NULL;
            *(u8 **)(trk + 4) = NULL;
            while (voice != NULL) {
                next = *(u8 **)(voice + 4);
                SetVoiceState_(voice, 3);
                voice = next;
            }
        }
        mask >>= 1;
    }
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void SetITTrackVol(u32 id, s32 vol) {
    void SetVoiceVol_(void *voice, u16 vol);
    struct Trk {
        u8 *head;
        u8 *active;
        u8 pad8[0x12];
        u16 gvol;
        u8 pad1C[0xC];
        s32 vol;
        u8 pad2C[0x44];
    };
    extern u8 g_song_80375520[0x30];
    extern struct Trk g_trk_80375550[0x20];
    u8 *voice;
    u16 gvol;

    voice = g_trk_80375550[id].head;
    g_trk_80375550[id].vol = vol;
    gvol = GetITGVol_(g_song_80375520, (u8 *)&g_trk_80375550[id]);
    g_trk_80375550[id].gvol = gvol;
    while (voice != NULL) {
        SetVoiceVol_(voice, gvol);
        voice = *(u8 **)(voice + 4);
    }
}

void SetTrackVol(u8 *trk) {
    void SetVoiceVol_(void *voice, s32 vol);
    u8 *voice;

    for (voice = *(u8 **)trk; voice != NULL; voice = *(u8 **)(voice + 4)) {
        SetVoiceVol_(voice, *(s16 *)(trk + 0x1A));
    }
}

void SetTrackPan(u8 *trk) {
    void SetVoiceVol_(void *voice, s32 vol);
    u8 *voice;

    for (voice = *(u8 **)trk; voice != NULL; voice = *(u8 **)(voice + 4)) {
        *(s32 *)(voice + 0x40) = *(s32 *)(trk + 0x2C);
        SetVoiceVol_(voice, *(s16 *)(trk + 0x1A));
    }
}

#pragma pop

#pragma pop

#pragma pop

#pragma pop
void StopActive(void *arg0) {
    M2C_UNK SetVoiceState_(s32, M2C_UNK);
    M2C_FIELD(arg0, void **, 0) = (void *) M2C_FIELD(M2C_FIELD(arg0, void **, 0), void **, 4);
    SetVoiceState_(M2C_FIELD(arg0, s32 *, 4), 3);
    M2C_FIELD(arg0, s32 *, 4) = 0;
}

#pragma push
#pragma optimization_level 1
void DestroyITVoice(u8 *voice) {
    extern u8 g_trk_80375550[0x20][0x70];
    u8 *trk;
    u8 *v;

    if (voice[1] > 0x20) {
        return;
    }
    trk = g_trk_80375550[voice[1]];
    if (*(u8 **)(trk + 4) == voice) {
        *(u8 **)(trk + 4) = NULL;
        *(u8 **)trk = *(u8 **)(voice + 4);
        return;
    }
    for (v = *(u8 **)trk; *(u8 **)(v + 4) != voice; v = *(u8 **)(v + 4)) {
    }
    *(u8 **)(v + 4) = *(u8 **)(voice + 4);
}
#pragma pop

s32 AudioMIDI_Create(void) {
    extern u8 g_song_80376990[0x24];
    extern u8 g_chn_803769B4[0x200];
    extern void *g_pHdr_803C78F4;

    memset(g_song_80376990, 0, 0x24);
    memset(g_chn_803769B4, 0, 0x200);
    g_pHdr_803C78F4 = NULL;
    return 1;
}

s32 AudioMIDI_Init(void) {
    M2C_UNK DestroyMIDI();
    extern u32 g_pHdr_803C78F4;
    if ((u32) g_pHdr_803C78F4 != 0U) {
        DestroyMIDI();
    }
    return 1;
}

void AudioMIDI_Destroy(void) {

}

void DestroyMIDI(void) {
    void SetMIDIState(s32 state);
    void DestroyMIDI_(void *hdr);
    extern u8 g_musicTitle[0x20];
    extern void *g_pHdr_803C78F4;
    if (g_pHdr_803C78F4 != NULL) {
        SetMIDIState(3);
        DestroyMIDI_(g_pHdr_803C78F4);
        g_pHdr_803C78F4 = NULL;
        memset(g_musicTitle, 0, 0x20);
    }
}

#pragma push
#pragma optimization_level 1
void SetMIDIVol(void) {
    extern u8 g_chn_803769B4[0x200];
    void SetVoiceVol_(void *voice, s32 vol);
    void SetGVol(void *chn);
    u8 *chn;
    u8 *voice;
    s32 i;

    for (chn = g_chn_803769B4, i = 0x10; i != 0; chn += 0x20, i--) {
        SetGVol(chn);
        for (voice = *(u8 **)chn; voice != NULL; voice = *(u8 **)(voice + 4)) {
            SetVoiceVol_(voice, *(s32 *)(chn + 0x14));
        }
    }
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void SetMIDIPitch(u32 pitch) {
    extern u8 g_chn_803769B4[0x200];
    extern u8 g_song_80376990[0x24];
    s8 SetVoiceRate_(void *voice);
    u32 i;
    u8 *chn;
    u8 *voice;

    *(u32 *)(g_song_80376990 + 0x20) = pitch;
    for (chn = g_chn_803769B4, i = 0x10; i != 0; chn += 0x20, i--) {
        *(s32 *)(chn + 0x18) = Mul16_(GetPitchWheel_(chn[0x1D], chn[0x1C]), pitch);
        for (voice = *(u8 **)chn; voice != NULL; voice = *(u8 **)(voice + 4)) {
            *(s32 *)(voice + 0x34) = *(s32 *)(chn + 0x18);
            SetVoiceRate_(voice);
        }
    }
}
#pragma pop

#pragma pop

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void SetMIDITempo(u32 tempo) {
    extern void *g_pHdr_803C78F4;
    extern u8 g_song_80376990[0x24];

    *(u32 *)(g_song_80376990 + 0x10) = -1U / tempo;
    *(u32 *)(g_song_80376990 + 4) = (u32)Mul16_(*(u32 *)(g_song_80376990 + 0xC), *(u32 *)(g_song_80376990 + 0x10)) / *(u16 *)((u8 *)g_pHdr_803C78F4 + 0x18);
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetMIDIMarker(u8 marker, s8 keep) {
    extern void *g_pHdr_803C78F4;
    extern u8 g_mrk_80376FB4[0x40][8];
    extern u8 g_song_80376990[0x24];

    if (g_pHdr_803C78F4 != NULL && marker < 0x40 && *(u32 *)g_mrk_80376FB4[marker] != 0) {
        g_song_80376990[0x1E] = marker;
        *(s8 *)&g_song_80376990[0x1F] = keep == 0;
        return;
    }
    g_song_80376990[0x1E] = 0;
    g_song_80376990[0x1F] = 0;
}
#pragma pop

void SetMIDITrackPriority(void) {

}

void SetMIDITrackOn(u32 mask, s8 deferred) {
    extern u32 g_trkOn;
    extern u32 g_trkOff;
    extern u8 g_chn_803769B4[0x200];
    u8 *chn;

    if (deferred) {
        g_trkOff &= ~mask;
        g_trkOn |= mask;
        return;
    }
    for (chn = g_chn_803769B4; mask != 0; chn += 0x20) {
        chn[4] |= mask & 1;
        mask >>= 1;
    }
}

#pragma push
#pragma optimization_level 1
void SetMIDITrackOff(u32 mask, s8 deferred) {
    extern u32 g_trkOn;
    extern u32 g_trkOff;
    extern u8 g_chn_803769B4[0x200];
    void ChannelOff(void *chn);
    u8 *chn;

    if (deferred) {
        g_trkOn &= ~mask;
        g_trkOff |= mask;
        return;
    }
    for (chn = g_chn_803769B4; mask != 0; chn += 0x20) {
        if (mask & 1) {
            chn[4] = 0;
            ChannelOff(chn);
        }
        mask >>= 1;
    }
}
#pragma pop

void SetMIDITrackVol(void) {

}

u32 GetEnvRate(u16 arg0) {
    if (arg0 != 0) {
        return -1U / (u32) (arg0 * 0x3E8);
    }
    return -1U;
}

void SetGVol(void *arg0) {
    s32 Mul16_(s32, s32);
    extern s32 g_musicVol;
    s32 vol = M2C_FIELD(arg0, u8 *, 0xB) * M2C_FIELD(arg0, u8 *, 0xA);
    M2C_FIELD(arg0, s32 *, 0x14) = Mul16_((u32)(vol * vol) / (u32)3970, g_musicVol);
}

void ReleaseMIDIVoice(void *arg0) {
    void SetVoicePriority_(void *);
    u32 rate;
    if (M2C_FIELD(arg0, u8 *, 0x16) != 4) {
        M2C_FIELD(arg0, u8 *, 0x16) = 4;
        M2C_FIELD(arg0, u8 *, 0x17) = 0;
        rate = GetEnvRate(M2C_FIELD(M2C_FIELD(arg0, void **, 0x10), u16 *, 0x1E));
        if (rate + 0x10000 == 0xFFFF) {
            rate = 0;
            M2C_FIELD(arg0, s32 *, 0x1C) = 0x10000;
        }
        M2C_FIELD(arg0, u32 *, 0x18) = rate;
        M2C_FIELD(arg0, u8 *, 0x14) -= 15;
        SetVoicePriority_(arg0);
    }
}

void ChannelOff(void *arg0) {
    void *v;
    for (v = M2C_FIELD(arg0, void **, 0); v != NULL; v = M2C_FIELD(v, void **, 4)) {
        ReleaseMIDIVoice(v);
    }
}

#pragma push
#pragma optimization_level 1
void DestroyMIDIVoice(u8 *voice) {
    extern u8 g_chn_803769B4[0x10][0x20];
    u8 *v;

    v = *(u8 **)g_chn_803769B4[voice[1]];
    if (v == voice) {
        *(u8 **)g_chn_803769B4[voice[1]] = *(u8 **)(voice + 4);
    } else {
        for (; *(u8 **)(v + 4) != voice; v = *(u8 **)(v + 4)) {
        }
        *(u8 **)(v + 4) = *(u8 **)(voice + 4);
    }
    voice[0x15] = 0;
    voice[1] = 0xFF;
}
#pragma push
#pragma optimization_level 1
u8 *DestroyVoice(u8 *voice) {
    extern u8 g_chn_803769B4[0x10][0x20];
    u8 *next = *(u8 **)(voice + 4);
    u8 *v;

    v = *(u8 **)g_chn_803769B4[voice[1]];
    if (v == voice) {
        *(u8 **)g_chn_803769B4[voice[1]] = next;
    } else {
        for (; *(u8 **)(v + 4) != voice; v = *(u8 **)(v + 4)) {
        }
        *(u8 **)(v + 4) = next;
    }
    SetVoiceState_(voice, 3);
    return next;
}
#pragma pop
s32 AudioMusic_Create(void) {
    extern s32 g_musicState;
    extern u8 g_musicTitle[0x20];
    extern u32 g_musicVol;
    extern u32 g_musicVol1_803C7934;
    extern u32 g_musicVol2_803C7930;

    g_musicVol = 0x7FFF;
    g_musicVol1_803C7934 = 0x7FFF;
    g_musicVol2_803C7930 = 0x7FFF;
    g_musicState = 0;
    g_musicTitle[0x1F] = 0;
    return 1;
}
#pragma push
#pragma optimization_level 1
s32 AudioMusic_Init(void) {
    s32 AudioVoid(void);
    extern u8 g_musicTitle[0x20];
    extern s32 (*SetMusicState)();
    extern s32 (*DestroyMusic)();
    extern s32 (*SetMusicVolume)();
    extern s32 (*SetMusicPitch)();
    extern s32 (*SetMusicTempo)();
    extern s32 (*SetMusicOrder)();
    extern s32 (*SetMusicTrackPriority)();
    extern s32 (*SetMusicTrackOn)();
    extern s32 (*SetMusicTrackOff)();
    extern s32 (*SetMusicTrackVol)();

    DestroyMusic = AudioVoid;
    SetMusicState = AudioVoid;
    SetMusicVolume = AudioVoid;
    SetMusicPitch = AudioVoid;
    SetMusicTempo = AudioVoid;
    SetMusicOrder = AudioVoid;
    SetMusicTrackPriority = AudioVoid;
    SetMusicTrackOn = AudioVoid;
    SetMusicTrackOff = AudioVoid;
    SetMusicTrackVol = AudioVoid;
    memset(g_musicTitle, 0, 0x20);
    return 1;
}
#pragma pop
void SetMusicVol(u32 vol) {
    extern u32 g_musicVol;
    extern u32 g_musicVol1_803C7934;
    extern u32 g_musicVol2_803C7930;
    extern s32 (*SetMusicVolume)();

    g_musicVol1_803C7934 = vol;
    g_musicVol = (g_musicVol1_803C7934 * g_musicVol2_803C7930) >> 15;
    SetMusicVolume();
}
void SetMusicGVol(u32 vol) {
    extern u32 g_musicVol;
    extern u32 g_musicVol1_803C7934;
    extern u32 g_musicVol2_803C7930;
    extern s32 (*SetMusicVolume)();

    g_musicVol2_803C7930 = vol;
    g_musicVol = (g_musicVol1_803C7934 * g_musicVol2_803C7930) >> 15;
    SetMusicVolume();
}
#pragma pop
s32 AudioVoid(void) {
    return 0;
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
s8 Audio_Create(void) {
    extern void (*SongUpdate)(void);
    extern void (*VoiceUpdate)(void);
    extern s8 g_init_803C7938;
    s8 AudioMusic_Create(void);
    s8 AudioIT_Create(void);
    s8 AudioMIDI_Create(void);
    s8 AudioVoice_Create(void);
    s8 Audio_Create_(void);

    if (!g_init_803C7938) {
        VoiceUpdate = NULL;
        SongUpdate = NULL;
        if (AudioMusic_Create() && AudioIT_Create() && AudioMIDI_Create() && AudioVoice_Create() && Audio_Create_()) {
            g_init_803C7938 = 1;
        }
    }
    return g_init_803C7938;
}
#pragma pop

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
s8 Audio_Init(void) {
    extern void (*SongUpdate)(void);
    extern void (*VoiceUpdate)(void);
    s8 AudioMusic_Init(void);
    s8 AudioIT_Init(void);
    s8 AudioMIDI_Init(void);
    s8 AudioVoice_Init(void);
    s8 Audio_Init_(void);

    VoiceUpdate = NULL;
    SongUpdate = NULL;
    if (AudioMusic_Init() && AudioIT_Init() && AudioMIDI_Init() && AudioVoice_Init() && Audio_Init_()) {
        return 1;
    }
    return 0;
}
#pragma pop
void Audio_Destroy(void) {
    extern void (*SongUpdate)(void);
    extern void (*VoiceUpdate)(void);
    extern u8 g_init_803C7938;
    void AudioIT_Destroy(void);
    void AudioMIDI_Destroy(void);
    void AudioVoice_Destroy(void);
    void Audio_Destroy_(void);
    *(void (*volatile *)(void))&VoiceUpdate = NULL;
    *(void (*volatile *)(void))&SongUpdate = NULL;
    AudioIT_Destroy();
    AudioMIDI_Destroy();
    AudioVoice_Destroy();
    Audio_Destroy_();
    g_init_803C7938 = 0;
}

void Audio_Update(s32 arg0) {
    extern void (*SongUpdate)(void);
    extern void (*VoiceUpdate)(void);
    if (*(void (*volatile *)(void))&SongUpdate != NULL) {
        SongUpdate();
    }
    if (*(void (*volatile *)(void))&VoiceUpdate != NULL) {
        VoiceUpdate();
    }
}

void SteeringResetCallback(s32 index, s32 result) {
    if (result != 0) {
        g_lgDevices_803771D8[index].unk4D = -1;
    }
}

s8 AutoCalibrateAxisSigned(s8 *min, s8 *max, s8 value, s8 deadzone, s8 *out) {
    s32 lo = *min;
    s32 hi = *max;
    s32 range;
    s32 result;

    if (value < lo) {
        *min = value;
        lo = value;
    }
    if (value > hi) {
        hi = value;
        *max = value;
    }
    range = hi - lo - deadzone * 2;
    if (range == 0) {
        range = 1;
    }
    result = (value - (lo + deadzone)) * 255 / range - 127;
    if (result < -128) {
        result = -128;
    }
    if (result > 127) {
        result = 127;
    }
    *out = result;
    return result;
}

u8 AutoCalibrateAxisUnsigned(u8 *min, u8 *max, u8 value, u8 deadzone, u8 *out) {
    s32 lo = *min;
    s32 hi = *max;
    s32 range;
    s32 result;

    if (value < lo) {
        *min = value;
        lo = value;
    }
    if (value > hi) {
        hi = value;
        *max = value;
    }
    range = hi - lo - deadzone * 2;
    if (range == 0) {
        range = 1;
    }
    result = (value - (lo + deadzone)) * 255 / range;
    if (result < 0) {
        result = 0;
    }
    if (result > 255) {
        result = 255;
    }
    *out = result;
    return result;
}

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
void LGInit(void) {
    extern s32 g_lgInitialized_803C7948;
    void InitDevice(LGDevice *device, s32 index);
    void SIResetSteeringAsync(s32 chan, void *callback);
    void SIReadSteering(s32 chan, void *status);
    void SISetSteeringSamplingCallback(void *callback);
    void SteeringSamplingCallback(void);
    LGDevice *device;
    s32 i;

    if (g_lgInitialized_803C7948 == 0) {
        for (i = 0; i < 4; i++) {
            device = &g_lgDevices_803771D8[i];
            VDevice_Init(&device->vdev);
            InitDevice(device, i);
            SIResetSteeringAsync(i, SteeringResetCallback);
            SIReadSteering(i, device->unkC);
        }
        SISetSteeringSamplingCallback(SteeringSamplingCallback);
        g_lgInitialized_803C7948 = 1;
    }
}
#pragma pop

s32 LGClose(LGDevice *device) {
    if (device == NULL || (u32)device == 0xFFFFFFFF) {
        return -2;
    }
    if (!device->opened) {
        return -2;
    }
    device->opened = 0;
    device->started = 0;
    VDevice_Shutdown(&device->vdev);
    device->unk8 = -1;
    return 0;
}

s32 LGStartForceEffect(u32 handle) {
    u32 index = handle >> 16;
    LGDevice *device;

    if (index >= 4) {
        return -2;
    }
    device = &g_lgDevices_803771D8[index];
    if (!device->opened) {
        return -2;
    }
    if (device->started) {
        if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened) {
            device->opened = 0;
            device->started = 0;
            VDevice_Shutdown(&device->vdev);
            device->unk8 = -1;
        }
        return -2;
    }
    return VDevice_StartEffect(&device->vdev, handle);
}

s32 VDevice_DestroyEffect(VDevice *dev, u32 handle);
s32 LGDestroyForceEffect(u32 handle) {
    u32 index = handle >> 16;
    LGDevice *device;

    if (index >= 4) {
        return -2;
    }
    device = &g_lgDevices_803771D8[index];
    if (!device->opened) {
        return -2;
    }
    if (device->started) {
        if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened) {
            device->opened = 0;
            device->started = 0;
            VDevice_Shutdown(&device->vdev);
            device->unk8 = -1;
        }
        return -2;
    }
    return VDevice_DestroyEffect(&device->vdev, handle);
}

#pragma push
#pragma optimize_for_size on
void zero_mem_aligned(void *dst, u32 n) {
    u8 *cdst = (u8 *)dst - 1;
    u32 *ldst;
    u32 i;

    if (n >= 32) {
        i = n >> 5;
        ldst = (u32 *)dst - 1;
        if (i) {
            do {
                *++ldst = 0;
                *++ldst = 0;
                *++ldst = 0;
                *++ldst = 0;
                *++ldst = 0;
                *++ldst = 0;
                *++ldst = 0;
                *++ldst = 0;
            } while (--i);
        }
        i = (n & 31) >> 2;
        if (i) {
            do {
                *++ldst = 0;
            } while (--i);
        }
        cdst = (u8 *)ldst + 3;
        n &= 3;
    }
    if (n) {
        do {
            *++cdst = 0;
        } while (--n);
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
void VDevice_Init(VDevice *dev) {
    s32 i;

    SimThread_Init(dev, dev);
    dev->unk11F4 = 0xFF;
    for (i = 0; i < 64; i++) {
        dev->activeEffects[i] = NULL;
    }
    if (g_bGammaInitialized_803C794C == 0) {
        VDevice_RecalcGammaTable();
        g_bGammaInitialized_803C794C = 1;
    }
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 VDevice_DownloadEffect(VDevice *dev, u32 *handle, u8 *desc) {
    s32 ret = 0;
    u32 index;
    Effect *effect;

    *handle = VDevice_GetFreeEffect(dev);
    if (*handle == 0xFFFFFFFF) {
        ret = -5;
    } else {
        index = *handle & 0x3F;
        effect = &dev->effects[index];
        switch (desc[0]) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            Effect_Init(effect);
            effect->state = (EffectState *)dev;
            Effect_UpdateEffect(effect, desc);
            break;
        default:
            ret = -2;
            *handle = -1;
            break;
        }
        if (effect != NULL) {
            dev->activeEffects[index] = effect;
        }
    }
    return ret;
}

s32 VDevice_UpdateEffect(VDevice *dev, u32 handle, u8 *desc) {
    s32 ret = -2;
    Effect *effect = dev->activeEffects[handle & 0x3F];
    if (effect != NULL) {
        ret = Effect_UpdateEffect(effect, desc);
    }
    return ret;
}

s32 VDevice_DestroyEffect(VDevice *dev, u32 handle) {
    s32 ret = -2;
    u32 index = handle & 0x3F;
    if (dev->activeEffects[index] != NULL) {
        dev->activeEffects[index] = NULL;
        ret = 0;
    }
    return ret;
}

s32 VDevice_StartEffect(VDevice *dev, u32 handle) {
    s32 Effect_StartEffect(void *arg0);
    s32 ret = -2;
    Effect *effect = dev->activeEffects[handle & 0x3F];
    if (effect != NULL) {
        ret = Effect_StartEffect(effect);
    }
    return ret;
}

#pragma push
#pragma optimize_for_size on
s32 VDevice_Initialize(VDevice *dev, s32 arg1) {
    s32 i;

    dev->unk11F4 = 0xFF;
    for (i = 0; i < 64; i++) {
        dev->activeEffects[i] = NULL;
    }
    dev->unkF0 = arg1;
    return 0;
}

s32 VDevice_Shutdown(VDevice *dev) {
    s32 i;

    for (i = 0; i < 64; i++) {
        if (dev->activeEffects[i] != NULL) {
            dev->activeEffects[i] = NULL;
        }
    }
    return 0;
}

u32 VDevice_GetFreeEffect(VDevice *dev) {
    static u32 jumbleeffectid;
    static u32 createcount;
    u32 handle = 0xFFFFFFFF;
    s32 i;
    u32 index;

    for (i = 0; i < 64; i++) {
        index = (i + jumbleeffectid) & 0x3F;
        if (dev->activeEffects[index] == NULL) {
            jumbleeffectid = index + 1;
            jumbleeffectid &= 0x3F;
            handle = (*(s32 *)dev->unkF0 << 16) | ((createcount & 0xFF) << 8) | (index & 0x3F);
            break;
        }
    }
    return handle;
}

#pragma pop

#pragma pop
s32 Effect_StartEffect(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x28) = (s32) (M2C_FIELD(arg0, s32 *, 0x28) | 1);
    M2C_FIELD(arg0, s32 *, 0x28) = (s32) (M2C_FIELD(arg0, s32 *, 0x28) & 0xFFFFFFFD);
    M2C_FIELD(arg0, s32 *, 0x2C) = 0;
    M2C_FIELD(arg0, s32 *, 0x30) = (s32) M2C_FIELD(arg0, s32 *, 8);
    if (M2C_FIELD(arg0, s32 *, 0x28) & 0x40) {
        M2C_FIELD(arg0, s32 *, 0x38) = (s32) (M2C_FIELD(arg0, u16 *, 0x12) << 0x10);
    }
    return 0;
}

#pragma push
#pragma optimize_for_size on
void Effect_PolarToRect(s32 magnitude, u8 angle, s32 *out) {
    s32 round;
    s32 value;
    s32 sine;
    u32 a = angle;

    if (a < 0x40) {
        sine = g_iQuarterSineTable_8037BB7C[a];
    } else if (a < 0x80) {
        sine = g_iQuarterSineTable_8037BB7C[0x7F - a];
    } else if (a < 0xC0) {
        sine = -g_iQuarterSineTable_8037BB7C[a - 0x80];
    } else {
        sine = -g_iQuarterSineTable_8037BB7C[0xFF - a];
    }
    sine = magnitude * sine;
    round = 0x1FE00;
    if (sine < 0) {
        round = -0x1FE00;
    }
    value = sine * 0x7F;
    value += round;
    *out = -(value / 0x3FC00);
}

s32 Effect_UpdateSpring(Effect *effect, EffectCondition *cond) {
    s32 low = effect->deadbandLow;
    s32 pos = effect->state->position;
    s32 high;
    s32 force;
    s32 sat;

    if (pos < low) {
        sat = cond->negSaturation;
        force = pos - low;
        force = (force * cond->negCoefficient) / 0x20000;
        if (-sat > force) {
            force = -sat;
        }
        if (sat < force) {
            force = sat;
        }
        return -force;
    }
    high = effect->deadbandHigh;
    if (pos > high) {
        sat = cond->posSaturation;
        force = pos - high;
        force = (force * cond->posCoefficient) / 0x20000;
        if (-sat > force) {
            force = -sat;
        }
        if (sat < force) {
            force = sat;
        }
        return -force;
    }
    return 0;
}

s32 Effect_UpdateDamper(Effect *effect, EffectCondition *cond) {
    s32 vel = effect->state->velocity;
    s32 coef;
    s32 sat;
    s32 force;

    if (vel < 0) {
        coef = -cond->negCoefficient;
        sat = cond->negSaturation;
    } else {
        coef = -cond->posCoefficient;
        sat = cond->posSaturation;
    }
    vel *= coef;
    force = (vel * 20) / 0xFF000;
    if (-sat > force) {
        force = -sat;
    }
    if (sat < force) {
        force = sat;
    }
    return force;
}
#pragma pop

void ShortCommandProc(s32 chan) {
    GBAControl *gba = &__GBA[chan];

    if (gba->ret != 0) {
        return;
    }
    if (gba->input[0] != 0 || gba->input[1] != 4) {
        gba->ret = 1;
        return;
    }
    *gba->status = gba->input[2] & 0x3A;
}

s32 GBAResetAsync(s32 chan, u8 *status, GBACallback callback) {
    GBAControl *gba = &__GBA[chan];

    if (gba->callback != NULL) {
        return 2;
    }
    gba->output[0] = 0xFF;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 3, ShortCommandProc);
}

#pragma push
#pragma peephole off
s32 GBAReset(s32 chan, u8 *status) {
    s32 ret;

    ret = GBAResetAsync(chan, status, (GBACallback)__GBASyncCallback);
    if (ret != 0) {
        return ret;
    }
    return __GBASync(chan);
}
#pragma pop

s32 OnReset_800C4F14(void) {
    extern s32 __GBAReset;
    __GBAReset = 1;
    return 1;
}

static void F23(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0) {
        ret = GBAResetAsync(chan, bootInfo->status, F25);
    }
    if (ret != 0) {
        bootInfo->begin = 0;
        callback = bootInfo->callback;
        if (callback != NULL) {
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

static void F25(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0 && *bootInfo->status != D54_800DAFF8[0x25]) {
        ret = 3;
    }
    if (ret == 0) {
        ret = GBAGetStatusAsync(chan, bootInfo->status, F27);
    }
    if (ret != 0) {
        bootInfo->begin = 0;
        callback = bootInfo->callback;
        if (callback != NULL) {
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

s32 FileManager_Create(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_files[i].slot = -1;
    }
    for (i = 0; i < 5; i++) {
        g_appInfo[i][0] = 0;
    }
    return 1;
}

static void F29(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0) {
        __GBAX02(chan, bootInfo->readbuf);
    } else {
        bootInfo->begin = 0;
        callback = bootInfo->callback;
        if (callback != NULL) {
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

static void F39(s32 chan, s32 ret);
s32 GBAWriteAsync(s32 chan, u8 *src, u8 *status, GBACallback callback);
static void F37(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0) {
        if ((u8)((bootInfo->readbuf[0] ^ bootInfo->initialCode) |
                 (bootInfo->readbuf[1] ^ (bootInfo->initialCode >> 8)) |
                 ((bootInfo->readbuf[3] ^ (bootInfo->initialCode >> 24)) |
                  (bootInfo->readbuf[2] ^ (bootInfo->initialCode >> 16))))) {
            ret = 3;
        } else {
            ret = GBAWriteAsync(chan, bootInfo->readbuf, bootInfo->status, F39);
        }
    }
    if (ret != 0) {
        bootInfo->begin = 0;
        callback = bootInfo->callback;
        if (callback != NULL) {
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

static void F39(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0) {
        *bootInfo->status = 0;
    }
    bootInfo->begin = 0;
    callback = bootInfo->callback;
    if (callback != NULL) {
        bootInfo->callback = NULL;
        callback(chan, ret);
    }
    gba->ret = ret;
}

void ReadProc(s32 chan) {
    GBAControl *gba;

    gba = &__GBA[chan];
    if (gba->ret != 0) {
        return;
    }
    memcpy(gba->ptr, gba->input, 4);
    *gba->status = gba->input[4] & 0x3A;
}

s32 GBARead(s32 chan, u8 *dst, u8 *status) {
    GBAControl *gba = &__GBA[chan];
    s32 ret;

    if (gba->callback != NULL) {
        ret = 2;
    } else {
        gba->output[0] = 0x14;
        gba->ptr = dst;
        gba->status = status;
        gba->callback = __GBASyncCallback;
        ret = __GBATransfer(chan, 1, 5, ReadProc);
    }
    if (ret != 0) {
        return ret;
    }
    return __GBASync(chan);
}

s32 __GBATransfer(s32 chan, s32 w1, s32 r1, void *proc);
void ReadProc(s32 chan);
void WriteProc(s32 chan);
void WriteProc(s32 chan) {
    GBAControl *gba = &__GBA[chan];

    if (gba->ret != 0) {
        return;
    }
    *gba->status = gba->input[0] & 0x3A;
}

#pragma push
#pragma peephole off
#pragma use_lmw_stmw on
s32 GBAWriteAsync(s32 chan, u8 *src, u8 *status, GBACallback callback) {
    GBAControl *gba = &__GBA[chan];

    if (gba->callback != NULL) {
        return 2;
    }
    gba->output[0] = 0x15;
    memcpy(&gba->output[1], src, 4);
    gba->ptr = src;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 5, 1, WriteProc);
}
#pragma pop

void __GBASyncCallback(s32 chan) {
    void OSWakeupThread(void *);
    OSWakeupThread(__GBA[chan].threadQueue);
}

s32 __GBASync(s32 chan) {
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32);
    void OSSleepThread(void *);
    GBAControl *gba = &__GBA[chan];
    s32 enabled;
    s32 ret;

    enabled = OSDisableInterrupts();
    while (gba->callback != NULL) {
        OSSleepThread(gba->threadQueue);
    }
    ret = gba->ret;
    OSRestoreInterrupts(enabled);
    return ret;
}

s64 __GBASetDelay(s32 chan, s64 delay) {
    GBAControl *gba = &__GBA[chan];
    s64 oldDelay = gba->delay;
    gba->delay = delay;
    return oldDelay;
}

void FileManager_Destroy(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void FileManager_Release(void) {
    void File_Close(s32 handle);
    s16 i;

    for (i = 0; i < 4; i++) {
        if (g_files[i].slot != -1) {
            File_Close((g_files[i].id << 16) | (g_files[i].slot << 8) | i);
        }
    }
}

void FileManager_SetAppInfo(const char *a, const char *b, const char *c, const char *d, const char *e) {
    char *strcpy(char *dst, const char *src);

    strcpy(g_appInfo[0], a);
    strcpy(g_appInfo[1], b);
    strcpy(g_appInfo[2], c);
    strcpy(g_appInfo[3], d);
    strcpy(g_appInfo[4], e);
}

s32 stricmp(const char *s1, const char *s2) {
    char c1, c2;

    while (*s1) {
        c2 = tolower(*(u8 *)s2);
        c1 = tolower(*(u8 *)s1);
        if (c1 != c2) {
            c1 = tolower(*(u8 *)s1);
            c2 = tolower(*(u8 *)s2);
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return *s2 ? -1 : 0;
}

s32 FileManager_GetDeviceFreeSpace(s32 device, u32 *freeBytes, u32 *totalBytes) {
    freeBytes[1] = 0x32000;
    freeBytes[0] = 0;
    totalBytes[1] = 0x32000;
    totalBytes[0] = 0;
    return 1;
}

s32 CARDFormat(s32 chan);
s8 FileManager_IsDeviceFormatted(s32 device) {
    return m_memcardstatus_803C7970[device - 11] == 0;
}

s8 FileManager_FormatDevice(s32 device) {
    m_memcardstatus_803C7970[device - 11] = CARDFormat(device - 11);
    return m_memcardstatus_803C7970[device - 11] == 0;
}

s32 File_Seek(s32 handle, s32 offset, s8 mode) {
    s8 idx = (u8)handle;

    if (mode == 0) {
        g_files[idx].position = offset;
    } else {
        g_files[idx].position += offset;
    }
    return 0;
}

s32 File_Write(u32 id, s32 size, void *data) {
    s8 device = (u8)(id >> 16);

    if (device >= 11) {
        return MemCard_Write__FUllPv(device, size, data);
    }
    return 0;
}

s32 File_Delete(s32 device, const char *fileName) {
    s32 chan = device - 11;

    if (CARDMount(chan, CardWorkArea, 0) != 0) {
        return 0;
    }
    if (CARDDelete(chan, fileName) == 0) {
        CARDUnmount(chan);
        return 1;
    }
    CARDUnmount(chan);
    return 0;
}

void MemCard_Close__Fl(s32 device) {
    s32 chan = device - 11;

    m_memcardstatus_803C7970[chan] = CARDClose(&memCardfileInfo[chan]);
    m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
    OSReport("CARDClose\n");
}

s32 MemCard_Read__FUllPv(u32 device, s32 size, void *data) {
    u8 buffer[0x2000] __attribute__((aligned(32)));
    s32 chan = device - 11;

    m_memcardstatus_803C7970[chan] = CARDRead(&memCardfileInfo[chan], buffer, memCardBlockSize[chan], 0);
    memcpy(data, buffer + 0x840, size);
    return size;
}

#pragma push
#pragma readonly_strings on
void LidHandler_Init(void) {
    s32 DVDOpen(const char *name, void *fileInfo);
    extern u32 m_isopen_803C7984;
    extern u32 m_curtime_803C7988;
    extern u8 m_fileInfo_803867E0[0x3C];

    if (m_isopen_803C7984 == 0) {
        DVDOpen("bsfile", m_fileInfo_803867E0);
        m_isopen_803C7984 = 1;
        m_curtime_803C7988 = 0;
    }
}
#pragma pop

void LidHandler_Heartbeat(void) {
    s32 DVDGetCommandBlockStatus(void *block);
    s32 DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void *callback, s32 prio);
    extern u8 cd_test_block[0x20];
    extern s32 g_iDeltaTimeMS;
    extern u32 m_curtime_803C7988;
    extern u8 m_fileInfo_803867E0[0x3C];

    m_curtime_803C7988 += g_iDeltaTimeMS;
    if (m_curtime_803C7988 > 500) {
        if (DVDGetCommandBlockStatus(m_fileInfo_803867E0) == 0) {
            DVDReadAsyncPrio(m_fileInfo_803867E0, cd_test_block, 0x20, 0, NULL, 2);
            m_curtime_803C7988 = 0;
        }
    }
}

s32 Input_GetPlayerDevice(s32 player) {
    s16 i;

    player--;
    if ((u32)player > 4) {
        for (i = 0; i < 4; i++) {
            if ((s8)g_VirtualPad[i].unk0[1] == 0) {
                g_VirtualPad[i].unk0[1] = 1;
                g_VirtualPad[i].unk0[2] = 1;
                return i;
            }
        }
        return -2;
    }
    g_VirtualPad[player].unk0[1] = 1;
    g_VirtualPad[player].unk0[2] = 1;
    return player;
}

s8 Input_KeyPress(s32 chan, u8 key) {
    s32 i;
    if (key > 0x14) {
        return 0;
    }
    if (chan == -1) {
        for (i = 0; i < 4; i++) {
            if ((g_VirtualPad[i].keyPress >> key) & 1) {
                return 1;
            }
        }
    } else {
        return (g_VirtualPad[chan].keyPress >> key) & 1;
    }
    return 0;
}

u8 Input_KeyDown(s32 chan, u8 key) {
    s32 i;
    if (key == 0xFF) {
        return 0;
    }
    if (chan == -1) {
        for (i = 0; i < 4; i++) {
            if ((g_VirtualPad[i].keyDown >> key) & 1) {
                return g_VirtualPad[chan].keyValue[key];
            }
        }
    } else if ((g_VirtualPad[chan].keyDown >> key) & 1) {
        return g_VirtualPad[chan].keyValue[key];
    }
    return 0;
}

void InitTables__Fv(void);
void Input_Init(void);
void *__nw__FUl(u32 size);
void *__ct__8LGWheelsFv(void *self);
void PadSamplingCallback__Fv(void);
void GBAConnectivityInit__Fv(void);
void *PADSetSamplingCallback(void *callback);
void Input_SetForceFeedbackActive(s32 chan, u8 active) {
    g_VirtualPad[chan].forceFeedbackActive = active;
}

void SISetSamplingRate(u32 msec);
s32 Input_Create(void) {
    void *w;
    InitTables__Fv();
    Input_Init();
    SISetSamplingRate(1);
    w = __nw__FUl(0x1430);
    if (w != NULL) {
        w = __ct__8LGWheelsFv(w);
    }
    wheels = w;
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 0, 0x50);
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 1, 0x64);
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 2, 0x64);
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 3, 0x64);
    PADSetSamplingCallback(PadSamplingCallback__Fv);
    GBAConnectivityInit__Fv();
    return 1;
}

void Input_Destroy(void) {
    M2C_UNK PADControlMotor(M2C_UNK, M2C_UNK);
    PADControlMotor(0, 0);
    PADControlMotor(1, 0);
    PADControlMotor(2, 0);
    PADControlMotor(3, 0);
}

void Input_SetForceFeedback(s32 chan, f32 strength, f32 duration) {
    void PADControlMotor(s32 chan, u32 command);
    if (g_VirtualPad[chan].forceFeedbackActive) {
        rumbleStatus[chan].duration = 32.0f * duration * strength;
        rumbleStatus[chan].active = 1;
        PADControlMotor(chan, 1);
    }
}

void Input_SetAlternativeForceFeedback(void) {

}

void Input_ForceFeedbackPause(void) {

}

s32 Input_GetContollersPluggedIn(void) {
    return 0xF;
}

void Wheel_PlaySpringForce(s32 chan, s8 offset, u8 saturation, s16 coefficient) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlaySpringForce__8LGWheelsFlScUcs(wheels, chan, offset, saturation, coefficient);
    }
}

void Wheel_PlayDamperForce(s32 chan, u8 coefficient, s16 offset) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayDamperForce__8LGWheelsFlUcs(wheels, chan, coefficient, offset);
    }
}

void PlayConstantForce__8LGWheelsFlsUs(void *wheels, s32 chan, u8 coefficient, s16 offset);
void Wheel_PlayConstantForce(s32 chan, u8 coefficient, s16 offset) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayConstantForce__8LGWheelsFlsUs(wheels, chan, coefficient, offset);
    }
}

void Wheel_PlayCarAirborne(s32 chan) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayCarAirborne__8LGWheelsFl(wheels, chan);
    }
}

void StopCarAirborne__8LGWheelsFl(void *wheels, s32 chan);
void Wheel_StopCarAirborne(s32 chan) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        StopCarAirborne__8LGWheelsFl(wheels, chan);
    }
}

void Wheel_PlayFrontalCollisionForce(s32 chan, u8 force) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayFrontalCollisionForce__8LGWheelsFlUc(wheels, chan, force);
    }
}

s32 Wheel_IsConnected(s32 chan) {
    return IsConnected__8LGWheelsFl(wheels, chan);
}

void Wheel_PlayAllAirborne(void) {

}

void PadSamplingCallback__Fv(void) {
    PADStatus pads[4];
    s32 i;

    PADRead(pads);
    PADClamp(pads);
    for (i = 0; i < 4; i++) {
        if (pads[i].err != -3) {
            memcpy(&ngcPad[i], &pads[i], sizeof(PADStatus));
        }
    }
}

void PADRead(PADStatus *status);
void PADClamp(PADStatus *status);
void PlaySpringForce__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient);
void JBCallback__Fll(s32 chan, s32 result) {
    Param_803C6A40[chan].state = 2;
    Param_803C6A40[chan].result = result;
}

void GBACloseCallback__FlP11DVDFileInfo(s32 result, void *fileInfo);
void GBAConnectivityInit__Fv(void) {
    extern u8 GBAfileInfo_803C6C10[0x3C];
    extern u8 ClientImage_80386A40[0x40000];
    extern s32 gLinkFinished;
    extern u32 ImageSize_803C79A0;

    gLinkFinished = 0;
    DVDOpen("client_thread.bin", GBAfileInfo_803C6C10);
    ImageSize_803C79A0 = *(u32 *)(GBAfileInfo_803C6C10 + 0x34);
    DVDReadAsyncPrio(GBAfileInfo_803C6C10, ClientImage_80386A40, (*(u32 *)(GBAfileInfo_803C6C10 + 0x34) + 0x1F) & ~0x1F, 0, GBACloseCallback__FlP11DVDFileInfo, 2);
}

void GBAConnectivityLoop__Fv(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 __ct__8ConstantFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

s32 __ct__5ForceFv(s32 arg0) {
    M2C_UNK InitVars__5ForceFv();
    InitVars__5ForceFv();
    return arg0;
}

void InitVars__5ForceFv(Force *force) {
    s32 i;
    s32 j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            force->playing[i][j] = 0;
            force->effectIds[i][j] = 0xFFFFFFFF;
        }
    }
}

s32 Start__5ForceFll(Force *force, s32 chan, s32 effect) {
    s32 LGStartForceEffect(u32 handle);
    s32 result = 0;

    if (force->effectIds[chan][effect] != 0xFFFFFFFF) {
        result = LGStartForceEffect(force->effectIds[chan][effect]);
        if (result < 0) {
            OSReport("ERROR: Failed to start force effect on channel %d\n", chan);
        } else {
            force->playing[chan][effect] = 1;
        }
    } else {
        OSReport("ERROR: Trying to start force effect on channel %d but we have an invalid effectid\n", chan);
    }
    return result;
}

s32 Destroy__5ForceFll(Force *force, s32 chan, s32 effect) {
    s32 LGDestroyForceEffect(u32 handle);
    s32 result = 0;

    if (force->effectIds[chan][effect] != 0xFFFFFFFF) {
        result = LGDestroyForceEffect(force->effectIds[chan][effect]);
        if (result < 0) {
            OSReport("ERROR: Failed to destroy force effect on channel %d\n", chan);
        } else {
            force->playing[chan][effect] = 0;
        }
    } else {
        OSReport("ERROR: Trying to destroy force effect on channel %d but we have an invalid effectid\n", chan);
    }
    return result;
}

void SetOverallForceGain__5ForceFRUll(void *force, u32 *effectId, s32 gain) {
    s32 LGSetDeviceProperty(u32 id, s32 prop, s32 value);

    if (LGSetDeviceProperty(*effectId, 3, gain) < 0) {
        OSReport("ERROR: Failed to set overall force gain.\n");
    }
}

s32 GetOverallForceGain__5ForceFRUl(void *force, u32 *effectId) {
    s32 LGGetDeviceProperty(u32 id, s32 prop, void *value);
    s32 gain;
    s32 result;

    result = LGGetDeviceProperty(*effectId, 3, &gain);
    if (result < 0) {
        OSReport("ERROR: Failed to get overall force gain.\n");
        return result;
    }
    return gain;
}

#pragma push
#pragma dont_inline on
void *__ct__8LGWheelsFv(void *self) {
    u8 *lg = self;

    __ct__6WheelsFv((Wheels *)(lg + 0x82C));
    __ct__5ForceFv((s32)(lg + 0x10AC));
    __ct__9ConditionFv((s32)(lg + 0x114C));
    __ct__8ConstantFv((s32)(lg + 0x11EC));
    __ct__8PeriodicFv((s32)(lg + 0x128C));
    __ct__4RampFv((s32)(lg + 0x132C));
    lg[0x13CC] = 0xFF;
    lg[0x13CD] = 0;
    InitLGLibrary__8LGWheelsFv(lg);
    lg[0x828] = 0;
    lg[0x829] = 0;
    lg[0x82A] = 0;
    lg[0x82B] = 0;
    return lg;
}
#pragma pop

void InitLGLibrary__8LGWheelsFv(void *wheels) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

u8 SameSpringForceParams__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient) {
    u8 *p = (u8 *)wheels + chan * 4;

    if (*(s8 *)(p + 0x13CE) == offset && p[0x13CF] == saturation && *(s16 *)(p + 0x13D0) == coefficient) {
        return 1;
    }
    return 0;
}

u8 SameConstantForceParams__8LGWheelsFlsUs(void *wheels, s32 chan, s16 offset, u16 coefficient) {
    u8 *p = (u8 *)wheels + chan * 4;

    if (*(s16 *)(p + 0x13DE) == offset && *(u16 *)(p + 0x13E0) == coefficient) {
        return 1;
    }
    return 0;
}

u8 SameDamperForceParams__8LGWheelsFlUcs(void *wheels, s32 chan, u8 coefficient, s16 offset) {
    u8 *p = (u8 *)wheels + chan * 4;

    if (p[0x13EE] == coefficient && *(s16 *)(p + 0x13F0) == offset) {
        return 1;
    }
    return 0;
}

u8 SameFrontalCollisionForceParams__8LGWheelsFls(LGWheels *wheels, s32 chan, s16 force) {
    return wheels->frontalCollisionParams[chan] == force;
}

void PlayCarAirborne__8LGWheelsFl(void *wheels, s32 chan) {
    if (IsOpened__6WheelsFl((Wheels *)((u8 *)wheels + 0x82C), chan)) {
        if (GetOverallForceGain__8LGWheelsFl(wheels, chan) != 0) {
            *((u8 *)wheels + 0x13CC) = GetOverallForceGain__8LGWheelsFl(wheels, chan);
            SetOverallForceGain__8LGWheelsFll(wheels, chan, 0);
            ((u8 *)wheels + 0x828)[chan] = 1;
        }
    } else {
        OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
    }
}

void StopCarAirborne__8LGWheelsFl(void *wheels, s32 chan) {
    if (IsOpened__6WheelsFl((Wheels *)((LGWheels *)wheels)->wheels, chan)) {
        SetOverallForceGain__8LGWheelsFll(wheels, chan, ((LGWheels *)wheels)->savedGain);
        ((LGWheels *)wheels)->airborne[chan] = 0;
    } else {
        OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
    }
}

u8 IsOpened__6WheelsFl(Wheels *wheels, s32 chan);
s8 IsConnected__6WheelsFl(Wheels *wheels, s32 chan);
u8 PedalsConnected__6WheelsFl(Wheels *wheels, s32 chan);
void GenerateNonLinValues__6WheelsFlUc(Wheels *wheels, s32 chan, u8 coefficient);

u8 IsOpened__8LGWheelsFl(void *wheels, s32 chan) {
    return IsOpened__6WheelsFl((Wheels *)((u8 *)wheels + 0x82C), chan);
}

s8 IsConnected__8LGWheelsFl(void *wheels, s32 chan) {
    return IsConnected__6WheelsFl((Wheels *)((u8 *)wheels + 0x82C), chan);
}

u8 PedalsConnected__8LGWheelsFl(void *wheels, s32 chan) {
    return PedalsConnected__6WheelsFl((Wheels *)((u8 *)wheels + 0x82C), chan);
}

#pragma push
#pragma dont_inline on
void SetOverallForceGain__8LGWheelsFll(void *wheels, s32 chan, s32 gain) {
    LGWheels *lg = wheels;

    SetOverallForceGain__5ForceFRUll(lg->force, &lg->overallGainId[chan], gain);
}
#pragma pop

#pragma push
#pragma dont_inline on
s32 GetOverallForceGain__8LGWheelsFl(void *wheels, s32 chan) {
    LGWheels *lg = wheels;

    return GetOverallForceGain__5ForceFRUl(lg->force, &lg->overallGainId[chan]);
}
#pragma pop

void GenerateNonLinValues__8LGWheelsFlUc(void *wheels, s32 chan, u8 coefficient) {
    GenerateNonLinValues__6WheelsFlUc((Wheels *)((u8 *)wheels + 0x82C), chan, coefficient);
    memcpy((u8 *)wheels + 0x28, (u8 *)wheels + 0x854, 0x800);
}

s32 __ct__8PeriodicFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

s32 __ct__4RampFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

Wheels *__ct__6WheelsFv(Wheels *wheels) {
    s32 i;

    for (i = 0; i < 4; i++) {
        ((s32 *)wheels->unk828)[i] = -1;
        wheels->status[i].err = -1;
    }
    memset((u8 *)wheels + 0x858, 0, 0x28);
    return wheels;
}

u8 IsOpened__6WheelsFl(Wheels *wheels, s32 chan) {
    return wheels->status[chan].err == 0;
}

s8 IsConnected__6WheelsFl(Wheels *wheels, s32 chan) {
    u32 SIProbe(s32 chan);

    wheels->type[chan] = SIProbe(chan);
    switch (wheels->type[chan]) {
    case 0x08000000:
        return 1;
    default:
        return 0;
    }
}

u8 PedalsConnected__6WheelsFl(Wheels *wheels, s32 chan) {
    return (wheels->status[chan].flags >> 3) & 1;
}

void GenerateNonLinValues__6WheelsFlUc(Wheels *wheels, s32 chan, u8 coefficient) {
    f32 CalculateNonLinValue__6WheelsFlUcss(Wheels *wheels, s32 input, u8 coefficient, s16 min, s16 max);
    s32 i;

    for (i = 0; i < 256; i++) {
        wheels->nonLinValues[chan][i] = CalculateNonLinValue__6WheelsFlUcss(wheels, i, coefficient, -128, 127);
    }
}

f32 CalculateNonLinValue__6WheelsFlUcss(Wheels *wheels, s32 input, u8 coefficient, s16 min, s16 max) {
    f32 x;
    f32 k;
    s32 half;
    s32 mid;

    k = (f32)coefficient / 100.0f;
    x = ((f32)input - 127.5f) / 127.5f;
    half = (max - min) / 2;
    mid = (max + min) / 2;

    return (f32)half * ((1.0f - k) * x + k * (x * (x * x))) + (f32)mid;
}

s32 __ct__9ConditionFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

f32 fn_800A08D4(void) {
    f64 fn_8009FAF8();
    return (f32) fn_8009FAF8();
}

void fn_800A2450(void) {
    extern s32 lbl_803C7620;
    extern s32 lbl_803C7624;
    lbl_803C7620 = 1;
    lbl_803C7624 = 1;
}

void fn_800A24A4(void) {

}

void fn_800A2588(void) {

}

void fn_800A2864(void) {
    M2C_UNK memcpy();
    memcpy();
}

void fn_800A28AC(void) {

}

s8 fn_800A2F84(void) {
    extern u32 lbl_803C6EC0;
    return (s8) ((u32) lbl_803C6EC0 >> 0x1FU);
}

void fn_800A3184(void) {

}

void fn_800A3188(void) {

}

void fn_800A324C(void) {

}

void fn_800A3C74(void) {
    M2C_UNK fn_800A3C98();
    fn_800A3C98();
}

void fn_800A3C94(void) {

}

void fn_800A4C1C(void) {
    M2C_UNK fn_800A3A44();
    fn_800A3A44();
}

void fn_800A4C3C(void) {
    M2C_UNK fn_800A3A98();
    fn_800A3A98();
}

void fn_800A5BEC(void) {
    M2C_UNK fn_800B1E04();
    fn_800B1E04();
}

void fn_800A7CDC(void) {
    M2C_UNK fn_800AA91C();
    fn_800AA91C();
}

s32 fn_800A7FC4(void) {
    extern s32 lbl_803C767C;
    return lbl_803C767C;
}

void fn_800A830C(void) {
    M2C_UNK fn_800AC00C();
    fn_800AC00C();
}

void fn_800A9A84(void) {

}

void fn_800A9B50(void) {

}

s32 fn_800A9B54(void) {
    return 0;
}

void fn_800AC00C(void) {

}

void fn_800AC30C(void) {

}

void fn_800AC310(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0x280;
    *arg3 = 0x1E0;
}

f32 fn_800AF400(void) {
    void *fn_800A7FCC();
    return M2C_FIELD(fn_800A7FCC(), f32 *, 0x270);
}

void fn_800B1F50(s8 arg0, s8 arg1, s8 arg2, s8 arg3) {
    extern M2C_UNK lbl_803C7720;
    M2C_FIELD(&lbl_803C7720, s8 *, 0) = arg0;
    M2C_FIELD(&lbl_803C7720, s8 *, 1) = arg1;
    M2C_FIELD(&lbl_803C7720, s8 *, 2) = arg2;
    M2C_FIELD(&lbl_803C7720, s8 *, 3) = arg3;
}

void fn_800B2C58(void) {
    M2C_UNK fn_800B2C78();
    fn_800B2C78();
}

void fn_800B302C(void) {
    M2C_UNK fn_800ABB90();
    fn_800ABB90();
}

void fn_800B304C(void) {

}

void fn_800B31FC(void) {
    M2C_UNK LCDisable();
    extern s32 lbl_803C7758;
    LCDisable();
    lbl_803C7758 = 0;
}

void fn_800B4948(void) {
    s32 fn_800B46D8();
    M2C_UNK fn_800B47D4(s32);
    M2C_UNK fn_800B4A18();
    s32 temp_r31;

loop_1:
    temp_r31 = fn_800B46D8();
    fn_800B4A18();
    fn_800B47D4(temp_r31);
    goto loop_1;
}

void fn_800BDD14(void) {
    M2C_UNK fn_800C293C(M2C_UNK);
    fn_800C293C(0xD05);
}

void fn_800BE358(void) {
    extern s32 lbl_803C78B0;
    lbl_803C78B0 = 0;
}

void fn_800BE510(void) {
    M2C_UNK fn_800BC868();
    extern s32 lbl_803C7940;
    lbl_803C7940 = 0;
    fn_800BC868();
}

void fn_800BE538(s8 arg0) {
    extern s8 lbl_803C78CC;
    lbl_803C78CC = arg0;
}

void fn_800BE568(void) {
    M2C_UNK fn_800BCEA0();
    fn_800BCEA0();
}

void fn_800BED54(void) {

}

void fn_800C08E0(void) {

}

void fn_800C1020(void) {

}

void fn_800C1108(void) {

}

s32 fn_800C267C(void) {
    return 0;
}

void fn_800C6B00(void) {
    M2C_UNK fn_800C6B20();
    fn_800C6B20();
}

void fn_800C8810(void) {

}

void fn_800C8814(void) {

}

s32 fn_800C8818(void) {
    return 0xF;
}

void fn_800C8B50(void) {

}

void fn_800CA8C8(void) {
    M2C_UNK fn_800CA1D0();
    fn_800CA1D0();
}

void fn_800CAE68(void) {
    M2C_UNK fn_800C2FEC();
    fn_800C2FEC();
}

void fn_800CB9CC(s32 arg0) {
    M2C_UNK fn_800CBEF4(s32);
    fn_800CBEF4(arg0 + 0x82C);
}

void fn_800CB9F0(s32 arg0) {
    M2C_UNK fn_800CBF10(s32);
    fn_800CBF10(arg0 + 0x82C);
}

void fn_800CBA14(s32 arg0) {
    M2C_UNK fn_800CBF78(s32);
    fn_800CBF78(arg0 + 0x82C);
}

