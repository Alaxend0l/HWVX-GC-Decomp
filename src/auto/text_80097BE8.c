/* Functions from 0x80097BE8 to 0x800CC320, found by decomp-toolkit. */
#include "common.h"

#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)
static inline double fabs(double x) { return __fabs(x); }

void *GetRenderWindow();
void Gfx_SetWorldMatrix(f32 *matrix);
void Gfx_SetProjectionMatrix(f32 fov);
void MultiplyMatrix4x4(void *arg0, void *arg1, void *arg2);
void MatrixToMtxTranspose(void *arg0, void *arg1);
s32 OSSendMessage(void *queue, void *msg, s32 flags);
s32 OSReceiveMessage(void *queue, void **msg, s32 flags);
u32 MemCpyA_(u32 dst, u32 src, u32 len);
BOOL DBGEXIImm(void *buffer, s32 bytecounter, u32 write);

volatile u32 __EXIRegs[16] : 0xCC006800;

static inline void DBGEXISelect(u32 v) {
    u32 regs = __EXIRegs[10];
    regs &= 0x405;
    regs |= 0x80 | (v << 4);
    __EXIRegs[10] = regs;
}

static inline BOOL DBGEXIDeselect(void) {
    __EXIRegs[10] &= 0x405;
    return TRUE;
}

static inline BOOL DBGEXISync(void) {
    while (__EXIRegs[13] & 1) {
    }
    return TRUE;
}

static inline BOOL DBGReadStatus_inline(u32 *status) {
    BOOL err;
    u32 cmd;

    DBGEXISelect(4);
    cmd = 0x40000000;
    err = !DBGEXIImm(&cmd, 2, 1);
    err |= !DBGEXISync();
    err |= !DBGEXIImm(status, 4, 0);
    err |= !DBGEXISync();
    err |= !DBGEXIDeselect();
    return !err;
}

static inline BOOL DBGWriteMailbox(u32 mailbox) {
    BOOL err;
    u32 cmd;

    DBGEXISelect(4);
    cmd = (mailbox & 0x1FFFFFFF) | 0xC0000000;
    err = !DBGEXIImm(&cmd, 4, 1);
    err |= !DBGEXISync();
    err |= !DBGEXIDeselect();
    return !err;
}

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

static inline void GXPosition3f32(f32 x, f32 y, f32 z) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

static inline void GXColor3u8(const u8 r, const u8 g, const u8 b) {
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
}

static inline void GXColor4u8(const u8 r, const u8 g, const u8 b, const u8 a) {
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
    GXWGFifo.u8 = a;
}

static inline void GXTexCoord2f32(f32 s, f32 t) {
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}

static inline void GXPosition3s16(s16 x, s16 y, s16 z) {
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = z;
}

static inline void GXTexCoord2s16(const s16 s, const s16 t) {
    GXWGFifo.s16 = s;
    GXWGFifo.s16 = t;
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

s32 __count_trailing_zerol(u32 x);

static inline int __fpclassifyd(double x) {
    switch ((*(int *)&x) & 0x7FF00000) {
    case 0x7FF00000:
        if (((*(int *)&x) & 0x000FFFFF) || (*((int *)&x + 1) & 0xFFFFFFFF)) {
            return 1;
        } else {
            return 2;
        }
    case 0:
        if (((*(int *)&x) & 0x000FFFFF) || (*((int *)&x + 1) & 0xFFFFFFFF)) {
            return 5;
        } else {
            return 3;
        }
    }
    return 4;
}

static inline int __count_trailing_zero(double x) {
    u32 *l = (u32 *)&x;

    if (l[1] != 0) {
        return __count_trailing_zerol(l[1]);
    }
    return 32 + __count_trailing_zerol(l[0] | 0x00100000);
}

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

typedef struct decform {
    char style;
    char unk1;
    s16 digits;
} decform;

static inline int __must_round(const decimal *d, int digits) {
    const u8 *i = d->sig.text + digits;

    if (*i > 5) {
        return 1;
    }
    if (*i < 5) {
        return -1;
    }
    {
        const u8 *e = d->sig.text + d->sig.length;

        for (i++; i < e; i++) {
            if (*i != 0) {
                return 1;
            }
        }
    }
    if (d->sig.text[digits - 1] & 1) {
        return 1;
    }
    return -1;
}

static inline void __rounddec(decimal *d, int digits) {
    if (digits > 0 && digits < d->sig.length) {
        int unkBool = __must_round(d, digits);

        d->sig.length = digits;
        if (unkBool >= 0) {
            __dorounddecup(d, digits);
        }
    }
}

extern u8 CardWorkArea[0xA000];
extern CARDFileInfo memCardfileInfo[2];
extern s32 memCardBlockSize[2];
extern s32 m_memcardstatus_803C7970[2];

s32 MemCard_Write__FUllPv(u32 id, s32 size, void *data);

typedef struct FileEntry {
    s32 id;
    u8 unk4[0xC - 0x4];
    s32 size;
    u8 unk10[0x14 - 0x10];
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
    f32 frameRate;
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
    s32 videoDecodeCount;
    u8 unkD4[0xE4 - 0xD4];
    struct {
        u8 *ptr;
        s32 frameNumber;
    } readBuffer[10];
    struct {
        u8 *ytexture;
        u8 *utexture;
        u8 *vtexture;
        s32 frameNumber;
    } textureSet[3];
    struct {
        s16 *buffer;
        s16 *curPtr;
        u32 validSample;
        s32 frameNumber;
    } audioBuffer[3];
    u8 unk194[0x198 - 0x194];
} THPPlayer;

typedef struct THPComponent {
    u8 quantizationTableSelector;
    u8 DCTableSelector;
    u8 ACTableSelector;
    s16 predDC;
} THPComponent;

typedef struct THPHuffmanTab {
    u8 quick[32];
    u8 increment[32];
    u8 *Vij;
    s32 maxCode[18];
    s32 valPtr[18];
    u8 Vij1;
    u8 unkD5[0xE0 - 0xD5];
} THPHuffmanTab;

typedef struct THPFileInfo {
    u8 unk0[0x300];
    THPHuffmanTab huffmanTabs[4];
    THPComponent components[3];
    u16 xPixelSize;
    u16 yPixelSize;
    u16 MCUsPerRow;
    u16 decompressedY;
    u8 unk69A[0x69C - 0x69A];
    u8 *c;
    u32 currByte;
    u32 cnt;
    u8 validHuffmanTabs;
    u8 RST;
    u16 nMCU;
    u16 currMCU;
    u8 unk6AE[0x6BC - 0x6AE];
} THPFileInfo;

extern THPFileInfo *__THPInfo_803C7890;
extern u8 *__THPHuffmanBits_803C7824;
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

u8 AudioDecodeThread_80370108[0x310];
u8 AudioDecodeThreadStack[0x1000];
u8 FreeAudioBufferQueue_80371418[0x20];
void *FreeAudioBufferMessage[3];
u8 DecodedAudioBufferQueue_80371444[0x20];
void *DecodedAudioBufferMessage[3];
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

typedef struct Block {
    struct Block *prev_;
    struct Block *next_;
    u32 max_size_;
    u32 size_;
} Block;

SubBlock *Block_subBlock(Block *ths, u32 size);
void Block_link(Block *ths, SubBlock *sb);

static inline void SubBlock_split(SubBlock *ths, u32 sz) {
    SubBlock *np = (SubBlock *)((u8 *)ths + sz);
    s32 isfree = !(ths->size_ & 2);
    s32 isalloc = !isfree;
    s32 isprevalloc = ths->size_ & 4;
    void *bp = (void *)(((u32)ths->bp_ & ~1) | 1);
    u32 origsize = ths->size_ & 0xFFFFFFF8;

    ths->bp_ = bp;
    ths->size_ = sz;
    if (isprevalloc) {
        ths->size_ |= 4;
    }
    if (isalloc) {
        ths->size_ |= 2;
        np->size_ |= 4;
    } else {
        *(u32 *)((u8 *)np - sizeof(u32)) = sz;
    }
    np->bp_ = bp;
    np->size_ = origsize - sz;
    if (isalloc) {
        np->size_ |= 4;
    }
    if (isalloc) {
        np->size_ |= 2;
        *(u32 *)((u8 *)np + (origsize - sz)) |= 4;
    } else {
        *(u32 *)((u8 *)np + (origsize - sz) - sizeof(u32)) = origsize - sz;
    }
    if (isfree) {
        np->next_ = ths->next_;
        np->next_->prev_ = np;
        np->prev_ = ths;
        ths->next_ = np;
    }
}

typedef struct FixSubBlock {
    struct FixBlock *block_;
    struct FixSubBlock *next_;
} FixSubBlock;

typedef struct FixBlock {
    struct FixBlock *prev_;
    struct FixBlock *next_;
    u32 client_size_;
    FixSubBlock *start_;
    u32 n_allocated_;
} FixBlock;

extern const u32 fix_pool_sizes_800CD960[6];

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

typedef struct InputUnk108 {
    u8 unk0[0xA];
    u8 unkA;
    u8 unkB;
} InputUnk108;

static RumbleStatus rumbleStatus[4];
static s32 s_rumbleTime[4];
static u8 s_unk30[0x40];
static u8 s_unk70[0x98];
static InputUnk108 s_unk108[4];
extern s8 g_padUpPos[4];
extern s8 g_padDownPos[4];
extern u32 g_StopInput;

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
void PlayFrontalCollisionForce__8LGWheelsFlUc(struct LGWheels *wheels, s32 chan, u8 force);
void PlayDamperForce__8LGWheelsFlUcs(void *wheels, s32 chan, u8 coefficient, s16 offset);
void GenerateNonLinValues__8LGWheelsFlUc(void *wheels, s32 chan, u8 coefficient);
typedef struct LGWheels {
    u8 unk0[0x828];
    u8 airborne[4];
    u8 wheels[0x1054 - 0x82C];
    u32 overallGainId[4];
    u8 unk1064[0x10AC - 0x1064];
    u8 force[0x114C - 0x10AC];
    Force condition;
    Force constant;
    Force periodic;
    Force ramp;
    u8 savedGain;
    u8 unk13CD[0x140E - 0x13CD];
    s16 frontalCollisionParams[4];
} LGWheels;

u8 IsOpened__6WheelsFl(Wheels *wheels, s32 chan);
u8 SameDamperForceParams__8LGWheelsFlUcs(void *wheels, s32 chan, u8 coefficient, s16 offset);
s32 DownloadForce__9ConditionFllRUlUcUlUlScUcUcUcss(Force *force, s32 chan, s32 effect, u32 *device, u8 type, u32 duration, u32 startDelay, s8 offset, u8 deadband, u8 saturationPos, u8 saturationNeg, s16 coefficientPos, s16 coefficientNeg);
s32 UpdateForce__9ConditionFllUcUlUlScUcUcUcss(Force *force, s32 chan, s32 effect, u8 type, u32 duration, u32 startDelay, s8 offset, u8 deadband, u8 posSaturation, u8 negSaturation, s16 posCoefficient, s16 negCoefficient);
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

typedef struct GBASecParam {
    u8 readbuf[4];
    s32 paletteColor;
    s32 paletteSpeed;
    s32 length;
    u32 *out;
    u8 _padding0[12];
    u32 keyA;
    s32 keyB;
    u8 _padding1[24];
} GBASecParam;

typedef struct GBADSPTaskInfo {
    u32 state;
    u32 priority;
    u32 flags;
    void *iram_mmem_addr;
    u32 iram_length;
    u32 iram_addr;
    void *dram_mmem_addr;
    u32 dram_length;
    u32 dram_addr;
    u16 dsp_init_vector;
    u16 dsp_resume_vector;
    void *init_cb;
    void *res_cb;
    void *done_cb;
    void *req_cb;
    u8 unk38[0x50 - 0x38];
} GBADSPTaskInfo;

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
    GBADSPTaskInfo task;
    GBASecParam *param;
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
static void F23(s32 chan, s32 ret);
static void F25(s32 chan, s32 ret);
extern u8 D35_800DB028[0x380];
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

typedef struct EffectEnvelope {
    s32 attackTime;
    s32 fadeTime;
    u8 attackLevel;
    u8 fadeLevel;
} EffectEnvelope;

static inline s32 Effect_MulDiv(s32 a, s32 b, s32 c) {
    s32 half = c / 2;

    if (a < 0) {
        half = -half;
    }
    a *= b;
    a += half;
    return a / c;
}

static inline s32 Effect_WaveLookup(s16 *table, u32 a) {
    if (a < 0x40) {
        return table[a];
    } else if (a < 0x80) {
        return table[0x7F - a];
    } else if (a < 0xC0) {
        return -table[a - 0x80];
    } else {
        return -table[0xFF - a];
    }
}

typedef struct EffectConstant {
    s16 magnitude;
    u16 angle;
    EffectEnvelope envelope;
} EffectConstant;

typedef struct EffectRamp {
    s16 start;
    s16 end;
    u16 angle;
} EffectRamp;

typedef struct EffectPeriodic {
    u8 magnitude;
    u8 unk1;
    u16 angle;
    u16 period;
    u16 phase;
    s16 offset;
    u8 unkA[2];
    EffectEnvelope envelope;
} EffectPeriodic;

typedef struct Effect {
    u8 type;
    u8 unk1[3];
    u32 duration;
    u32 startDelay;
    union {
        EffectConstant constant;
        EffectRamp ramp;
        EffectPeriodic periodic;
        EffectCondition condition;
        u8 raw[0x18];
    } params;
    EffectState *state;
    u32 flags;
    u32 elapsed;
    u32 delay;
    s32 force;
    union {
        s32 deadbandLow;
        u32 phase;
    };
    s32 deadbandHigh;
} Effect;


typedef struct SimThread {
    struct VDevice *owner;
    u32 lastTick;
    s32 pos;
    s32 posScaled;
    s32 weights[10];
    s32 history[10];
    s32 historyIdx;
    s32 accel;
    s32 deltaPos[16];
    s32 deltaTime[16];
    s32 deltaIdx;
    s32 lastForce;
} SimThread;

typedef struct VDevice {
    SimThread sim;
    s32 unkF0;
    Effect *activeEffects[64];
    Effect effects[64];
    s32 unk11F4;
} VDevice;

typedef struct SISteeringStatus {
    u16 button;
    u8 misc;
    s8 steering;
    u8 gas;
    u8 brake;
    u8 left;
    u8 right;
    s8 err;
    u8 unk9;
} SISteeringStatus;

typedef struct LGDevice {
    u8 unk0[4];
    u8 opened : 1;
    u8 started : 1;
    u8 unk4_pad : 6;
    s32 unk8;
    SISteeringStatus status;
    u8 unk16[2];
    SISteeringStatus calibMin;
    SISteeringStatus calibMax;
    SISteeringStatus calibDeadzone;
    SISteeringStatus prev;
    u8 unk40[4];
    u16 curButton;
    u8 curMisc;
    s8 curSteering;
    u8 unk48[5];
    s8 unk4D;
    u8 unk4E[2];
    VDevice vdev;
} LGDevice;

extern LGDevice g_lgDevices_803771D8[4];
extern s32 g_bGammaInitialized_803C794C;

void SimThread_Init(VDevice *dev, VDevice *owner);

static inline void SimThread_SetWeights(SimThread *t, const s32 *weights) {
    s32 i;

    for (i = 0; i < 10; i++) {
        t->weights[i] = weights[i];
        t->history[i] = 0;
    }
    t->historyIdx = 0;
}

void VDevice_RecalcGammaTable(void);
extern s8 g_iGammaLookup_8037BAF8[129];
s32 VDevice_Shutdown(VDevice *dev);
void VDevice_Init(VDevice *dev);
s32 VDevice_StartEffect(VDevice *dev, u32 handle);
u32 VDevice_GetFreeEffect(VDevice *dev);
void Effect_Init(Effect *effect);
s32 Effect_UpdateEffect(Effect *effect, u8 *desc);

extern s16 g_iQuarterSineTable_8037BB7C[0x40];
extern s16 g_iRampUpTable_8037BBFC[0x40];
extern s32 g_bWaveTablesInitialized_803C7958;

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

typedef struct GDLLODEntry {
    s32 geometry;
    s32 matrix;
} GDLLODEntry;

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

typedef struct GDLRectVtx {
    f32 x;
    f32 y;
    f32 z;
    u8 unkC[0x10 - 0xC];
    f32 s;
    f32 t;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u8 unk1C[0x20 - 0x1C];
} GDLRectVtx;

typedef struct GDLRect {
    GDLRectVtx v[4];
    u16 tex;
    u8 blendMode;
    u8 window;
} GDLRect;

typedef struct GDLVec4 {
    f32 x, y, z, w;
} GDLVec4;

typedef struct GDLLine {
    GDLVec4 p0;
    GDLVec4 p1;
    GXColor c0;
    GXColor c1;
    u8 blendMode;
    u8 pad29[0x30 - 0x29];
} GDLLine;

typedef struct GDL3DLine {
    GDLVec4 p0;
    GDLVec4 p1;
    u8 r;
    u8 g;
    u8 b;
    u8 pad23[0x30 - 0x23];
} GDL3DLine;

typedef struct GDL2DFXLine {
    GDLVec4 p0;
    GDLVec4 p1;
    GXColor c0;
    GXColor c1;
    u8 unk28;
    u8 pad29[0x30 - 0x29];
} GDL2DFXLine;

typedef struct GDLGroupTri {
    u8 type;
    u8 unk1;
    u8 unk2;
    u8 pad3;
    u16 tex;
    u16 unk6;
    s32 unk8;
    u32 count;
    u32 unk10;
    u8 unk14[0x18 - 0x14];
    f32 unk18;
    f32 unk1C;
} GDLGroupTri;

typedef struct GDLLightState {
    GDLVec4 pointLights[3];
    Vector lightDir;
    u32 numPointLights;
    GXColor pointColors[3];
    GXColor lightColor;
    GXColor ambientColor;
    u8 unk58[0x60 - 0x58];
} GDLLightState;

typedef struct GDLAlphaGroup {
    f32 matrix[16];
    s32 unk40;
    s32 unk44;
    u8 unk48[0x4C - 0x48];
    GXColor color;
    u8 unk50;
    u8 window;
    u8 unk52[0x54 - 0x52];
} GDLAlphaGroup;

typedef struct GDL2DSprite {
    f32 x;
    f32 y;
    f32 z;
    f32 unkC;
    u8 unk10[0x1C - 0x10];
    f32 unk1C;
    u8 unk20[0x24 - 0x20];
    GXColor color;
    u16 tex;
    u8 flag;
    u8 window;
    u8 unk2C[0x30 - 0x2C];
} GDL2DSprite;

typedef struct GDLEnvmapState {
    f32 unk0;
    f32 unk4;
    u16 unk8;
    u16 unkA;
} GDLEnvmapState;

typedef struct GDLSphere {
    f32 x;
    f32 y;
    f32 z;
    u8 unkC[0x14 - 0xC];
    f32 radius;
    u8 unk18[0x20 - 0x18];
} GDLSphere;

typedef struct GDLPointSprite {
    f32 x;
    f32 y;
    f32 z;
    f32 size;
    u32 color;
    u16 tex;
    u8 blendMode;
    u8 pad17;
} GDLPointSprite;

typedef struct GDLMatrix {
    f32 m[16];
} GDLMatrix __attribute__((aligned(16)));

typedef struct GDLTriVtx {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
    f32 s;
    f32 t;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u8 unk1C[0x20 - 0x1C];
} GDLTriVtx;

typedef struct GDLTri {
    GDLTriVtx v[3];
    u16 tex;
    s16 blendMode;
} GDLTri;

typedef struct GDL {
    u8 unk0[0x4];
    u32 num2DSprites;
    u32 numSprites;
    u32 numAlphaGroups;
    u8 unk10[0x14 - 0x10];
    s32 numTris;
    u32 numRects2D;
    u8 unk1C[0x20 - 0x1C];
    u32 numRects;
    u32 num3DLines;
    u8 unk28[0x2C - 0x28];
    u32 numSpheres;
    u8 unk30[0x34 - 0x30];
    u32 numLines;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 numGroupTris;
    u8 unk48[0x4C - 0x48];
    u32 numLightStates;
    u32 numCachedMatrices;
    s32 numDrawEntries;
    u32 numLODEntries;
    u8 unk5C[0x68 - 0x5C];
    u32 numPointSprites;
    s32 numEnvmapStates;
    u32 num2DFXLines;
    u32 numMatrices;
    u8 unk78[0x80 - 0x78];
    u16 alphaTable[256];
    GDLAlphaEntry alphaEntries[(0x9EC0 - 0x280) / 8];
    GDL2DSprite sprites2D[2000];
    GDLSprite sprites[0x7CF];
    u8 unk38C90[0x38CC0 - 0x38C90];
    GDLTri tris[2000];
    GDLAlphaGroup alphaGroups[0x400];
    GDLRect rects2D[0x400];
    GDLRect rects[0x20];
    u8 unkA0A80[0xA2B80 - 0xA0A80];
    GDL3DLine lines3D[2000];
    GDL2DFXLine fxLines2D[200];
    GDLSphere spheres[(0xC0040 - 0xBC800) / 0x20];
    GDLLine lines[0x100];
    u8 unkC3040[0xC6EC0 - 0xC3040];
    GDLLightState lightStates[0x80];
    f32 cachedMatrices[0x578][16];
    GDLDrawEntry drawEntries[(0xE9CC0 - 0xDFCC0) / 0x14];
    GDLLODEntry lodEntries[(0xEDCC0 - 0xE9CC0) / 8];
    GDLPointSprite pointSprites[1000];
    GDLEnvmapState envmapStates[0x20];
    u32 matrices[0x578];
    u8 unkF51E0[0x24CDE0 - 0xF51E0];
    void *unk24CDE0;
    GDLGroupTri *groupTris;
    void *unk24CDE8;
    void *unk24CDEC;
    void *unk24CDF0;
    u16 spriteHeads[0x1FF];
    u8 unk24D1F2;
} GDL;

typedef struct GeometryGroup {
    u16 unk0;
    u16 flags;
    u8 unk4[0x20 - 0x4];
} GeometryGroup;

typedef struct Geometry {
    u16 unk0;
    u16 numGroups;
    u16 unk4;
    u16 unk6;
    GeometryGroup *groups;
    f32 *lodDistances;
    f32 bboxMin[4];
    f32 bboxMax[4];
} Geometry;

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
    u16 wrapMode;
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
    f32 height;
    u8 unk4[0xC - 0x4];
    u16 texture;
    u8 caseMode;
    u8 unkF;
    s8 inUse;
    u8 unk11;
    u16 refCount;
    FontChar chars[128];
} Font;

typedef struct FontManager {
    u32 numFonts;
    Font fonts[16];
} FontManager;

typedef struct DLSkinGroup {
    Geometry *lod;
    s32 lightState;
    f32 *matrices;
    u32 count;
    u16 matrix;
    u8 group;
    u8 window;
} DLSkinGroup;

typedef struct print_format {
    u8 justification_options;
    u8 sign_options;
    u8 precision_specified;
    u8 alternate_form;
    u8 argument_options;
    u8 conversion_char;
    s32 field_width;
    s32 precision;
} print_format;

typedef struct DLGroup {
    Geometry *geometry;
    s32 lightState;
    u16 matrix;
    u16 unkA;
    u8 group;
    u8 unkD;
    u8 unkE;
    u8 window;
} DLGroup;

typedef struct DisplayList {
    u16 numOpaqueGroups;
    u16 numLitGroups;
    u16 unk4;
    u16 numAlphaGroups;
    u8 unk8[0x10 - 0x8];
    u16 numSkinGroups;
    u8 unk12[0x20 - 0x12];
    DLGroup opaqueGroups[0x9C4];
    DLGroup litGroups[0x358];
    DLGroup alphaGroups[0x380];
    DLSkinGroup skinGroups[0x100];
    u16 unk11DE0[0x1FF];
    u16 unk121DE[0x1FF];
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
    s32 unk84;
    s32 unk88;
    u16 debugFont;
    s8 lightChanged;
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

#pragma push
#pragma auto_inline off
void __sys_free(void *ptr) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    OSFreeToHeap(__OSCurrHeap, (s32)ptr);
}
#pragma pop

s32 OSAllocFromHeap(s32 heap, s32 ptr);
#pragma push
#pragma auto_inline off
void *__sys_alloc(u32 size) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    return (void *)OSAllocFromHeap(__OSCurrHeap, size);
}
#pragma pop

#pragma push
#pragma auto_inline off
void exit(s32 status) {
    extern void (*_dtors[])(void);
    extern void (*__stdio_exit)(void);
    void __destroy_global_chain(void);
    void (**dtor)(void);

    if (!__aborting) {
        __destroy_global_chain();
        dtor = _dtors;
        while (*dtor != NULL) {
            (*dtor)();
            dtor++;
        }
        if (__stdio_exit) {
            (*__stdio_exit)();
            __stdio_exit = 0;
        }
    }
    while (__atexit_curr_func_803C75DC > 0) {
        (*__atexit_funcs_8030DC18[--__atexit_curr_func_803C75DC])();
    }
    if (__console_exit) {
        (*__console_exit)();
        __console_exit = 0;
    }
    _ExitProcess();
}
#pragma pop

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

#pragma push
#pragma auto_inline off
extern u8 protopool_8030DD18[0x34];
extern u8 init_803C75E8;

static inline void *get_malloc_pool(void) {
    if (!init_803C75E8) {
        memset(protopool_8030DD18, 0, sizeof(protopool_8030DD18));
        init_803C75E8 = 1;
    }
    return protopool_8030DD18;
}

void free(void *ptr) {
    void deallocate_from_fixed_pools(void *pool, void *ptr, u32 size);
    void Block_link(Block *ths, SubBlock *sb);
    void *pool;
    u32 size;
    Block *bp;
    Block *n;
    SubBlock *sb;
    s32 empty;

    pool = get_malloc_pool();
    if (ptr == NULL) {
        return;
    }
    size = !(*(u32 *)((u8 *)ptr - 4) & 1) ? (*(FixBlock **)((u8 *)ptr - 4))->client_size_
                                          : (*(u32 *)((u8 *)ptr - 8) & ~7) - 8;
    if (size <= 0x44) {
        deallocate_from_fixed_pools(pool, ptr, size);
        return;
    }
    sb = (SubBlock *)((u8 *)ptr - 8);
    bp = (Block *)((u32)sb->bp_ & ~1);
    Block_link(bp, sb);
    sb = (SubBlock *)((u8 *)bp + 16);
    empty = 0;
    if (!(sb->size_ & 2) && (sb->size_ & ~7) == (bp->size_ & ~7) - 24) {
        empty = 1;
    }
    if (empty) {
        n = bp->next_;
        if (n == bp) {
            n = NULL;
        }
        if (*(Block **)pool == bp) {
            *(Block **)pool = n;
        }
        if (n != NULL) {
            n->prev_ = bp->prev_;
            n->prev_->next_ = n;
        }
        bp->prev_ = bp->next_ = NULL;
        __sys_free(bp);
    }
}
#pragma pop
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

void deallocate_from_fixed_pools(void *pool, void *ptr, u32 size) {
    typedef struct FixStart {
        FixBlock *tail_;
        FixBlock *head_;
    } FixStart;
    typedef struct mem_pool_obj {
        Block *start_;
        FixStart fix_start[6];
    } mem_pool_obj;
    u32 i = 0;
    FixSubBlock *p;
    FixBlock *b;
    FixStart *fs;

    while (size > fix_pool_sizes_800CD960[i]) {
        ++i;
    }
    fs = &((mem_pool_obj *)pool)->fix_start[i];
    p = (FixSubBlock *)((u8 *)ptr - 4);
    b = p->block_;
    if (b->start_ == NULL && fs->head_ != b) {
        if (fs->tail_ == b) {
            fs->head_ = fs->head_->prev_;
            fs->tail_ = fs->tail_->prev_;
        } else {
            b->prev_->next_ = b->next_;
            b->next_->prev_ = b->prev_;
            b->next_ = fs->head_;
            b->prev_ = b->next_->prev_;
            b->prev_->next_ = b;
            b->next_->prev_ = b;
            fs->head_ = b;
        }
    }
    p->next_ = b->start_;
    b->start_ = p;
    if (--b->n_allocated_ == 0) {
        Block *bp;
        Block *n;
        SubBlock *sb;
        s32 empty;

        if (fs->head_ == b) {
            fs->head_ = b->next_;
        }
        if (fs->tail_ == b) {
            fs->tail_ = b->prev_;
        }
        b->prev_->next_ = b->next_;
        b->next_->prev_ = b->prev_;
        if (fs->head_ == b) {
            fs->head_ = NULL;
        }
        if (fs->tail_ == b) {
            fs->tail_ = NULL;
        }
        sb = (SubBlock *)((u8 *)b - 8);
        bp = (Block *)((u32)sb->bp_ & ~1);
        Block_link(bp, sb);
        sb = (SubBlock *)((u8 *)bp + 16);
        empty = 0;
        if (!(sb->size_ & 2) && (sb->size_ & ~7) == (bp->size_ & ~7) - 24) {
            empty = 1;
        }
        if (empty) {
            n = bp->next_;
            if (n == bp) {
                n = NULL;
            }
            if (((mem_pool_obj *)pool)->start_ == bp) {
                ((mem_pool_obj *)pool)->start_ = n;
            }
            if (n != NULL) {
                n->prev_ = bp->prev_;
                n->prev_->next_ = n;
            }
            bp->prev_ = bp->next_ = NULL;
            __sys_free(bp);
        }
    }
}

void *soft_allocate_from_var_pools(Block **pool_obj, u32 size, u32 *max_size);
void FixBlock_construct(FixBlock *ths, FixBlock *prev, FixBlock *next, s32 index, FixSubBlock *chunk, u32 chunk_size);
#pragma push
#pragma use_lmw_stmw on
void *allocate_from_fixed_pools(void *pool, u32 size) {
    typedef struct FixStart {
        FixBlock *tail_;
        FixBlock *head_;
    } FixStart;
    typedef struct mem_pool_obj {
        Block *start_;
        FixStart fix_start[6];
    } mem_pool_obj;
    void *allocate_from_var_pools(void *pool, u32 size);
    u32 i = 0;
    FixSubBlock *p;
    FixStart *fs;

    while (size > fix_pool_sizes_800CD960[i]) {
        ++i;
    }
    fs = &((mem_pool_obj *)pool)->fix_start[i];
    if (fs->head_ == NULL || fs->head_->start_ == NULL) {
        u32 size_received;
        u32 n;
        void *newblock;
        u32 nsave;
        u32 u_size;

        n = 0xFEC / (fix_pool_sizes_800CD960[i] + sizeof(void *));
        if (n > 0x100) {
            n = 0x100;
        }
        nsave = n;
        while (n >= 10) {
            newblock = soft_allocate_from_var_pools(pool, n * (fix_pool_sizes_800CD960[i] + sizeof(void *)) + sizeof(FixBlock), &size_received);
            if (newblock != NULL) {
                break;
            }
            if (size_received > sizeof(FixBlock)) {
                n = (size_received - sizeof(FixBlock)) / (fix_pool_sizes_800CD960[i] + sizeof(void *));
            } else {
                n = 0;
            }
        }
        if (newblock == NULL && n < nsave) {
            newblock = allocate_from_var_pools(pool, nsave * (fix_pool_sizes_800CD960[i] + sizeof(void *)) + sizeof(FixBlock));
            if (newblock == NULL) {
                return NULL;
            }
        }
        u_size = !(*(u32 *)((u8 *)newblock - 4) & 1) ? (*(FixBlock **)((u8 *)newblock - 4))->client_size_
                                                    : (*(u32 *)((u8 *)newblock - 8) & ~7) - 8;
        if (fs->head_ == NULL) {
            fs->head_ = newblock;
            fs->tail_ = newblock;
        }
        FixBlock_construct(newblock, fs->tail_, fs->head_, i, (FixSubBlock *)((u8 *)newblock + sizeof(FixBlock)), u_size - sizeof(FixBlock));
        fs->head_ = newblock;
    }
    p = fs->head_->start_;
    fs->head_->start_ = p->next_;
    ++fs->head_->n_allocated_;
    if (fs->head_->start_ == NULL) {
        fs->head_ = fs->head_->next_;
        fs->tail_ = fs->tail_->next_;
    }
    return (u8 *)p + 4;
}
#pragma pop

static inline Block *link_new_block(Block **pool_obj, u32 size) {
    Block *bp;
    SubBlock *sb;

    size = (size + 24 + 7) & ~7;
    if (size < 0x10000) {
        size = 0x10000;
    }
    bp = __sys_alloc(size);
    if (bp == NULL) {
        return NULL;
    }
    bp->size_ = size | 3;
    *(u32 *)((u8 *)bp + size - 8) = bp->size_;
    sb = (SubBlock *)((u8 *)bp + 16);
    sb->bp_ = (void *)((u32)bp | 1);
    sb->size_ = size - 24;
    *(u32 *)((u8 *)sb + (size - 24) - 4) = size - 24;
    bp->max_size_ = size - 24;
    *(u32 *)((u8 *)bp + (bp->size_ & ~7) - 4) = 0;
    Block_link(bp, sb);
    if (*pool_obj != NULL) {
        bp->prev_ = (*pool_obj)->prev_;
        bp->prev_->next_ = bp;
        bp->next_ = *pool_obj;
        (*pool_obj)->prev_ = bp;
        *pool_obj = bp;
    } else {
        *pool_obj = bp;
        bp->prev_ = bp;
        bp->next_ = bp;
    }
    return bp;
}

void *allocate_from_var_pools(Block **pool_obj, u32 size) {
    Block *bp;
    void *ptr;

    size = (size + 8 + 7) & ~7;
    if (size < 0x50) {
        size = 0x50;
    }
    bp = *pool_obj ? *pool_obj : link_new_block(pool_obj, size);
    if (bp == NULL) {
        return NULL;
    }
    for (;;) {
        if (size <= bp->max_size_) {
            ptr = Block_subBlock(bp, size);
            if (ptr != NULL) {
                *pool_obj = bp;
                break;
            }
        }
        bp = bp->next_;
        if (bp == *pool_obj) {
            bp = link_new_block(pool_obj, size);
            if (bp == NULL) {
                return NULL;
            }
            ptr = Block_subBlock(bp, size);
            break;
        }
    }
    return (u8 *)ptr + 8;
}

void *soft_allocate_from_var_pools(Block **pool_obj, u32 size, u32 *max_size) {
    Block *bp;
    void *ptr;

    size = (size + 8 + 7) & ~7;
    if (size < 0x50) {
        size = 0x50;
    }
    *max_size = 0;
    bp = *pool_obj;
    if (bp == NULL) {
        return NULL;
    }
    for (;;) {
        if (size <= bp->max_size_) {
            ptr = Block_subBlock(bp, size);
            if (ptr != NULL) {
                *pool_obj = bp;
                break;
            }
        }
        if (bp->max_size_ > 8 && *max_size < bp->max_size_ - 8) {
            *max_size = bp->max_size_ - 8;
        }
        bp = bp->next_;
        if (bp == *pool_obj) {
            return NULL;
        }
    }
    return (u8 *)ptr + 8;
}

void FixBlock_construct(FixBlock *ths, FixBlock *prev, FixBlock *next, s32 index, FixSubBlock *chunk, u32 chunk_size) {
    u32 fixSubBlock_size;
    u32 n;
    u32 i;
    FixSubBlock *np;
    u8 *p;

    ths->prev_ = prev;
    ths->next_ = next;
    prev->next_ = ths;
    next->prev_ = ths;
    ths->client_size_ = fix_pool_sizes_800CD960[index];
    fixSubBlock_size = fix_pool_sizes_800CD960[index] + sizeof(FixSubBlock *);
    n = chunk_size / fixSubBlock_size;
    p = (u8 *)chunk;
    for (i = 0; i < n - 1; i++) {
        np = (FixSubBlock *)p;
        np->block_ = ths;
        np->next_ = (FixSubBlock *)(p += fixSubBlock_size);
    }
    np = (FixSubBlock *)p;
    np->block_ = ths;
    np->next_ = NULL;
    ths->start_ = chunk;
    ths->n_allocated_ = 0;
}

static inline SubBlock *SubBlock_merge_prev(SubBlock *ths, SubBlock **start) {
    u32 prevsz;
    SubBlock *p;

    if (!(ths->size_ & 0x04)) {
        prevsz = *(u32 *)((u8 *)ths - sizeof(u32));
        if (prevsz & 0x02) {
            return ths;
        }
        p = (SubBlock *)((u8 *)ths - prevsz);
        p->size_ &= ~0xFFFFFFF8;
        p->size_ |= (prevsz + (ths->size_ & 0xFFFFFFF8)) & 0xFFFFFFF8;
        if (!(p->size_ & 0x02)) {
            *(u32 *)((u8 *)p + prevsz + (ths->size_ & 0xFFFFFFF8) - sizeof(u32)) = prevsz + (ths->size_ & 0xFFFFFFF8);
        }
        if (*start == ths) {
            *start = (*start)->next_;
        }
        ths->next_->prev_ = ths->prev_;
        ths->next_->prev_->next_ = ths->next_;
        return p;
    }
    return ths;
}

#pragma push
#pragma auto_inline off
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
#pragma pop

static inline void SubBlock_set_free(SubBlock *ths) {
    u32 this_size = ths->size_ & 0xFFFFFFF8;

    ths->size_ &= ~0x02;
    ((SubBlock *)((u8 *)ths + this_size))->size_ &= ~0x04;
    *(u32 *)((u8 *)ths + this_size - sizeof(u32)) = this_size;
}

#pragma push
void Block_link(Block *ths, SubBlock *sb) {
    SubBlock **st;

    SubBlock_set_free(sb);
    st = (SubBlock **)((u8 *)ths + (ths->size_ & 0xFFFFFFF8) - sizeof(SubBlock *));
    if (*st != NULL) {
        sb->prev_ = (*st)->prev_;
        sb->prev_->next_ = sb;
        sb->next_ = *st;
        (*st)->prev_ = sb;
        *st = sb;
        *st = SubBlock_merge_prev(*st, st);
        SubBlock_merge_next(*st, st);
    } else {
        *st = sb;
        sb->prev_ = sb;
        sb->next_ = sb;
    }
    if (ths->max_size_ < ((*st)->size_ & 0xFFFFFFF8)) {
        ths->max_size_ = (*st)->size_ & 0xFFFFFFF8;
    }
}

SubBlock *Block_subBlock(Block *ths, u32 size) {
    SubBlock *st;
    SubBlock *sb;
    u32 sb_size;
    u32 max_found;

    st = *(SubBlock **)((u8 *)ths + (ths->size_ & 0xFFFFFFF8) - sizeof(SubBlock *));
    if (st == NULL) {
        return NULL;
    }
    sb = st;
    sb_size = sb->size_ & 0xFFFFFFF8;
    max_found = sb_size;
    while (sb_size < size) {
        sb = sb->next_;
        sb_size = sb->size_ & 0xFFFFFFF8;
        if (max_found < sb_size) {
            max_found = sb_size;
        }
        if (sb == st) {
            ths->max_size_ = max_found;
            return NULL;
        }
    }
    if (sb_size - size >= 0x50) {
        SubBlock_split(sb, size);
    }
    *(SubBlock **)((u8 *)ths + (ths->size_ & 0xFFFFFFF8) - sizeof(SubBlock *)) = sb->next_;
    sb_size = sb->size_ & 0xFFFFFFF8;
    sb->size_ |= 2;
    *(u32 *)((u8 *)sb + sb_size) |= 4;
    {
        SubBlock **start = (SubBlock **)((u8 *)ths + (ths->size_ & 0xFFFFFFF8) - sizeof(SubBlock *));
        if (*start == sb) {
            *start = sb->next_;
        }
        if (*start == sb) {
            *start = NULL;
            ths->max_size_ = 0;
        } else {
            sb->next_->prev_ = sb->prev_;
            sb->prev_->next_ = sb->next_;
        }
    }
    return sb;
}

#pragma pop
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

void __num2dec_internal(decimal *d, double x);
void __num2dec(const decform *form, double x, decimal *d) {
    short digits = form->digits;
    int i;

    __num2dec_internal(d, x);
    if (d->sig.text[0] > 9) {
        return;
    }
    if (digits > SIGDIGLEN) {
        digits = SIGDIGLEN;
    }
    __rounddec(d, digits);
    while (d->sig.length < digits) {
        d->sig.text[d->sig.length++] = 0;
    }
    d->exp -= d->sig.length - 1;
    for (i = 0; i < d->sig.length; i++) {
        d->sig.text[i] += '0';
    }
}

void __two_exp_800991F0(decimal *result, short exp);
void __timesdec(decimal *result, const decimal *x, const decimal *y);
void __ull2dec(decimal *result, unsigned long long val);
double frexp(double x, int *eptr);
double ldexp(double x, int exp);
double modf(double x, double *iptr);
void __num2dec_internal(decimal *d, double x) {
    char sign = (char)(*(u32 *)&x >> 31);

    if (x == 0.0) {
        d->sign = sign;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = 0;
        return;
    }

    if (!(__fpclassifyd(x) > 2)) {
        d->sign = sign;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = (__fpclassifyd(x) == 1) ? 'N' : 'I';
        return;
    }

    if (sign) {
        x = -x;
    }

    {
        int exp;
        double frac = frexp(x, &exp);
        short num_bits_extract = 53 - __count_trailing_zero(frac);
        decimal int_d;
        decimal pow2_d;
        double integer;

        __two_exp_800991F0(&pow2_d, (short)(exp - num_bits_extract));
        modf(ldexp(frac, num_bits_extract), &integer);
        __ull2dec(&int_d, (unsigned long long)integer);
        __timesdec(d, &int_d, &pow2_d);
        d->sign = sign;
    }
}

static void __str2dec(decimal *d, const char *s, short exp) {
    int i;

    d->exp = exp;
    d->sign = 0;

    for (i = 0; i < SIGDIGLEN && *s;) {
        d->sig.text[i++] = *s++ - '0';
    }
    d->sig.length = i;

    if (*s != 0) {
        if (*s < 5) {
            return;
        }
        {
            const char *p = s + 1;
            for (; *p != 0; p++) {
                if (*p != '0') {
                    goto round_up;
                }
            }
            if ((d->sig.text[i - 1] & 1) == 0) {
                return;
            }
        }
    round_up:
        __dorounddecup(d, d->sig.length);
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

void __ull2dec(decimal *result, unsigned long long val) {
    result->sign = 0;

    if (val == 0) {
        result->exp = 0;
        result->sig.length = 1;
        result->sig.text[0] = 0;
        return;
    }

    result->sig.length = 0;

    for (; val != 0; val /= 10) {
        result->sig.text[result->sig.length++] = val % 10;
    }

    {
        u8 *i = result->sig.text;
        u8 *j = result->sig.text + result->sig.length;

        for (; i < --j; ++i) {
            u8 t = *i;
            *i = *j;
            *j = t;
        }
    }

    result->exp = result->sig.length - 1;
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

#pragma push
#pragma auto_inline off
s32 fflush(FILE *file) {
    s32 __flush_all(void);
    s32 __flush_buffer(FILE *file, u32 *bytes_flushed);
    s32 ftell(FILE *file);
    s32 pos;

    if (file == NULL) {
        return __flush_all();
    }
    if (file->state.error || file->mode.file_kind == __closed_file) {
        return -1;
    }
    if (file->mode.io_mode == 1) {
        return 0;
    }
    if (file->state.io_state >= 3) {
        file->state.io_state = 2;
    }
    if (file->state.io_state == 2) {
        file->buffer_len = 0;
    }
    if (file->state.io_state != 1) {
        file->state.io_state = 0;
        return 0;
    }
    if (file->mode.file_kind != __disk_file) {
        pos = 0;
    } else {
        pos = ftell(file);
    }
    if (__flush_buffer(file, NULL)) {
        file->state.error = 1;
        file->buffer_len = 0;
        return -1;
    }
    file->state.io_state = 0;
    file->position = pos;
    file->buffer_len = 0;
    return 0;
}
#pragma pop

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

s32 fseek(FILE *file, u32 offset, s32 file_mode) {
    return _fseek(file, offset, file_mode);
}

int _fseek(FILE *file, u32 offset, int file_mode) {
    extern s32 errno;
    s32 __flush_buffer(FILE *file, u32 *bytes_flushed);
    s32 (*position_proc)(u32, u32 *, s32, void *);

    if (file->mode.file_kind != 1 || file->state.error) {
        errno = 0x28;
        return -1;
    }
    if (file->state.io_state == 1) {
        if (__flush_buffer(file, NULL)) {
            file->state.error = 1;
            file->buffer_len = 0;
            errno = 0x28;
            return -1;
        }
    }
    if (file_mode == 1) {
        s32 pos;
        u32 charsInUndoBuffer;

        file_mode = 0;
        if ((file->mode.file_kind != 1 && file->mode.file_kind != 2) || file->state.error) {
            errno = 0x28;
            pos = -1;
        } else {
            charsInUndoBuffer = file->state.io_state;
            if (charsInUndoBuffer == 0) {
                pos = file->position;
            } else {
                pos = file->buffer_pos + (file->buffer_ptr - file->buffer);
                if (charsInUndoBuffer >= 3) {
                    pos -= charsInUndoBuffer - 2;
                }
            }
        }
        offset += pos;
    }
    if (file_mode != 2 && file->mode.io_mode != 3 &&
        (file->state.io_state == 2 || file->state.io_state == 3)) {
        if (offset >= file->position || offset < file->buffer_pos) {
            file->state.io_state = 0;
        } else {
            file->buffer_ptr = file->buffer + (offset - file->buffer_pos);
            file->buffer_len = file->position - offset;
            file->state.io_state = 2;
        }
    } else {
        file->state.io_state = 0;
    }
    if (file->state.io_state == 0) {
        if ((position_proc = (s32 (*)(u32, u32 *, s32, void *))file->position_fn) != NULL &&
            position_proc(file->handle, &offset, file_mode, file->ref_con)) {
            file->state.error = 1;
            file->buffer_len = 0;
            errno = 0x28;
            return -1;
        }
        file->state.eof = 0;
        file->position = offset;
        file->buffer_len = 0;
    }
    return 0;
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

void __copy_longs_aligned(void *dst, const void *src, unsigned long n);
void __copy_longs_rev_aligned(void *dst, const void *src, unsigned long n);
void __copy_longs_unaligned(void *dst, const void *src, unsigned long n);
void __copy_longs_rev_unaligned(void *dst, const void *src, unsigned long n);
void *memmove(void *dst, const void *src, unsigned long n) {
    const unsigned char *p;
    unsigned char *q;
    int rev = ((unsigned long)src < (unsigned long)dst);

    if (n >= 32) {
        if (((unsigned long)dst ^ (unsigned long)src) & 3) {
            if (!rev) {
                __copy_longs_unaligned(dst, src, n);
            } else {
                __copy_longs_rev_unaligned(dst, src, n);
            }
        } else {
            if (!rev) {
                __copy_longs_aligned(dst, src, n);
            } else {
                __copy_longs_rev_aligned(dst, src, n);
            }
        }
        return dst;
    }
    if (!rev) {
        for (p = (const unsigned char *)src - 1, q = (unsigned char *)dst - 1, n++; --n;) {
            *++q = *++p;
        }
    } else {
        for (p = (const unsigned char *)src + n, q = (unsigned char *)dst + n, n++; --n;) {
            *--q = *--p;
        }
    }
    return dst;
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

#pragma push
#pragma use_lmw_stmw on
int sprintf(char *s, const char *format, ...) {
    typedef struct __va_list_struct {
        char gpr;
        char fpr;
        char reserved[2];
        char *input_arg_area;
        char *reg_save_area;
    } va_list[1];
    int __pformatter(void *(*)(void *, const char *, u32), void *, const char *, char *);
    void *__StringWrite(void *osc, const char *Buffer, u32 NumChars);
    va_list args;
    int end;
    __OutStrCtrl osc;

    __builtin_va_info(&args);
    osc.CharStr = s;
    osc.MaxCharCount = 0xFFFFFFFF;
    osc.CharsWritten = 0;
    end = __pformatter(&__StringWrite, &osc, format, (char *)args);
    if (s) {
        s[(u32)end < 0xFFFFFFFF ? end : 0xFFFFFFFF - 1] = '\0';
    }
    return end;
}
#pragma pop

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

int printf(const char *format, ...) {
    typedef struct __va_list_struct {
        char gpr;
        char fpr;
        char reserved[2];
        char *input_arg_area;
        char *reg_save_area;
    } va_list[1];
    extern u8 __files[];
    extern void *__FileWrite(void *, const char *, u32);
    int fwide(void *, int);
    int __pformatter(void *(*)(void *, const char *, u32), void *, const char *, char *);
    va_list args;
    void *stream = __files + 0x50;

    if (fwide(stream, -1) >= 0) {
        return -1;
    }
    __builtin_va_info(&args);
    return __pformatter(&__FileWrite, stream, format, (char *)args);
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

static void round_decimal(decimal *dec, int new_length) {
    char c;
    char *p;
    int carry;

    if (new_length < 0) {
    return_zero:
        dec->exp = 0;
        dec->sig.length = 1;
        *dec->sig.text = '0';
        return;
    }

    if (new_length >= dec->sig.length) {
        return;
    }

    p = (char *)dec->sig.text + new_length + 1;
    c = *--p - '0';

    if (c == 5) {
        char *q = (char *)dec->sig.text + dec->sig.length;

        while (--q > p && *q == '0') {
        }

        carry = (q == p) ? p[-1] & 1 : 1;
    } else {
        carry = (c > 5);
    }

    while (new_length != 0) {
        c = *--p - '0' + carry;

        if ((carry = (c > 9)) != 0 || c == 0) {
            --new_length;
        } else {
            *p = c + '0';
            break;
        }
    }

    if (carry != 0) {
        dec->exp += 1;
        dec->sig.length = 1;
        *dec->sig.text = '1';
        return;
    } else if (new_length == 0) {
        goto return_zero;
    }

    dec->sig.length = new_length;
}

char *long2str_8009C164(s32 num, char *buff, print_format format) {
    u32 unsigned_num;
    u32 base;
    char *p;
    s32 n;
    s32 digits;
    s32 minus;

    unsigned_num = num;
    minus = 0;
    p = buff;
    *--p = 0;
    digits = 0;

    if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o')) {
        return p;
    }

    switch (format.conversion_char) {
    case 'd':
    case 'i':
        base = 10;
        if (num < 0) {
            unsigned_num = -unsigned_num;
            minus = 1;
        }
        break;
    case 'o':
        base = 8;
        format.sign_options = 0;
        break;
    case 'u':
        base = 10;
        format.sign_options = 0;
        break;
    case 'x':
    case 'X':
        base = 16;
        format.sign_options = 0;
        break;
    }

    do {
        n = unsigned_num % base;
        unsigned_num /= base;
        if (n < 10) {
            n += '0';
        } else {
            n -= 10;
            if (format.conversion_char == 'x') {
                n += 'a';
            } else {
                n += 'A';
            }
        }
        *--p = n;
        ++digits;
    } while (unsigned_num != 0);

    if (base == 8 && format.alternate_form && *p != '0') {
        *--p = '0';
        ++digits;
    }

    if (format.justification_options == 2) {
        format.precision = format.field_width;
        if (minus || format.sign_options != 0) {
            --format.precision;
        }
        if (base == 16 && format.alternate_form) {
            format.precision -= 2;
        }
    }

    if (buff - p + format.precision > 509) {
        return 0;
    }

    while (digits < format.precision) {
        *--p = '0';
        ++digits;
    }

    if (base == 16 && format.alternate_form) {
        *--p = format.conversion_char;
        *--p = '0';
    }

    if (minus) {
        *--p = '-';
    } else if (format.sign_options == 1) {
        *--p = '+';
    } else if (format.sign_options == 2) {
        *--p = ' ';
    }

    return p;
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

int strcmp(const char *str1, const char *str2) {
    register u8 *left = (u8 *)str1;
    register u8 *right = (u8 *)str2;
    u32 align, l1, r1, x;

    l1 = *left;
    r1 = *right;
    if (l1 - r1) {
        return l1 - r1;
    }

    if ((align = ((s32)left & 3)) != ((s32)right & 3)) {
        goto bytecopy;
    }

    if (align) {
        if (l1 == 0) {
            return 0;
        }
        for (align = 3 - align; align; align--) {
            l1 = *(++left);
            r1 = *(++right);
            if (l1 - r1) {
                return l1 - r1;
            }
            if (l1 == 0) {
                return 0;
            }
        }
        left++;
        right++;
    }

    l1 = *(u32 *)left;
    r1 = *(u32 *)right;
    x = l1 + 0xFEFEFEFF;
    if (x & 0x80808080) {
        goto adjust;
    }

    while (l1 == r1) {
        l1 = *(++((u32 *)left));
        r1 = *(++((u32 *)right));
        x = l1 + 0xFEFEFEFF;
        if (x & 0x80808080) {
            goto adjust;
        }
    }

    if (l1 > r1) {
        return 1;
    }
    return -1;

adjust:
    l1 = *left;
    r1 = *right;
    if (l1 - r1) {
        return l1 - r1;
    }

bytecopy:
    if (l1 == 0) {
        return 0;
    }

    do {
        l1 = *(++left);
        r1 = *(++right);
        if (l1 - r1) {
            return l1 - r1;
        }
        if (l1 == 0) {
            return 0;
        }
    } while (1);
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

double atan(double x);
double __ieee754_atan2(double y, double x) {
    double z;
    int k, m, hx, hy, ix, iy;
    unsigned lx, ly;

    hx = __HI(x);
    ix = hx & 0x7fffffff;
    lx = __LO(x);
    hy = __HI(y);
    iy = hy & 0x7fffffff;
    ly = __LO(y);
    if (((unsigned)ix | ((lx | -lx) >> 31)) > 0x7ff00000 || ((unsigned)iy | ((ly | -ly) >> 31)) > 0x7ff00000) {
        return x + y;
    }
    if (((hx - 0x3ff00000) | lx) == 0) {
        return atan(y);
    }
    m = ((hy >> 31) & 1) | ((hx >> 30) & 2);

    if ((iy | ly) == 0) {
        switch (m) {
        case 0:
        case 1:
            return y;
        case 2:
            return 3.1415926535897931160E+00 + 1.0e-300;
        case 3:
            return -3.1415926535897931160E+00 - 1.0e-300;
        }
    }
    if ((ix | lx) == 0) {
        return (hy < 0) ? -1.5707963267948965580E+00 - 1.0e-300 : 1.5707963267948965580E+00 + 1.0e-300;
    }
    if (ix == 0x7ff00000) {
        if (iy == 0x7ff00000) {
            switch (m) {
            case 0:
                return 7.8539816339744827900E-01 + 1.0e-300;
            case 1:
                return -7.8539816339744827900E-01 - 1.0e-300;
            case 2:
                return 3.0 * 7.8539816339744827900E-01 + 1.0e-300;
            case 3:
                return -3.0 * 7.8539816339744827900E-01 - 1.0e-300;
            }
        } else {
            switch (m) {
            case 0:
                return 0.0;
            case 1:
                return -0.0;
            case 2:
                return 3.1415926535897931160E+00 + 1.0e-300;
            case 3:
                return -3.1415926535897931160E+00 - 1.0e-300;
            }
        }
    }
    if (iy == 0x7ff00000) {
        return (hy < 0) ? -1.5707963267948965580E+00 - 1.0e-300 : 1.5707963267948965580E+00 + 1.0e-300;
    }

    k = (iy - ix) >> 20;
    if (k > 60) {
        z = 1.5707963267948965580E+00 + 0.5 * 1.2246467991473531772E-16;
    } else if (hx < 0 && k < -60) {
        z = 0.0;
    } else {
        z = atan(fabs(y / x));
    }
    switch (m) {
    case 0:
        return z;
    case 1:
        __HI(z) ^= 0x80000000;
        return z;
    case 2:
        return 3.1415926535897931160E+00 - (z - 1.2246467991473531772E-16);
    default:
        return (z - 1.2246467991473531772E-16) - 3.1415926535897931160E+00;
    }
}

double __kernel_cos(double x, double y) {
    double a, hz, z, r, qx;
    int ix;
    ix = __HI(x) & 0x7fffffff;
    if (ix < 0x3e400000) {
        if (((int)x) == 0) {
            return 1.00000000000000000000e+00;
        }
    }
    z = x * x;
    r = z * (4.16666666666666019037e-02 + z * (-1.38888888888741095749e-03 + z * (2.48015872894767294178e-05 + z * (-2.75573143513906633035e-07 + z * (2.08757232129817482790e-09 + z * -1.13596475577881948265e-11)))));
    if (ix < 0x3FD33333) {
        return 1.00000000000000000000e+00 - (0.5 * z - (z * r - x * y));
    } else {
        if (ix > 0x3fe90000) {
            qx = 0.28125;
        } else {
            __HI(qx) = ix - 0x00200000;
            __LO(qx) = 0;
        }
        hz = 0.5 * z - qx;
        a = 1.00000000000000000000e+00 - qx;
        return a - (hz - (z * r - x * y));
    }
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

extern const double T_800CDC88[];
double __kernel_tan(double x, double y, int iy) {
    double z, r, v, w, s;
    int ix, hx;

    hx = __HI(x);
    ix = hx & 0x7fffffff;
    if (ix < 0x3e300000) {
        if ((int)x == 0) {
            if (((ix | __LO(x)) | (iy + 1)) == 0) {
                return 1.0 / fabs(x);
            } else {
                return (iy == 1) ? x : -1.0 / x;
            }
        }
    }
    if (ix >= 0x3FE59428) {
        if (hx < 0) {
            x = -x;
            y = -y;
        }
        z = 7.85398163397448278999e-01 - x;
        w = 3.06161699786838301793e-17 - y;
        x = z + w;
        y = 0.0;
    }
    z = x * x;
    w = z * z;
    r = T_800CDC88[1] + w * (T_800CDC88[3] + w * (T_800CDC88[5] + w * (T_800CDC88[7] + w * (T_800CDC88[9] + w * T_800CDC88[11]))));
    v = z * (T_800CDC88[2] + w * (T_800CDC88[4] + w * (T_800CDC88[6] + w * (T_800CDC88[8] + w * (T_800CDC88[10] + w * T_800CDC88[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r += T_800CDC88[0] * s;
    w = x + r;
    if (ix >= 0x3FE59428) {
        v = (double)iy;
        return (double)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }
    if (iy == 1) {
        return w;
    } else {
        double a, t;
        z = w;
        __LO(z) = 0;
        v = r - (z - x);
        t = a = -1.0 / w;
        __LO(t) = 0;
        s = 1.0 + t * z;
        return t + a * (s + t * v);
    }
}

double atan(double x) {
    extern const double atanhi_800CDCF0[];
    extern const double atanlo[];
    extern const double aT[];
    double w, s1, s2, z;
    int ix, hx, id;

    hx = __HI(x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x44100000) {
        if (ix > 0x7ff00000 || (ix == 0x7ff00000 && (__LO(x) != 0))) {
            return x + x;
        }
        if (hx > 0) {
            return atanhi_800CDCF0[3] + atanlo[3];
        } else {
            return -atanhi_800CDCF0[3] - atanlo[3];
        }
    }
    if (ix < 0x3fdc0000) {
        if (ix < 0x3e200000) {
            if (1.0e300 + x > 1.0) {
                return x;
            }
        }
        id = -1;
    } else {
        x = __fabs(x);
        if (ix < 0x3ff30000) {
            if (ix < 0x3fe60000) {
                id = 0;
                x = (2.0 * x - 1.0) / (2.0 + x);
            } else {
                id = 1;
                x = (x - 1.0) / (x + 1.0);
            }
        } else {
            if (ix < 0x40038000) {
                id = 2;
                x = (x - 1.5) / (1.0 + 1.5 * x);
            } else {
                id = 3;
                x = -1.0 / x;
            }
        }
    }
    z = x * x;
    w = z * z;
    s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
    s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));
    if (id < 0) {
        return x - x * (s1 + s2);
    } else {
        z = atanhi_800CDCF0[id] - ((x * (s1 + s2) - atanlo[id]) - x);
        return (hx < 0) ? -z : z;
    }
}

double __kernel_tan(double x, double y, int iy);
int __ieee754_rem_pio2(double x, double *y);
#pragma push
#pragma auto_inline off
double copysign(double x, double y) {
    __HI(x) = (__HI(x) & 0x7fffffff) | (__HI(y) & 0x80000000);
    return x;
}
#pragma pop

double __kernel_cos(double x, double y);
double __kernel_sin(double x, double y, int iy);
#pragma push
#pragma auto_inline off
#pragma dont_inline on
double cos(double x) {
    double y[2], z = 0.0;
    int n, ix;
    ix = __HI(x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_cos(x, z);
    } else if (ix >= 0x7ff00000) {
        return x - x;
    } else {
        n = __ieee754_rem_pio2(x, y);
        switch (n & 3) {
        case 0:
            return __kernel_cos(y[0], y[1]);
        case 1:
            return -__kernel_sin(y[0], y[1], 1);
        case 2:
            return -__kernel_cos(y[0], y[1]);
        default:
            return __kernel_sin(y[0], y[1], 1);
        }
    }
}

double floor(double x)
{
    int i0, i1, j0;
    unsigned i, j;
    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0) {
            if (1.0e300 + x > 0.0) {
                if (i0 >= 0) {
                    i0 = i1 = 0;
                } else if (((i0 & 0x7fffffff) | i1) != 0) {
                    i0 = 0xbff00000;
                    i1 = 0;
                }
            }
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) {
                return x;
            }
            if (1.0e300 + x > 0.0) {
                if (i0 < 0) {
                    i0 += (0x00100000) >> j0;
                }
                i0 &= (~i);
                i1 = 0;
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400) {
            return x + x;
        } else {
            return x;
        }
    } else {
        i = ((unsigned)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) {
            return x;
        }
        if (1.0e300 + x > 0.0) {
            if (i0 < 0) {
                if (j0 == 20) {
                    i0 += 1;
                } else {
                    j = i1 + (1 << (52 - j0));
                    if (j < i1) {
                        i0 += 1;
                    }
                    i1 = j;
                }
            }
            i1 &= (~i);
        }
    }
    __HI(x) = i0;
    __LO(x) = i1;
    return x;
}

#pragma pop

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

double ldexp(double x, int n) {
    int k, hx, lx;

    if (!(__fpclassifyd(x) > 2) || x == 0.0) {
        return x;
    }
    hx = __HI(x);
    lx = __LO(x);
    k = (hx & 0x7ff00000) >> 20;
    if (k == 0) {
        if ((lx | (hx & 0x7fffffff)) == 0) {
            return x;
        }
        x *= 1.80143985094819840000e+16;
        hx = __HI(x);
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (n < -50000) {
            return 1.0e-300 * x;
        }
    }
    if (k == 0x7ff) {
        return x + x;
    }
    k = k + n;
    if (k > 0x7fe) {
        return 1.0e+300 * copysign(1.0e+300, x);
    }
    if (k > 0) {
        __HI(x) = (hx & 0x800fffff) | (k << 20);
        return x;
    }
    if (k <= -54) {
        if (n > 50000) {
            return 1.0e+300 * copysign(1.0e+300, x);
        } else {
            return 1.0e-300 * copysign(1.0e-300, x);
        }
    }
    k += 54;
    __HI(x) = (hx & 0x800fffff) | (k << 20);
    return x * 5.55111512312578270212e-17;
}

double modf(double x, double *iptr)
{
    int i0, i1, j0;
    unsigned i;
    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0) {
            *(int *)iptr = i0 & 0x80000000;
            *(1 + (int *)iptr) = 0;
            return x;
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) {
                *iptr = x;
                __HI(x) &= 0x80000000;
                __LO(x) = 0;
                return x;
            } else {
                *(int *)iptr = i0 & (~i);
                *(1 + (int *)iptr) = 0;
                return x - *iptr;
            }
        }
    } else if (j0 > 51) {
        *iptr = x;
        __HI(x) &= 0x80000000;
        __LO(x) = 0;
        return x;
    } else {
        i = ((unsigned)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) {
            *iptr = x;
            __HI(x) &= 0x80000000;
            __LO(x) = 0;
            return x;
        } else {
            *(int *)iptr = i0;
            *(1 + (int *)iptr) = i1 & (~i);
            return x - *iptr;
        }
    }
}

#pragma push
#pragma auto_inline off
#pragma dont_inline on
double sin(double x) {
    double y[2], z = 0.0;
    int n, ix;
    ix = __HI(x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        return __kernel_sin(x, z, 0);
    } else if (ix >= 0x7ff00000) {
        return x - x;
    } else {
        n = __ieee754_rem_pio2(x, y);
        switch (n & 3) {
        case 0:
            return __kernel_sin(y[0], y[1], 1);
        case 1:
            return __kernel_cos(y[0], y[1]);
        case 2:
            return -__kernel_sin(y[0], y[1], 1);
        default:
            return -__kernel_cos(y[0], y[1]);
        }
    }
}
#pragma pop

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

double pow(double x, double y) {
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

#pragma push
#pragma use_lmw_stmw on
s32 DBWrite(const void *src, u32 size) {
    extern volatile u8 SendCount_803C6EB0;
    volatile u32 *regs = (volatile u32 *)0xCC000000;
    volatile u32 *cpr;
    volatile u32 *cr;
    BOOL err;
    u32 v;
    u32 busyFlag;
    u32 cmd1;
    u32 cmd2;
    u32 cmd3;
    u32 cmd4;
    BOOL interrupts = OSDisableInterrupts();

    cpr = (volatile u32 *)((u8 *)regs + 0x6800);
    do {
        *(cpr += 10) = (*(volatile u32 *)((u8 *)regs + 0x6828) & 0x405) | 0xC0;
        cmd1 = 0x40000000;
        err = !DBGEXIImm(&cmd1, 2, 1);
        cr = (volatile u32 *)((u8 *)regs + 0x6800);
        while (*(cr += 13) & 1) {
        }
        err |= !DBGEXIImm(&busyFlag, 4, 0);
        while (*cr & 1) {
        }
        *cpr &= 0x405;
    } while (busyFlag & 2);

    SendCount_803C6EB0++;
    if (SendCount_803C6EB0 & 1) {
        v = 0x1000;
    } else {
        v = 0;
    }

    while (!DBGWrite(v | 0x1C000, (void *)src, (size + 3) & ~3)) {
    }

    do {
        *cpr = (*cpr & 0x405) | 0xC0;
        cmd2 = 0x40000000;
        err = !DBGEXIImm(&cmd2, 2, 1);
        while (*cr & 1) {
        }
        err |= !DBGEXIImm(&busyFlag, 4, 0);
        while (*cr & 1) {
        }
        *cpr &= 0x405;
    } while (busyFlag & 2);

    v = SendCount_803C6EB0;
    do {
        *cpr = (*cpr & 0x405) | 0xC0;
        cmd3 = ((0x1F000000 | v << 16 | size) & 0x1FFFFFFF) | 0xC0000000;
        err = !DBGEXIImm(&cmd3, 4, 1);
        while (*cr & 1) {
        }
        *cpr &= 0x405;
    } while (err);

    do {
        do {
            *cpr = (*cpr & 0x405) | 0xC0;
            cmd4 = 0x40000000;
            err = !DBGEXIImm(&cmd4, 2, 1);
            while (*cr & 1) {
            }
            err |= !DBGEXIImm(&busyFlag, 4, 0);
            while (*cr & 1) {
            }
            *cpr &= 0x405;
        } while (err);
    } while (busyFlag & 2);

    OSRestoreInterrupts(interrupts);
    return 0;
}
#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
BOOL DBGWrite(u32 count, void *buffer, s32 size) {
    BOOL DBGEXIImm(void *buffer, s32 bytecounter, u32 write);
    volatile u32 *regs = (volatile u32 *)0xCC000000;
    u32 *buf = (u32 *)buffer;
    BOOL err;
    volatile u32 *cpr;
    volatile u32 *cr;
    u32 cmd;
    u32 data;

    cpr = (volatile u32 *)((u8 *)regs + 0x6800);
    *(cpr += 10) = (*(volatile u32 *)((u8 *)regs + 0x6828) & 0x405) | 0xC0;
    cmd = ((count << 8) & 0x1FFFC00) | 0xA0000000;
    err = !DBGEXIImm(&cmd, 4, 1);
    cr = (volatile u32 *)((u8 *)regs + 0x6800);
    while (*(cr += 13) & 1) {
    }
    while (size != 0) {
        data = *buf++;
        err |= !DBGEXIImm(&data, 4, 1);
        while (*cr & 1) {
        }
        size -= 4;
        if (size < 0) {
            size = 0;
        }
    }
    *cpr &= 0x405;
    return !err;
}

BOOL DBGRead(u32 count, void *buffer, s32 size) {
    BOOL DBGEXIImm(void *buffer, s32 bytecounter, u32 write);
    volatile u32 *regs = (volatile u32 *)0xCC000000;
    u32 *buf = (u32 *)buffer;
    BOOL err;
    volatile u32 *cpr;
    volatile u32 *cr;
    u32 cmd;
    u32 data;

    cpr = (volatile u32 *)((u8 *)regs + 0x6800);
    *(cpr += 10) = (*(volatile u32 *)((u8 *)regs + 0x6828) & 0x405) | 0xC0;
    cmd = ((count << 8) & 0x1FFFC00) | 0x20000000;
    err = !DBGEXIImm(&cmd, 4, 1);
    cr = (volatile u32 *)((u8 *)regs + 0x6800);
    while (*(cr += 13) & 1) {
    }
    while (size != 0) {
        err |= !DBGEXIImm(&data, 4, 0);
        while (*cr & 1) {
        }
        *buf++ = data;
        size -= 4;
        if (size < 0) {
            size = 0;
        }
    }
    *cpr &= 0x405;
    return !err;
}

#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
#pragma scheduling 604
BOOL DBGEXIImm(void *buffer, s32 bytecounter, u32 write) {
    u8 *tempPointer;
    u32 writeOutValue;
    int i;

    if (write) {
        tempPointer = buffer;
        writeOutValue = 0;
        for (i = 0; i < bytecounter; i++) {
            u8 *temp = (u8 *)buffer + i;
            writeOutValue |= *temp << ((3 - i) << 3);
        }
        __EXIRegs[14] = writeOutValue;
    }
    __EXIRegs[13] = 1 | write << 2 | (bytecounter - 1) << 4;
    DBGEXISync();
    if (!write) {
        writeOutValue = __EXIRegs[14];
        tempPointer = buffer;
        for (i = 0; i < bytecounter; i++) {
            *tempPointer++ = (u8)(writeOutValue >> ((3 - i) << 3));
        }
    }
    return TRUE;
}
#pragma pop

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

void MultiplySubMatrix(f32 a[4][4], f32 b[4][4], f32 out[4][4]) {
    f32 tmp[4][4];

    tmp[0][0] = b[0][0] * a[0][0] + b[1][0] * a[0][1] + b[2][0] * a[0][2];
    tmp[0][1] = b[0][1] * a[0][0] + b[1][1] * a[0][1] + b[2][1] * a[0][2];
    tmp[0][2] = b[0][2] * a[0][0] + b[1][2] * a[0][1] + b[2][2] * a[0][2];
    tmp[0][3] = b[0][3] * a[0][0] + b[1][3] * a[0][1] + b[2][3] * a[0][2];
    tmp[1][0] = b[0][0] * a[1][0] + b[1][0] * a[1][1] + b[2][0] * a[1][2];
    tmp[1][1] = b[0][1] * a[1][0] + b[1][1] * a[1][1] + b[2][1] * a[1][2];
    tmp[1][2] = b[0][2] * a[1][0] + b[1][2] * a[1][1] + b[2][2] * a[1][2];
    tmp[1][3] = b[0][3] * a[1][0] + b[1][3] * a[1][1] + b[2][3] * a[1][2];
    tmp[2][0] = b[0][0] * a[2][0] + b[1][0] * a[2][1] + b[2][0] * a[2][2];
    tmp[2][1] = b[0][1] * a[2][0] + b[1][1] * a[2][1] + b[2][1] * a[2][2];
    tmp[2][2] = b[0][2] * a[2][0] + b[1][2] * a[2][1] + b[2][2] * a[2][2];
    tmp[2][3] = b[0][3] * a[2][0] + b[1][3] * a[2][1] + b[2][3] * a[2][2];
    memcpy(out, tmp, sizeof(tmp));
}

void MultiplyMatrix(void *arg0, void *arg1, void *arg2) {
    f32 *a = (f32 *)arg0;
    f32 *b = (f32 *)arg1;
    f32 *d = (f32 *)arg2;
    d[0] = b[8] * a[2] + (b[0] * a[0] + b[4] * a[1]);
    d[1] = b[9] * a[2] + (b[1] * a[0] + b[5] * a[1]);
    d[2] = b[10] * a[2] + (b[2] * a[0] + b[6] * a[1]);
    d[3] = 0.0f;
    d[4] = b[8] * a[6] + (b[0] * a[4] + b[4] * a[5]);
    d[5] = b[9] * a[6] + (b[1] * a[4] + b[5] * a[5]);
    d[6] = b[10] * a[6] + (b[2] * a[4] + b[6] * a[5]);
    d[7] = 0.0f;
    d[8] = b[8] * a[10] + (b[0] * a[8] + b[4] * a[9]);
    d[9] = b[9] * a[10] + (b[1] * a[8] + b[5] * a[9]);
    d[10] = b[10] * a[10] + (b[2] * a[8] + b[6] * a[9]);
    d[11] = 0.0f;
    d[12] = b[12] + (b[8] * a[14] + (b[0] * a[12] + b[4] * a[13]));
    d[13] = b[13] + (b[9] * a[14] + (b[1] * a[12] + b[5] * a[13]));
    d[14] = b[14] + (b[10] * a[14] + (b[2] * a[12] + b[6] * a[13]));
    d[15] = 1.0f;
}

void SetOrientation(f32 *m, f32 x, f32 y, f32 z) {
    f32 c;
    f32 s;
    f32 t;

    m[9] = 0.0f;
    m[8] = 0.0f;
    m[6] = 0.0f;
    m[4] = 0.0f;
    m[2] = 0.0f;
    m[1] = 0.0f;
    m[10] = 1.0f;
    m[5] = 1.0f;
    m[0] = 1.0f;
    m[11] = 0.0f;
    m[7] = 0.0f;
    m[3] = 0.0f;
    m[15] = 1.0f;

    if (z != 0.0f) {
        c = cos(z);
        s = sin(z);
        t = m[0];
        m[0] = c * t - s * m[1];
        m[1] = c * m[1] + s * t;
        t = m[4];
        m[4] = c * t - s * m[5];
        m[5] = c * m[5] + s * t;
        t = m[8];
        m[8] = c * t - s * m[9];
        m[9] = c * m[9] + s * t;
    }
    if (y != 0.0f) {
        c = cos(y);
        s = sin(y);
        t = m[1];
        m[1] = c * t - s * m[2];
        m[2] = c * m[2] + s * t;
        t = m[5];
        m[5] = c * t - s * m[6];
        m[6] = c * m[6] + s * t;
        t = m[9];
        m[9] = c * t - s * m[10];
        m[10] = c * m[10] + s * t;
    }
    if (x != 0.0f) {
        c = cos(x);
        s = sin(x);
        t = m[0];
        m[0] = c * t + s * m[2];
        m[2] = -s * t + c * m[2];
        t = m[4];
        m[4] = c * t + s * m[6];
        m[6] = -s * t + c * m[6];
        t = m[8];
        m[8] = c * t + s * m[10];
        m[10] = -s * t + c * m[10];
    }
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

void QuaternionToSubMatrix(void *quat, void *mtx) {
    f32 *q = (f32 *)quat;
    f32 *m = (f32 *)mtx;
    f32 s;
    f32 ww, yy, zz, xx;

    xx = q[0] * q[0];
    yy = q[1] * q[1];
    zz = q[2] * q[2];
    ww = q[3] * q[3];
    s = 1.0f / (xx + yy + zz + ww);
    m[0] = s * (ww + xx - yy - zz);
    m[5] = s * (ww - xx + yy - zz);
    m[10] = s * (ww - xx - yy + zz);
    s *= 2.0f;
    m[1] = s * (q[0] * q[1] + q[3] * q[2]);
    m[2] = s * (q[0] * q[2] - q[3] * q[1]);
    m[4] = s * (q[0] * q[1] - q[3] * q[2]);
    m[6] = s * (q[1] * q[2] + q[3] * q[0]);
    m[8] = s * (q[0] * q[2] + q[3] * q[1]);
    m[9] = s * (q[1] * q[2] - q[3] * q[0]);
    m[11] = 0.0f;
    m[7] = 0.0f;
    m[3] = 0.0f;
    m[15] = 1.0f;
}

void SubMatrixToQuaternion(void *mtx, void *quat) {
    f32 *m = (f32 *)mtx;
    f32 *q = (f32 *)quat;
    f32 c;
    f32 s;

    c = 0.5f * (m[0] + m[5] + m[10] - 1.0f);
    if (c <= -1.0f) {
        q[3] = 0.0f;
        q[0] = m[6] - m[9];
        q[1] = m[8] - m[2];
        q[2] = m[1] - m[4];
        return;
    }
    q[3] = Sqrt(0.5f * (1.0f + c));
    s = 1.0f / (4.0f * q[3]);
    q[0] = s * (m[6] - m[9]);
    q[1] = s * (m[8] - m[2]);
    q[2] = s * (m[1] - m[4]);
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

f32 LinearPointTest(Vector *point, Vector *start, Vector *end) {
    void PSVECSubtract(Vector *a, Vector *b, Vector *out);
    f32 PSVECDotProduct(Vector *a, Vector *b);
    Vector toPoint __attribute__((aligned(16)));
    Vector toEnd __attribute__((aligned(16)));
    f32 dot;

    PSVECSubtract(point, start, &toPoint);
    PSVECSubtract(end, start, &toEnd);
    dot = PSVECDotProduct(&toPoint, &toEnd);
    return dot / (MagnitudeVector(&toPoint) * MagnitudeVector(&toEnd));
}

void PSVECCrossProduct(Vector *a, Vector *b, Vector *axb);
s8 SegmentVsSphere(Vector *start, Vector *end, Vector *center, f32 radius) {
    void PSVECSubtract(Vector *a, Vector *b, Vector *a_b);
    f32 PSVECDotProduct(Vector *a, Vector *b);
    f32 PSVECSquareDistance(Vector *a, Vector *b);
    void PSVECCrossProduct(Vector *a, Vector *b, Vector *axb);
    Vector toCenter __attribute__((aligned(16)));
    Vector dir __attribute__((aligned(16)));
    Vector negToCenter __attribute__((aligned(16)));
    Vector startToCenter __attribute__((aligned(16)));
    Vector cross __attribute__((aligned(16)));
    f32 radiusSq = radius * radius;

    PSVECSubtract(start, end, &dir);
    if (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z < radiusSq) {
        return 1;
    }
    if (PSVECSquareDistance(start, center) < radiusSq) {
        return 1;
    }
    PSVECSubtract(center, end, &toCenter);
    if (PSVECDotProduct(&toCenter, &dir) < 0.0f) {
        return 0;
    }
    negToCenter.x = -toCenter.x;
    negToCenter.y = -toCenter.y;
    negToCenter.z = -toCenter.z;
    PSVECSubtract(start, center, &startToCenter);
    if (PSVECDotProduct(&negToCenter, &startToCenter) < 0.0f) {
        return 0;
    }
    PSVECCrossProduct(&toCenter, &dir, &cross);
    return cross.x * cross.x + cross.y * cross.y + cross.z * cross.z <
           radiusSq * (toCenter.z * toCenter.z + (toCenter.x * toCenter.x + toCenter.y * toCenter.y));
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

void Distance_Plane_4(Vector *planes, Vector *point, f32 *out) {
    s32 Distance_Plane();
    out[0] = Distance_Plane(&planes[0], point);
    out[1] = Distance_Plane(&planes[1], point);
    out[2] = Distance_Plane(&planes[2], point);
    out[3] = Distance_Plane(&planes[3], point);
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

void InitRenderMode(void *mode);
s32 main(void) {
    extern s64 g_resetTicks;
    extern s32 shithack;
    extern void *rmode;
    extern u32 m_ilpreset_803C7620;
    extern s32 m_reset;
    extern s32 m_cd_drive_status;
    extern s8 g_bBlockReset;
    void OSInit(void);
    void DVDInit(void);
    void VIInit(void);
    void PADInit(void);
    void CARDInit(void);
    void InitMem(void);
    s32 time(s32 *t);
    void srand(u32 seed);
    s8 Main_Init(void);
    s8 Main_Loop(void);
    void Main_Shutdown(void);
    void Main_Shutdown_NGC(void);
    s64 OSGetTime(void);
    s32 DVDGetDriveStatus(void);
    s32 OSGetResetButtonState(void);
    void VISetBlack(s32 black);
    void VIWaitForRetrace(void);
    void VIFlush(void);
    void OSResetSystem(s32 reset, u32 resetCode, s32 forceMenu);
    s32 running = 1;

    g_resetTicks = 0;
    shithack = 0;
    OSInit();
    DVDInit();
    VIInit();
    PADInit();
    CARDInit();
    InitRenderMode(rmode);
    InitMem();
    srand(time(NULL));
    if (!Main_Init()) {
        return 0;
    }
    Dprintf("Main Init\n");
    m_ilpreset_803C7620 = 0;
    m_reset = 0;
    g_resetTicks = OSGetTime() - (*(u32 *)0x800000F8 / 4) * 10;
    while (running) {
        m_cd_drive_status = DVDGetDriveStatus();
        if (!Main_Loop()) {
            running = 0;
        }
        if (m_reset && !g_bBlockReset && OSGetResetButtonState() == 0) {
            VISetBlack(1);
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            if ((OSGetTime() - g_resetTicks) / (*(u32 *)0x800000F8 / 4) < 8) {
                OSResetSystem(1, 0, 0);
            }
            if (m_ilpreset_803C7620 == 0) {
                OSResetSystem(0, 0, 0);
            } else {
                OSResetSystem(1, 0, 1);
            }
        }
        shithack = 1;
    }
    Dprintf("Main Shutdown\n");
    Main_Shutdown();
    Main_Shutdown_NGC();
    return 1;
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

void InitMem(void) {
    typedef struct {
        u32 viTVmode;
        u16 fbWidth;
        u16 efbHeight;
        u16 xfbHeight;
    } RenderMode;
    extern RenderMode *rmode;
    extern void *g_frameBuffer1;
    extern void *g_frameBuffer2;
    extern void *g_currentBuffer;
    void *arenaLo;
    void *arenaHi;
    u32 fbSize;

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    fbSize = (u16)(((u16)rmode->fbWidth + 15) & ~15) * rmode->xfbHeight * 2;
    g_frameBuffer1 = (void *)(((u32)arenaLo + 0x1F) & ~0x1F);
    g_frameBuffer2 = (void *)(((u32)g_frameBuffer1 + fbSize + 0x1F) & ~0x1F);
    g_currentBuffer = g_frameBuffer2;
    arenaLo = (void *)(((u32)g_frameBuffer2 + fbSize + 0x1F) & ~0x1F);
    OSSetArenaLo(arenaLo);

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
    OSSetArenaLo(arenaLo);
    arenaLo = (void *)(((u32)arenaLo + 0x1F) & ~0x1F);
    arenaHi = (void *)((u32)arenaHi & ~0x1F);
    OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
    OSSetArenaLo(arenaLo = arenaHi);
}

#pragma push
#pragma readonly_strings on
void InitRenderMode(void *mode) {
    extern void *rmode;
    extern u8 GXNtsc480IntDf[];
    extern u8 GXPal528IntDf[];
    extern u8 GXMpal480IntDf[];
    extern u8 rmodeobj[];
    void OSPanic(const char *file, s32 line, const char *msg, ...);
    s32 VIGetTvFormat(void);
    void GXAdjustForOverscan(void *rmin, void *rmout, u16 hor, u16 ver);
    void VIConfigure(void *rm);
    void InitVI(void);

    if (mode != NULL) {
        rmode = mode;
    } else {
        switch (VIGetTvFormat()) {
        case 0:
            rmode = GXNtsc480IntDf;
            break;
        case 1:
            rmode = GXPal528IntDf;
            break;
        case 2:
            rmode = GXMpal480IntDf;
            break;
        default:
            OSPanic("Main_NGC.c", 366, "DEMOInit: invalid TV format\n");
            break;
        }
        GXAdjustForOverscan(rmode, rmodeobj, 0, 16);
        rmode = rmodeobj;
    }
    VIConfigure(rmode);
    InitVI();
}
#pragma pop

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

#pragma push
#pragma auto_inline off
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
#pragma pop

void MemCpy32(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void MemClr32(void *dst, u32 size) {
    memset(dst, 0, size);
}

void HeapDump(void) {

}

#pragma push
#pragma dont_inline on
void Create_Plane(Vector *plane, Vector *normal, Vector *point) {
    f32 len;
    f32 inv;

    plane->x = normal->x;
    plane->y = normal->y;
    plane->z = normal->z;
    plane->w = -1.0f * (normal->z * point->z + (normal->x * point->x + normal->y * point->y));
    len = Sqrt(plane->z * plane->z + (plane->x * plane->x + plane->y * plane->y));
    if (len != 1.0f && len != 0.0f) {
        inv = 1.0f / len;
        plane->x *= inv;
        plane->y *= inv;
        plane->z *= inv;
        plane->w *= inv;
    }
}
#pragma pop

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
void RandomNumberGenerator_Init(void) {
    u32 OSGetTick(void);
    void srand(u32 seed);
    int rand(void);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    Vector axis __attribute__((aligned(16)));
    Vector q __attribute__((aligned(16)));
    f32 mtx[4][4] __attribute__((aligned(16)));
    u32 i;

    srand(OSGetTick());
    for (i = 0; i < 0x100; i++) {
        axis.x = (rand() & 0xFF) - 127;
        axis.y = (rand() & 0xFF) - 127;
        axis.z = (rand() & 0xFF) - 127;
        GenerateQuaternion(&axis, &q, (6.2831855f * rand()) / 32767.0f);
        QuaternionToSubMatrix(&q, mtx);
        g_randomNumberGenerator.vectors[i * 3 + 0].x = mtx[0][0];
        g_randomNumberGenerator.vectors[i * 3 + 0].y = mtx[0][1];
        g_randomNumberGenerator.vectors[i * 3 + 0].z = mtx[0][2];
        g_randomNumberGenerator.vectors[i * 3 + 1].x = mtx[1][0];
        g_randomNumberGenerator.vectors[i * 3 + 1].y = mtx[1][1];
        g_randomNumberGenerator.vectors[i * 3 + 1].z = mtx[1][2];
        g_randomNumberGenerator.vectors[i * 3 + 2].x = mtx[2][0];
        g_randomNumberGenerator.vectors[i * 3 + 2].y = mtx[2][1];
        g_randomNumberGenerator.vectors[i * 3 + 2].z = mtx[2][2];
    }
    g_randomNumberGenerator.unk3000 = 0;
    g_randomNumberGenerator.unk3004 = 0;
    g_randomNumberGenerator.index = 0;
    for (i = 0; i < 0x400; i++) {
        g_randomNumberGenerator.floats[i] = rand() / 32767.0f;
    }
}
#pragma pop

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

void TimeHandler_Heartbeat(void) {
    u32 OSGetTick(void);
    extern u32 max_time;
    extern u32 m_iLastTimeUs;
    extern u32 m_iDeltaTimeUs;
    extern u32 g_iDeltaTimeMS;
    extern u32 g_iLastDeltaTimeMS;
    extern u32 g_iLastTimeMS;
    extern u32 g_iDeltaTime60th;
    extern f32 g_fDeltaTimeMS;
    extern f32 g_fDeltaTime60th;
    extern f32 g_fDeltaTimeSec;
    extern f32 g_fDeltaTimeSec_inv;
    u32 now;

    now = (OSGetTick() * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
    if (max_time < now) {
        max_time = now;
    }
    if (m_iLastTimeUs > now) {
        m_iDeltaTimeUs = now + max_time - m_iLastTimeUs;
    } else {
        m_iDeltaTimeUs = now - m_iLastTimeUs;
    }
    if (m_iDeltaTimeUs >= 34000) {
        m_iDeltaTimeUs = 34000;
    }
    if (m_iDeltaTimeUs < 1000) {
        m_iDeltaTimeUs = 1000;
    }
    m_iLastTimeUs = now;
    g_iLastDeltaTimeMS = g_iDeltaTimeMS;
    g_fDeltaTimeMS = (f32)m_iDeltaTimeUs / 1000.0f;
    g_iDeltaTimeMS = m_iDeltaTimeUs / 1000;
    g_iLastTimeMS = m_iLastTimeUs / 1000;
    g_fDeltaTime60th = g_fDeltaTimeMS / (1000.0f / 60.0f);
    g_fDeltaTimeSec = g_fDeltaTimeMS / 1000.0f;
    g_iDeltaTime60th = g_fDeltaTime60th;
    g_fDeltaTimeSec_inv = 1.0f / g_fDeltaTimeSec;
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

#pragma push
#pragma use_lmw_stmw on
void AnimExtractKeyFrame_CompoundHybrid(void *animp, void *outp, f32 time) {
    typedef struct {
        u8 pad0[8];
        f32 minTime;
        f32 maxTime;
        u32 numBones;
        u32 numKeysPerFrame;
        u32 *mask;
        u16 *flags;
        f32 *times;
        u8 *data;
        u32 numFrames;
        u8 pad2C[4];
        u32 numCompound;
    } Anim;
    typedef struct {
        Vector rot;
        f32 pos[3];
        u32 info;
    } Bone;
    void CopyVector(Vector *dst, Vector *src);
    void Distance_Plane_4(Vector *planes, Vector *point, f32 *out);
    Vector tv __attribute__((aligned(16)));
    u32 i;
    u32 *mask;
    Bone *out;
    u8 *data;
    u32 bit;
    Anim *anim;
    u32 save;
    s32 idx;

    anim = (Anim *)animp;
    out = (Bone *)outp;
    idx = 0;
    mask = anim->mask;
    if (time < anim->minTime) {
        time = anim->minTime;
    }
    if (time > anim->maxTime) {
        time = anim->maxTime;
    }
    for (i = 1; i < anim->numFrames; i++) {
        if (time <= anim->times[i]) {
            idx = i - 1;
            break;
        }
    }
    data = &anim->data[(((anim->numKeysPerFrame - anim->numCompound) * sizeof(Bone)) + (anim->numCompound * 2 * sizeof(Bone))) * idx];
    tv.y = time * time;
    tv.x = time * tv.y;
    tv.z = time;
    tv.w = 1.0f;
    for (i = 0, bit = 0; i < anim->numBones; i++) {
        if (anim->flags[i] & 7) {
            if (*mask & (1 << bit++)) {
                save = out->info;
                Distance_Plane_4((Vector *)data, &tv, out->pos);
                out->info = save;
                data += 0x40;
            } else {
                out->pos[0] = ((Bone *)data)->pos[0];
                out->pos[1] = ((Bone *)data)->pos[1];
                out->pos[2] = ((Bone *)data)->pos[2];
                data += 0x20;
            }
            if (bit == 32) {
                bit = 0;
                mask++;
            }
        }
        if (anim->flags[i] & 0x78) {
            if (*mask & (1 << bit++)) {
                Distance_Plane_4((Vector *)data, &tv, (f32 *)&out->rot);
                data += 0x40;
            } else {
                CopyVector(&out->rot, (Vector *)(data + 0x10));
                data += 0x20;
            }
            if (bit == 32) {
                bit = 0;
                mask++;
            }
        }
        out++;
    }
}
#pragma pop

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

typedef struct AnimBone {
    Vector rot;
    f32 pos[3];
    u8 numChildren;
    u8 pop;
    u8 next;
    u8 pad[1];
} AnimBone;

#pragma push
#pragma use_lmw_stmw on
void AnimTraverseHeirarchyToMatrices(AnimBone *bones, u32 count, f32 *matrices) {
    void QuaternionMult(Vector *a, Vector *b, Vector *out);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void XformVec(Vector *m, Vector *v, Vector *out);
    Vector posStack[64] __attribute__((aligned(16)));
    Vector rotStack[64];
    Vector rot;
    Vector q;
    Vector pos;
    f32 *mtx;
    u32 i;
    Vector *posSp;
    Vector *rotSp;
    AnimBone *bone;
    u32 n;
    Vector *p;
    AnimBone *child;
    u32 j;

    mtx = matrices;
    bone = bones;
    posSp = posStack;
    rotSp = rotStack;
    rot.w = 1.0f;
    rot.x = rot.y = rot.z = 0.0f;
    memcpy(&pos, bones->pos, 12);
    for (i = 0; i < count; i++) {
        QuaternionMult(&rot, &bone->rot, &q);
        rot = q;
        QuaternionToSubMatrix(&q, mtx);
        mtx[12] = pos.x;
        mtx[13] = pos.y;
        mtx[14] = pos.z;
        if (bone->numChildren != 0) {
            child = &bone[1];
            n = bone->numChildren - 1;
            XformVec((Vector *)mtx, (Vector *)child->pos, &pos);
            if (n != 0) {
                posSp += n;
                p = posSp;
                child = &child[child->next];
                *rotSp++ = q;
                for (j = 0; j < n; j++) {
                    XformVec((Vector *)mtx, (Vector *)child->pos, --p);
                    child = &child[child->next];
                }
            }
        } else {
            pos = *--posSp;
            rotSp -= bone->pop + 1;
            rot = *rotSp++;
        }
        bone++;
        mtx += 16;
    }
}
#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
u32 Gfx_FontManager_Print(char *str, u32 id, u8 blendMode, f32 x, f32 y, f32 spacing) {
    extern FontManager g_fontManager;
    extern GXColor g_textcolor;
    GDLRect *Gfx_Add2DRectangle(void);
    u32 strlen(const char *);
    u32 i;
    Font *font = &g_fontManager.fonts[id];
    GXColor color = g_textcolor;
    u8 c;
    GDLRect *rect;

    if (id == 0 && g_fontManager.numFonts == 0) {
        return 0;
    }
    for (i = 0; i < strlen(str); i++) {
        c = str[i];
        if (font->caseMode == 1 || font->caseMode == 3) {
            if (c >= 'a' && c <= 'z') {
                c -= 0x20;
            }
        }
        if (c != ' ') {
            f32 x1 = x + (u8)font->chars[c].spacingX;
            f32 y1 = y + font->height;
            rect = Gfx_Add2DRectangle();
            rect->blendMode = blendMode;
            rect->tex = font->texture;
            *(GXColor *)&rect->v[0].r = color;
            *(GXColor *)&rect->v[1].r = color;
            *(GXColor *)&rect->v[2].r = color;
            *(GXColor *)&rect->v[3].r = color;
            rect->v[0].s = font->chars[c].unk0;
            rect->v[0].t = font->chars[c].unk4;
            rect->v[1].s = font->chars[c].unk0;
            rect->v[1].t = font->chars[c].unkC;
            rect->v[2].s = font->chars[c].unk8;
            rect->v[2].t = font->chars[c].unkC;
            rect->v[3].s = font->chars[c].unk8;
            rect->v[3].t = font->chars[c].unk4;
            rect->v[0].x = x;
            rect->v[0].y = y;
            rect->v[1].x = x;
            rect->v[1].y = y1;
            rect->v[2].x = x1;
            rect->v[2].y = y1;
            rect->v[3].x = x1;
            rect->v[3].y = y;
        }
        x += (u8)font->chars[c].spacingX + spacing;
    }
    return g_fontManager.fonts[id].height;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
void Gfx_FontManager_PrintScale(char *str, s32 id, f32 *scale, u8 blendMode, f32 x, f32 y, f32 spacing) {
    extern FontManager g_fontManager;
    extern GXColor g_textcolor;
    GDLRect *Gfx_Add2DRectangle(void);
    u32 strlen(const char *);
    u32 i;
    Font *font = &g_fontManager.fonts[id];
    GXColor color = g_textcolor;
    u8 c;
    GDLRect *rect;

    for (i = 0; i < strlen(str); i++) {
        c = str[i];
        if (font->caseMode == 1 || font->caseMode == 3) {
            if (c >= 'a' && c <= 'z') {
                c -= 0x20;
            }
        }
        if (c != ' ') {
            f32 x1 = (u8)font->chars[c].spacingX * scale[0] + x;
            f32 y1 = font->height * scale[1] + y;
            rect = Gfx_Add2DRectangle();
            rect->blendMode = blendMode;
            rect->tex = font->texture;
            *(GXColor *)&rect->v[0].r = color;
            *(GXColor *)&rect->v[1].r = color;
            *(GXColor *)&rect->v[2].r = color;
            *(GXColor *)&rect->v[3].r = color;
            rect->v[0].s = font->chars[c].unk0;
            rect->v[0].t = font->chars[c].unk4;
            rect->v[1].s = font->chars[c].unk0;
            rect->v[1].t = font->chars[c].unkC;
            rect->v[2].s = font->chars[c].unk8;
            rect->v[2].t = font->chars[c].unkC;
            rect->v[3].s = font->chars[c].unk8;
            rect->v[3].t = font->chars[c].unk4;
            rect->v[0].x = x;
            rect->v[0].y = y;
            rect->v[1].x = x;
            rect->v[1].y = y1;
            rect->v[2].x = x1;
            rect->v[2].y = y1;
            rect->v[3].x = x1;
            rect->v[3].y = y;
            rect->v[0].z = 0.0f;
            rect->v[1].z = 0.0f;
            rect->v[2].z = 0.0f;
            rect->v[3].z = 0.0f;
        }
        x += scale[0] * ((u8)font->chars[c].spacingX + spacing);
    }
}
#pragma pop

u32 strlen(const char *);
#pragma push
#pragma use_lmw_stmw on
#pragma dont_inline on
void Gfx_FontManager_GetPrintRect(char *str, s32 id, f32 *width, f32 *height, f32 spacing) {
    extern FontManager g_fontManager;
    Font *font = &g_fontManager.fonts[id];
    u32 w = 0;
    u32 i = 0;
    u32 len = strlen(str);
    u8 c;

    while (i < len) {
        c = str[i++];
        if (font->caseMode == 1 || font->caseMode == 3) {
            if (c >= 'a' && c <= 'z') {
                c -= 0x20;
            }
        }
        w = w + ((u8)font->chars[c].spacingX + spacing);
    }
    *width = (u32)(w - spacing);
    *height = font->height;
}
#pragma pop
void Gfx_AnimExtractKeyFrame(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_AnimTraverseHeirarchyToMatrices(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_DrawAnimGeometryAux_(Geometry *geometry, f32 *matrices, u32 count, f32 *matrix);
#pragma push
#pragma use_lmw_stmw on
void Gfx_DrawAnimGeometryAux(Geometry *geometry, f32 *matrices, u32 count, f32 *matrix) {
    extern f32 g_identity[];
    extern u8 g_CurrentViews;
    extern s32 g_currentWindow;
    s32 Gfx_CalcGeometryLOD(Geometry *geometry, f32 *matrix);
    void MultiplyMatrix(f32 *a, f32 *b, f32 *out);
    void Gfx_SetRenderWindow(u32 view);
    s32 Gfx_GetLightState(void);
    s32 Gfx_AddWorldMatrixState_CacheMatrix(u32 matrix);
    DLSkinGroup *DisplayList_AddGroup_Skin(void);
    f32 world[16] __attribute__((aligned(16)));
    DLSkinGroup *group;
    u16 matrixState;
    Geometry *lod;
    u32 level;
    u32 j;
    u32 i;

    if (g_CurrentViews != 1) {
        i = 1;
        do {
            if (g_CurrentViews & (1 << i)) {
                Gfx_SetRenderWindow(i);
            }
            if (matrix == NULL || matrix == g_identity) {
                level = Gfx_CalcGeometryLOD(geometry, matrices);
            } else {
                MultiplyMatrix(matrices, matrix, world);
                level = Gfx_CalcGeometryLOD(geometry, world);
            }
            if (level != 0xFFFFFFFF) {
                matrixState = Gfx_AddWorldMatrixState_CacheMatrix((u32)matrix);
                lod = (Geometry *)((u8 *)geometry->groups + level * 0x18);
                for (j = 0; j < lod->numGroups; j++) {
                    group = DisplayList_AddGroup_Skin();
                    group->lod = lod;
                    group->group = j;
                    group->matrix = matrixState;
                    group->matrices = matrices;
                    group->lightState = Gfx_GetLightState();
                    group->window = g_currentWindow;
                    group->count = count;
                }
            }
            i++;
        } while (i < 5);
        return;
    }
    Gfx_DrawAnimGeometryAux_(geometry, matrices, count, matrix);
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
void Gfx_DrawAnimGeometryAux_(Geometry *geometry, f32 *matrices, u32 count, f32 *matrix) {
    extern f32 g_identity[];
    extern u32 matrixlookup[];
    extern s32 g_IsAnim;
    s32 Gfx_CalcGeometryLOD(Geometry *geometry, f32 *matrix);
    void MultiplyMatrix(f32 *a, f32 *b, f32 *out);
    void PSMTXConcat(f32 a[3][4], f32 b[3][4], f32 ab[3][4]);
    void GXLoadPosMtxImm(f32 m[3][4], u32 id);
    void Gfx_DrawGeometryLOD_Immediate(Geometry *geometry, void *matrix);
    f32 world[16] __attribute__((aligned(16)));
    f32 bone[3][4] __attribute__((aligned(16)));
    f32 result[3][4] __attribute__((aligned(16)));
    f32 view[3][4] __attribute__((aligned(16)));
    u32 i;
    Geometry *lod;
    f32 *m;
    s32 level;

    if (matrix == NULL || matrix == g_identity) {
        level = Gfx_CalcGeometryLOD(geometry, matrices);
    } else {
        MultiplyMatrix(matrices, matrix, world);
        level = Gfx_CalcGeometryLOD(geometry, world);
    }
    lod = (Geometry *)((u8 *)geometry->groups + level * 0x18);
    MatrixToMtxTranspose(view, (u8 *)GetRenderWindow() + 0x40);
    m = matrices;
    for (i = 0; i < count; i++) {
        MatrixToMtxTranspose(bone, m);
        PSMTXConcat(view, bone, result);
        GXLoadPosMtxImm(result, matrixlookup[i]);
        m += 16;
    }
    g_IsAnim = 1;
    Gfx_DrawGeometryLOD_Immediate(lod, matrices);
    g_IsAnim = 0;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 Gfx_DisplayList_Create(void) {
    extern GDL *g_pGDL;
    u32 i;

    g_pGDL = Allocate(0x24D200, 0, "");
    if (g_pGDL == NULL) {
        return 0;
    }
    for (i = 0; i < 2000; i++) {
        g_pGDL->tris[i].v[0].w = 1.0f;
        g_pGDL->tris[i].v[1].w = 1.0f;
        g_pGDL->tris[i].v[2].w = 1.0f;
    }
    g_pGDL->unk24D1F2 = 0;
    return 1;
}
#pragma pop

#pragma push
#pragma dont_inline on
void Gfx_DisplayList_Destroy(void) {
    extern s32 g_pDL;
    extern s32 g_pGDL;
    Deallocate(g_pGDL);
    Deallocate(g_pDL);
}

void Gfx_SetRenderWindow(s32 window);
void Render_InitDisplayList(void) {
    extern GDL *g_pGDL;
    extern DisplayList *g_pDL;
    extern u32 g_DLBufferOn;
    extern GfxState g_state;

    memset(g_pGDL, 0, 0x80);
    memset(g_pGDL->spriteHeads, 0xFF, 0x3FE);
    memset(g_pDL->unk11DE0, 0xFF, 0x3FE);
    memset(g_pDL->unk121DE, 0xFF, 0x3FE);
    if (g_DLBufferOn) {
        g_pGDL->unk24CDEC = (u8 *)g_pGDL + 0x1DF7E0;
        g_pGDL->unk24CDE0 = (u8 *)g_pGDL + 0xF51E0;
        g_pGDL->unk24CDE8 = (u8 *)g_pGDL + 0x1915E0;
    } else {
        g_pGDL->unk24CDEC = (u8 *)g_pGDL + 0x2162E0;
        g_pGDL->unk24CDE0 = (u8 *)g_pGDL + 0x1433E0;
        g_pGDL->unk24CDE8 = (u8 *)g_pGDL + 0x1B86E0;
    }
    g_pGDL->groupTris = (GDLGroupTri *)g_pGDL->unkC3040;
    g_pGDL->unk24CDF0 = g_pGDL->pointSprites;
    g_DLBufferOn = !g_DLBufferOn;
    Gfx_SetRenderWindow(0);
    *(s32 *)((u8 *)GetRenderWindow() + 0x2C4) = 0;
    g_state.numPointLights = 0;
    g_state.lightChanged = 1;
    g_state.unk8F = 1;
    g_pGDL->numEnvmapStates = 1;
}

GDLLine *Gfx_Add3DLineFX(void) {
    extern GDL *g_pGDL;

    if (g_pGDL->numLines >= 0x100) {
        g_pGDL->numLines = 0;
    }
    return &g_pGDL->lines[g_pGDL->numLines++];
}

void Gfx_Add2DFXLine(GDLVec4 *p0, GDLVec4 *p1, GXColor *c0, GXColor *c1, u8 flag) {
    extern GDL *g_pGDL;

    if (g_pGDL->num2DFXLines == 200) {
        return;
    }
    g_pGDL->fxLines2D[g_pGDL->num2DFXLines].p0 = *p0;
    g_pGDL->fxLines2D[g_pGDL->num2DFXLines].p1 = *p1;
    g_pGDL->fxLines2D[g_pGDL->num2DFXLines].c0 = *c0;
    g_pGDL->fxLines2D[g_pGDL->num2DFXLines].c1 = *c1;
    g_pGDL->fxLines2D[g_pGDL->num2DFXLines].unk28 = flag;
    g_pGDL->num2DFXLines++;
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

void *Gfx_Begin_GroupTriFlat(u16 tex, s32 arg1, s32 arg2) {
    extern GDL *g_pGDL;
    u32 n;

    n = g_pGDL->numGroupTris;
    if (n >= 500) {
        n = 0;
        g_pGDL->numGroupTris = 0;
    }
    if (n != 0 && (&g_pGDL->groupTris[n])[-1].type == 0 && (&g_pGDL->groupTris[n])[-1].unk10 >= 0xFFFE &&
        tex == (&g_pGDL->groupTris[n])[-1].tex && (u16)arg1 == (&g_pGDL->groupTris[n])[-1].unk6) {
        g_pGDL->numGroupTris--;
    } else {
        g_pGDL->groupTris[n].type = 0;
        g_pGDL->groupTris[n].unk1 = 0;
        g_pGDL->groupTris[n].unk2 = arg2;
        g_pGDL->groupTris[n].unk8 = g_pGDL->unk38;
        g_pGDL->groupTris[n].count = 0;
        g_pGDL->groupTris[n].tex = tex;
        g_pGDL->groupTris[n].unk6 = arg1;
        g_pGDL->groupTris[n].unk10 = 0xFFFE;
        g_pGDL->groupTris[n].unk18 = 0.0f;
        g_pGDL->groupTris[n].unk1C = 0.0f;
    }
    return (u8 *)g_pGDL->unk24CDE0 + g_pGDL->unk38 * 0x50;
}

void *Gfx_Begin_GroupQuadFlat(u16 tex, u16 arg1, u8 arg2) {
    extern GDL *g_pGDL;
    u32 n;

    n = g_pGDL->numGroupTris;
    if (n >= 500) {
        n = 0;
        g_pGDL->numGroupTris = 0;
    }
    if (n != 0 && (&g_pGDL->groupTris[n])[-1].type == 4 && (&g_pGDL->groupTris[n])[-1].unk10 >= 0xFFFE &&
        tex == (&g_pGDL->groupTris[n])[-1].tex && arg1 == (&g_pGDL->groupTris[n])[-1].unk6) {
        g_pGDL->numGroupTris--;
    } else {
        g_pGDL->groupTris[n].type = 4;
        g_pGDL->groupTris[n].unk1 = 0;
        g_pGDL->groupTris[n].unk2 = arg2;
        g_pGDL->groupTris[n].unk8 = g_pGDL->unk40;
        g_pGDL->groupTris[n].count = 0;
        g_pGDL->groupTris[n].tex = tex;
        g_pGDL->groupTris[n].unk6 = arg1;
        g_pGDL->groupTris[n].unk10 = 0xFFFE;
        g_pGDL->groupTris[n].unk18 = 0.0f;
        g_pGDL->groupTris[n].unk1C = 0.0f;
    }
    return (u8 *)g_pGDL->unk24CDE8 + g_pGDL->unk40 * 0x50;
}

void *Gfx_Begin_GroupTriShaded(u16 tex, u16 arg1, u8 arg2) {
    extern GDL *g_pGDL;
    u32 n;

    n = g_pGDL->numGroupTris;
    if (n >= 500) {
        n = 0;
        g_pGDL->numGroupTris = 0;
    }
    if (n != 0 && (&g_pGDL->groupTris[n])[-1].type == 1 && (&g_pGDL->groupTris[n])[-1].unk10 >= 0xFFFE &&
        tex == (&g_pGDL->groupTris[n])[-1].tex && arg1 == (&g_pGDL->groupTris[n])[-1].unk6) {
        g_pGDL->numGroupTris--;
    } else {
        g_pGDL->groupTris[n].type = 1;
        g_pGDL->groupTris[n].unk1 = 0;
        g_pGDL->groupTris[n].unk2 = arg2;
        g_pGDL->groupTris[n].unk8 = g_pGDL->unk3C;
        g_pGDL->groupTris[n].count = 0;
        g_pGDL->groupTris[n].tex = tex;
        g_pGDL->groupTris[n].unk6 = arg1;
        g_pGDL->groupTris[n].unk10 = 0xFFFE;
        g_pGDL->groupTris[n].unk18 = 0.0f;
        g_pGDL->groupTris[n].unk1C = 0.0f;
    }
    return (u8 *)g_pGDL->unk24CDEC + g_pGDL->unk3C * 0x70;
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

void Gfx_Add2DSprite(Vector *pos, u16 tex, GXColor *color, u8 flag, f32 a, f32 b) {
    extern GDL *g_pGDL;
    extern s32 g_currentWindow;

    if (g_pGDL->num2DSprites == 2000) {
        return;
    }
    g_pGDL->sprites2D[g_pGDL->num2DSprites].x = pos->x;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].y = pos->y;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].z = pos->z;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].unkC = a;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].unk1C = b;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].color = *color;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].tex = tex;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].flag = flag;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].window = g_currentWindow;
    g_pGDL->num2DSprites++;
}

void Gfx_AddCorona(Vector *pos, u16 tex, GXColor *color, u8 flag, f32 a, f32 b) {
    extern GDL *g_pGDL;
    extern s32 g_currentWindow;

    if (g_pGDL->num2DSprites == 2000) {
        return;
    }
    g_pGDL->sprites2D[g_pGDL->num2DSprites].x = pos->x;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].y = pos->y;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].z = pos->z;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].unkC = a * *(f32 *)((u8 *)GetRenderWindow() + 0x2CC);
    g_pGDL->sprites2D[g_pGDL->num2DSprites].unk1C = b;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].color = *color;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].tex = tex;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].flag = flag;
    g_pGDL->sprites2D[g_pGDL->num2DSprites].window = g_currentWindow;
    g_pGDL->num2DSprites++;
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

void *Gfx_GetWorldMatrixState(u32 index);
#pragma push
#pragma auto_inline off
void DisplayList_AddAlphaGroup(s32 arg0, s32 arg1, u32 matrix, u8 arg3, GXColor *color) {
    extern GDL *g_pGDL;
    extern s32 g_currentWindow;
    extern u8 g_CurrentViews;

    if (g_pGDL->numAlphaGroups == 0x400) {
        return;
    }
    g_pGDL->alphaGroups[g_pGDL->numAlphaGroups].unk40 = arg0;
    g_pGDL->alphaGroups[g_pGDL->numAlphaGroups].unk44 = arg1;
    g_pGDL->alphaGroups[g_pGDL->numAlphaGroups].window = g_currentWindow;
    if (g_CurrentViews != 1) {
        Gfx_SetWorldMatrix(Gfx_GetWorldMatrixState(matrix));
    }
    memcpy(g_pGDL->alphaGroups[g_pGDL->numAlphaGroups].matrix, (u8 *)GetRenderWindow() + 0x80, 0x40);
    g_pGDL->alphaGroups[g_pGDL->numAlphaGroups].unk50 = arg3;
    g_pGDL->alphaGroups[g_pGDL->numAlphaGroups].color = *color;
    g_pGDL->numAlphaGroups++;
}
#pragma pop

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

s32 Gfx_GetLightState(void) {
    extern GDL *g_pGDL;
    extern GfxState g_state;
    u32 i;
    GDLLightState *ls;
    u32 idx;

    if (!g_state.lightChanged) {
        return g_pGDL->numLightStates - 1;
    }
    if (g_pGDL->numLightStates >= 0x80) {
        g_pGDL->numLightStates = 0;
    }
    idx = g_pGDL->numLightStates++;
    g_state.lightChanged = 0;
    ls = &g_pGDL->lightStates[idx];
    ls->ambientColor = g_state.ambientColor;
    ls->lightColor = g_state.lightColor;
    ls->lightDir = g_state.lightDir;
    ls->numPointLights = g_state.numPointLights;
    for (i = 0; i < ls->numPointLights; i++) {
        ls->pointLights[i].x = g_state.pointLights[i].position.x;
        ls->pointLights[i].y = g_state.pointLights[i].position.y;
        ls->pointLights[i].z = g_state.pointLights[i].position.z;
        ls->pointLights[i].w = g_state.pointLights[i].radius;
        ls->pointColors[i] = g_state.pointLights[i].color;
    }
    return idx;
}

void Gfx_SetEnvmap(u16 a, u16 b, s8 flag, f32 x, f32 y) {
    extern GfxState g_state;
    extern GDL *g_pGDL;
    u32 i;

    if (flag == 0) {
        for (i = 1; i < g_pGDL->numEnvmapStates; i++) {
            if (g_pGDL->envmapStates[i].unk0 == x && g_pGDL->envmapStates[i].unk4 == y &&
                g_pGDL->envmapStates[i].unkA == b && g_pGDL->envmapStates[i].unk8 == a) {
                return;
            }
        }
        g_pGDL->envmapStates[g_pGDL->numEnvmapStates].unk0 = x;
        g_pGDL->envmapStates[g_pGDL->numEnvmapStates].unk4 = y;
        g_pGDL->envmapStates[g_pGDL->numEnvmapStates].unkA = b;
        g_pGDL->envmapStates[g_pGDL->numEnvmapStates].unk8 = a;
        g_pGDL->numEnvmapStates++;
    } else {
        g_pGDL->envmapStates[0].unk0 = x;
        g_pGDL->envmapStates[0].unk4 = y;
        g_pGDL->envmapStates[0].unkA = b;
        g_pGDL->envmapStates[0].unk8 = a;
    }
    g_state.unk8F = flag;
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

#pragma push
#pragma dont_inline on
s32 Gfx_AddWorldMatrixState_CacheMatrix(u32 matrix) {
    extern GDL *g_pGDL;

    if (matrix == 0) {
        return 0xFFFE;
    }
    if (g_pGDL->numMatrices >= 0x578) {
        g_pGDL->numMatrices = 0;
    }
    if (g_pGDL->numCachedMatrices >= 0x578) {
        g_pGDL->numCachedMatrices = 0;
    }
    CopyMatrix(g_pGDL->cachedMatrices[g_pGDL->numCachedMatrices], (void *)matrix);
    g_pGDL->matrices[g_pGDL->numMatrices] = (u32)g_pGDL->cachedMatrices[g_pGDL->numCachedMatrices];
    g_pGDL->numCachedMatrices++;
    return g_pGDL->numMatrices++;
}

s32 Gfx_AddWorldMatrixState_CacheMatrixQuaternionScalePos(s32 quat, s32 scale, s32 pos) {
    extern GDL *g_pGDL;
    void QuaternionToSubMatrix(void *quat, void *mtx);

    ((Vector *)pos)->w = 1.0f;
    if (g_pGDL->numMatrices >= 0x578) {
        g_pGDL->numMatrices = 0;
    }
    if (g_pGDL->numCachedMatrices >= 0x578) {
        g_pGDL->numCachedMatrices = 0;
    }
    QuaternionToSubMatrix((void *)quat, g_pGDL->cachedMatrices[g_pGDL->numCachedMatrices]);
    if ((void *)scale != NULL) {
        ScaleMatrix(g_pGDL->cachedMatrices[g_pGDL->numCachedMatrices], (void *)scale);
    }
    CopyVector((Vector *)&g_pGDL->cachedMatrices[g_pGDL->numCachedMatrices][12], (Vector *)pos);
    g_pGDL->matrices[g_pGDL->numMatrices] = (u32)g_pGDL->cachedMatrices[g_pGDL->numCachedMatrices];
    g_pGDL->numCachedMatrices++;
    return g_pGDL->numMatrices++;
}

#pragma pop

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

void Gfx_ScreenOverlay(GXColor *color, u32 mode) {
    if (mode != *(u32 *)((u8 *)GetRenderWindow() + 0x2C4)) {
        *(GXColor *)((u8 *)GetRenderWindow() + 0x2C0) = *color;
        *(u32 *)((u8 *)GetRenderWindow() + 0x2C4) = mode;
        return;
    }
    if (color->r > ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->r) {
        ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->r = color->r;
    }
    if (color->g > ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->g) {
        ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->g = color->g;
    }
    if (color->b > ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->b) {
        ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->b = color->b;
    }
    if (color->a > ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->a) {
        ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->a = color->a;
    }
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

s32 Gfx_FrustrumCullSphereTest(Vector *center, f32 radius) {
    f32 PSVECDotProduct(Vector *a, Vector *b);
    Vector normal __attribute__((aligned(16)));
    s32 result;
    u32 i;
    f32 dist;

    result = 2;
    for (i = 0; i < 5; i++) {
        normal.x = ((f32 *)GetRenderWindow())[i * 4 + 0x70];
        normal.y = ((f32 *)GetRenderWindow())[i * 4 + 0x71];
        normal.z = ((f32 *)GetRenderWindow())[i * 4 + 0x72];
        dist = PSVECDotProduct(center, &normal) + ((f32 *)GetRenderWindow())[i * 4 + 0x73];
        if (dist > radius) {
            return 0;
        }
        if (dist > -radius) {
            result = 1;
        }
    }
    return result;
}

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

s32 Gfx_AddWorldMatrixState_CacheMatrixQuaternionScalePos(s32 quat, s32 scale, s32 pos);
void Gfx_DrawGeometry_QuaternionScalePos(s32 geometry, s32 quat, s32 scale, s32 pos) {
    extern GDL *g_pGDL;
    extern GfxState g_state;

    g_pGDL->drawEntries[g_pGDL->numDrawEntries].geometry = geometry;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].matrix = Gfx_AddWorldMatrixState_CacheMatrixQuaternionScalePos(quat, scale, pos);
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk10 = (g_state.unk7C >> 4) & 1;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk4 = g_state.unk74;
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].lightState = Gfx_GetLightState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].envmapState = Gfx_GetEnvmapState();
    g_pGDL->drawEntries[g_pGDL->numDrawEntries].unk11 = 0;
    g_pGDL->numDrawEntries++;
}

s32 Gfx_CalcGeometryLOD(Geometry *geometry, f32 *matrix) {
    extern u8 g_CurrentViews;
    void PSVECAdd(Vector *a, Vector *b, Vector *ab);
    void PSVECScale(Vector *src, Vector *dst, f32 scale);
    void PSVECSubtract(Vector *a, Vector *b, Vector *a_b);
    f32 PSVECDotProduct(Vector *a, Vector *b);
    Vector min __attribute__((aligned(16)));
    Vector max __attribute__((aligned(16)));
    Vector center __attribute__((aligned(16)));
    f32 dist;
    u16 i;

    if (geometry->numGroups == 1) {
        return 0;
    }
    PSVECAdd((Vector *)&matrix[12], (Vector *)geometry->bboxMin, &min);
    PSVECAdd((Vector *)&matrix[12], (Vector *)geometry->bboxMax, &max);
    PSVECAdd(&min, &max, &center);
    PSVECScale(&center, &center, 0.5f);
    PSVECSubtract(&center, (Vector *)((u8 *)GetRenderWindow() + 0x180), &min);
    dist = PSVECDotProduct(&min, (Vector *)((u8 *)GetRenderWindow() + 0x190));
    dist *= ((f32 *)GetRenderWindow())[0x9D];
    for (i = 0; i < geometry->numGroups; i++) {
        if (dist < geometry->lodDistances[i]) {
            break;
        }
    }
    if (i < geometry->numGroups) {
        if (g_CurrentViews != 1 && i + 1 < geometry->numGroups) {
            return i + 1;
        }
        return i;
    }
    return -1;
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

void Gfx_SetRenderWindowSize(s32 x0, s32 y0, s32 x1, s32 y1, s32 arg4) {
    typedef struct {
        u8 unk0[0x270];
        f32 fov;
        u8 unk274[4];
        f32 scaleX;
        f32 scaleY;
        u32 width;
        u32 height;
        u32 halfWidth;
        u32 halfHeight;
        f32 offsetX;
        f32 offsetY;
        f32 sizeX;
        f32 sizeY;
        f32 unk2A0;
        f32 unk2A4;
        f32 centerX;
        f32 centerY;
        s32 left;
        s32 right;
        s32 top;
        s32 bottom;
        u8 unk2C0[4];
        s32 unk2C4;
        s32 unk2C8;
        u8 unk2CC[4];
    } Window;
    extern Window g_window[];
    extern s32 g_currentWindow;
    void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
    Window *win = &g_window[g_currentWindow];
    u32 width = x1 - x0;
    u32 height = y1 - y0;
    u32 marginX = 0x1000 - width;
    u32 marginY = 0x1000 - height;

    win->unk2C8 = arg4;
    win->left = x0 + 1;
    win->top = y0 + 1;
    win->right = x1 - 1;
    win->bottom = y1 - 1;
    win->width = width;
    win->halfWidth = width >> 1;
    win->height = height;
    win->halfHeight = height >> 1;
    win->scaleX = marginX / 2.0f;
    win->scaleY = marginY / 2.0f;
    win->sizeX = (width * 16) / 640.0f;
    win->sizeY = (height * 16) / 480.0f;
    win->offsetX = ((marginX >> 1) - x0) * 16;
    win->offsetY = ((marginY >> 1) - y0) * 16;
    win->unk2A0 = 1.0f;
    win->unk2A4 = 2.0f;
    win->centerX = (marginX >> 1) * 16;
    win->centerY = (marginY >> 1) * 16;
    Gfx_SetCameraMatrix(NULL, NULL, NULL, NULL);
    memcpy((u8 *)&g_window[g_currentWindow] + 0x140, (u8 *)&g_window[g_currentWindow] + 0x40, 0x40);
    {
        s32 cur = g_currentWindow;
        g_window[cur].unk2C4 = 0;
        g_window[cur].fov = g_window[0].fov;
    }
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

void Gfx_TextureManager_Init(void) {
    extern u8 *m_texture;
    extern u32 m_totalTextures;
    void Render_DestroyTextureSlot(u32 id);
    void Deallocate(void *p);
    void *memset(void *dst, int c, u32 n);
    void Gfx_FontManager_Init(void);
    u32 i;

    for (i = 1; i < m_totalTextures; i++) {
        Render_DestroyTextureSlot(i);
        if (*(void **)(m_texture + i * 0x40 + 0x24) != NULL && *(u16 *)(m_texture + i * 0x40 + 0x3A) == 0) {
            Deallocate(*(void **)(m_texture + i * 0x40 + 0x24));
        }
        memset(m_texture + i * 0x40, 0, 0x20);
        *(u16 *)(m_texture + i * 0x40 + 0x36) = 0;
        *(u16 *)(m_texture + i * 0x40 + 0x30) = 0;
        *(u16 *)(m_texture + i * 0x40 + 0x32) = 0;
        *(void **)(m_texture + i * 0x40 + 0x24) = NULL;
    }
    Gfx_FontManager_Init();
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

#pragma push
#pragma readonly_strings on
#pragma opt_propagation off
u16 Gfx_LoadTexture_RAM(u8 **data) {
    extern u8 *m_texture;
    extern u32 m_totalTextures;
    extern s32 m_currentView;
    u8 *LoadBTF_RAM(u8 *data, u32 id);
    char buf[32];
    Texture *tex;
    u16 i;

    for (i = 1; i < m_totalTextures; i++) {
        if (((Texture *)m_texture)[i].data == NULL) {
            break;
        }
    }
    if (i >= m_totalTextures) {
        i = m_totalTextures - 1;
    }
    *data = LoadBTF_RAM(*data, i);
    sprintf(buf, "/pack:%d", i);
    memcpy(&((Texture *)m_texture)[i], buf, 0x20);
    tex = &((Texture *)m_texture)[i];
    tex->refCount++;
    tex = &((Texture *)m_texture)[i];
    tex->view = (s8)m_currentView;
    tex = &((Texture *)m_texture)[i];
    tex->unk2C = 0.25f;
    tex = &((Texture *)m_texture)[i];
    tex->unk3A = 1;
    tex = &((Texture *)m_texture)[i];
    tex->wrapMode = 0;
    return i;
}
#pragma pop

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

void Gfx_UnLoadTexture(u16 id) {
    extern u8 *m_texture;
    extern u32 m_totalTextures;
    void Render_DestroyTextureSlot(u32 id);
    void Deallocate(void *p);
    Texture *tex;

    if (id < m_totalTextures && m_texture != NULL) {
        tex = &((Texture *)m_texture)[id];
        if (--tex->refCount == 0) {
            Render_DestroyTextureSlot(id);
            tex = &((Texture *)m_texture)[id];
            if (tex->unk3A == 0) {
                Deallocate(tex->data);
            }
            tex = &((Texture *)m_texture)[id];
            memset(tex, 0, 0x20);
            tex = &((Texture *)m_texture)[id];
            tex->refCount = 0;
            tex = &((Texture *)m_texture)[id];
            tex->unk30 = 0;
            tex = &((Texture *)m_texture)[id];
            tex->width = 0;
            tex = &((Texture *)m_texture)[id];
            tex->data = NULL;
        }
    }
}

void MovieManager_Destroy(void) {

}

s32 MovieManager_GetTextureIndex(void) {
    return 0;
}

#pragma push
#pragma use_lmw_stmw on
void Render_CreateTextureSlot(u32 id) {
    void *GXGetTexObjData(void *obj);
    void GXInitTexObj(void *obj, void *image, u16 width, u16 height, s32 format, s32 wrapS, s32 wrapT, u8 mipmap);
    void GXInitTexObjLOD(void *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
    void DCFlushRange(void *addr, u32 len);
    extern u8 *m_texture;
    extern u8 m_gxTexObjArray_80322F30[][0x20];
    void *obj;
    s32 wrap;
    s32 format;
    void *data;
    Texture *texs;
    Texture *tex;

    tex = &((Texture *)m_texture)[id];
    wrap = tex->wrapMode == 0;
    obj = m_gxTexObjArray_80322F30[id];
    if (GXGetTexObjData(obj) != NULL) {
        texs = (Texture *)m_texture;
        switch (texs[id].unk38) {
        case 0:
        case 2:
            break;
        case 1:
            format = 6;
            break;
        case 4:
            format = 0xE;
            break;
        }
        GXInitTexObj(obj, GXGetTexObjData(obj), texs[id].unk30, texs[id].width, format, wrap, wrap, texs[id].height > 1 ? 1 : 0);
        return;
    }
    tex = &((Texture *)m_texture)[id];
    switch (tex->unk38) {
    case 0:
    case 2:
        break;
    case 1:
        DCFlushRange(tex->data, tex->unk30 * (tex->width * 4));
        format = 6;
        tex = &((Texture *)m_texture)[id];
        data = tex->data;
        tex->height = 0;
        break;
    case 4:
        DCFlushRange(tex->data, tex->unk30 * tex->width);
        texs = (Texture *)m_texture;
        format = 0xE;
        data = texs[id].data;
        break;
    }
    tex = &((Texture *)m_texture)[id];
    if (tex->unk30 > 0x200 || tex->width > 0x200) {
        return;
    }
    GXInitTexObj(obj, data, tex->unk30, tex->width, format, wrap, wrap, ((Texture *)m_texture)[id].height > 1 ? 1 : 0);
    texs = (Texture *)m_texture;
    if (texs[id].height > 1) {
        GXInitTexObjLOD(obj, 5, 1, 0.0f, (s32)(texs[id].height - 1) / 1.15f, -1.0f, 1, 1, 0);
    }
}
#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
s32 DrawChar(s32 x, s32 y, u16 code) {
    typedef struct {
        u32 dummy[8];
    } GXTexObj;
    typedef struct {
        u8 unk0[0x10];
        u16 cellWidth;
        u16 cellHeight;
        u8 unk14[4];
        u16 sheetFormat;
        u8 unk1A[4];
        u16 sheetWidth;
        u16 sheetHeight;
    } FontHeader;
    extern FontHeader *FontData_803C76A4;
    void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrap_s, s32 wrap_t, u8 mipmap);
    void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
    void GXLoadTexObj(GXTexObj *obj, s32 id);
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type);
    void GXSetTexCoordGen2(s32 dst, s32 func, s32 src, u32 mtx, u8 normalize, u32 pt);
    void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
    char *OSGetFontTexture(char *string, void **image, s32 *x, s32 *y, s32 *width);
    f32 mtx[3][4];
    GXTexObj tobj;
    char str[4];
    s32 width;
    void *image;
    s32 s;
    s32 t;
    s16 y1;
    s16 y0;
    s16 x1;
    s16 x0;
    s32 s0;
    s32 t0;
    s32 cw;
    s32 ch;

    if (code == 0) {
        return 0;
    }
    if (code < 0x100) {
        str[0] = code;
        str[1] = 0;
    } else {
        str[0] = code >> 8;
        str[1] = code;
        str[2] = 0;
    }
    OSGetFontTexture(str, &image, &s, &t, &width);
    GXInitTexObj(&tobj, image, FontData_803C76A4->sheetWidth, FontData_803C76A4->sheetHeight, FontData_803C76A4->sheetFormat, 0, 0, 0);
    GXInitTexObjLOD(&tobj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj, 0);
    PSMTXScale(mtx, 1.0f / FontData_803C76A4->sheetWidth, 1.0f / FontData_803C76A4->sheetHeight, 1.0f);
    GXLoadTexMtxImm(mtx, 30, 1);
    GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
    x0 = x;
    y0 = y;
    t0 = t;
    cw = FontData_803C76A4->cellWidth;
    ch = FontData_803C76A4->cellHeight;
    s0 = s;
    x1 = x0 + cw;
    y1 = y0 + ch;
    GXBegin(0x80, 0, 4);
    GXPosition3s16(x0, y0, 0);
    GXTexCoord2s16(s0, t0);
    GXPosition3s16(x1, y0, 0);
    GXTexCoord2s16(s0 + cw, t0);
    GXPosition3s16(x1, y1, 0);
    GXTexCoord2s16(s0 + cw, t0 + ch);
    GXPosition3s16(x0, y1, 0);
    GXTexCoord2s16(s0, t0 + ch);
    return width;
}

s32 DrawString(s32 x, s32 y, char *string) {
    typedef struct {
        u32 dummy[8];
    } GXTexObj;
    typedef struct {
        u8 unk0[0x10];
        u16 cellWidth;
        u16 cellHeight;
        u8 unk14[4];
        u16 sheetFormat;
        u8 unk1A[4];
        u16 sheetWidth;
        u16 sheetHeight;
    } FontHeader;
    extern FontHeader *FontData_803C76A4;
    void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrap_s, s32 wrap_t, u8 mipmap);
    void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
    void GXLoadTexObj(GXTexObj *obj, s32 id);
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type);
    void GXSetTexCoordGen2(s32 dst, s32 func, s32 src, u32 mtx, u8 normalize, u32 pt);
    void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
    char *OSGetFontTexture(char *string, void **image, s32 *x, s32 *y, s32 *width);
    f32 mtx[3][4];
    GXTexObj tobj;
    void *image;
    s32 s;
    s32 t;
    s32 width;
    s16 y1;
    s16 y0;
    s16 x1;
    s16 x0;
    s32 s0;
    s32 t0;
    s32 cw;
    s32 ch;
    s32 total = 0;

    while (*string != 0) {
        string = OSGetFontTexture(string, &image, &s, &t, &width);
        GXInitTexObj(&tobj, image, FontData_803C76A4->sheetWidth, FontData_803C76A4->sheetHeight, FontData_803C76A4->sheetFormat, 0, 0, 0);
        GXInitTexObjLOD(&tobj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        GXLoadTexObj(&tobj, 0);
        PSMTXScale(mtx, 1.0f / FontData_803C76A4->sheetWidth, 1.0f / FontData_803C76A4->sheetHeight, 1.0f);
        GXLoadTexMtxImm(mtx, 30, 1);
        GXSetTexCoordGen2(0, 1, 4, 30, 0, 125);
        x0 = x + total;
        y0 = y;
        t0 = t;
        cw = FontData_803C76A4->cellWidth;
        ch = FontData_803C76A4->cellHeight;
        s0 = s;
        x1 = x0 + cw;
        y1 = y0 + ch;
        GXBegin(0x80, 0, 4);
        GXPosition3s16(x0, y0, 0);
        GXTexCoord2s16(s0, t0);
        GXPosition3s16(x1, y0, 0);
        GXTexCoord2s16(s0 + cw, t0);
        GXPosition3s16(x1, y1, 0);
        GXTexCoord2s16(s0 + cw, t0 + ch);
        GXPosition3s16(x0, y1, 0);
        GXTexCoord2s16(s0, t0 + ch);
        total = total + width;
    }
    return total;
}

#pragma pop

#pragma push
#pragma dont_inline on
void Render_Create(void) {
    extern s32 m_fogMode_803C76CC;
    extern s32 m_fillMode;
    extern s32 m_currentView;
    extern s32 m_blendMode_803C76D0;
    extern s32 m_curLightState;
    extern s32 m_curTexture;
    extern s32 m_zmode_803C76B8;
    extern s32 m_zstate_803C76B4;
    extern s32 m_zwrite_803C76B0;
    extern GXColor g_textcolor;
    extern GXColor m_clearColor_803C76F4;
    extern GfxState g_state;
    extern f32 g_identity[];
    extern void *FontData_803C76A4;
    extern s32 __OSCurrHeap;
    void *OSAllocFromHeap(s32 heap, u32 size);
    void OSInitFont(void *fontData);

    m_fogMode_803C76CC = 0;
    m_fillMode = 0;
    m_currentView = 0;
    *(f32 *)((u8 *)GetRenderWindow() + 0x270) = 90.0f;
    memset((u8 *)GetRenderWindow() + 0xC0, 0, 0x40);
    g_state.unk93 = 0;
    memset(g_identity, 0, 0x40);
    g_identity[15] = 1.0f;
    g_identity[10] = 1.0f;
    g_identity[5] = 1.0f;
    g_identity[0] = 1.0f;
    memcpy(GetRenderWindow(), g_identity, 0x40);
    memcpy((u8 *)GetRenderWindow() + 0x40, g_identity, 0x40);
    memcpy((u8 *)GetRenderWindow() + 0x80, g_identity, 0x40);
    memcpy((u8 *)GetRenderWindow() + 0x100, g_identity, 0x40);
    m_blendMode_803C76D0 = 1;
    g_textcolor.a = 0xFF;
    g_textcolor.b = 0xFF;
    g_textcolor.g = 0xFF;
    g_textcolor.r = 0xFF;
    m_clearColor_803C76F4.b = 0;
    m_clearColor_803C76F4.g = 0;
    m_clearColor_803C76F4.r = 0;
    m_clearColor_803C76F4.a = 0;
    g_state.ambientColor.r = 0x30;
    g_state.ambientColor.g = 0x30;
    g_state.ambientColor.b = 0x30;
    g_state.ambientColor.a = 0xFF;
    g_state.lightColor.r = 0xC8;
    g_state.lightColor.g = 0xC8;
    g_state.lightColor.b = 0xC8;
    g_state.lightColor.a = 0xFF;
    g_state.lightDir.x = 0.5f;
    g_state.lightDir.y = 0.5f;
    g_state.lightDir.z = 0.0f;
    m_curLightState = -1;
    m_curTexture = 0;
    m_zmode_803C76B8 = 3;
    m_zstate_803C76B4 = 1;
    m_zwrite_803C76B0 = 1;
    FontData_803C76A4 = OSAllocFromHeap(__OSCurrHeap, 0x20120);
    OSInitFont(FontData_803C76A4);
}

void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
void Render_StateSetScissor(void) {
    f32 top;
    f32 wd;
    f32 ht;
    u32 stop;
    u32 swd;
    u32 sht;

    Gfx_SetProjectionMatrix(*(f32 *)((u8 *)GetRenderWindow() + 0x270));
    ht = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x284);
    wd = *(u32 *)((u8 *)GetRenderWindow() + 0x280);
    top = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x2B8);
    GXSetViewport(*(u32 *)((u8 *)GetRenderWindow() + 0x2B0), top, wd, ht, 0.0f, 1.0f);
    sht = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x284);
    swd = (f32)*(u32 *)((u8 *)GetRenderWindow() + 0x280);
    stop = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x2B8);
    GXSetScissor(*(u32 *)((u8 *)GetRenderWindow() + 0x2B0), stop, swd, sht);
}

void Render_BeginScene(void) {
    void GXInvalidateVtxCache(void);
    void GXInvalidateTexAll(void);
    void GXSetCopyClear(GXColor, u32);
    void GXSetColorUpdate(u8 enable);
    void GXSetAlphaUpdate(u8 enable);
    void DisplayList_Init(void);
    void Render_StateBlendMode(u32 mode);
    void Gfx_StateTextureBlendMode(s32 arg0);
    void Render_StateAlphaBlendEnable(s32 enable);
    void Render_StateCullMode(s32 mode);
    void Render_StateZEnable(s32 enable);
    void Render_StateZWriteEnable(s32 enable);
    void Render_StateFogEnable(s32 enable);
    extern GXColor m_clearColor_803C76F4;
    extern u8 g_CurrentViews;
    extern s32 g_debugTriCount;
    extern u32 *m_vertexBuffer;
    extern u32 m_VtxDescFlag;
    f32 top;
    f32 wd;
    f32 ht;
    u32 stop;
    u32 swd;
    u32 sht;

    Gfx_SetProjectionMatrix(*(f32 *)((u8 *)GetRenderWindow() + 0x270));
    ht = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x284);
    wd = *(u32 *)((u8 *)GetRenderWindow() + 0x280);
    top = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x2B8);
    GXSetViewport(*(u32 *)((u8 *)GetRenderWindow() + 0x2B0), top, wd, ht, 0.0f, 1.0f);
    sht = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x284);
    swd = (f32)*(u32 *)((u8 *)GetRenderWindow() + 0x280);
    stop = 0.5f * *(u32 *)((u8 *)GetRenderWindow() + 0x2B8);
    GXSetScissor(*(u32 *)((u8 *)GetRenderWindow() + 0x2B0), stop, swd, sht);
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    GXSetCopyClear(m_clearColor_803C76F4, 0xFFFFFF);
    Render_StateBlendMode(1);
    Gfx_StateTextureBlendMode(2);
    Render_StateAlphaBlendEnable(0);
    Render_StateCullMode(1);
    Render_StateZEnable(1);
    Render_StateZWriteEnable(1);
    if (g_CurrentViews != 1) {
        DisplayList_Init();
    }
    g_debugTriCount = 0;
    Render_StateFogEnable(1);
    m_vertexBuffer = 0;
    m_VtxDescFlag = 0;
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(0);
}

#pragma pop

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

void Gfx_SetFog(GXColor *color, u8 type, f32 start, f32 end) {
    extern GXColor m_fogColor_803C76C8;
    extern u8 m_fogType_803C76C4;
    extern f32 m_fogRange1_803C76C0;
    extern f32 m_fogRange2_803C76BC;
    u8 r, g, b, a;
    f32 scale;
    switch (type) {
    case 0:
        m_fogType_803C76C4 = 2;
        break;
    case 1:
        m_fogType_803C76C4 = 4;
        break;
    case 2:
        m_fogType_803C76C4 = 5;
        break;
    }
    a = color->a;
    b = color->b;
    r = color->r;
    g = color->g;
    m_fogColor_803C76C8.g = g;
    m_fogColor_803C76C8.r = r;
    m_fogColor_803C76C8.b = b;
    m_fogColor_803C76C8.a = 0xFF;
    if (a != 0) {
        scale = a / 255.0f;
        m_fogRange2_803C76BC = end / scale;
        m_fogRange1_803C76C0 = start / scale;
    } else {
        m_fogRange1_803C76C0 = 1.0f;
        m_fogRange2_803C76BC = 1.0f;
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

void CreateSilhouette(void *edges, u32 count, Vector *dir) {
    f32 PSVECDotProduct(Vector *a, Vector *b);
    struct Edge {
        Vector n0;
        f32 n1[3];
        s32 type;
    } *e;
    u32 i;
    f32 d0;
    f32 d1;

    e = edges;
    for (i = 0; i < count; i++) {
        d0 = PSVECDotProduct(&e->n0, dir);
        d1 = PSVECDotProduct((Vector *)e->n1, dir);
        if (d0 < 0.0f && d1 > 0.0f) {
            e->type = 2;
        } else if (d0 > 0.0f && d1 < 0.0f) {
            e->type = 1;
        } else {
            e->type = 0;
        }
        e++;
    }
}

void DrawVolume(f32 (*verts)[3], void *edgeData, u32 numEdges, f32 dx, f32 dy, f32 dz) {
    typedef struct VolumeEdge {
        u8 unk0[0xC];
        u16 v0;
        u16 v1;
        u8 unk10[0x1C - 0x10];
        u32 flags;
    } VolumeEdge;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    VolumeEdge *edge;
    u32 i;

    edge = edgeData;
    for (i = 0; i < numEdges; i++) {
        if (edge->flags & 2) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(verts[edge->v0][0], verts[edge->v0][1], verts[edge->v0][2]);
            GXWGFifo.u32 = 0xFF0004;
            GXPosition3f32(verts[edge->v1][0], verts[edge->v1][1], verts[edge->v1][2]);
            GXWGFifo.u32 = 0xFF0004;
            GXPosition3f32(dx + verts[edge->v1][0], dy + verts[edge->v1][1], dz + verts[edge->v1][2]);
            GXWGFifo.u32 = 0xFF0004;
            GXPosition3f32(dx + verts[edge->v0][0], dy + verts[edge->v0][1], dz + verts[edge->v0][2]);
            GXWGFifo.u32 = 0xFF0004;
        }
        if (edge->flags & 1) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(verts[edge->v1][0], verts[edge->v1][1], verts[edge->v1][2]);
            GXWGFifo.u32 = 0xFF0004;
            GXPosition3f32(verts[edge->v0][0], verts[edge->v0][1], verts[edge->v0][2]);
            GXWGFifo.u32 = 0xFF0004;
            GXPosition3f32(dx + verts[edge->v0][0], dy + verts[edge->v0][1], dz + verts[edge->v0][2]);
            GXWGFifo.u32 = 0xFF0004;
            GXPosition3f32(dx + verts[edge->v1][0], dy + verts[edge->v1][1], dz + verts[edge->v1][2]);
            GXWGFifo.u32 = 0xFF0004;
        }
        edge++;
    }
}

#pragma push
#pragma use_lmw_stmw on
void Render_GeometryListSplitScreen(void) {
    extern u8 g_CurrentViews;
    extern DisplayList *g_pDL;
    extern f32 g_identity[];
    extern s32 m_curLightState;
    void Gfx_SetRenderWindow(u32 view);
    void Render_StateSetScissor(void);
    void *Gfx_GetWorldMatrixState(u32 index);
    void RenderGroup(Geometry *geometry, u32 group, void *matrix);
    u8 view;
    u32 i;

    for (view = 1; view < 5; view++) {
        if (g_CurrentViews & (1 << view)) {
            Gfx_SetRenderWindow(view);
            Render_StateSetScissor();
            for (i = 0; i < g_pDL->numOpaqueGroups; i++) {
                Geometry *geometry = g_pDL->opaqueGroups[i].geometry;
                void *matrix = Gfx_GetWorldMatrixState(g_pDL->opaqueGroups[i].matrix);
                if (g_pDL->opaqueGroups[i].window == view) {
                    if (matrix == g_identity) {
                        Gfx_SetWorldMatrix(0);
                    } else {
                        Gfx_SetWorldMatrix(matrix);
                    }
                    RenderGroup(geometry, g_pDL->opaqueGroups[i].group, matrix);
                }
            }
            for (i = 0; i < g_pDL->numLitGroups; i++) {
                Geometry *geometry = g_pDL->litGroups[i].geometry;
                void *matrix = Gfx_GetWorldMatrixState(g_pDL->litGroups[i].matrix);
                m_curLightState = g_pDL->litGroups[i].lightState;
                if (g_pDL->litGroups[i].window == view) {
                    if (matrix == g_identity) {
                        Gfx_SetWorldMatrix(0);
                    } else {
                        Gfx_SetWorldMatrix(matrix);
                    }
                    RenderGroup(geometry, g_pDL->litGroups[i].group, matrix);
                }
            }
            for (i = 0; i < g_pDL->numAlphaGroups; i++) {
                Geometry *geometry = g_pDL->alphaGroups[i].geometry;
                void *matrix = Gfx_GetWorldMatrixState(g_pDL->alphaGroups[i].matrix);
                if (g_pDL->alphaGroups[i].window == view) {
                    if (matrix == g_identity) {
                        Gfx_SetWorldMatrix(0);
                    } else {
                        Gfx_SetWorldMatrix(matrix);
                    }
                    RenderGroup(geometry, g_pDL->alphaGroups[i].group, matrix);
                }
            }
        }
    }
}
#pragma pop

#pragma push
#pragma dont_inline on
void Render_GeometryList(void) {
    extern GDL *g_pGDL;
    extern s32 m_curLightState;
    extern s32 m_curEnvmapState;
    void Render_Geometry();
    void Gfx_DisableState(u32 state);
    void *Gfx_GetWorldMatrixState(u32 index);
    void Gfx_DrawGeometryLOD_Immediate(Geometry *geometry, void *matrix);
    u32 i;
    s32 geometry;
    void *matrix;
    GXColor color;

    for (i = 0; i < g_pGDL->numDrawEntries; i++) {
        geometry = g_pGDL->drawEntries[i].geometry;
        matrix = Gfx_GetWorldMatrixState(g_pGDL->drawEntries[i].matrix);
        if (g_pGDL->drawEntries[i].unk10) {
            Gfx_EnableState(0x10);
            color = g_pGDL->drawEntries[i].unk4;
            Gfx_SetBaseColor(&color);
        }
        m_curLightState = g_pGDL->drawEntries[i].lightState;
        m_curEnvmapState = g_pGDL->drawEntries[i].envmapState;
        Render_Geometry(geometry, matrix);
        if (g_pGDL->drawEntries[i].unk10) {
            Gfx_DisableState(0x10);
        }
    }
    for (i = 0; i < g_pGDL->numLODEntries; i++) {
        Geometry *lod = (Geometry *)g_pGDL->lodEntries[i].geometry;
        void *lodMatrix = Gfx_GetWorldMatrixState(g_pGDL->lodEntries[i].matrix);
        Gfx_DrawGeometryLOD_Immediate(lod, lodMatrix);
    }
}

void Render_Geometry(Geometry *geom, f32 *m) {
    void PSVECAdd(Vector *a, Vector *b, Vector *ab);
    void PSVECScale(Vector *src, Vector *dst, f32 scale);
    void PSVECSubtract(Vector *a, Vector *b, Vector *a_b);
    f32 PSVECDotProduct(Vector *a, Vector *b);
    f32 _fabs(f32 x);
    s32 Gfx_FrustrumCullSphereTest(Vector *center, f32 radius);
    void Gfx_DisableState(u32 state);
    void Gfx_DrawGeometryLOD_Immediate(Geometry *geometry, void *matrix);
    Vector *bmin = (Vector *)geom->bboxMin;
    Vector *bmax = (Vector *)geom->bboxMax;
    Vector min __attribute__((aligned(16)));
    Vector max __attribute__((aligned(16)));
    Vector center __attribute__((aligned(16)));
    f32 scale[4] __attribute__((aligned(16)));
    Vector out __attribute__((aligned(16)));
    f32 radius;
    u16 i;

    if (geom->unk4 == 0) {
        PSVECAdd((Vector *)&m[12], bmin, &min);
        PSVECAdd((Vector *)&m[12], bmax, &max);
        PSVECAdd(&min, &max, &center);
        PSVECScale(&center, &center, 0.5f);
        scale[0] = geom->bboxMin[3];
        scale[1] = geom->bboxMin[3];
        scale[2] = geom->bboxMin[3];
        scale[3] = 0.0f;
        XformVecOrientation((Vector *)m, (Vector *)scale, &out);
        radius = _fabs(out.x);
        out.y = _fabs(out.y);
        out.z = _fabs(out.z);
        if (out.y > radius) {
            radius = out.y;
        }
        if (out.z > radius) {
            radius = out.z;
        }
        switch (Gfx_FrustrumCullSphereTest(&center, radius)) {
        case 1:
            Gfx_EnableState(0x20000);
            break;
        case 2:
            Gfx_DisableState(0x20000);
            break;
        default:
            return;
        }
        PSVECSubtract(&center, (Vector *)((u8 *)GetRenderWindow() + 0x180), &min);
        radius = PSVECDotProduct(&min, (Vector *)((u8 *)GetRenderWindow() + 0x190));
        radius *= ((f32 *)GetRenderWindow())[0x9D];
        for (i = 0; i < geom->numGroups; i++) {
            if (radius < geom->lodDistances[i]) {
                break;
            }
        }
        if (geom->numGroups == 1 && geom->lodDistances[0] == 0.0f) {
            i = 0;
        }
        if (i < geom->numGroups) {
            Gfx_DrawGeometryLOD_Immediate((Geometry *)((u8 *)geom->groups + i * 0x18), m);
        }
        Gfx_EnableState(0x20000);
    }
}

#pragma pop
void RenderGroup(Geometry *geometry, u32 group, void *matrix);
void EnvMap(Geometry *geometry, u32 group, void *matrix, s32 mode);
void Render_DrawOpaqueGroup(Geometry *geometry, u32 group, void *matrix);
static inline void RenderOpaqueGroup(Geometry *geometry, u32 group, void *matrix) {
    extern u8 g_CurrentViews;
    Render_DrawOpaqueGroup(geometry, group, matrix);
    if ((geometry->groups[group].flags & 0x10) && g_CurrentViews == 1) {
        EnvMap(geometry, group, matrix, 2);
    }
}
void Gfx_DrawGeometryLOD_Immediate(Geometry *geometry, void *matrix) {
    extern f32 g_identity[];
    extern u32 m_fillMode;
    u16 i;

    if (geometry != NULL) {
        if (matrix == g_identity) {
            Gfx_SetWorldMatrix(0);
        } else {
            Gfx_SetWorldMatrix(matrix);
        }
        for (i = 0; i < geometry->numGroups; i++) {
            geometry->groups[i].flags |= m_fillMode;
            RenderGroup(geometry, i, matrix);
            geometry->groups[i].flags &= ~m_fillMode;
        }
    }
}

void RenderGroup(Geometry *geometry, u32 group, void *matrix) {
    extern GfxState g_state;
    void DrawGroupWireframe(Geometry *geometry, u32 group, void *matrix);
    GeometryGroup *groups;

    if (geometry->groups[group].flags & 0x4000) {
        return;
    }
    if (geometry->groups[group].flags & 0x80) {
        GXColor color = g_state.unk74;
        DisplayList_AddAlphaGroup((s32)geometry, group, 0, (g_state.unk7C >> 4) & 1, &color);
    } else if (geometry->groups[group].flags & 1) {
        GXColor color = g_state.unk74;
        DisplayList_AddAlphaGroup((s32)geometry, group, 0, (g_state.unk7C >> 4) & 1, &color);
    } else if (geometry->groups[group].flags & 2) {
        EnvMap(geometry, group, matrix, 1);
    } else if (geometry->groups[group].flags & 0x100) {
        DrawGroupWireframe(geometry, group, matrix);
    } else {
        RenderOpaqueGroup(geometry, group, matrix);
    }
}

#pragma push
#pragma use_lmw_stmw on
void EnvMap(Geometry *geometry, u32 group, void *matrix, s32 mode) {
    extern GDL *g_pGDL;
    extern s32 m_curEnvmapState;
    extern u32 g_IsAnim;
    extern u32 m_VtxDescFlag;
    void *GetRenderWindow();
    void Render_SetTextureHandle(u16 id);
    void Render_StateAlphaBlendEnable(s32 enable);
    void Render_StateBlendMode(u8 mode);
    void Render_SetConstColor(GXColor *color);
    void SetTriEnvVtxDescIdx(void);
    void SetVertexBuffer();
    void GXSetVtxDesc(s32 attr, s32 type);
    void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type);
    void GXSetTexCoordGen2(s32 dst, s32 func, s32 src, u32 mtx, u8 normalize, u32 pt);
    void DCFlushRange(void *addr, u32 len);
    void GXCallDisplayList(void *list, u32 nbytes);
    u32 opaque;
    GeometryGroup *grp;
    void *dl;
    GXColor color;
    f32 mtx[3][4];
    u8 blend;

    opaque = mode & 1;
    grp = &geometry->groups[group];
    dl = *(void **)((u8 *)grp + 0x1C);
    color.r = ((u8 *)grp)[4];
    color.g = ((u8 *)grp)[5];
    color.b = ((u8 *)grp)[6];
    color.a = ((u8 *)grp)[7];
    if (!opaque) {
        Render_SetTextureHandle(g_pGDL->envmapStates[m_curEnvmapState].unk8);
        blend = 5;
        if (mode & 2) {
            blend = 2;
        }
        Render_StateAlphaBlendEnable(1);
        Render_StateBlendMode(blend);
        MatrixToMtxTranspose(mtx, (u8 *)GetRenderWindow() + 0x80);
        color.r = ((u8 *)grp)[4] >> 2;
        color.g = ((u8 *)grp)[5] >> 2;
        color.b = ((u8 *)grp)[6] >> 2;
    } else {
        Render_SetTextureHandle(g_pGDL->envmapStates[m_curEnvmapState].unkA);
        MatrixToMtxTranspose(mtx, (u8 *)GetRenderWindow() + 0x80);
    }
    mtx[0][0] *= 0.5f;
    mtx[0][1] *= 0.5f;
    mtx[0][2] *= 0.5f;
    mtx[0][3] = 0.5f;
    mtx[1][0] *= 0.5f;
    mtx[1][1] *= 0.5f;
    mtx[1][2] *= 0.5f;
    mtx[1][3] = 0.5f;
    GXLoadTexMtxImm(mtx, 30, 1);
    GXSetTexCoordGen2(0, 1, 1, 30, 0, 125);
    Render_SetConstColor(&color);
    SetTriEnvVtxDescIdx();
    if (g_IsAnim) {
        GXSetVtxDesc(0, 1);
        m_VtxDescFlag = 0;
    }
    SetVertexBuffer(geometry->lodDistances, grp->flags, geometry->unk4);
    DCFlushRange(dl, *(u32 *)((u8 *)grp + 0x10));
    GXCallDisplayList(dl, *(u32 *)((u8 *)grp + 0x10));
    Render_SetConstColor(NULL);
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    if (!opaque) {
        Render_StateAlphaBlendEnable(0);
    }
}
#pragma pop
void Render_UseLighting(u8 arg0) {
    M2C_UNK GXSetChanCtrl(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK, s32, M2C_UNK, M2C_UNK);
    extern s32 m_pointLightMask_803C771C;
    if (arg0 != 0) {
        GXSetChanCtrl(4, 1, 0, 0, m_pointLightMask_803C771C | 1, 2, 2);
        return;
    }
    GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
}

void Render_UpdateLightState(u32 index) {
    extern GDL *g_pGDL;
    extern u8 m_totalLights_803C7718;
    extern u32 m_pointLightMask_803C771C;
    void *GetRenderWindow();
    void InverseXformVecOrientation(Vector *m, Vector *v, Vector *out);
    void GXInitLightPos(void *lt_obj, f32 x, f32 y, f32 z);
    void GXInitLightColor(void *lt_obj, GXColor color);
    void GXLoadLightObjImm(void *lt_obj, s32 light);
    void GXSetChanAmbColor(s32 chan, GXColor amb_color);
    void GXInitLightDistAttn(void *lt_obj, f32 ref_dist, f32 ref_br, s32 dist_func);
    void GXInitLightAttnK(void *lt_obj, f32 k0, f32 k1, f32 k2);
    u8 lightObj[0x40];
    u8 pointObj[0x40];
    Vector dir;
    GXColor ambient;
    u8 i;
    GDLLightState *ls;

    if (index == 0xFFFFFFFF) {
        index = 0;
    }
    ls = &g_pGDL->lightStates[index];
    {
        GXColor color;
        color.r = ls->lightColor.r;
        color.g = ls->lightColor.g;
        color.b = ls->lightColor.b;
        color.a = 0xFF;
        memset(lightObj, 0, 0x40);
        InverseXformVecOrientation((Vector *)GetRenderWindow(), &ls->lightDir, &dir);
        GXInitLightPos(lightObj, -10000.0f * dir.x, 10000.0f * dir.y, -10000.0f * dir.z);
        GXInitLightColor(lightObj, color);
        GXLoadLightObjImm(lightObj, 1);
    }
    ambient.r = ls->ambientColor.r;
    ambient.g = ls->ambientColor.g;
    ambient.b = ls->ambientColor.b;
    ambient.a = 0xFF;
    GXSetChanAmbColor(4, ambient);
    m_pointLightMask_803C771C = 0;
    m_totalLights_803C7718 = 0;
    for (i = 0; i < ls->numPointLights; i++) {
        if (m_totalLights_803C7718 <= 4) {
            GXColor pointColor;
            m_totalLights_803C7718++;
            pointColor.r = ls->pointColors[i].r;
            pointColor.g = ls->pointColors[i].g;
            pointColor.b = ls->pointColors[i].b;
            pointColor.a = 0xFF;
            memset(pointObj, 0, 0x40);
            GXInitLightPos(pointObj, ls->pointLights[i].x, ls->pointLights[i].y, ls->pointLights[i].z);
            GXInitLightColor(pointObj, pointColor);
            GXInitLightDistAttn(pointObj, ls->pointLights[i].w, 0.99999f, 1);
            GXInitLightAttnK(pointObj, 0.0f, 0.0f, 0.01f);
            GXLoadLightObjImm(pointObj, 1 << m_totalLights_803C7718);
            m_pointLightMask_803C771C |= 1 << m_totalLights_803C7718;
        }
    }
}

#pragma push
#pragma dont_inline on
void Render_DrawSpheres(void) {
    extern s32 m_cullMode;
    extern GDL *g_pGDL;
    extern GDLMatrix g_identity;
    void Render_SetTextureHandle(s32 handle);
    void GXDrawSphere(s32 numMajor, s32 numMinor);
    u32 i;
    s32 cullMode;
    GDLMatrix mtx;

    cullMode = m_cullMode;
    Render_SetTextureHandle(0xFFFF);
    Render_StateCullMode(2);
    mtx = g_identity;
    for (i = 0; i < g_pGDL->numSpheres; i++) {
        mtx.m[12] = g_pGDL->spheres[i].x;
        mtx.m[13] = g_pGDL->spheres[i].y;
        mtx.m[14] = g_pGDL->spheres[i].z;
        mtx.m[0] = mtx.m[5] = mtx.m[10] = g_pGDL->spheres[i].radius;
        Gfx_SetWorldMatrix(mtx.m);
        GXDrawSphere(8, 16);
    }
    Render_StateCullMode(cullMode);
}
#pragma pop
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

void Render_Draw3DLines(void) {
    extern GDL *g_pGDL;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void Gfx_SetWorldMatrix(void *matrix);
    void Render_SetTextureHandle(s32 handle);
    void Render_StateAlphaBlendEnable(s32 enable);
    void Set3DLineVtxDesc(void);
    u32 i;

    Gfx_SetWorldMatrix(NULL);
    Render_SetTextureHandle(0xFFFF);
    Render_StateAlphaBlendEnable(0);
    Set3DLineVtxDesc();
    GXBegin(0xA8, 0, g_pGDL->num3DLines * 2);
    for (i = 0; i < g_pGDL->num3DLines; i++) {
        GXPosition3f32(g_pGDL->lines3D[i].p0.x, g_pGDL->lines3D[i].p0.y, g_pGDL->lines3D[i].p0.z);
        GXColor3u8(g_pGDL->lines3D[i].r, g_pGDL->lines3D[i].g, g_pGDL->lines3D[i].b);
        GXPosition3f32(g_pGDL->lines3D[i].p1.x, g_pGDL->lines3D[i].p1.y, g_pGDL->lines3D[i].p1.z);
        GXColor3u8(g_pGDL->lines3D[i].r, g_pGDL->lines3D[i].g, g_pGDL->lines3D[i].b);
    }
}

void Render_Draw3DFXLines(void) {
    extern GDL *g_pGDL;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void Gfx_SetWorldMatrix(void *matrix);
    void Render_SetTextureHandle(s32 handle);
    void Render_StateBlendMode(u8 mode);
    void Set3DLineVtxDesc(void);
    u32 i;

    Gfx_SetWorldMatrix(NULL);
    Render_SetTextureHandle(0xFFFF);
    Set3DLineVtxDesc();
    Render_StateBlendMode(g_pGDL->lines[0].blendMode);
    for (i = 0; i < g_pGDL->numLines; i++) {
        GXBegin(0xA8, 0, 2);
        GXPosition3f32(g_pGDL->lines[i].p0.x, g_pGDL->lines[i].p0.y, g_pGDL->lines[i].p0.z);
        GXColor3u8(g_pGDL->lines[i].c0.r, g_pGDL->lines[i].c0.g, g_pGDL->lines[i].c0.b);
        GXPosition3f32(g_pGDL->lines[i].p1.x, g_pGDL->lines[i].p1.y, g_pGDL->lines[i].p1.z);
        GXColor3u8(g_pGDL->lines[i].c1.r, g_pGDL->lines[i].c1.g, g_pGDL->lines[i].c1.b);
    }
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

#pragma push
#pragma dont_inline on
void Gfx_SetWorldMatrix(f32 *matrix) {
    extern GfxState g_state;
    extern f32 g_identity[];
    extern f32 GXCameraSpace_8036CF80[3][4];
    void GXLoadPosMtxImm(f32 m[3][4], u32 id);
    void GXSetCurrentMtx(u32 id);

    if (matrix == NULL) {
        if (g_state.unk93) {
            return;
        }
        g_state.unk93 = 1;
        matrix = g_identity;
    } else {
        g_state.unk93 = 0;
    }
    memcpy(GetRenderWindow(), matrix, 0x40);
    MultiplyMatrix4x4(GetRenderWindow(), (u8 *)GetRenderWindow() + 0x40, (u8 *)GetRenderWindow() + 0x80);
    MultiplyMatrix4x4((u8 *)GetRenderWindow() + 0x80, (u8 *)GetRenderWindow() + 0xC0, (u8 *)GetRenderWindow() + 0x100);
    MatrixToMtxTranspose(GXCameraSpace_8036CF80, (u8 *)GetRenderWindow() + 0x80);
    GXLoadPosMtxImm(GXCameraSpace_8036CF80, 0);
    GXSetCurrentMtx(0);
}

void Gfx_SetProjectionMatrix(f32 fov) {
    void C_MTXPerspective(f32 m[4][4], f32 fovY, f32 aspect, f32 n, f32 f);
    void GXSetProjection(f32 m[4][4], s32 type);
    void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
    f32 Gfx_GetProjectionMatrix(void);
    f32 mtx[4][4];
    f32 value;
    f32 angle;

    angle = fov;
    value = sin(0.017453292f * (0.9f * fov));
    *(f32 *)((u8 *)GetRenderWindow() + 0x274) = value;
    *(f32 *)((u8 *)GetRenderWindow() + 0x270) = fov;
    fov = 90.0f / fov;
    value = -(fov * *(u32 *)((u8 *)GetRenderWindow() + 0x2B4));
    *(f32 *)((u8 *)GetRenderWindow() + 0xC0) = value;
    value = 1.333f * (fov * *(u32 *)((u8 *)GetRenderWindow() + 0x2BC));
    *(f32 *)((u8 *)GetRenderWindow() + 0xD4) = value;
    value = 1.0f;
    *(f32 *)((u8 *)GetRenderWindow() + 0xEC) = value;
    value = 20000.0f;
    *(f32 *)((u8 *)GetRenderWindow() + 0xF8) = value;
    value = *(u32 *)((u8 *)GetRenderWindow() + 0x2B4);
    *(f32 *)((u8 *)GetRenderWindow() + 0xE0) = value;
    value = *(u32 *)((u8 *)GetRenderWindow() + 0x2BC);
    *(f32 *)((u8 *)GetRenderWindow() + 0xE4) = value;
    Gfx_SetCameraMatrix((u8 *)GetRenderWindow() + 0x180, (u8 *)GetRenderWindow() + 0x190, (u8 *)GetRenderWindow() + 0x1A0, (u8 *)GetRenderWindow() + 0x1B0);
    value = 1.0 / sin(0.008726646f * Gfx_GetProjectionMatrix());
    *(f32 *)((u8 *)GetRenderWindow() + 0x2CC) = value;
    value = *(u32 *)((u8 *)GetRenderWindow() + 0x284);
    value = *(u32 *)((u8 *)GetRenderWindow() + 0x280) / value;
    fov = *(u32 *)((u8 *)GetRenderWindow() + 0x280);
    C_MTXPerspective(mtx, 1.0714285f * angle * *(u32 *)((u8 *)GetRenderWindow() + 0x284) / fov, value, 1.0f, 20000.0f);
    GXSetProjection(mtx, 0);
}

#pragma pop

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

void Render_StateSetScissor(void);
void GXGetViewportv(f32 *vp);
void GXGetProjectionv(f32 *p);
void GXProject(f32 x, f32 y, f32 z, f32 mtx[3][4], f32 *pm, f32 *vp, f32 *sx, f32 *sy, f32 *sz);
void Gfx_TransformVector(Vector *in, f32 *out) {
    extern f32 GXCameraSpace_8036CF80[3][4];
    f32 pm[7];
    f32 vp[6];

    Render_StateSetScissor();
    GXGetViewportv(vp);
    GXGetProjectionv(pm);
    vp[3] *= 1.0714f;
    GXProject(in->x, in->y, in->z, GXCameraSpace_8036CF80, pm, vp, &out[0], &out[1], &out[2]);
    out[3] = -(*(f32 *)((u8 *)GetRenderWindow() + 0x13C) +
               (in->z * *(f32 *)((u8 *)GetRenderWindow() + 0x12C) +
                (in->x * *(f32 *)((u8 *)GetRenderWindow() + 0x10C) +
                 in->y * *(f32 *)((u8 *)GetRenderWindow() + 0x11C))));
}

#pragma pop

#pragma push
#pragma use_lmw_stmw on
void Render_Print(void) {
    extern u8 g_screenText[0xA00];
    extern GfxState g_state;
    u32 FontManager_Print(char *str, f32 x, f32 y, u16 font);
    f32 y;
    s32 i;
    s32 j;
    char buf[88];

    y = 10.0f;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 80; j++) {
            buf[j] = g_screenText[i * 80 + j];
        }
        buf[j] = 0;
        y += FontManager_Print(buf, 10.0f, y, g_state.debugFont);
    }
}
#pragma pop

void Render_DrawPointSprites(void) {
    extern GDL *g_pGDL;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void GXSetPointSize(u8 size, s32 fmt);
    void Render_SetTextureHandle(s32 handle);
    void Render_StateBlendMode(u8 mode);
    void Render_StateAlphaBlendEnable(s32 enable);
    void SetTriVtxDesc(void);
    u32 i;
    u32 color;

    SetTriVtxDesc();
    Render_StateAlphaBlendEnable(1);
    for (i = 0; i < g_pGDL->numPointSprites; i++) {
        Render_SetTextureHandle(g_pGDL->pointSprites[i].tex);
        Render_StateBlendMode(g_pGDL->pointSprites[i].blendMode);
        GXSetPointSize(6.0f * g_pGDL->pointSprites[i].size, 5);
        GXBegin(0xB8, 0, 1);
        GXPosition3f32(g_pGDL->pointSprites[i].x, g_pGDL->pointSprites[i].y, g_pGDL->pointSprites[i].z);
        color = g_pGDL->pointSprites[i].color;
        GXColor4u8(color >> 24, color >> 16, color >> 8, color);
        GXTexCoord2f32(0.0f, 0.0f);
    }
    Render_StateAlphaBlendEnable(0);
}

void Render_DrawAlphaTriangle(u16 index) {
    extern GDL *g_pGDL;
    extern s32 g_debugTriCount;
    extern GfxState g_state;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void Render_SetTextureHandle(u16 id);
    void Render_StateBlendMode(s32 mode);
    void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
    void SetTriFlatVtxDesc(void);
    void SetTriVtxDesc(void);

    g_debugTriCount++;
    Render_StateBlendMode(g_pGDL->tris[index].blendMode);
    Render_SetTextureHandle(g_pGDL->tris[index].tex);
    Gfx_SetCameraMatrix(NULL, NULL, NULL, NULL);
    if (g_state.unk84 == 0xFFFF) {
        SetTriFlatVtxDesc();
        GXBegin(0x90, 0, 3);
        GXPosition3f32(g_pGDL->tris[index].v[0].x, g_pGDL->tris[index].v[0].y, g_pGDL->tris[index].v[0].z);
        GXColor4u8(g_pGDL->tris[index].v[0].r, g_pGDL->tris[index].v[0].g, g_pGDL->tris[index].v[0].b, g_pGDL->tris[index].v[0].a);
        GXPosition3f32(g_pGDL->tris[index].v[1].x, g_pGDL->tris[index].v[1].y, g_pGDL->tris[index].v[1].z);
        GXColor4u8(g_pGDL->tris[index].v[1].r, g_pGDL->tris[index].v[1].g, g_pGDL->tris[index].v[1].b, g_pGDL->tris[index].v[1].a);
        GXPosition3f32(g_pGDL->tris[index].v[2].x, g_pGDL->tris[index].v[2].y, g_pGDL->tris[index].v[2].z);
        GXColor4u8(g_pGDL->tris[index].v[2].r, g_pGDL->tris[index].v[2].g, g_pGDL->tris[index].v[2].b, g_pGDL->tris[index].v[2].a);
    } else {
        SetTriVtxDesc();
        GXBegin(0x90, 0, 3);
        GXPosition3f32(g_pGDL->tris[index].v[0].x, g_pGDL->tris[index].v[0].y, g_pGDL->tris[index].v[0].z);
        GXColor4u8(g_pGDL->tris[index].v[0].r, g_pGDL->tris[index].v[0].g, g_pGDL->tris[index].v[0].b, g_pGDL->tris[index].v[0].a);
        GXTexCoord2f32(g_pGDL->tris[index].v[0].s, g_pGDL->tris[index].v[0].t);
        GXPosition3f32(g_pGDL->tris[index].v[1].x, g_pGDL->tris[index].v[1].y, g_pGDL->tris[index].v[1].z);
        GXColor4u8(g_pGDL->tris[index].v[1].r, g_pGDL->tris[index].v[1].g, g_pGDL->tris[index].v[1].b, g_pGDL->tris[index].v[1].a);
        GXTexCoord2f32(g_pGDL->tris[index].v[1].s, g_pGDL->tris[index].v[1].t);
        GXPosition3f32(g_pGDL->tris[index].v[2].x, g_pGDL->tris[index].v[2].y, g_pGDL->tris[index].v[2].z);
        GXColor4u8(g_pGDL->tris[index].v[2].r, g_pGDL->tris[index].v[2].g, g_pGDL->tris[index].v[2].b, g_pGDL->tris[index].v[2].a);
        GXTexCoord2f32(g_pGDL->tris[index].v[2].s, g_pGDL->tris[index].v[2].t);
    }
}

void Render_ScreenFlash(void) {
    void *GetRenderWindow();
    void Render_StateZWriteEnable(s32 enable);
    void Render_StateAlphaBlendEnable(s32 enable);
    void Render_StateBlendMode(u32 mode);
    void Render_SetTextureHandle(u16 id);
    void Set2DRectFlatVtxDesc(void);
    void GXBegin(s32 type, s32 fmt, u16 nverts);
    f32 w;
    f32 zero = 0.0f;

    if (*(u32 *)((u8 *)GetRenderWindow() + 0x2C4) != 0) {
        Render_StateZWriteEnable(0);
        Render_StateAlphaBlendEnable(1);
        Render_StateBlendMode(*(u32 *)((u8 *)GetRenderWindow() + 0x2C4));
        Render_SetTextureHandle(0xFFFF);
        Set2DRectFlatVtxDesc();
        GXBegin(0x80, 0, 4);
        GXPosition2f32(zero, zero);
        GXColor4u8(((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->r, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->g,
                   ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->b, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->a);
        GXPosition2f32(zero, *(u32 *)((u8 *)GetRenderWindow() + 0x2B8));
        GXColor4u8(((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->r, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->g,
                   ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->b, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->a);
        w = *(u32 *)((u8 *)GetRenderWindow() + 0x2B0);
        GXPosition2f32(w, *(u32 *)((u8 *)GetRenderWindow() + 0x2B8));
        GXColor4u8(((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->r, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->g,
                   ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->b, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->a);
        GXPosition2f32(*(u32 *)((u8 *)GetRenderWindow() + 0x2B0), zero);
        GXColor4u8(((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->r, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->g,
                   ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->b, ((GXColor *)((u8 *)GetRenderWindow() + 0x2C0))->a);
        Render_StateZWriteEnable(1);
        Render_StateAlphaBlendEnable(0);
    }
}

#pragma push
#pragma dont_inline on
void Render_DrawRectanglesWindow(void) {
    extern GDL *g_pGDL;
    extern u8 g_CurrentViews;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void Gfx_SetRenderWindow(s32 window);
    void Render_StateSetScissor(void);
    u32 i;
    u32 window = 0;
    u32 j;

    if (g_pGDL->numRects == 0) {
        return;
    }
    Set2DRectVtxDesc();
    for (i = 0; i < g_pGDL->numRects; i++) {
        if (g_CurrentViews != 1 && window != g_pGDL->rects[i].window) {
            window = g_pGDL->rects[i].window;
            Gfx_SetRenderWindow(window);
            Render_StateSetScissor();
            Gfx_SetOrthoMatrix();
        }
        Render_StateAlphaBlendEnable(g_pGDL->rects[i].blendMode > 1);
        Render_SetTextureHandle(g_pGDL->rects[i].tex);
        Render_StateBlendMode(g_pGDL->rects[i].blendMode);
        GXBegin(0x80, 0, 4);
        for (j = 0; j < 4; j++) {
            {
                f32 y = g_pGDL->rects[i].v[j].y;
                f32 x = g_pGDL->rects[i].v[j].x;
                GXWGFifo.f32 = x;
                GXWGFifo.f32 = y;
            }
            {
                u8 a = g_pGDL->rects[i].v[j].a;
                u8 b = g_pGDL->rects[i].v[j].b;
                u8 g = g_pGDL->rects[i].v[j].g;
                u8 c = g_pGDL->rects[i].v[j].r;
                GXWGFifo.u8 = c;
                GXWGFifo.u8 = g;
                GXWGFifo.u8 = b;
                GXWGFifo.u8 = a;
            }
            {
                f32 t = g_pGDL->rects[i].v[j].t;
                f32 s = g_pGDL->rects[i].v[j].s;
                GXWGFifo.f32 = s;
                GXWGFifo.f32 = t;
            }
        }
    }
    if (window != 0) {
        Gfx_SetRenderWindow(0);
        Render_StateSetScissor();
        Gfx_SetOrthoMatrix();
    }
    Render_StateAlphaBlendEnable(1);
}
#pragma pop

#pragma push
#pragma dont_inline on
void Render_DrawRectangles(void) {
    extern GDL *g_pGDL;
    extern s32 g_debugTriCount;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    u32 i;
    u32 j;
    s32 blend;

    if (g_pGDL->numRects2D == 0) {
        return;
    }
    g_debugTriCount += g_pGDL->numRects2D * 2;
    Render_StateZWriteEnable(0);
    Set2DRectVtxDesc();
    for (i = 0; i < g_pGDL->numRects2D; i++) {
        blend = g_pGDL->rects2D[i].blendMode;
        Render_StateAlphaBlendEnable(blend > 1);
        Render_StateBlendMode(blend);
        Render_SetTextureHandle(g_pGDL->rects2D[i].tex);
        GXBegin(0x80, 0, 4);
        for (j = 0; j < 4; j++) {
            {
                f32 y = g_pGDL->rects2D[i].v[j].y;
                f32 x = g_pGDL->rects2D[i].v[j].x;
                GXWGFifo.f32 = x;
                GXWGFifo.f32 = y;
            }
            {
                u8 a = g_pGDL->rects2D[i].v[j].a;
                u8 b = g_pGDL->rects2D[i].v[j].b;
                u8 g = g_pGDL->rects2D[i].v[j].g;
                u8 c = g_pGDL->rects2D[i].v[j].r;
                GXWGFifo.u8 = c;
                GXWGFifo.u8 = g;
                GXWGFifo.u8 = b;
                GXWGFifo.u8 = a;
            }
            {
                f32 t = g_pGDL->rects2D[i].v[j].t;
                f32 s = g_pGDL->rects2D[i].v[j].s;
                GXWGFifo.f32 = s;
                GXWGFifo.f32 = t;
            }
        }
    }
    Render_StateAlphaBlendEnable(1);
}
#pragma pop

#pragma push
#pragma dont_inline on
void Render_DrawRectangle(void *rect) {
    typedef struct RectVertex {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
        f32 u;
        f32 v;
        u8 r;
        u8 g;
        u8 b;
        u8 a;
        u8 pad[4];
    } RectVertex;
    typedef struct Rectangle {
        RectVertex v[4];
        u16 texture;
        u8 blendMode;
    } Rectangle;
    extern s32 m_cullMode;
    extern s32 g_debugTriCount;
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    void Render_LoadCamera(void);
    s32 cullMode;
    Rectangle *r = rect;
    u32 i;

    cullMode = m_cullMode;
    g_debugTriCount += 2;
    Render_StateCullMode(0);
    Render_StateAlphaBlendEnable(r->blendMode > 1);
    Render_SaveCamera();
    Gfx_SetOrthoMatrix();
    Render_StateZWriteEnable(0);
    Render_StateBlendMode(r->blendMode);
    Render_SetTextureHandle(r->texture);
    Set2DRectVtxDesc();
    GXBegin(0x80, 0, 4);
    for (i = 0; i < 4; i++) {
        {
            f32 y = r->v[i].y;
            f32 x = r->v[i].x;
            GXWGFifo.f32 = x;
            GXWGFifo.f32 = y;
        }
        {
            u8 a = r->v[i].a;
            u8 b = r->v[i].b;
            u8 g = r->v[i].g;
            u8 c = r->v[i].r;
            GXWGFifo.u8 = c;
            GXWGFifo.u8 = g;
            GXWGFifo.u8 = b;
            GXWGFifo.u8 = a;
        }
        {
            f32 t = r->v[i].v;
            f32 s = r->v[i].u;
            GXWGFifo.f32 = s;
            GXWGFifo.f32 = t;
        }
    }
    if (r->blendMode != 0) {
        Render_StateAlphaBlendEnable(0);
    }
    Render_StateZWriteEnable(1);
    Render_StateCullMode(cullMode);
    Render_LoadCamera();
}
#pragma pop

void Gfx_SetTextColor(s8 arg0, s8 arg1, s8 arg2, s8 arg3) {
    extern M2C_UNK g_textcolor;
    M2C_FIELD(&g_textcolor, s8 *, 0) = arg0;
    M2C_FIELD(&g_textcolor, s8 *, 1) = arg1;
    M2C_FIELD(&g_textcolor, s8 *, 2) = arg2;
    M2C_FIELD(&g_textcolor, s8 *, 3) = arg3;
}

s32 CreateVideoDecodeThread(s32 priority, void *param) {
    typedef struct VideoDecodeData {
        u8 thread[0x310];
        u8 stack[0x1000];
        u8 freeQueue[0x20];
        void *freeMessage[3];
        u8 decodedQueue[0x20];
        void *decodedMessage[3];
    } VideoDecodeData;
    s32 OSCreateThread(void *thread, void *func, void *param, void *stack, u32 stackSize, s32 priority, u16 attr);
    void OSInitMessageQueue(void *queue, void *msgArray, s32 msgCount);
    void VideoDecoderForOnMemory(void *bufAddr);
    void *VideoDecoder(void *param);
    extern u8 VideoDecodeThread_8036CFF0[0x310];
    extern s32 VideoDecodeThreadCreated_803C7728;
    extern s32 First_803C772C;
    VideoDecodeData *data = (VideoDecodeData *)VideoDecodeThread_8036CFF0;

    if (param) {
        if (OSCreateThread(data->thread, VideoDecoderForOnMemory, param, data->stack + 0x1000, 0x1000, priority, 1) == 0) {
            return 0;
        }
    } else {
        if (OSCreateThread(data->thread, VideoDecoder, NULL, data->stack + 0x1000, 0x1000, priority, 1) == 0) {
            return 0;
        }
    }
    OSInitMessageQueue(data->freeQueue, data->freeMessage, 3);
    OSInitMessageQueue(data->decodedQueue, data->decodedMessage, 3);
    VideoDecodeThreadCreated_803C7728 = 1;
    First_803C772C = 1;
    return 1;
}

void VideoDecodeThreadStart(void) {
    extern s32 VideoDecodeThreadCreated_803C7728;
    extern u8 VideoDecodeThread_8036CFF0[0x310];
    if (VideoDecodeThreadCreated_803C7728) {
        OSResumeThread(VideoDecodeThread_8036CFF0);
    }
}

void VideoDecodeThreadCancel(void) {
    extern s32 VideoDecodeThreadCreated_803C7728;
    extern u8 VideoDecodeThread_8036CFF0[0x310];
    if (VideoDecodeThreadCreated_803C7728) {
        OSCancelThread(VideoDecodeThread_8036CFF0);
        VideoDecodeThreadCreated_803C7728 = 0;
    }
}

void *VideoDecoder(void *param) {
    void VideoDecode(THPReadBuffer *readBuffer);
    void *PopReadedBuffer(void);
    void *PopReadedBuffer2(void);
    void PushFreeReadBuffer(void *arg0);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    THPReadBuffer *readBuffer;
    s32 old;
    u32 curFrame;

    while (TRUE) {
        if (ActivePlayer.audioExist) {
            while (ActivePlayer.videoDecodeCount < 0) {
                readBuffer = PopReadedBuffer2();
                curFrame = (readBuffer->frameNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
                if (curFrame == ActivePlayer.numFrames - 1 && !(ActivePlayer.playFlag & 1)) {
                    VideoDecode(readBuffer);
                }
                PushFreeReadBuffer(readBuffer);
                old = OSDisableInterrupts();
                ActivePlayer.videoDecodeCount++;
                OSRestoreInterrupts(old);
            }
        }
        if (ActivePlayer.audioExist) {
            readBuffer = PopReadedBuffer2();
        } else {
            readBuffer = PopReadedBuffer();
        }
        VideoDecode(readBuffer);
        PushFreeReadBuffer(readBuffer);
    }
}

void VideoDecoderForOnMemory(void *bufAddr) {
    void VideoDecode(THPReadBuffer *readBuffer);
    void OSSuspendThread(void *thread);
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    extern u8 VideoDecodeThread_8036CFF0[0x310];
    THPReadBuffer readBuffer;
    s32 tmp;
    s32 size;
    s32 readFrame;
    s32 old;

    size = ActivePlayer.initReadSize;
    readBuffer.ptr = bufAddr;
    readFrame = 0;
    while (1) {
        if (ActivePlayer.audioExist) {
            while (ActivePlayer.videoDecodeCount < 0) {
                old = OSDisableInterrupts();
                ActivePlayer.videoDecodeCount++;
                OSRestoreInterrupts(old);
                tmp = (readFrame + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
                if (tmp == ActivePlayer.numFrames - 1) {
                    if (ActivePlayer.playFlag & 1) {
                        size = *(s32 *)readBuffer.ptr;
                        readBuffer.ptr = ActivePlayer.movieData;
                    } else {
                        break;
                    }
                } else {
                    tmp = *(s32 *)readBuffer.ptr;
                    readBuffer.ptr += size;
                    size = tmp;
                }
                readFrame++;
            }
        }
        readBuffer.frameNumber = readFrame;
        VideoDecode(&readBuffer);
        tmp = (readFrame + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
        if (tmp == ActivePlayer.numFrames - 1) {
            if (ActivePlayer.playFlag & 1) {
                size = *(s32 *)readBuffer.ptr;
                readBuffer.ptr = ActivePlayer.movieData;
            } else {
                OSSuspendThread(VideoDecodeThread_8036CFF0);
            }
        } else {
            tmp = *(s32 *)readBuffer.ptr;
            readBuffer.ptr += size;
            size = tmp;
        }
        readFrame++;
    }
}

#pragma push
#pragma use_lmw_stmw on
void VideoDecode(THPReadBuffer *readBuffer) {
    typedef struct THPTextureSet {
        u8 *ytexture;
        u8 *utexture;
        u8 *vtexture;
        s32 frameNumber;
    } THPTextureSet;
    void *PopFreeTextureSet(void);
    void PushDecodedTextureSet(void *arg0);
    s32 THPVideoDecode(void *file, void *tileY, void *tileU, void *tileV, void *work);
    void PrepareReady(void *arg0);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    s32 OSSuspendThread(void *thread);
    extern u8 VideoDecodeThread_8036CFF0[0x310];
    extern s32 First_803C772C;
    THPTextureSet *textureSet;
    s32 old;
    u32 i;
    u32 *compSizePtr;
    u8 *ptr;

    ptr = readBuffer->ptr + ActivePlayer.numComponents * 4 + 8;
    compSizePtr = (u32 *)(readBuffer->ptr + 8);
    textureSet = PopFreeTextureSet();
    for (i = 0; i < ActivePlayer.numComponents; i++) {
        switch (ActivePlayer.frameComp[i]) {
        case 0:
            *(s32 *)&ActivePlayer.playAudioBuffer = THPVideoDecode(ptr, textureSet->ytexture, textureSet->utexture, textureSet->vtexture, *(void **)&ActivePlayer.unk88[0xC]);
            if (*(s32 *)&ActivePlayer.playAudioBuffer) {
                if (First_803C772C) {
                    PrepareReady((void *)0);
                    First_803C772C = 0;
                }
                OSSuspendThread(VideoDecodeThread_8036CFF0);
            }
            textureSet->frameNumber = readBuffer->frameNumber;
            PushDecodedTextureSet(textureSet);
            old = OSDisableInterrupts();
            ActivePlayer.videoDecodeCount++;
            OSRestoreInterrupts(old);
            break;
        }
        ptr += *compSizePtr;
        compSizePtr++;
    }
    if (First_803C772C) {
        PrepareReady((void *)1);
        First_803C772C = 0;
    }
}
#pragma pop

void *PopFreeTextureSet(void) {
    extern u8 FreeTextureSetQueue_8036E300[0x20];
    void *msg;

    OSReceiveMessage(FreeTextureSetQueue_8036E300, &msg, 1);
    return msg;
}

void PushFreeTextureSet(void *arg0) {
    extern u8 FreeTextureSetQueue_8036E300[0x20];
    OSSendMessage(FreeTextureSetQueue_8036E300, arg0, 0);
}

void *PopDecodedTextureSet(s32 arg0) {
    extern u8 DecodedTextureSetQueue_8036E32C[0x20];
    void *msg;

    if (OSReceiveMessage(DecodedTextureSetQueue_8036E32C, &msg, arg0) == 1) {
        return msg;
    }
    return NULL;
}

void PushDecodedTextureSet(void *arg0) {
    extern u8 DecodedTextureSetQueue_8036E32C[0x20];
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

#pragma push
#pragma use_lmw_stmw on
void THPGXYuv2RgbDraw(u32 *yImage, u32 *uImage, u32 *vImage, s16 x, s16 y, s16 textureWidth, s16 textureHeight, s16 polygonWidth, s16 polygonHeight) {
    typedef struct {
        u32 dummy[8];
    } GXTexObj;
    void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, s32 format, s32 wrap_s, s32 wrap_t, u8 mipmap);
    void GXInitTexObjLOD(GXTexObj *obj, s32 min_filt, s32 mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, u8 bias_clamp, u8 do_edge_lod, s32 max_aniso);
    void GXLoadTexObj(GXTexObj *obj, s32 id);
    void GXBegin(s32 type, s32 vtxfmt, u16 nverts);
    GXTexObj tobj0;
    GXTexObj tobj1;
    GXTexObj tobj2;

    GXInitTexObj(&tobj0, yImage, (u16)textureWidth, (u16)textureHeight, 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj0, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj0, 0);
    GXInitTexObj(&tobj1, uImage, (u16)(textureWidth >> 1), (u16)(textureHeight >> 1), 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj1, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj1, 1);
    GXInitTexObj(&tobj2, vImage, (u16)(textureWidth >> 1), (u16)(textureHeight >> 1), 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj2, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&tobj2, 2);
    GXBegin(0x80, 7, 4);
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 1;
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 1;
}
#pragma pop

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

u32 THPPlayerCalcNeedMemory(void);
BOOL THPPlayerGetVideoInfo(THPVideoInfo *videoInfo);
BOOL THPPlayerPlay(void);
s32 Mpeg_StartMovie(char *name, f32 volume) {
    typedef struct {
        u32 viTVmode;
        u16 fbWidth;
        u16 efbHeight;
    } RenderMode;
    BOOL THPPlayerOpen(char *fileName, BOOL onMemory);
    BOOL THPPlayerSetBuffer(void *buffer);
    BOOL THPPlayerPrepare(s32 frameNum, s32 playFlag);
    void *OSAllocFromHeap(s32 heap, u32 size);
    void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
    void OSPanic(const char *file, int line, const char *msg, ...);
    extern u8 badMojo_803C7734;
    extern RenderMode *m_rendermode_803C7754;
    extern THPVideoInfo m_videoInfo_803C7744;
    extern u32 m_movie_x_803C7740;
    extern u32 m_movie_y_803C773C;
    extern void *m_movie_buffer_803C774C;
    extern u32 m_movieVolume_803C7738;
    char *p = name;
    u32 size;

    if (badMojo_803C7734) {
        return 0;
    }
    for (; *p; p++) {
        *p = tolower(*p);
        if (*p == '\\') {
            *p = '/';
        }
    }
    if (!THPPlayerOpen(name, FALSE)) {
        OSPanic("THPPlay.c", 93, "Fail to open the thp file\n");
    }
    GXSetViewport(0.0f, 0.0f, m_rendermode_803C7754->fbWidth, m_rendermode_803C7754->efbHeight, 0.0f, 1.0f);
    THPPlayerGetVideoInfo(&m_videoInfo_803C7744);
    m_movie_x_803C7740 = (m_rendermode_803C7754->fbWidth - m_videoInfo_803C7744.xSize) / 2;
    m_movie_y_803C773C = (m_rendermode_803C7754->efbHeight - m_videoInfo_803C7744.ySize) / 2;
    size = THPPlayerCalcNeedMemory();
    m_movie_buffer_803C774C = OSAllocFromHeap(__OSCurrHeap, size);
    if (m_movie_buffer_803C774C == NULL) {
        OSPanic("THPPlay.c", 113, "Can't allocate the memory\n");
    }
    THPPlayerSetBuffer(m_movie_buffer_803C774C);
    if (!THPPlayerPrepare(0, 0)) {
        OSPanic("THPPlay.c", 122, "Fail to prepare\n");
    }
    THPPlayerPlay();
    m_movieVolume_803C7738 = 46.0f * volume;
}

#pragma push
#pragma dont_inline on
void Mpeg_StopMovie(void) {
    void *AIRegisterDMACallback(void (*callback)(void));
    void THPPlayerStop(void);
    BOOL THPPlayerClose(void);
    void THPPlayerQuit(void);
    extern u8 badMojo_803C7734;
    extern s32 m_rendermode_803C7754;
    extern void *audio_restore_cb;
    extern s32 m_movie_buffer_803C774C;
    u8 *rmode;

    if (badMojo_803C7734 == 0) {
        THPPlayerStop();
        THPPlayerClose();
        GXSetNumChans(1);
        GXSetNumTexGens(1);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevOp(0, 0);
        GXSetDispCopyGamma(1);
        rmode = (u8 *)m_rendermode_803C7754;
        GXSetCopyFilter(rmode[0x19], rmode + 0x1A, 1, rmode + 0x32);
        AIRegisterDMACallback(audio_restore_cb);
        THPPlayerQuit();
        VIWaitForRetrace();
        Render_FlipBuffers();
        GXSetPixelFmt(1, 0);
        OSFreeToHeap(__OSCurrHeap, m_movie_buffer_803C774C);
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
        Gfx_BeginScene();
        Gfx_EndScene();
        Render_FlipBuffers();
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
        VIWaitForRetrace();
    }
}
#pragma pop
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

#pragma push
#pragma use_lmw_stmw on
BOOL THPPlayerSetBuffer(u8 *buffer) {
    void DCInvalidateRange(void *addr, u32 nBytes);
    u32 i;
    u8 *ptr;
    u32 ysize;
    u32 uvsize;

    if (ActivePlayer.open && ActivePlayer.state == 0) {
        ptr = buffer;
        if (ActivePlayer.onMemory) {
            ActivePlayer.movieData = ptr;
            ptr += ActivePlayer.movieDataSize;
        } else {
            for (i = 0; i < 10; i++) {
                ActivePlayer.readBuffer[i].ptr = ptr;
                ptr += (ActivePlayer.bufSize + 0x1F) & ~0x1F;
            }
        }

        ysize = (ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize + 0x1F) & ~0x1F;
        uvsize = (ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4 + 0x1F) & ~0x1F;
        for (i = 0; i < 3; i++) {
            ActivePlayer.textureSet[i].ytexture = ptr;
            DCInvalidateRange(ptr, ysize);
            ptr += ysize;
            ActivePlayer.textureSet[i].utexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
            ActivePlayer.textureSet[i].vtexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
        }

        if (ActivePlayer.audioExist) {
            for (i = 0; i < 3; i++) {
                ActivePlayer.audioBuffer[i].buffer = (s16 *)ptr;
                ActivePlayer.audioBuffer[i].curPtr = (s16 *)ptr;
                ActivePlayer.audioBuffer[i].validSample = 0;
                ptr += (ActivePlayer.audioMaxSamples * 4 + 0x1F) & ~0x1F;
            }
        }

        *(u8 **)&ActivePlayer.unk88[0xC] = ptr;
        return TRUE;
    }
    return FALSE;
}
#pragma pop

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

BOOL ProperTimingForGettingNextFrame(void) {
    u32 VIGetNextField(void);
    s32 frameRate;

    if (ActivePlayer.playFlag & 2) {
        if (VIGetNextField() == 0) {
            return TRUE;
        }
    } else if (ActivePlayer.playFlag & 4) {
        if (VIGetNextField() == 1) {
            return TRUE;
        }
    } else {
        frameRate = ActivePlayer.frameRate * 100.0f;
        ActivePlayer.curCount = (*(s64 *)&ActivePlayer.unkC0 * frameRate) / 5994;
        if (ActivePlayer.prevCount != ActivePlayer.curCount) {
            ActivePlayer.prevCount = ActivePlayer.curCount;
            return TRUE;
        }
    }
    return FALSE;
}

#pragma push
#pragma use_lmw_stmw on
s32 THPPlayerDrawCurrentFrame(void *rmode, u32 x, u32 y, u32 polygonW, u32 polygonH) {
    typedef struct THPTextureSet {
        u8 *ytexture;
        u8 *utexture;
        u8 *vtexture;
        s32 frameNumber;
    } THPTextureSet;
    void THPGXYuv2RgbSetup(void *rmode);
    void THPGXYuv2RgbDraw(u8 *y_data, u8 *u_data, u8 *v_data, s16 x, s16 y, s16 textureWidth, s16 textureHeight, s16 polygonWidth, s16 polygonHeight);
    void THPGXRestore(void);
    s32 currFrame;

    if (ActivePlayer.open && ActivePlayer.state != 0 && *(THPTextureSet **)&ActivePlayer.unkD4[0x8] != NULL) {
        THPGXYuv2RgbSetup(rmode);
        THPGXYuv2RgbDraw((*(THPTextureSet **)&ActivePlayer.unkD4[0x8])->ytexture, (*(THPTextureSet **)&ActivePlayer.unkD4[0x8])->utexture,
                         (*(THPTextureSet **)&ActivePlayer.unkD4[0x8])->vtexture, x, y, ActivePlayer.videoInfo.xSize,
                         ActivePlayer.videoInfo.ySize, polygonW, polygonH);
        THPGXRestore();
        currFrame = ((*(THPTextureSet **)&ActivePlayer.unkD4[0x8])->frameNumber + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
        return currFrame;
    }
    return -1;
}

extern u16 VolumeTable_800DAE30[];
void *PopDecodedAudioBuffer(s32 arg0);
void PushFreeAudioBuffer(void *arg0);
s32 THPPlayerAudioOutput(s16 *destination, u32 sample, s32 volume) {
    s32 ret;
    u32 requestSample;
    s16 *dst;
    s32 vol;
    THPAudioBuffer *buf;
    u32 sampleNum;
    s16 *src;
    s32 mix;
    u32 i;

    if (volume < 0) {
        volume = 0;
    }
    if (volume > 0x7F) {
        volume = 0x7F;
    }
    vol = VolumeTable_800DAE30[volume];

    if (ActivePlayer.open && ActivePlayer.unk9D == 2 && ActivePlayer.audioExist) {
        requestSample = sample;
        dst = destination;
        ret = 0;
        while (TRUE) {
            if (*(THPAudioBuffer **)&ActivePlayer.unkD4[0xC] == NULL) {
                buf = PopDecodedAudioBuffer(0);
                *(THPAudioBuffer **)&ActivePlayer.unkD4[0xC] = buf;
                if (buf == NULL) {
                    break;
                }
                *(s32 *)&ActivePlayer.unkD4[0x4] = buf->frameNumber;
            }
            buf = *(THPAudioBuffer **)&ActivePlayer.unkD4[0xC];
            sampleNum = buf->validSample;
            if (sampleNum) {
                if (sampleNum >= requestSample) {
                    sampleNum = requestSample;
                }
                src = buf->curPtr;
                for (i = 0; i < sampleNum * 2; i++) {
                    mix = *dst + ((vol * *src) >> 15);
                    if (mix < -32767) {
                        mix = -32767;
                    }
                    if (mix > 32768) {
                        mix = 32768;
                    }
                    *dst = mix;
                    dst++;
                    src++;
                }
                ret += sampleNum;
                requestSample -= sampleNum;
                (*(THPAudioBuffer **)&ActivePlayer.unkD4[0xC])->validSample -= sampleNum;
                (*(THPAudioBuffer **)&ActivePlayer.unkD4[0xC])->curPtr = src;
                if ((*(THPAudioBuffer **)&ActivePlayer.unkD4[0xC])->validSample == 0) {
                    PushFreeAudioBuffer(*(THPAudioBuffer **)&ActivePlayer.unkD4[0xC]);
                    buf = PopDecodedAudioBuffer(0);
                    *(THPAudioBuffer **)&ActivePlayer.unkD4[0xC] = buf;
                    if (buf != NULL) {
                        *(s32 *)&ActivePlayer.unkD4[0x4] = buf->frameNumber;
                    }
                }
                if (requestSample == 0) {
                    break;
                }
            }
        }
        return ret;
    }
    return 0;
}

#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
void *Reader(void *param) {
    s32 DVDReadPrio(void *fileInfo, void *addr, s32 length, s32 offset, s32 prio);
    s32 OSSuspendThread(void *thread);
    void *PopFreeReadBuffer(void);
    void PrepareReady(void *arg0);
    void PushReadedBuffer(void *arg0);
    extern u8 ReadThread_8036ED20[];
    THPReadBuffer *readBuffer;
    s32 offset;
    s32 size;
    s32 readFrame;
    s32 result;
    s32 frameNumber;

    readFrame = 0;
    offset = *(s32 *)ActivePlayer.unkB0;
    size = ActivePlayer.initReadSize;
    while (1) {
        readBuffer = PopFreeReadBuffer();
        result = DVDReadPrio(&ActivePlayer.fileInfo, readBuffer->ptr, size, offset, 2);
        if (result != size) {
            if (result == -1) {
                *(s32 *)&ActivePlayer.dispTextureSet = -1;
            }
            if (readFrame == 0) {
                PrepareReady(0);
            }
            OSSuspendThread(ReadThread_8036ED20);
        }
        readBuffer->frameNumber = readFrame;
        PushReadedBuffer(readBuffer);
        offset += size;
        size = *(s32 *)readBuffer->ptr;
        frameNumber = (readFrame + ActivePlayer.initReadFrame) % ActivePlayer.numFrames;
        if (frameNumber == ActivePlayer.numFrames - 1) {
            if (ActivePlayer.playFlag & 1) {
                offset = *(s32 *)&ActivePlayer.unk5C[0x64 - 0x5C];
            } else {
                OSSuspendThread(ReadThread_8036ED20);
            }
        }
        readFrame++;
    }
}
#pragma pop

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

s32 CreateAudioDecodeThread(s32 priority, void *param) {
    s32 OSCreateThread(void *thread, void *func, void *param, void *stack, u32 stackSize, s32 priority, u16 attr);
    void OSInitMessageQueue(void *queue, void *msgArray, s32 msgCount);
    void AudioDecoder(void);
    void AudioDecoderForOnMemory(void *bufAddr);
    extern s32 AudioDecodeThreadCreated_803C7770;

    if (param) {
        if (OSCreateThread(AudioDecodeThread_80370108, AudioDecoderForOnMemory, param, AudioDecodeThreadStack + 0x1000, 0x1000, priority, 1) == 0) {
            return 0;
        }
    } else {
        if (OSCreateThread(AudioDecodeThread_80370108, AudioDecoder, NULL, AudioDecodeThreadStack + 0x1000, 0x1000, priority, 1) == 0) {
            return 0;
        }
    }
    OSInitMessageQueue(FreeAudioBufferQueue_80371418, FreeAudioBufferMessage, 3);
    OSInitMessageQueue(DecodedAudioBufferQueue_80371444, DecodedAudioBufferMessage, 3);
    AudioDecodeThreadCreated_803C7770 = 1;
    return 1;
}

void AudioDecodeThreadStart(void) {
    extern s32 AudioDecodeThreadCreated_803C7770;
    extern u8 AudioDecodeThread_80370108[0x310];
    if (AudioDecodeThreadCreated_803C7770) {
        OSResumeThread(AudioDecodeThread_80370108);
    }
}

void AudioDecodeThreadCancel(void) {
    extern s32 AudioDecodeThreadCreated_803C7770;
    extern u8 AudioDecodeThread_80370108[0x310];
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
    extern u8 AudioDecodeThread_80370108[0x310];
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
    extern u8 FreeAudioBufferQueue_80371418[0x20];
    void *msg;

    OSReceiveMessage(FreeAudioBufferQueue_80371418, &msg, 1);
    return msg;
}

void PushFreeAudioBuffer(void *arg0) {
    extern u8 FreeAudioBufferQueue_80371418[0x20];
    OSSendMessage(FreeAudioBufferQueue_80371418, arg0, 0);
}

void *PopDecodedAudioBuffer(s32 arg0) {
    extern u8 DecodedAudioBufferQueue_80371444[0x20];
    void *msg;

    if (OSReceiveMessage(DecodedAudioBufferQueue_80371444, &msg, arg0) == 1) {
        return msg;
    }
    return NULL;
}

void PushDecodedAudioBuffer(void *arg0) {
    extern u8 DecodedAudioBufferQueue_80371444[0x20];
    OSSendMessage(DecodedAudioBufferQueue_80371444, arg0, 1);
}

#pragma push
#pragma use_lmw_stmw on
s32 THPVideoDecode(void *file, void *tileY, void *tileU, void *tileV, void *work) {
    extern u8 *__THPWorkArea_803C788C;
    extern s32 __THPInitFlag_803C7894;
    u32 PPCMfhid2(void);
    void DCZeroRange(void *addr, u32 nBytes);
    s32 __THPReadHuffmanTableSpecification(void);
    s32 __THPReadFrameHeader(void);
    void __THPRestartDefinition(void);
    u8 __THPReadQuantizationTable(void);
    u8 __THPReadScaneHeader(void);
    void __THPSetupBuffers(void);
    void __THPDecompressYUV(void *tileY, void *tileU, void *tileV);
    u8 all_done, status;
    s32 errorCode;

    if (!file) {
        goto _err_no_input;
    }
    if (tileY == NULL || tileU == NULL || tileV == NULL) {
        goto _err_no_output;
    }
    if (!work) {
        goto _err_no_work;
    }
    if (!(PPCMfhid2() & 0x10000000)) {
        goto _err_lc_not_enabled;
    }
    if (__THPInitFlag_803C7894 == FALSE) {
        goto _err_not_initialized;
    }

    __THPWorkArea_803C788C = work;
    __THPInfo_803C7890 = (THPFileInfo *)(((u32)__THPWorkArea_803C788C + 31) & ~31);
    __THPWorkArea_803C788C = (u8 *)((u32)__THPInfo_803C7890 + sizeof(THPFileInfo));
    DCZeroRange(__THPInfo_803C7890, sizeof(THPFileInfo));
    __THPInfo_803C7890->cnt = 33;
    __THPInfo_803C7890->decompressedY = 0;
    __THPInfo_803C7890->c = file;
    all_done = FALSE;

    for (;;) {
        if (*(__THPInfo_803C7890->c)++ != 255) {
            goto _err_bad_syntax;
        }
        while (*__THPInfo_803C7890->c == 255) {
            __THPInfo_803C7890->c++;
        }
        status = *(__THPInfo_803C7890->c)++;

        if (status <= 0xD7) {
            if (status == 196) {
                status = __THPReadHuffmanTableSpecification();
                if (status != 0) {
                    goto _err_bad_status;
                }
            } else if (status == 192) {
                status = __THPReadFrameHeader();
                if (status != 0) {
                    goto _err_bad_status;
                }
            } else {
                goto _err_unsupported_marker;
            }
        } else if (0xD8 <= status && status <= 0xDF) {
            if (status == 221) {
                __THPRestartDefinition();
            } else if (status == 219) {
                status = __THPReadQuantizationTable();
                if (status != 0) {
                    goto _err_bad_status;
                }
            } else if (status == 218) {
                status = __THPReadScaneHeader();
                if (status != 0) {
                    goto _err_bad_status;
                }
                all_done = TRUE;
            } else if (status == 216) {
            } else {
                goto _err_unsupported_marker;
            }
        } else if (0xE0 <= status) {
            if ((224 <= status && status <= 239) || status == 254) {
                __THPInfo_803C7890->c += (__THPInfo_803C7890->c)[0] << 8 | (__THPInfo_803C7890->c)[1];
            } else {
                goto _err_unsupported_marker;
            }
        }
        if (all_done) {
            break;
        }
    }

    __THPSetupBuffers();
    __THPDecompressYUV(tileY, tileU, tileV);
    return 0;

_err_no_input:
    errorCode = 25;
    goto _err_exit;
_err_no_output:
    errorCode = 27;
    goto _err_exit;
_err_no_work:
    errorCode = 26;
    goto _err_exit;
_err_unsupported_marker:
    errorCode = 11;
    goto _err_exit;
_err_bad_syntax:
    errorCode = 3;
    goto _err_exit;
_err_bad_status:
    errorCode = status;
    goto _err_exit;
_err_lc_not_enabled:
    errorCode = 28;
    goto _err_exit;
_err_not_initialized:
    errorCode = 29;
_err_exit:
    return errorCode;
}
#pragma pop

s32 __THPReadFrameHeader(void) {
    s32 i;
    u8 utmp8;
    u8 *p;

    __THPInfo_803C7890->c += 2;
    if (*(__THPInfo_803C7890->c)++ != 8) {
        return 10;
    }
    __THPInfo_803C7890->yPixelSize = (u16)((__THPInfo_803C7890->c[0] << 8) | __THPInfo_803C7890->c[1]);
    __THPInfo_803C7890->c += 2;
    __THPInfo_803C7890->xPixelSize = (u16)((__THPInfo_803C7890->c[0] << 8) | __THPInfo_803C7890->c[1]);
    __THPInfo_803C7890->c += 2;
    if (*(__THPInfo_803C7890->c)++ != 3) {
        return 12;
    }
    for (i = 0; (u8)i < 3; i++) {
        __THPInfo_803C7890->c++;
        utmp8 = *(__THPInfo_803C7890->c)++;
        if (((u8)i == 0 && utmp8 != 0x22) || ((u8)i > 0 && utmp8 != 0x11)) {
            return 19;
        }
        p = __THPInfo_803C7890->c++;
        __THPInfo_803C7890->components[i].quantizationTableSelector = *p;
    }
    return 0;
}

u8 __THPReadScaneHeader(void) {
    s32 i;
    s32 compId;

    __THPInfo_803C7890->c += 2;
    if (*(__THPInfo_803C7890->c)++ != 3) {
        return 12;
    }
    for (i = 0; (u8)i < 3; i++) {
        __THPInfo_803C7890->c++;
        compId = *(__THPInfo_803C7890->c)++;
        __THPInfo_803C7890->components[i].DCTableSelector = compId >> 4;
        __THPInfo_803C7890->components[i].ACTableSelector = compId & 15;
        if ((__THPInfo_803C7890->validHuffmanTabs & (1U << (compId >> 4))) == 0) {
            return 15;
        }
        if ((__THPInfo_803C7890->validHuffmanTabs & (1U << ((compId & 15) + 1))) == 0) {
            return 15;
        }
    }
    __THPInfo_803C7890->c += 3;
    __THPInfo_803C7890->MCUsPerRow = (u16)((__THPInfo_803C7890->xPixelSize + 15) / 16);
    __THPInfo_803C7890->components[0].predDC = 0;
    __THPInfo_803C7890->components[1].predDC = 0;
    __THPInfo_803C7890->components[2].predDC = 0;
    return 0;
}

static void __THPHuffGenerateSizeTable(void);
void __THPHuffGenerateCodeTable(void);
void __THPHuffGenerateDecoderTables(u8 tabIndex);
s32 __THPReadHuffmanTableSpecification(void) {
    extern u8 *__THPWorkArea_803C788C;
    u8 t_class, id, i, tab_index;
    u16 length, num_Vij;

    __THPHuffmanSizeTab_803C7828 = __THPWorkArea_803C788C;
    __THPHuffmanCodeTab_803C782C = (u16 *)((u32)__THPWorkArea_803C788C + 256 + 1);
    length = (u16)((__THPInfo_803C7890->c[0] << 8) | __THPInfo_803C7890->c[1]);
    __THPInfo_803C7890->c += 2;
    length -= 2;

    for (;;) {
        i = *(__THPInfo_803C7890->c)++;
        id = (u8)(i & 15);
        t_class = (u8)(i >> 4);
        __THPHuffmanBits_803C7824 = __THPInfo_803C7890->c;
        tab_index = (u8)((id << 1) + t_class);
        num_Vij = 0;

        for (i = 0; i < 16; i++) {
            num_Vij += *(__THPInfo_803C7890->c)++;
        }

        __THPInfo_803C7890->huffmanTabs[tab_index].Vij = __THPInfo_803C7890->c;
        __THPInfo_803C7890->c += num_Vij;
        __THPHuffGenerateSizeTable();
        __THPHuffGenerateCodeTable();
        __THPHuffGenerateDecoderTables(tab_index);
        __THPInfo_803C7890->validHuffmanTabs |= 1 << tab_index;
        length -= 17 + num_Vij;

        if (length == 0) {
            break;
        }
    }
    return 0;
}

#pragma push
#pragma opt_lifetimes off
static void __THPHuffGenerateSizeTable(void) {
    s32 p, l, i;
    u8 size;

    p = 0;
    for (l = 1; l <= 16; l++) {
        i = __THPHuffmanBits_803C7824[l - 1];
        size = l;
        while (i--) {
            __THPHuffmanSizeTab_803C7828[p] = size;
            p++;
        }
    }
    __THPHuffmanSizeTab_803C7828[p] = 0;
}
#pragma pop

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

#pragma push
#pragma optimization_level 3
void __THPHuffGenerateDecoderTables(u8 tabIndex) {
    s32 p, l;
    THPHuffmanTab *h;

    p = 0;
    h = &__THPInfo_803C7890->huffmanTabs[tabIndex];
    l = 1;
    while (l <= 16) {
        if (__THPHuffmanBits_803C7824[l - 1]) {
            h->valPtr[l] = p - __THPHuffmanCodeTab_803C782C[p];
            p += __THPHuffmanBits_803C7824[l - 1];
            h->maxCode[l] = __THPHuffmanCodeTab_803C782C[p - 1];
        } else {
            h->maxCode[l] = -1;
            h->valPtr[l] = -1;
        }
        l++;
    }
    h->maxCode[17] = 0xfffff;
}
#pragma pop

void __THPRestartDefinition(void) {
    __THPInfo_803C7890->RST = TRUE;
    __THPInfo_803C7890->c += 2;
    __THPInfo_803C7890->nMCU = (u16)((__THPInfo_803C7890->c[0] << 8) | __THPInfo_803C7890->c[1]);
    __THPInfo_803C7890->c += 2;
    __THPInfo_803C7890->currMCU = __THPInfo_803C7890->nMCU;
}

void __THPPrepBitStream(void) {
    extern THPHuffmanTab *Ydchuff_803C7780;
    extern THPHuffmanTab *Udchuff_803C77A0;
    extern THPHuffmanTab *Vdchuff_803C77C0;
    extern THPHuffmanTab *Yachuff_803C77E0;
    extern THPHuffmanTab *Uachuff_803C7800;
    extern THPHuffmanTab *Vachuff_803C7820;
    u32 *ptr;
    u32 offset;
    u32 i, j;
    u32 code;

    ptr = (u32 *)((u32)__THPInfo_803C7890->c & 0xFFFFFFFC);
    offset = (u32)__THPInfo_803C7890->c & 3;
    if (__THPInfo_803C7890->cnt != 33) {
        __THPInfo_803C7890->cnt -= (3 - offset) * 8;
    } else {
        __THPInfo_803C7890->cnt = (offset * 8) + 1;
    }
    __THPInfo_803C7890->c = (u8 *)ptr;
    __THPInfo_803C7890->currByte = *ptr;

    for (i = 0; i < 4; i++) {
        if (__THPInfo_803C7890->validHuffmanTabs & (1 << i)) {
            for (code = 0; code < 32; code++) {
                __THPInfo_803C7890->huffmanTabs[i].quick[code] = 0xFF;
                for (j = 0; j < 5; j++) {
                    s32 tmp = code >> (4 - j);
                    if (tmp <= __THPInfo_803C7890->huffmanTabs[i].maxCode[j + 1]) {
                        __THPInfo_803C7890->huffmanTabs[i].quick[code] =
                            (__THPInfo_803C7890->huffmanTabs[i].Vij + __THPInfo_803C7890->huffmanTabs[i].valPtr[j + 1])[tmp];
                        __THPInfo_803C7890->huffmanTabs[i].increment[code] = j + 1;
                        j = 99;
                    }
                }
            }
        }
    }

    Ydchuff_803C7780 = &__THPInfo_803C7890->huffmanTabs[__THPInfo_803C7890->components[0].DCTableSelector << 1];
    Udchuff_803C77A0 = &__THPInfo_803C7890->huffmanTabs[__THPInfo_803C7890->components[1].DCTableSelector << 1];
    Vdchuff_803C77C0 = &__THPInfo_803C7890->huffmanTabs[__THPInfo_803C7890->components[2].DCTableSelector << 1];
    Yachuff_803C77E0 = &__THPInfo_803C7890->huffmanTabs[(__THPInfo_803C7890->components[0].ACTableSelector << 1) + 1];
    Uachuff_803C7800 = &__THPInfo_803C7890->huffmanTabs[(__THPInfo_803C7890->components[1].ACTableSelector << 1) + 1];
    Vachuff_803C7820 = &__THPInfo_803C7890->huffmanTabs[(__THPInfo_803C7890->components[2].ACTableSelector << 1) + 1];
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

s32 AudioVoice_Init_(void) {
    typedef struct {
        u8 unk0[0x3C];
        u32 delay[3];
        u32 feedback[3];
        u32 output[3];
    } AXFX_DELAY;
    typedef struct VOICE {
        u8 unk0[4];
        struct VOICE *next;
        u8 unk8[0x68];
    } VOICE;
    extern VOICE g_voice[0x20];
    extern AXFX_DELAY g_reverb_803715B0[2];
    extern u32 m_InitOnce_803C7898;
    extern VOICE *g_pFree_803C78A4;
    extern s32 g_active_803C78A0;
    extern void AXFXDelayCallback(void);
    void SetVoiceState_(void *voice, s32 state);
    void AudioVoice_Destroy_(void);
    s32 AXFXDelayInit(void *delay);
    void AXRegisterAuxBCallback(void *callback, void *context);
    void AXRegisterAuxACallback(void *callback, void *context);
    u32 i;

    for (i = 0; i < 0x20; i++) {
        SetVoiceState_(&g_voice[i], 3);
        g_voice[i].next = &g_voice[i + 1];
    }
    g_voice[0x1F].next = NULL;
    g_pFree_803C78A4 = g_voice;
    g_active_803C78A0 = -1;
    if (m_InitOnce_803C7898 == 0) {
        AudioVoice_Destroy_();
    }
    m_InitOnce_803C7898 = 0;
    for (i = 0; i < 2; i++) {
        g_reverb_803715B0[i].delay[0] = 10;
        g_reverb_803715B0[i].delay[1] = 10;
        g_reverb_803715B0[i].delay[2] = 10;
        g_reverb_803715B0[i].feedback[0] = 0;
        g_reverb_803715B0[i].feedback[1] = 0;
        g_reverb_803715B0[i].feedback[2] = 0;
        g_reverb_803715B0[i].output[0] = 0;
        g_reverb_803715B0[i].output[1] = 0;
        g_reverb_803715B0[i].output[2] = 0;
        AXFXDelayInit(&g_reverb_803715B0[i]);
    }
    AXRegisterAuxACallback(AXFXDelayCallback, &g_reverb_803715B0[0]);
    AXRegisterAuxBCallback(AXFXDelayCallback, &g_reverb_803715B0[1]);
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

void DestroyVoice_(void *arg0);
void *GetVoice_(u8 type, u32 priority) {
    extern void *g_pFree_803C78A4;
    s32 OSDisableInterrupts();
    void OSRestoreInterrupts(s32);
    void *AXAcquireVoice(u32 priority, void (*callback)(void *), u32 userContext);
    u8 *voice;
    void *axv;
    s32 level;

    if (g_pFree_803C78A4 == NULL) {
        OSReport("g_pFree NULL\n");
        return NULL;
    }
    level = OSDisableInterrupts();
    axv = AXAcquireVoice(priority, DestroyVoice_, (u32)g_pFree_803C78A4);
    if (axv == NULL) {
        return NULL;
    }
    voice = g_pFree_803C78A4;
    g_pFree_803C78A4 = *(void **)((u8 *)g_pFree_803C78A4 + 4);
    *(void **)(voice + 4) = NULL;
    *(void **)(voice + 0x50) = axv;
    voice[0] = type;
    voice[0x15] = 3;
    voice[0x14] = priority;
    voice[0x54] = 1;
    *(s16 *)(voice + 0x5A) = -1;
    *(s16 *)(voice + 0x64) = -1;
    *(s16 *)(voice + 0x6C) = -1;
    voice[0x55] = 0;
    voice[0x56] = 0;
    voice[0x57] = 0;
    *(s32 *)(voice + 0x48) = 0x7FFF;
    OSRestoreInterrupts(level);
    return voice;
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

#pragma push
#pragma optimization_level 1
void StartVoice_(void *voice) {
    extern u32 g_silence;
    s32 OSDisableInterrupts();
    void OSRestoreInterrupts(s32);
    u8 *axv;
    s32 level;
    u8 *v;
    u32 last;

    v = voice;
    level = OSDisableInterrupts();
    axv = *(u8 **)(v + 0x50);
    switch (v[0x2C]) {
    case 8:
        *(u16 *)(axv + 0x1D6) = 0x100;
        *(u16 *)(axv + 0x1A8) = 0x19;
        break;
    case 16:
        *(u16 *)(axv + 0x1D6) = 0x800;
        *(u16 *)(axv + 0x1A8) = 0xA;
        break;
    default:
        *(u16 *)(axv + 0x1D6) = 0;
        *(u16 *)(axv + 0x1A8) = 0;
        break;
    }
    *(u32 *)(axv + 0x1C) |= 0x20000;
    *(u32 *)(axv + 0x1B2) = *(u32 *)(v + 0x20);
    last = *(u32 *)(v + 0x28) - 1;
    *(u32 *)(axv + 0x1AE) = last + *(u32 *)(v + 0x20);
    if (*(u32 *)(v + 0x24) != 0xFFFFFFFF) {
        *(u16 *)(axv + 0x1A6) = 1;
        *(u32 *)(axv + 0x1AA) = *(u32 *)(v + 0x20) + *(u32 *)(v + 0x24);
    } else {
        *(u16 *)(axv + 0x1A6) = 0;
        *(u32 *)(axv + 0x1AA) = g_silence;
        if (v[0x2C] == 16) {
            *(u32 *)(axv + 0x1AA) >>= 1;
        }
    }
    *(u32 *)(axv + 0x1C) |= 0x1000;
    *(u16 *)(axv + 0x140) = 0;
    *(u16 *)(axv + 0x142) = 2;
    *(u32 *)(axv + 0x1DE) = (1.0f / 48000.0f) * ((f32)*(u32 *)(v + 0x30) * (f32)*(u32 *)(v + 0x34));
    *(u16 *)(axv + 0x1E2) = 0;
    if (v[0x54]) {
        *(u16 *)(axv + 0x1E4) = 0;
        *(u16 *)(axv + 0x1E6) = 0;
        *(u16 *)(axv + 0x1E8) = 0;
        *(u16 *)(axv + 0x1EA) = 0;
        *(u32 *)(axv + 0x1C) |= 0x40000 | 1;
    } else {
        *(u32 *)(axv + 0x1C) |= 0x80000 | 1;
    }
    if (v[0]) {
        *(u16 *)(axv + 0x144) = 0x203;
    } else {
        *(u16 *)(axv + 0x144) = 0x13;
    }
    *(u16 *)(axv + 0x152) = 0;
    *(u16 *)(axv + 0x156) = 0;
    *(u16 *)(axv + 0x15A) = 0;
    *(u16 *)(axv + 0x15E) = 0;
    *(u32 *)(axv + 0x1C) |= 2;
    *(u16 *)(axv + 0x146) = 1;
    *(u32 *)(axv + 0x1C) |= 4;
    v[0x54] = 0;
    v[0x15] = 1;
    OSRestoreInterrupts(level);
}
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

#pragma push
#pragma optimization_level 1
void SetReverb_(u32 idx, u32 output, u32 feedback, u32 delay) {
    s32 AXFXDelaySettings(void *delay);
    typedef struct {
        u8 unk0[0x3C];
        u32 delay[3];
        u32 feedback[3];
        u32 output[3];
    } AXFX_DELAY;
    extern AXFX_DELAY g_reverb_803715B0[2];

    if (delay < 10) {
        delay = 10;
    }
    g_reverb_803715B0[idx].delay[0] = delay;
    g_reverb_803715B0[idx].delay[1] = delay;
    feedback = (feedback * 100) >> 15;
    g_reverb_803715B0[idx].feedback[0] = feedback;
    g_reverb_803715B0[idx].feedback[1] = feedback;
    if (idx == 1) {
        output = (output * 100) >> 15;
        g_reverb_803715B0[0].output[0] = output;
        g_reverb_803715B0[0].output[1] = output;
        AXFXDelaySettings(&g_reverb_803715B0[1]);
    } else {
        g_reverb_803715B0[1].output[0] = 100;
        g_reverb_803715B0[1].output[1] = 100;
        AXFXDelaySettings(&g_reverb_803715B0[0]);
    }
}
#pragma pop

#pragma pop

#pragma push
#pragma optimization_level 1
s32 CreateIT_(u8 *hdr, u8 **smp) {
    u32 HAllocA_(s32 size);
    extern u32 g_aheap;
    s32 size;
    u32 i;

    i = *(u16 *)(hdr + 0x24) - 1;
    size = (*(u32 *)(smp[i] + 0x30) << ((smp[i][0x12] >> 1) & 1)) + *(u32 *)(smp[i] + 0x48);
    g_aheap = HAllocA_(size);
    if (*(void **)&g_aheap == NULL) {
        return 0;
    }
    DCFlushRange(*(void **)(smp[0] + 0x48), size);
    MemCpyA_(g_aheap, *(u32 *)(smp[0] + 0x48), size);
    *(u32 *)(smp[0] + 0x48) = 0;
    for (i = 0; i < *(u16 *)(hdr + 0x24); i++) {
        *(u32 *)(smp[i] + 0x48) += g_aheap;
        if (smp[i][0x12] & 2) {
            *(u32 *)(smp[i] + 0x48) >>= 1;
        }
    }
    return 1;
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

#pragma push
#pragma opt_propagation off
s32 CreateMIDI_(u8 *hdr) {
    u32 HAllocA_(s32 size);
    extern u32 g_aheap;
    u8 *data;
    s32 size;
    u32 i;
    u32 *smp;

    data = hdr + 0x20 + *(u32 *)(hdr + 0x10) * 0x20;
    size = *(u32 *)(hdr + 4) - (data - hdr - 8);
    g_aheap = HAllocA_(size);
    if (*(void **)&g_aheap == NULL) {
        return 0;
    }
    DCFlushRange(hdr + 0x20 + *(u32 *)(hdr + 0x10) * 0x20, size);
    MemCpyA_(g_aheap, *(u32 *)(hdr + 0x14), size);
    smp = (u32 *)(hdr + 0x20);
    for (i = 0; i < *(u32 *)(hdr + 0x10); i++) {
        smp[0] += g_aheap;
        if (((u8 *)smp)[0x11] == 0x10) {
            smp[0] >>= 1;
            smp[1] >>= 1;
            ((s32 *)smp)[2] >>= 1;
        }
        smp += 8;
    }
    return 1;
}
#pragma pop

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
s8 CmpID_(u8 *chunk, s32 type) {
    s32 GetID_();
    s8 result = GetID_(chunk) == type;
    return result;
}

s8 CmpType_(u8 *chunk, s32 type) {
    s32 GetType_();
    s8 result = GetType_(chunk) == type;
    return result;
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

#pragma push
#pragma optimization_level 1
#pragma use_lmw_stmw on
s32 Audio_Init_(void) {
    typedef struct MemBlk {
        u32 addr;
        s32 size;
        u32 unk8;
        u32 unkC;
    } MemBlk;
    extern MemBlk g_memblk_80371670[];
    extern s32 g_last_803C78B4;
    extern u32 g_heap_803C78C4;
    extern s32 g_size_803C78C0;
    extern u32 g_silence;
    extern s32 g_free_803C78B8;
    void ARQFlushQueue(void);
    u32 ARGetBaseAddress(void);
    u32 ARGetSize(void);
    void *OSAllocFromHeap(s32 heap, u32 size);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    void *memset(void *dst, int c, u32 n);
    int printf();
    u32 i;
    s32 level;
    void *buf;

    ARQFlushQueue();
    g_silence = ARGetBaseAddress();
    g_heap_803C78C4 = g_silence + 0x800;
    g_size_803C78C0 = ARGetSize() - g_heap_803C78C4;
    g_memblk_80371670[0].addr = g_heap_803C78C4;
    g_memblk_80371670[0].size = g_size_803C78C0;
    g_memblk_80371670[0].unk8 = -1;
    g_memblk_80371670[0].unkC = -1;
    g_last_803C78B4 = 0;
    g_memblk_80371670[1].size = 0;
    g_memblk_80371670[1].unk8 = -1;
    g_memblk_80371670[1].unkC = 2;
    g_free_803C78B8 = 1;
    for (i = 2; i < 0xFF; i++) {
        g_memblk_80371670[i].size = 0;
        g_memblk_80371670[i].unk8 = i - 1;
        g_memblk_80371670[i].unkC = i + 1;
    }
    g_memblk_80371670[0xFE].unkC = -1;
    level = OSDisableInterrupts();
    buf = OSAllocFromHeap(__OSCurrHeap, 0x800);
    OSRestoreInterrupts(level);
    memset(buf, 0, 0x800);
    MemCpyA_(g_silence, (u32)buf, 0x800);
    level = OSDisableInterrupts();
    OSFreeToHeap(__OSCurrHeap, (s32)buf);
    OSRestoreInterrupts(level);
    printf("Audio :: Initialized (%ik available RAM)\n", g_size_803C78C0 >> 10);
    return 1;
}
#pragma pop

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

u32 MAllocA_(s32 size) {
    typedef struct MemBlk {
        u32 addr;
        s32 size;
        u32 unk8;
        u32 unkC;
    } MemBlk;
    extern MemBlk g_memblk_80371670[];
    extern s32 g_last_803C78B4;
    extern u32 g_free_803C78B8;
    int printf();
    u32 i;
    u32 n;

    i = 0;
    size = (size + 0x1F) & ~0x1F;
    while (i != 0xFFFFFFFF) {
        if (g_memblk_80371670[i].size >= size) {
            if (g_memblk_80371670[i].size > size) {
                if (g_free_803C78B8 == 0xFFFFFFFF) {
                    return 0;
                }
                n = g_free_803C78B8;
                g_free_803C78B8 = g_memblk_80371670[n].unkC;
                g_memblk_80371670[n].addr = g_memblk_80371670[i].addr;
                g_memblk_80371670[n].size = g_memblk_80371670[i].size - size;
                g_memblk_80371670[n].unk8 = i;
                g_memblk_80371670[n].unkC = g_memblk_80371670[i].unkC;
                if (g_memblk_80371670[n].unkC == 0xFFFFFFFF) {
                    g_last_803C78B4 = n;
                }
                g_memblk_80371670[i].unkC = n;
            }
            g_memblk_80371670[i].addr += g_memblk_80371670[i].size - size;
            g_memblk_80371670[i].size = -size;
            return g_memblk_80371670[i].addr;
        }
        i = g_memblk_80371670[i].unkC;
    }
    printf("Audio :: MAllocA_ - Insufficient ARAM  size: %i\n", size);
    return 0;
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

s32 SetSFXState(s32 state) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_sfxState_803C78D0;
    extern s32 g_sfxvol_803C78D4;
    extern void (*VoiceUpdate)(void);
    void ProcessVoice(void);
    void SetVoiceVol_(void *voice, s32 vol);
    void StartVoice_(void *voice);
    void SetVoiceState_(void *voice, s32 state);
    u32 i;
    u8 *voice;
    s32 old;

    if (state == 0 || state == g_sfxState_803C78D0) {
        return g_sfxState_803C78D0;
    }
    if (state != 1) {
        VoiceUpdate = NULL;
    }
    voice = g_voice[0];
    switch (state) {
    case 2:
        for (i = 0x20; i != 0; voice += 0x70, i--) {
            if (voice[0] == 1 && voice[0x15] == 1) {
                SetVoiceState_(voice, 2);
            }
        }
        break;
    case 3:
        for (i = 0x20; i != 0; voice += 0x70, i--) {
            if (voice[0] == 1 && voice[0x15] != 0) {
                SetVoiceState_(voice, 2);
                voice[0x15] = 3;
            }
        }
        break;
    case 4:
        for (i = 0x20; i != 0; voice += 0x70, i--) {
            if (voice[0] == 1) {
                voice[0x15] = 3;
            }
        }
        voice = g_voice[0];
    case 1:
        for (i = 0x20; i != 0; voice += 0x70, i--) {
            if (voice[0] == 1) {
                if (voice[0x15] == 2) {
                    SetVoiceState_(voice, 1);
                } else if (voice[0x15] == 3) {
                    StartVoice_(voice);
                    SetVoiceVol_(voice, g_sfxvol_803C78D4);
                }
            }
        }
        state = 1;
        VoiceUpdate = ProcessVoice;
        break;
    }
    old = g_sfxState_803C78D0;
    g_sfxState_803C78D0 = state;
    return old;
}

u32 GetVoice(u32 handle, u8 priority) {
    u8 *GetVoice_(s32 type, s32 priority);
    u8 *voice;
    SFX *sfx;

    handle--;
    if (handle < 0xFE && g_sound_80373560[handle].data != 0) {
        sfx = &g_sound_80373560[handle];
        voice = GetVoice_(1, priority * 2);
        if (voice != NULL) {
            *(SFX **)(voice + 0x10) = sfx;
            *(u32 *)(voice + 0x20) = *(u32 *)((u8 *)sfx + 0x0);
            *(u32 *)(voice + 0x28) = *(u32 *)((u8 *)sfx + 0x4);
            *(u32 *)(voice + 0x24) = *(u32 *)((u8 *)sfx + 0x8);
            *(u32 *)(voice + 0x30) = *(u32 *)((u8 *)sfx + 0xC);
            *(u32 *)(voice + 0x34) = 0x10000;
            voice[0x2C] = *((u8 *)sfx + 0x11);
            *(s32 *)(voice + 0x38) = 0x7FFF;
            *(u32 *)(voice + 0x3C) = *(u16 *)((u8 *)sfx + 0x12);
            *(s32 *)(voice + 0x48) = 0x7FFF;
            *(s32 *)(voice + 0x40) = 0;
            *(s32 *)(voice + 0x44) = -0x7FFF;
            voice[1] = handle;
            voice[2]++;
            return *(u32 *)voice;
        }
    }
    return 0;
}

#pragma push
#pragma optimization_level 1
void ReleaseVoice(u32 id) {
    extern u8 g_voice[0x20][0x70];

    if (id == *(u32 *)g_voice[(u8)id]) {
        SetVoiceState_(g_voice[(u8)id], 3);
    }
}

s32 SetVoiceState(u32 id, s32 state) {
    extern u8 g_voice[0x20][0x70];
    extern s32 g_sfxState_803C78D0;
    extern s32 g_sfxvol_803C78D4;
    void SetVoiceVol_(void *voice, s32 vol);
    void StartVoice_(void *voice);
    u8 *voice = g_voice[(u8)id];

    if (*(u32 *)voice == id) {
        if (voice[0x15] == state) {
            return state;
        }
        switch (state) {
        case 1:
            if (voice[0x15] == 2) {
                SetVoiceState_(voice, 1);
                g_sfxState_803C78D0 = 1;
                break;
            }
        case 4:
            StartVoice_(voice);
            SetVoiceVol_(voice, g_sfxvol_803C78D4);
            g_sfxState_803C78D0 = 1;
            break;
        case 2:
        case 3:
            SetVoiceState_(voice, 2);
            break;
        }
        return voice[0x15];
    }
    return 0;
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

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void CreateIT(u8 *data, u32 arg1) {
    typedef struct ITSong {
        u8 unk0[0xE30];
        u8 *ins[100];
        u8 *smp[100];
        u8 *pat[100];
    } ITSong;
    extern u8 *g_pHdr_803C78E8;
    extern u8 *g_pOrder_803C78E4;
    extern u8 g_song_80375520[0x30];
    extern u8 g_musicTitle[0x20];
    extern void (*DestroySongVoice)(void *);
    extern s32 __OSCurrHeap;
    void *OSAllocFromHeap(s32 heap, u32 size);
    void DestroyITVoice(void *voice);
    ITSong *song = (ITSong *)g_song_80375520;
    s32 i;
    u32 *insOff;
    u32 *smpOff;
    u32 *patOff;

    if (data == NULL || arg1 == 0) {
        return;
    }
    i = *(u32 *)(data + 0x38);
    g_pHdr_803C78E8 = OSAllocFromHeap(__OSCurrHeap, i);
    memcpy(g_pHdr_803C78E8, data, i);
    data = g_pHdr_803C78E8;
    *(s32 *)(g_pHdr_803C78E8 + 0x3C) = 1;
    g_pOrder_803C78E4 = data + 0xC0;
    insOff = (u32 *)(&data[*(u16 *)(g_pHdr_803C78E8 + 0x20)] + 0xC0);
    smpOff = insOff + *(u16 *)(g_pHdr_803C78E8 + 0x22);
    patOff = smpOff + *(u16 *)(g_pHdr_803C78E8 + 0x24);
    for (i = 0; i < *(u16 *)(g_pHdr_803C78E8 + 0x22); i++) {
        song->ins[i] = &data[insOff[i]];
    }
    for (i = 0; i < *(u16 *)(g_pHdr_803C78E8 + 0x24); i++) {
        song->smp[i] = &data[smpOff[i]];
    }
    for (i = 0; i < *(u16 *)(g_pHdr_803C78E8 + 0x26); i++) {
        song->pat[i] = &data[patOff[i]];
    }
    *(u32 *)(song->smp[0] + 0x48) = arg1;
    CreateIT_(g_pHdr_803C78E8, song->smp);
    memcpy(g_musicTitle, g_pHdr_803C78E8 + 4, 0x1A);
    DestroySongVoice = DestroyITVoice;
    g_musicTitle[0x1A] = 0;
}
#pragma pop

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

s32 PlayNote(u8 *trk) {
    extern u8 g_song_80375520[0x30];
    void SetVoiceVol_(void *voice, s32 vol);
    void StartVoice_(void *voice);
    void StopActive(void *arg0);
    u32 rate;
    u8 stolen;
    u8 *voice;
    u8 *sample;

    stolen = 0;
    rate = GetNoteRate_(*(u32 *)(*(u8 **)(trk + 8) + 0x3C), trk[0x19]);
    if (rate > 0x2EE00 || rate < 0x60) {
        if (!stolen && *(u8 **)(trk + 4) != NULL) {
            StopActive(trk);
        }
        return 0;
    }
    if (*(u8 **)(trk + 4) == NULL) {
        *(u8 **)(trk + 4) = GetVoice_(0, trk[0x12]);
        if (*(u8 **)(trk + 4) == NULL) {
            return 0;
        }
        (*(u8 **)(trk + 4))[1] = trk[0x10];
        (*(u8 **)(trk + 4))[2] = trk[0x14];
        *(u8 **)(*(u8 **)(trk + 4) + 4) = *(u8 **)trk;
        *(u8 **)trk = *(u8 **)(trk + 4);
    }
    voice = *(u8 **)(trk + 4);
    *(u8 **)(voice + 0x10) = *(u8 **)(trk + 8);
    sample = *(u8 **)(trk + 8);
    *(s32 *)(voice + 0x20) = *(s32 *)(sample + 0x48);
    *(s32 *)(voice + 0x24) = (sample[0x12] & 0x10) ? *(s32 *)(sample + 0x34) : -1;
    *(s32 *)(voice + 0x28) = (sample[0x12] & 0x10) ? *(s32 *)(sample + 0x38) : *(s32 *)(sample + 0x30);
    voice[0x2C] = (sample[0x12] & 2) ? 0x10 : 8;
    *(u32 *)(voice + 0x30) = rate;
    *(s32 *)(voice + 0x34) = *(s32 *)(g_song_80375520 + 0x1C);
    *(s32 *)(voice + 0x38) = *(s32 *)(trk + 0x44);
    *(s32 *)(voice + 0x3C) = sample[0x11] << 9;
    *(s32 *)(voice + 0x40) = *(s32 *)(trk + 0x2C);
    *(s32 *)(voice + 0x44) = -0x7FFF;
    StartVoice_(voice);
    SetVoiceVol_(voice, *(s16 *)(trk + 0x1A));
    return 1;
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

u8 g_song_80376990[0x24];
u8 g_chn_803769B4[0x200];

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

#pragma push
#pragma optimization_level 1
void CreateMIDI(u8 *hdr, u8 *snd) {
    typedef struct {
        u8 pad[0x14];
        u8 prog;
        u8 pad15;
        u8 lo;
        u8 hi;
        u8 pad18[8];
    } MIDIInst;
    typedef struct {
        u8 pad[0x10];
        u32 count;
        u32 size;
        u8 pad18[8];
        MIDIInst inst[1];
    } MIDISnd;
    extern u8 *g_pHdr_803C78F4;
    extern MIDISnd *g_pSnd_803C78F0;
    extern MIDIInst *g_pProg_80376BB4[0x80];
    extern MIDIInst *g_pDrum_80376DB4[0x80];
    extern s32 g_musicState;
    extern void (*DestroySongVoice)(void *);
    extern s32 __OSCurrHeap;
    void *OSAllocFromHeap(s32 heap, u32 size);
    void FindMarkers(u8 *p);
    void DestroyMIDIVoice(u8 *voice);
    s32 CreateMIDI_(u8 *hdr);
    u32 i;
    s32 j;

    if (hdr == NULL || snd == NULL) {
        return;
    }
    i = *(u32 *)(hdr + 4) + 8;
    g_pHdr_803C78F4 = OSAllocFromHeap(__OSCurrHeap, i + ((MIDISnd *)snd)->size);
    g_pSnd_803C78F0 = (MIDISnd *)(g_pHdr_803C78F4 + i);
    memcpy(g_pHdr_803C78F4, hdr, i);
    memcpy(g_pSnd_803C78F0, snd, ((MIDISnd *)snd)->size);
    g_pSnd_803C78F0->size += (u32)snd;
    FindMarkers(g_pHdr_803C78F4 + 0x22);
    memset(g_pProg_80376BB4, 0, sizeof(g_pProg_80376BB4));
    memset(g_pDrum_80376DB4, 0, sizeof(g_pDrum_80376DB4));
    j = 0x80;
    for (i = 0; i < g_pSnd_803C78F0->count; i++) {
        if (g_pSnd_803C78F0->inst[i].prog != 0x80) {
            if (j != g_pSnd_803C78F0->inst[i].prog) {
                j = g_pSnd_803C78F0->inst[i].prog;
                g_pProg_80376BB4[j] = &g_pSnd_803C78F0->inst[i];
            }
        } else {
            for (j = g_pSnd_803C78F0->inst[i].lo; j <= g_pSnd_803C78F0->inst[i].hi; j++) {
                g_pDrum_80376DB4[j] = &g_pSnd_803C78F0->inst[i];
            }
            j = 0x80;
        }
    }
    CreateMIDI_((u8 *)g_pSnd_803C78F0);
    g_musicState = 0;
    DestroySongVoice = (void (*)(void *))DestroyMIDIVoice;
}
#pragma pop

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
    extern u8 g_chn_803769B4[0x200];
    u8 *v;

    v = *(u8 **)&g_chn_803769B4[voice[1] * 0x20];
    if (v == voice) {
        *(u8 **)&g_chn_803769B4[voice[1] * 0x20] = *(u8 **)(voice + 4);
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
    extern u8 g_chn_803769B4[0x200];
    u8 *next = *(u8 **)(voice + 4);
    u8 *v;

    v = *(u8 **)&g_chn_803769B4[voice[1] * 0x20];
    if (v == voice) {
        *(u8 **)&g_chn_803769B4[voice[1] * 0x20] = next;
    } else {
        for (; *(u8 **)(v + 4) != voice; v = *(u8 **)(v + 4)) {
        }
        *(u8 **)(v + 4) = next;
    }
    SetVoiceState_(voice, 3);
    return next;
}

#pragma push
#pragma optimization_level 1
void JumpToMarker(void) {
    typedef struct {
        u32 time;
        u32 unk4;
        u32 unk8;
        u32 unkC;
        u32 unk10;
        u8 *pos;
        u32 delta;
        u8 unk1C;
        u8 unk1D;
        u8 marker;
        u8 unk1F;
        u32 pitch;
    } MIDISong;
    typedef struct {
        MIDISong song;
        u8 chn[0x10][0x20];
        u8 prog[0x200];
        u8 drum[0x200];
        u8 mrk[0x40][8];
    } MIDIState;
    extern u32 g_trkOn;
    extern u32 g_trkOff;
    u32 ReadVar_(u8 **pp);
    MIDIState *m = (MIDIState *)g_song_80376990;
    u32 mask;
    u8 *chn;
    u32 i;

    for (chn = m->chn[0], mask = g_trkOn; mask != 0; chn += 0x20) {
        chn[4] |= mask & 1;
        mask >>= 1;
    }
    for (chn = m->chn[0]; g_trkOff != 0; chn += 0x20) {
        chn[4] &= ~(g_trkOff & 1);
    }
    g_trkOn = 0;
    g_trkOff = 0;
    m->song.pos = *(u8 **)m->mrk[m->song.marker];
    m->song.time = 0;
    m->song.unk8 = 0;
    m->song.delta = ReadVar_(&m->song.pos);
    m->song.unk1D = m->mrk[m->song.marker][5];
    m->song.unk1C = m->mrk[m->song.marker][4];
    for (chn = m->chn[0], i = 0x10; i != 0; chn += 0x20, i--) {
        ChannelOff(chn);
        chn[0x1F] = 0;
        chn[0x1D] = 0x40;
        *(u32 *)(chn + 0x18) = 0x10000;
    }
    m->song.marker = 0;
}
#pragma pop

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
void CreateMusic(u8 *data, u32 size) {
    s8 CmpType_(u8 *chunk, s32 type);
    s8 CmpID_(u8 *chunk, s32 id);
    void CreateMIDI(u8 *data, u32 size);
    void CreateIT(u8 *data, u32 size);
    extern void *DestroyMusic;
    extern void *SetMusicState;
    extern void *SetMusicVolume;
    extern void *SetMusicPitch;
    extern void *SetMusicTempo;
    extern void *SetMusicOrder;
    extern void *SetMusicTrackPriority;
    extern void *SetMusicTrackOn;
    extern void *SetMusicTrackOff;
    extern void *SetMusicTrackVol;
    void DestroyMIDI();
    void SetMIDIState();
    void SetMIDIVol();
    void SetMIDIPitch();
    void SetMIDITempo();
    void SetMIDITrackPriority();
    void SetMIDITrackOn();
    void SetMIDITrackOff();
    void SetMIDITrackVol();
    void SetITState();
    void SetITPitch();
    void SetITTempo();
    void SetITTrackPriority();
    void SetITTrackOn();
    void SetITTrackOff();
    void SetITTrackVol();

    if (data == NULL || size == 0) {
        return;
    }
    if (CmpType_(data, 'MIDI')) {
        DestroyMusic = DestroyMIDI;
        SetMusicState = SetMIDIState;
        SetMusicVolume = SetMIDIVol;
        SetMusicPitch = SetMIDIPitch;
        SetMusicTempo = SetMIDITempo;
        SetMusicOrder = SetMIDIMarker;
        SetMusicTrackPriority = SetMIDITrackPriority;
        SetMusicTrackOn = SetMIDITrackOn;
        SetMusicTrackOff = SetMIDITrackOff;
        SetMusicTrackVol = SetMIDITrackVol;
        CreateMIDI(data, size);
    } else if (CmpID_(data, 'IMPM')) {
        DestroyMusic = DestroyIT;
        SetMusicState = SetITState;
        SetMusicVolume = SetITVol;
        SetMusicPitch = SetITPitch;
        SetMusicTempo = SetITTempo;
        SetMusicOrder = SetITOrder;
        SetMusicTrackPriority = SetITTrackPriority;
        SetMusicTrackOn = SetITTrackOn;
        SetMusicTrackOff = SetITTrackOff;
        SetMusicTrackVol = SetITTrackVol;
        CreateIT(data, size);
    }
}
#pragma pop

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
#pragma dont_inline on
void HandlePedals(u8 *cur, SISteeringStatus *status, SISteeringStatus *calib) {
    s32 combined;

    if ((status->misc ^ calib[3].misc) & 8) {
        *(s32 *)&calib[4] = 0x40;
        calib[0].gas = 0x50;
        calib[0].brake = 0x50;
        combined = (calib[0].gas - calib[0].brake) / 2;
        if (combined < -128) {
            combined = -128;
        } else if (combined > 127) {
            combined = 127;
        }
        calib[0].left = combined;
        calib[1].gas = 0xB0;
        calib[1].brake = 0xB0;
        combined = (calib[1].gas - calib[1].brake) / 2;
        if (combined < -128) {
            combined = -128;
        } else if (combined > 127) {
            combined = 127;
        }
        calib[1].left = combined;
        calib[2].gas = 10;
        calib[2].brake = 10;
    }
    if (*(s32 *)&calib[4] > 0) {
        (*(s32 *)&calib[4])--;
        cur[5] = 0;
        cur[4] = 0;
        cur[6] = 0;
    } else if (status->misc & 8) {
        AutoCalibrateAxisUnsigned(&calib[0].gas, &calib[1].gas, status->gas, calib[2].gas, &cur[4]);
        AutoCalibrateAxisUnsigned(&calib[0].brake, &calib[1].brake, status->brake, calib[2].brake, &cur[5]);
        combined = (cur[4] - cur[5]) / 2;
        if (combined < -128) {
            combined = -128;
        } else if (combined > 127) {
            combined = 127;
        }
        cur[6] = combined;
    } else {
        cur[5] = 0;
        cur[4] = 0;
        cur[6] = 0;
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
#pragma dont_inline on
void HandleTriggers(u8 *cur, SISteeringStatus *status, SISteeringStatus *calib) {
    s32 diff;

    if ((status->button ^ calib[3].button) & 0x40) {
        diff = status->left - calib[2].right;
        calib[1].right = 0 > diff ? 0 : diff;
        cur[7] = 0xFF;
    } else if (status->button & 0x40) {
        cur[7] = 0xFF;
    } else {
        AutoCalibrateAxisUnsigned(&calib[0].right, &calib[1].right, status->left, calib[2].right, &cur[7]);
    }
    if ((status->button ^ calib[3].button) & 0x20) {
        diff = status->right - *(u8 *)&calib[2].err;
        calib[1].err = 0 > diff ? 0 : diff;
        cur[8] = 0xFF;
    } else if (status->button & 0x20) {
        cur[8] = 0xFF;
    } else {
        AutoCalibrateAxisUnsigned((u8 *)&calib[0].err, (u8 *)&calib[1].err, status->right, *(u8 *)&calib[2].err, &cur[8]);
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
#pragma dont_inline on
void SteeringSamplingCallback(void) {
    void SIReadSteering(s32 chan, void *status);
    void HandlePedals(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    void HandleTriggers(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    void SimThread_Step(VDevice *dev);
    s32 i;
    LGDevice *device;
    SISteeringStatus *status;

    for (i = 0; i < 4; i++) {
        device = &g_lgDevices_803771D8[i];
        if (device->opened) {
            status = &device->status;
            SIReadSteering(i, status);
            switch (status->err) {
            case -1:
                device->unk4D = -1;
                device->started = 1;
                break;
            case 0:
                AutoCalibrateAxisSigned(&device->calibMin.steering, &device->calibMax.steering, status->steering,
                                        device->calibDeadzone.steering, &device->curSteering);
                HandlePedals(&device->curButton, status, &device->calibMin);
                HandleTriggers(&device->curButton, status, &device->calibMin);
                device->curButton = status->button;
                device->curMisc = status->misc;
                device->prev = *status;
                device->unk4D = 0;
                SimThread_Step(&device->vdev);
                break;
            }
        }
    }
}

void InitDevice(LGDevice *device, s32 index) {
    void HandlePedals(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    void HandleTriggers(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    s32 combined;

    *(s32 *)device->unk0 = index;
    device->opened = 0;
    device->started = 0;
    device->unk8 = -1;
    device->status.button = 0;
    device->status.misc = 0;
    device->status.steering = 0;
    device->status.gas = 0x80;
    device->status.brake = 0x80;
    device->status.left = 0x80;
    device->status.right = 0x80;
    device->status.err = -1;
    device->calibMin.steering = -0x40;
    device->calibMin.right = 0x50;
    *(u8 *)&device->calibMin.err = 0x50;
    device->calibMax.steering = 0x40;
    device->calibMax.right = 0xB0;
    *(u8 *)&device->calibMax.err = 0xB0;
    device->calibDeadzone.steering = 6;
    device->calibDeadzone.right = 0xC;
    *(u8 *)&device->calibDeadzone.err = 0xC;
    device->calibMin.gas = 0x50;
    device->calibMin.brake = 0x50;
    combined = (device->calibMin.gas - device->calibMin.brake) / 2;
    if (combined < -128) {
        combined = -128;
    } else if (combined > 127) {
        combined = 127;
    }
    device->calibMin.left = combined;
    device->calibMax.gas = 0xB0;
    device->calibMax.brake = 0xB0;
    combined = (device->calibMax.gas - device->calibMax.brake) / 2;
    if (combined < -128) {
        combined = -128;
    } else if (combined > 127) {
        combined = 127;
    }
    device->calibMax.left = combined;
    device->calibDeadzone.gas = 10;
    device->calibDeadzone.brake = 10;
    AutoCalibrateAxisSigned(&device->calibMin.steering, &device->calibMax.steering, device->status.steering,
                            device->calibDeadzone.steering, &device->curSteering);
    HandlePedals(&device->curButton, &device->status, &device->calibMin);
    HandleTriggers(&device->curButton, &device->status, &device->calibMin);
    device->curButton = device->status.button;
    device->curMisc = device->status.misc;
    device->prev = device->status;
    device->unk4D = 0;
}

#pragma pop

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
            SIReadSteering(i, (u8 *)device + 0xC);
        }
        SISetSteeringSamplingCallback(SteeringSamplingCallback);
        g_lgInitialized_803C7948 = 1;
    }
}

#pragma push
#pragma use_lmw_stmw on
#pragma dont_inline on
s32 LGOpen(s32 chan, LGDevice **out) {
    void InitDevice(LGDevice *device, s32 index);
    s32 SIResetSteeringAsync(s32 chan, void *callback);
    void SIReadSteering(s32 chan, void *status);
    void HandlePedals(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    void HandleTriggers(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    s32 VDevice_Initialize(VDevice *dev, s32 arg1);
    s32 LGClose(LGDevice *device);
    LGDevice *device;
    SISteeringStatus *calib;
    SISteeringStatus *status;
    u8 *cur;

    if (chan >= 4 || chan < 0) {
        return -2;
    }
    if (out == NULL) {
        return -2;
    }
    device = &g_lgDevices_803771D8[chan];
    if (device->started) {
        LGClose(device);
    }
    if (device->opened) {
        *out = (LGDevice *)device->unk8;
        return -3;
    }
    if (SIResetSteeringAsync(chan, SteeringResetCallback) != -1) {
        InitDevice(device, chan);
        status = &device->status;
        SIReadSteering(chan, status);
        switch (g_lgDevices_803771D8[chan].status.err) {
        case -1:
            g_lgDevices_803771D8[chan].unk4D = -1;
            return -4;
        case 0:
            calib = &device->calibMin;
            cur = (u8 *)&device->curButton;
            AutoCalibrateAxisSigned(&calib->steering, &calib[1].steering,
                                    g_lgDevices_803771D8[chan].status.steering,
                                    g_lgDevices_803771D8[chan].calibDeadzone.steering, (s8 *)cur + 3);
            HandlePedals(cur, status, calib);
            HandleTriggers(cur, status, calib);
            g_lgDevices_803771D8[chan].curButton = g_lgDevices_803771D8[chan].status.button;
            g_lgDevices_803771D8[chan].curMisc = g_lgDevices_803771D8[chan].status.misc;
            g_lgDevices_803771D8[chan].prev = g_lgDevices_803771D8[chan].status;
            g_lgDevices_803771D8[chan].unk4D = 0;
            break;
        }
        g_lgDevices_803771D8[chan].unk4D = 0;
        *out = device;
        *(s32 *)device->unk0 = chan;
        device->unk8 = (s32)*out;
        VDevice_Initialize(&device->vdev, (s32)device);
        device->opened = 1;
        return 0;
    }
    return -4;
}
#pragma pop

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

s32 LGGetDeviceProperty(LGDevice *device, u32 property, s32 *value, u32 size) {
    if (device == NULL || (u32)device == 0xFFFFFFFF) {
        return -2;
    }
    if (value == NULL) {
        return -2;
    }
    if (property != 3) {
        return -2;
    }
    if (!device->opened) {
        return -2;
    }
    if (device->started) {
        LGClose(device);
        return -2;
    }
    *value = device->vdev.unk11F4;
    return 0;
}

s32 LGSetDeviceProperty(LGDevice *device, u32 property, s32 value) {
    if (device == NULL || (u32)device == 0xFFFFFFFF) {
        return -2;
    }
    if (value < 0 || value > 0xFF) {
        return -2;
    }
    if (property != 3) {
        return -2;
    }
    if (!device->opened) {
        return -2;
    }
    if (device->started) {
        LGClose(device);
        return -2;
    }
    device->vdev.unk11F4 = value;
    return 0;
}

#pragma push
#pragma use_lmw_stmw on
#pragma optimize_for_size on
s32 LGRead(void *status) {
    typedef struct LGStatus {
        u16 data[4];
        u8 unk8;
        s8 err;
    } LGStatus;
    LGStatus *out = status;
    s32 i;
    LGDevice *device;

    if (status == NULL) {
        return -2;
    }
    for (i = 0; i < 4; i++) {
        device = &g_lgDevices_803771D8[i];
        if (device->started) {
            LGClose(device);
        }
        if (device->opened) {
            out[i] = *(LGStatus *)((u8 *)device + 0x44);
        } else {
            out[i].err = -1;
        }
    }
    return 0;
}
#pragma pop

s32 VDevice_DownloadEffect(VDevice *dev, u32 *handle, u8 *desc);
s32 LGDownloadForceEffect(LGDevice *device, u32 *handle, u8 *desc) {
    if (device == NULL || (u32)device == 0xFFFFFFFF) {
        return -2;
    }
    if (!device->opened) {
        return -2;
    }
    if (handle == NULL) {
        return -2;
    }
    if (desc == NULL) {
        return -2;
    }
    if (device->started) {
        LGClose(device);
        return -2;
    }
    return VDevice_DownloadEffect(&device->vdev, handle, desc);
}

s32 VDevice_UpdateEffect(VDevice *dev, u32 handle, u8 *desc);
s32 LGUpdateForceEffect(u32 handle, u8 *desc) {
    u32 index = handle >> 16;
    LGDevice *device;

    if (index >= 4) {
        return -2;
    }
    device = &g_lgDevices_803771D8[index];
    if (!device->opened) {
        return -2;
    }
    if (desc == NULL) {
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
    return VDevice_UpdateEffect(&device->vdev, handle, desc);
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
#pragma optimize_for_size on
#pragma dont_inline on
void VDevice_RecalcGammaTable(void) {
    s32 i;
    f32 gamma = 0.6666667f;

    for (i = 0; i < 129; i++) {
        g_iGammaLookup_8037BAF8[i] = (i < 0) ? -(128.0f * (f32)pow((f32)-i / 128.0f, gamma) + 0.5f)
                                             : 128.0f * (f32)pow((f32)i / 128.0f, gamma) + 0.5f;
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

#pragma push
#pragma dont_inline on
void Effect_Init(struct Effect *effect) {
    s32 i;

    zero_mem_aligned(effect, 0x40);
    if (g_bWaveTablesInitialized_803C7958 == 0) {
        for (i = 0; i < 0x40; i++) {
            g_iQuarterSineTable_8037BB7C[i] = 1024.0f * (f32)sin((3.1415927f * (f32)i) / 128.0f);
            g_iRampUpTable_8037BBFC[i] = (i * 0x400) / 63;
        }
        g_bWaveTablesInitialized_803C7958 = 1;
    }
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
s32 Effect_PerformEnvelope(Effect *effect, EffectEnvelope *env, s32 magnitude);
void Effect_PolarToRect(s32 magnitude, s32 angle, s32 *out);
s32 Effect_UpdateSpring(Effect *effect, EffectCondition *cond);
s32 Effect_UpdateDamper(Effect *effect, EffectCondition *cond);
s32 Effect_Update(Effect *effect, u32 dt) {
    s32 active;
    s16 magnitude;
    s32 wave;
    s32 round;
    s32 level;
    u32 a;
    EffectCondition *cond;

    if (effect->flags & 1) {
        if (effect->delay != 0) {
            effect->delay -= dt;
            if ((s32)effect->delay < 0) {
                effect->delay = 0;
            }
        } else {
            effect->elapsed += dt;
            if (effect->duration != 0xFFFFFFFF && effect->duration < effect->elapsed && (effect->flags & 2)) {
                effect->flags &= ~1;
            }
        }
    }
    effect->flags |= 2;

    active = 0;
    if ((effect->flags & 1) && effect->delay == 0) {
        active = 1;
    }
    if (active) {
        if (effect->flags & 0x10) {
            Effect_PolarToRect(Effect_PerformEnvelope(effect, &effect->params.constant.envelope, effect->params.constant.magnitude), effect->params.constant.angle, &effect->force);
        } else if (effect->flags & 0x20) {
            Effect_PolarToRect(effect->params.ramp.start + Effect_MulDiv(effect->params.ramp.end - effect->params.ramp.start, effect->elapsed, effect->duration), effect->params.ramp.angle, &effect->force);
        } else if (effect->flags & 0x40) {
            dt = (dt << 24) / effect->params.periodic.period;
            effect->phase += dt;
            if (effect->phase >= 0x1000000) {
                effect->phase -= effect->phase & 0xFF000000;
            }
            magnitude = Effect_PerformEnvelope(effect, &effect->params.periodic.envelope, effect->params.periodic.magnitude);
            switch (effect->type) {
            case 2:
                wave = Effect_WaveLookup(g_iQuarterSineTable_8037BB7C, (effect->phase >> 16) & 0xFF);
                break;
            case 3:
                wave = -0x400;
                if ((effect->phase >> 16) < 0x80) {
                    wave = 0x400;
                }
                break;
            case 4:
                wave = Effect_WaveLookup(g_iRampUpTable_8037BBFC, (effect->phase >> 16) & 0xFF);
                break;
            case 5:
                a = (effect->phase >> 16) & 0xFF;
                wave = g_iRampUpTable_8037BBFC[a >> 2];
                break;
            case 6:
                a = (effect->phase >> 16) & 0xFF;
                wave = g_iRampUpTable_8037BBFC[(0xFF - a) >> 2];
                break;
            default:
                wave = 0;
                break;
            }
            round = 0x200;
            if (wave < 0) {
                round = -0x200;
            }
            level = wave * magnitude;
            level += round;
            Effect_PolarToRect(level / 0x400 + effect->params.periodic.offset, effect->params.periodic.angle, &effect->force);
        } else if (effect->flags & 0x80) {
            cond = &effect->params.condition;
            switch (effect->type) {
            case 7:
                effect->force = Effect_UpdateSpring(effect, cond);
                break;
            case 8:
                effect->force = Effect_UpdateDamper(effect, cond);
                if (effect->force == 0) {
                    effect->force++;
                }
                break;
            default:
                effect->force = 0;
                break;
            }
        } else {
            return 0;
        }
    } else {
        effect->force = 0;
    }
    return 1;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
s32 Effect_PerformEnvelope(Effect *effect, EffectEnvelope *env, s32 magnitude) {
    u32 elapsed = effect->elapsed;
    s32 start = magnitude < 0 ? -magnitude : magnitude;
    s32 negative = magnitude < 0;
    s32 level = start;
    u32 fadeStart;
    s32 t;
    s32 half;
    s32 delta;

    if (elapsed < env->attackTime) {
        delta = start - env->attackLevel;
        half = env->attackTime / 2;
        if ((s32)elapsed < 0) {
            half = -half;
        }
        level = env->attackLevel + (s32)(half + elapsed * delta) / env->attackTime;
    } else if (effect->duration != 0xFFFFFFFF) {
        fadeStart = effect->duration - env->fadeTime;
        if (elapsed > fadeStart) {
            if (elapsed < effect->duration) {
                t = elapsed - fadeStart;
                delta = env->fadeLevel - start;
                half = env->fadeTime / 2;
                if (t < 0) {
                    half = -half;
                }
                level = start + (half + t * delta) / env->fadeTime;
            } else {
                level = env->fadeLevel;
            }
        }
    }
    if (negative) {
        level = -level;
    }
    return level;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
void Effect_PolarToRect(s32 magnitude, s32 angle, s32 *out) {
    s32 round;
    s32 value;
    s32 sine;
    u32 a = (u8)angle;

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

void SimThread_Init(VDevice *dev, VDevice *owner) {
    static f32 a[10] = {
        0.0995392948f, 0.0998378024f, 0.1000645086f, 0.1002266929f, 0.1003241092f,
        0.1003241092f, 0.1002266929f, 0.1000645086f, 0.0998378024f, 0.0995392948f,
    };
    static s32 ia[10];
    SimThread *t = &dev->sim;
    s32 w[1] = {0x1000};
    s32 i;

    t->lastTick = 0;
    t->historyIdx = 0;
    t->deltaIdx = 0;
    for (i = 0; i < 16; i++) {
        t->deltaPos[i] = 0;
        t->deltaTime[i] = 0;
    }
    t->owner = owner;
    t->lastForce = 0;
    t->posScaled = 0;
    t->pos = 0;
    t->accel = 0;
    t->lastTick = OSGetTick() / ((*(u32 *)0x800000F8 / 4) / 1000);
    SimThread_SetWeights(t, w);
    for (i = 0; i < 10; i++) {
        ia[i] = 4096.0f * a[i];
    }
    SimThread_SetWeights(t, ia);
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

s32 GBAGetProcessStatus(s32 chan, u8 *percentp) {
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    s64 OSGetTime(void);
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    s32 enabled;
    s32 ret;
    u8 percent;
    s64 t;

    enabled = OSDisableInterrupts();
    if (bootInfo->callback != NULL) {
        ret = 2;
        percent = (bootInfo->curOffset * 100) / bootInfo->realLength;
        if (bootInfo->begin != 0) {
            t = OSGetTime();
            if ((t - bootInfo->begin) / ((*(u32 *)0x800000F8 / 4) / 1000) < 5500) {
                percent = (percent * (t - bootInfo->begin)) / (((*(u32 *)0x800000F8 / 4) / 1000) * 5500);
            }
            if (percent >= 100) {
                percent = 100;
            }
        }
        if (percentp != NULL) {
            *percentp = percent;
        }
    } else if (gba->callback != NULL) {
        ret = 2;
    } else {
        ret = 0;
    }
    OSRestoreInterrupts(enabled);
    return ret;
}

s32 GBAGetProcessStatus(s32 chan, u8 *percentp);
#pragma push
#pragma optimize_for_size on
s32 GBAJoyBootAsync(s32 chan, s32 paletteColor, s32 paletteSpeed, u8 *programp, s32 length, u8 *status, GBACallback callback) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    u8 percent;
    s32 ret;

    if (chan & ~3) {
        return 4;
    }
    if (length == 0 || 0x40000 < length) {
        return 4;
    }
    if (paletteSpeed < -4 || paletteSpeed > 4) {
        return 4;
    }
    if (paletteColor < 0 || paletteColor > 6) {
        return 4;
    }
    if (programp[0xac] * programp[0xad] * programp[0xae] * programp[0xaf] == 0) {
        return 4;
    }
    ret = GBAGetProcessStatus(chan, &percent);
    if (ret != 0) {
        return ret;
    }
    bootInfo->paletteColor = paletteColor;
    bootInfo->paletteSpeed = paletteSpeed;
    bootInfo->programp = programp;
    bootInfo->length = length;
    bootInfo->status = status;
    bootInfo->callback = callback;
    bootInfo->curOffset = D54_800DAFF8[8];
    ret = GBAGetStatusAsync(chan, bootInfo->status, F23);
    if (ret != 0) {
        bootInfo->callback = NULL;
    }
    return ret;
}
#pragma pop
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
    if (ret) {
        bootInfo->begin = 0;
        callback = bootInfo->callback;
        if (callback) {
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

#pragma push
#pragma peephole off
void __GBAX02(s32 chan, u8 *readbuf) {
    void DSPAddTask(GBADSPTaskInfo *task);
    GBAControl *gba;
    GBABootInfo *bootInfo;
    GBASecParam *param;
    GBADSPTaskInfo *task;

    gba = &__GBA[chan];
    bootInfo = &gba->bootInfo;
    param = gba->param;
    memcpy(param, readbuf, 4);
    param->paletteColor = bootInfo->paletteColor;
    param->paletteSpeed = bootInfo->paletteSpeed;
    param->length = bootInfo->length;
    param->out = &param->keyA;
    DCInvalidateRange(&param->keyA, 32);
    DCFlushRange(param, 32);

    task = &gba->task;
    task->priority = 255;
    task->iram_mmem_addr = (void *)((u32)D35_800DB028 - 0x80000000);
    task->iram_length = sizeof(D35_800DB028);
    task->iram_addr = 0;
    task->dsp_init_vector = 16;
    task->init_cb = F23;
    task->res_cb = NULL;
    task->done_cb = F25;
    task->req_cb = NULL;
    DSPAddTask(task);
}

#pragma pop

void UpdatePointers(u32 *offsets, u32 count, u32 base) {
    u32 i;

    for (i = 0; i < count; i++) {
        *(u32 *)(base + offsets[i]) += base;
    }
}

void *LoadDataChunkFile(const char *name) {
    s32 File_Open(u32 dev, const char *name, u32 flags, s32 *size);
    u32 File_Read(s32 handle, u32 size, void *buf);
    void File_Close(s32 handle);
    struct {
        u32 count;
        u32 size;
    } header;
    u32 *relocs;
    u8 *data;
    s32 handle;
    u32 i;

    handle = File_Open(0, name, 0, 0);
    if (handle == -1) {
        return NULL;
    }
    header.size = 0;
    header.count = 0;
    File_Read(handle, 8, &header);
    data = Allocate(header.size, 0, "");
    relocs = Allocate(header.count * 4, 0, "");
    if (File_Read(handle, header.count * 4, relocs) == 0) {
        return NULL;
    }
    if (File_Read(handle, header.size, data) == 0) {
        return NULL;
    }
    File_Close(handle);
    {
        u32 count = header.count;
        u32 *p = relocs;
        for (i = 0; i < count; i++) {
            *(u32 *)(data + p[i]) += (u32)data;
        }
    }
    Deallocate((s32)relocs);
    return data;
}

s32 GBAReadAsync(s32 chan, u8 *dst, u8 *status, GBACallback callback);
static void F29(s32 chan, s32 ret);
void F27(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0 && *bootInfo->status != D54_800DAFF8[0]) {
        ret = 3;
    }
    if (ret == 0) {
        ret = GBAReadAsync(chan, bootInfo->readbuf, bootInfo->status, F29);
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

void __GBAX01(s32 chan, s32 ret) {
    void F31(s32 chan, s32 ret);
    s32 GBAWriteAsync(s32 chan, u8 *src, u8 *status, GBACallback callback);
    s64 OSGetTime(void);
    GBAControl *gba;
    GBABootInfo *bootInfo;
    GBACallback callback;
    s32 minLength;
    u8 *d;

    gba = &__GBA[chan];
    d = D54_800DAFF8;
    bootInfo = &gba->bootInfo;
    if (ret == 0) {
        bootInfo->keyA = gba->param->keyA;
        bootInfo->keyB = gba->param->keyB;
        if (bootInfo->readbuf[3] == 0 || bootInfo->readbuf[2] == 0 ||
            (bootInfo->keyA & (d[5] << 9)) == 0 || bootInfo->readbuf[1] == 0 ||
            (bootInfo->keyA >> 15) == 0 || bootInfo->readbuf[0] == 0) {
            ret = 3;
        } else {
            bootInfo->i = ~d[0x24] & (bootInfo->length + d[0x24]);
            minLength = d[0x14] << d[0x21];
            if (bootInfo->i < minLength) {
                bootInfo->i = minLength;
            }
            bootInfo->realLength = bootInfo->i;
            bootInfo->i -= minLength;
            bootInfo->i >>= d[0x20];
            bootInfo->writebuf[0] = (s32)bootInfo->keyB;
            bootInfo->writebuf[1] = (s32)bootInfo->keyB >> 8;
            bootInfo->writebuf[3] = (s32)bootInfo->keyB >> 24;
            bootInfo->writebuf[2] = (s32)bootInfo->keyB >> 16;
            bootInfo->crc = (d[0x26] + 1) << d[0x22];
            bootInfo->curOffset = d[8];
            bootInfo->begin = OSGetTime();
            bootInfo->firstXfer = 1;
            ret = GBAWriteAsync(chan, bootInfo->writebuf, bootInfo->status, F31);
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

static void F35(s32 chan, s32 ret);
#pragma push
#pragma opt_common_subs off
static void F33(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0) {
        for (bootInfo->i = 33; bootInfo->i < 36; bootInfo->i++) {
            ret = (u8)((bootInfo->readbuf[3] ^ (bootInfo->dummyWord[D54_800DAFF8[bootInfo->i]] >> 24)) |
                       (bootInfo->readbuf[2] ^ (bootInfo->dummyWord[D54_800DAFF8[bootInfo->i]] >> 16)) |
                       (bootInfo->readbuf[1] ^ (bootInfo->dummyWord[D54_800DAFF8[bootInfo->i]] >> 8)) |
                       (bootInfo->readbuf[0] ^ bootInfo->dummyWord[D54_800DAFF8[bootInfo->i]])) << 24;
            if (ret != 0) {
                break;
            }
        }
        if (ret != 0) {
            ret = 3;
        } else {
            bootInfo->start = OSGetTick();
            ret = GBAGetStatusAsync(chan, bootInfo->status, F35);
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
#pragma pop
static void F37(s32 chan, s32 ret);
s64 __GBASetDelay(s32 chan, s64 delay);
static void F35(s32 chan, s32 ret) {
    GBAControl *gba = &__GBA[chan];
    GBABootInfo *bootInfo = &gba->bootInfo;
    GBACallback callback;

    if (ret == 0) {
        if ((*(u32 *)0x800000F8 / 4) * 10 <= OSGetTick() - bootInfo->start) {
            ret = 3;
        } else if (*bootInfo->status & 0x32) {
            ret = 3;
        } else if (*bootInfo->status != 8) {
            ret = GBAGetStatusAsync(chan, bootInfo->status, F35);
        } else {
            __GBASetDelay(chan, 8 * ((*(u32 *)0x800000F8 / 4) / 1000));
            ret = GBAReadAsync(chan, bootInfo->readbuf, bootInfo->status, F37);
            __GBASetDelay(chan, (60 * ((*(u32 *)0x800000F8 / 4) / 125000)) / 8);
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

#pragma push
#pragma peephole off
s32 GBAWrite(s32 chan, u8 *src, u8 *status) {
    GBAControl *gba = &__GBA[chan];
    s32 ret;

    if (gba->callback != NULL) {
        ret = 2;
    } else {
        gba->output[0] = 0x15;
        memcpy(&gba->output[1], src, 4);
        gba->ptr = src;
        gba->status = status;
        gba->callback = __GBASyncCallback;
        ret = __GBATransfer(chan, 5, 1, WriteProc);
    }
    if (ret != 0) {
        return ret;
    }
    return __GBASync(chan);
}

void __GBAHandler(s32 chan, u32 error, void *context) {
    extern s32 __GBAReset;
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    GBAControl *gba = &__GBA[chan];
    void (*proc)(s32);
    void (*callback)(s32, s32);
    u8 tmpContext[0x2C8];

    if (__GBAReset != 0) {
        return;
    }
    if (error & 0xF) {
        gba->ret = 1;
    } else {
        gba->ret = 0;
    }
    if (gba->proc) {
        proc = (void (*)(s32))gba->proc;
        gba->proc = NULL;
        proc(chan);
    }
    if (gba->callback) {
        OSClearContext(tmpContext);
        OSSetCurrentContext(tmpContext);
        callback = (void (*)(s32, s32))gba->callback;
        gba->callback = NULL;
        callback(chan, gba->ret);
        OSClearContext(tmpContext);
        OSSetCurrentContext(context);
    }
}

#pragma pop

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

#pragma push
#pragma peephole off
void TypeAndStatusCallback(s32 chan, u32 type) {
    extern s32 __GBAReset;
    void *OSGetCurrentContext(void);
    void OSClearContext(void *);
    void OSSetCurrentContext(void *);
    void __OSReschedule(void);
    s32 SITransfer(s32 chan, void *output, s32 outputBytes, void *input, s32 inputBytes, void *callback, s64 delay);
    GBAControl *gba = &__GBA[chan];
    void *context;
    void (*proc)(s32);
    void (*callback)(s32, s32);
    u8 tmpContext[0x2C8];

    if (__GBAReset != 0) {
        return;
    }
    if ((type & 0xFF) != 0 || (type & 0xFFFF0000) != 0x40000) {
        gba->ret = 1;
    } else {
        if (SITransfer(chan, gba->output, gba->outputBytes, gba->input, gba->inputBytes, __GBAHandler, gba->delay)) {
            return;
        }
        gba->ret = 2;
    }
    if (gba->proc) {
        proc = (void (*)(s32))gba->proc;
        gba->proc = NULL;
        proc(chan);
    }
    if (gba->callback) {
        context = OSGetCurrentContext();
        OSClearContext(tmpContext);
        OSSetCurrentContext(tmpContext);
        callback = (void (*)(s32, s32))gba->callback;
        gba->callback = NULL;
        callback(chan, gba->ret);
        OSClearContext(tmpContext);
        OSSetCurrentContext(context);
        __OSReschedule();
    }
}
#pragma pop
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

s32 FileManager_GetChecksum(u32 size, u8 *data) {
    u32 i;
    s32 sum = 0;

    for (i = 0; i < size; i++) {
        sum += data[i] + 1;
    }
    return sum;
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

s32 FileManager_GetDeviceLastError(u32 device) {
    extern s32 m_cd_drive_status;
    s32 chan;

    if (device == 11 || device == 12) {
        chan = device - 11;
        if (chan < 0) {
            return 0;
        }
        switch (m_memcardstatus_803C7970[chan]) {
        case -6:
            return -3;
        case -128:
        case -5:
            return -4;
        case -2:
            return -6;
        case -9:
            return -1;
        case -100:
            return -2;
        case -13:
            return -5;
        }
    }
    if (device == 1) {
        switch (m_cd_drive_status) {
        case -1:
            return -11;
        case 11:
            return -10;
        case 6:
            return -9;
        case 5:
            return -7;
        case 4:
            return -8;
        }
    }
    return 0;
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

s32 MemCard_Open__FUlPCScUlPl(u32 device, const char *fileName, u32 flags, s32 *unused);
#pragma push
#pragma dont_inline on
s32 File_Open(u32 dev, const char *name, u32 flags, s32 *size) {
    extern struct { s32 fileInfo; char name[0x100]; } g_filedata_PC[4];
    s32 DVDOpen(const char *name, void *fileInfo);
    char *p;
    s32 i;
    s8 ok;

    p = (char *)name;
    strrchr(name, '.');
    while (*p != 0) {
        *p = tolower((u8)*p);
        if (*p == '\\') {
            *p = '/';
        }
        p++;
    }
    for (i = 0; i < 4; i++) {
        if (g_files[i].slot == -1) {
            break;
        }
    }
    if (i == 4) {
        return 0;
    }
    if (dev >= 11) {
        ok = MemCard_Open__FUlPCScUlPl(dev, name, flags, size);
    } else {
        g_filedata_PC[i].fileInfo = (s32)Allocate(0x3C, 0, "");
        ok = DVDOpen(name, (void *)g_filedata_PC[i].fileInfo);
    }
    if (!ok) {
        return -1;
    }
    g_files[i].slot = 1;
    g_files[i].position = 0;
    if (dev < 11) {
        g_files[i].size = *(s32 *)(g_filedata_PC[i].fileInfo + 0x34);
    }
    strcpy(g_filedata_PC[i].name, name);
    if (size != NULL) {
        *size = g_files[i].size;
    }
    if (dev >= 11) {
        return (dev << 16) | (g_files[i].slot << 8) | i;
    }
    return (g_files[i].slot << 8) | i;
}
#pragma pop

#pragma push
#pragma dont_inline on
void File_Close(s32 handle) {
    extern struct { s32 fileInfo; u8 pad[0x100]; } g_filedata_PC[4];
    s32 DVDClose(void *fileInfo);
    void MemCard_Close__Fl(s32 device);
    s32 slot = (s8)(u8)((u32)handle >> 8);
    s32 idx = (s8)(u8)handle;
    s32 device = (s8)(u8)((u32)handle >> 16);

    if (device >= 11) {
        MemCard_Close__Fl(device);
        g_files[idx].slot = -1;
        return;
    }
    if (!(idx >= 0 && idx < 4 && slot == g_files[idx].slot)) {
        return;
    }
    DVDClose((void *)g_filedata_PC[idx].fileInfo);
    Deallocate(g_filedata_PC[idx].fileInfo);
    g_filedata_PC[idx].fileInfo = 0;
    g_files[idx].slot = -1;
}
#pragma pop

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

u32 FileManager_GetDeviceStatus(u32 device) {
    s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectorSize);
    s32 CARDMount(s32 chan, void *workArea, void *detachCallback);
    s32 CARDCheck(s32 chan);
    s32 CARDGetEncoding(s32 chan, u16 *encode);
    s32 CARDFreeBlocks(s32 chan, s32 *byteNotUsed, s32 *filesNotUsed);
    s32 CARDOpen(s32 chan, const char *fileName, CARDFileInfo *fileInfo);
    s32 CARDClose(CARDFileInfo *fileInfo);
    s32 CARDUnmount(s32 chan);
    u16 encoding;
    s32 memSize;
    s32 sectorSize;
    s32 bytesNotUsed;
    s32 filesNotUsed;
    CARDFileInfo fileInfo;
    s32 result;

    if (device == 11 || device == 12) {
        result = CARDProbeEx(device - 11, &memSize, &sectorSize);
        if (result != 0) {
            m_memcardstatus_803C7970[device - 11] = result;
            return 3;
        }
        if (sectorSize != 0x2000) {
            m_memcardstatus_803C7970[device - 11] = -100;
            return 3;
        }
        result = CARDMount(device - 11, CardWorkArea, 0);
        if (result == -6) {
            if (m_memcardstatus_803C7970[device - 11] != -5) {
                result = CARDCheck(device - 11);
            } else {
                result = m_memcardstatus_803C7970[device - 11];
            }
        }
        if ((m_memcardstatus_803C7970[device - 11] = result) != 0) {
            return 3;
        }
        CARDGetEncoding(device - 11, &encoding);
        if (encoding != 0) {
            m_memcardstatus_803C7970[device - 11] = -13;
            CARDUnmount(device - 11);
            return 3;
        }
        CARDFreeBlocks(device - 11, &bytesNotUsed, &filesNotUsed);
        result = CARDOpen(device - 11, g_appInfo[3], &fileInfo);
        if (result == 0) {
            CARDClose(&fileInfo);
        }
        CARDUnmount(device - 11);
        if (result == -4 && (bytesNotUsed == 0 || filesNotUsed == 0)) {
            m_memcardstatus_803C7970[device - 11] = -9;
            return 3;
        }
        return 1;
    }
    if (device == 0) {
        return 1;
    }
    return device == 1;
}

s32 CARDGetSectorSize(s32 chan, s32 *size);
s32 CARDOpen(s32 chan, const char *fileName, CARDFileInfo *fileInfo);
s32 CARDCreate(s32 chan, const char *fileName, s32 size, CARDFileInfo *fileInfo);
u32 FileManager_GetDeviceStatus(u32 device);
#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
s32 MemCard_Open__FUlPCScUlPl(u32 device, const char *fileName, u32 flags, s32 *unused) {
    u32 chan;
    CARDFileInfo *fileInfo;
    s32 *blockSize;

    chan = device - 11;
    if (FileManager_GetDeviceStatus(device) == 3) {
        return 0;
    }
    m_memcardstatus_803C7970[chan] = CARDMount(chan, CardWorkArea, 0);
    OSReport("CARDMount\n");
    if (m_memcardstatus_803C7970[chan] < 0) {
        OSReport("CARD error %d\n", m_memcardstatus_803C7970[chan]);
        return 0;
    }
    blockSize = &memCardBlockSize[chan];
    CARDGetSectorSize(chan, blockSize);
    fileInfo = &memCardfileInfo[chan];
    m_memcardstatus_803C7970[chan] = CARDOpen(chan, fileName, fileInfo);
    if (m_memcardstatus_803C7970[chan] == 0) {
        return 1;
    }
    if (m_memcardstatus_803C7970[chan] == -4 && (flags & 1)) {
        m_memcardstatus_803C7970[chan] = CARDCreate(chan, fileName, *blockSize, fileInfo);
        if (m_memcardstatus_803C7970[chan] < 0) {
            m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
            OSReport("CARD error %d\n", m_memcardstatus_803C7970[chan]);
            return 0;
        }
        OSReport("CARDCreate\n");
    } else {
        m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
        return 0;
    }
    return 1;
}
#pragma pop

#pragma push
#pragma readonly_strings on
void MemCard_Close__Fl(s32 device) {
    s32 chan = device - 11;

    m_memcardstatus_803C7970[chan] = CARDClose(&memCardfileInfo[chan]);
    m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
    OSReport("CARDClose\n");
}
#pragma pop

s32 MemCard_Read__FUllPv(u32 device, s32 size, void *data) {
    u8 buffer[0x2000] __attribute__((aligned(32)));
    s32 chan = device - 11;

    m_memcardstatus_803C7970[chan] = CARDRead(&memCardfileInfo[chan], buffer, memCardBlockSize[chan], 0);
    memcpy(data, buffer + 0x840, size);
    return size;
}

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
s32 MemCard_Write__FUllPv(u32 id, s32 size, void *data) {
    typedef struct CARDStat {
        char fileName[32];
        u32 length;
        u32 time;
        u8 gameName[4];
        u8 company[2];
        u8 bannerFormat;
        u8 __padding;
        u32 iconAddr;
        u16 iconFormat;
        u16 iconSpeed;
        u32 commentAddr;
        u32 offsetBanner;
        u32 offsetBannerTlut;
        u32 offsetIcon[8];
        u32 offsetIconTlut;
        u32 offsetData;
    } CARDStat;
    s32 CARDWrite(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset);
    s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat);
    s32 CARDSetStatus(s32 chan, s32 fileNo, CARDStat *stat);
    extern u8 rawData_800DB404[0x800];
    CARDStat stat;
    u8 buffer[0x2000] __attribute__((aligned(32)));
    s32 chan = id - 11;

    memset(buffer, 0, 0x40);
    sprintf((char *)buffer, "Hot Wheels Velocity X Save");
    sprintf((char *)buffer + 0x20, "Game Data");
    memcpy(buffer + 0x40, rawData_800DB404, 0x800);
    memcpy(buffer + 0x840, data, size);
    m_memcardstatus_803C7970[chan] = CARDWrite(&memCardfileInfo[chan], buffer, memCardBlockSize[chan], 0);
    if (m_memcardstatus_803C7970[chan] < 0) {
        if (m_memcardstatus_803C7970[chan] == -5) {
            CARDDelete(chan, g_appInfo[3]);
        }
        m_memcardstatus_803C7970[chan] = CARDUnmount(chan);
        OSReport("CARD error %d\n", m_memcardstatus_803C7970[chan]);
        return 0;
    }
    OSReport("CARDWrite\n");
    m_memcardstatus_803C7970[chan] = CARDGetStatus(chan, memCardfileInfo[chan].fileNo, &stat);
    stat.commentAddr = 0;
    stat.iconAddr = 0x40;
    stat.bannerFormat = (u8)((stat.bannerFormat & ~3) | 0);
    stat.bannerFormat = (u8)((stat.bannerFormat & ~4) | 0);
    stat.iconFormat = (u16)((stat.iconFormat & ~3) | 2);
    stat.iconSpeed = (u16)((stat.iconSpeed & ~3) | 3);
    stat.iconSpeed = (u16)((stat.iconSpeed & ~(3 << 2)) | (0 << 2));
    m_memcardstatus_803C7970[chan] = CARDSetStatus(chan, memCardfileInfo[chan].fileNo, &stat);
    return 1;
}
#pragma pop

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

s32 Input_GetAnalog(s32 chan, s16 *stickX, s16 *stickY, s16 *substickX, s16 *substickY) {
    f32 _fabs(f32 x);
    u32 i;

    if (chan == -1) {
        for (i = 0; i < 4; i++) {
            if (stickX != NULL && _fabs(*stickX) > _fabs((s8)g_VirtualPad[i].keyValue[0x10])) {
                *stickX = (s8)g_VirtualPad[i].keyValue[0x10];
            }
            if (stickY != NULL && _fabs(*stickY) > _fabs((s8)g_VirtualPad[i].keyValue[0x11])) {
                *stickY = (s8)g_VirtualPad[i].keyValue[0x11];
            }
            if (substickX != NULL && _fabs(*substickX) > _fabs((s8)g_VirtualPad[i].keyValue[0x12])) {
                *substickX = (s8)g_VirtualPad[i].keyValue[0x12];
            }
            if (substickY != NULL && _fabs(*substickY) > _fabs((s8)g_VirtualPad[i].keyValue[0x13])) {
                *substickY = (s8)g_VirtualPad[i].keyValue[0x13];
            }
        }
    } else {
        if (stickX != NULL) {
            *stickX = (s8)g_VirtualPad[chan].keyValue[0x10];
        }
        if (stickY != NULL) {
            *stickY = (s8)g_VirtualPad[chan].keyValue[0x11];
        }
        if (substickX != NULL) {
            *substickX = (s8)g_VirtualPad[chan].keyValue[0x12];
        }
        if (substickY != NULL) {
            *substickY = (s8)g_VirtualPad[chan].keyValue[0x13];
        }
    }
    return 1;
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

#pragma push
#pragma use_lmw_stmw on
void Input_Init(void) {
    u32 i;

    memset(s_unk30, 0, 8);
    memset(s_unk70, 0, 8);
    for (i = 0; i < 4; i++) {
        memset(&g_VirtualPad[i], 0, sizeof(VirtualPad));
        g_VirtualPad[i].unk0[2] = 1;
        g_VirtualPad[i].forceFeedbackActive = 1;
        g_padUpPos[i] = 0;
        g_padDownPos[i] = 0;
        rumbleStatus[i].duration = -1;
        rumbleStatus[i].active = 0;
        s_unk108[i].unkA = 0;
    }
    s_rumbleTime[0] = 200;
    s_rumbleTime[1] = 200;
    g_StopInput = 0;
}
#pragma pop

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

extern u16 g_pKeyToDI[20];
void InitTables__Fv(void) {
    memset(g_pKeyToDI, 0, sizeof(g_pKeyToDI));
    g_pKeyToDI[0] = 0x1000;
    g_pKeyToDI[1] = 0;
    g_pKeyToDI[2] = 1;
    g_pKeyToDI[3] = 2;
    g_pKeyToDI[4] = 8;
    g_pKeyToDI[5] = 4;
    g_pKeyToDI[6] = 0x200;
    g_pKeyToDI[7] = 0x400;
    g_pKeyToDI[8] = 0x800;
    g_pKeyToDI[9] = 0x100;
    g_pKeyToDI[10] = 0;
    g_pKeyToDI[11] = 0x40;
    g_pKeyToDI[12] = 0x10;
    g_pKeyToDI[13] = 0x20;
    g_pKeyToDI[14] = 0;
    g_pKeyToDI[15] = 0;
    g_pKeyToDI[16] = 0;
    g_pKeyToDI[17] = 0;
    g_pKeyToDI[18] = 0;
    g_pKeyToDI[19] = 0;
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

void PlayConstantForce__8LGWheelsFlsUs(void *wheels, s32 chan, s16 magnitude, u16 direction);
void Wheel_PlayConstantForce(s32 chan, s16 magnitude, u16 direction) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayConstantForce__8LGWheelsFlsUs(wheels, chan, magnitude, direction);
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

s32 Destroy__5ForceFll(Force *force, s32 chan, s32 effect);
#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
#pragma dont_inline on
s32 DownloadForce__8ConstantFllRUlUlUlsUsUlUlUcUc(Force *force, s32 chan, s32 effect, u32 *device, u32 duration, u32 startDelay, s16 magnitude, u16 direction, u32 attackTime, u32 fadeTime, u8 attackLevel, u8 fadeLevel) {
    struct {
        u8 type;
        u32 duration;
        u32 startDelay;
        s16 magnitude;
        u16 direction;
        u32 attackTime;
        u32 fadeTime;
        u8 attackLevel;
        u8 fadeLevel;
        u8 pad[10];
    } desc;
    s32 result = 0;

    if (force->effectIds[chan][effect] != 0xFFFFFFFF) {
        Destroy__5ForceFll(force, chan, effect);
    }
    if (*device != 0xFFFFFFFF) {
        memset(&desc, 0, sizeof(desc));
        desc.type = 0;
        desc.duration = duration;
        desc.startDelay = startDelay;
        desc.magnitude = magnitude;
        desc.direction = direction;
        desc.attackTime = attackTime;
        desc.attackLevel = attackLevel;
        desc.fadeTime = fadeTime;
        desc.fadeLevel = fadeLevel;
        result = LGDownloadForceEffect((LGDevice *)*device, &force->effectIds[chan][effect], (u8 *)&desc);
        if (result < 0) {
            OSReport("ERROR: DownloadForce(constant force) on channel %d returned %d\n", chan, result);
            force->effectIds[chan][effect] = 0xFFFFFFFF;
        }
    } else {
        OSReport("ERROR: Trying to download a constant force to channel %d but wheel has not been opened.\n", chan);
    }
    return result;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
s32 UpdateForce__8ConstantFllUlUlsUsUlUlUcUc(Force *force, s32 chan, s32 effect, u32 duration, u32 startDelay, s16 magnitude, u16 direction, u32 attackTime, u32 fadeTime, u8 attackLevel, u8 fadeLevel) {
    s32 LGUpdateForceEffect(u32 handle, u8 *desc);
    struct {
        u8 type;
        u32 duration;
        u32 startDelay;
        s16 magnitude;
        u16 direction;
        u32 attackTime;
        u32 fadeTime;
        u8 attackLevel;
        u8 fadeLevel;
        u8 pad[10];
    } desc;
    s32 result;

    memset(&desc, 0, sizeof(desc));
    desc.type = 0;
    desc.duration = duration;
    desc.startDelay = startDelay;
    desc.magnitude = magnitude;
    desc.direction = direction;
    desc.attackTime = attackTime;
    desc.attackLevel = attackLevel;
    desc.fadeTime = fadeTime;
    desc.fadeLevel = fadeLevel;
    result = LGUpdateForceEffect(force->effectIds[chan][effect], (u8 *)&desc);
    if (result < 0) {
        OSReport("ERROR: UpdateForce(constant force) on channel %d returned %d\n", chan, result);
        force->effectIds[chan][effect] = 0xFFFFFFFF;
    }
    return result;
}
#pragma pop

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

void ReadAll__8LGWheelsFv(void *wheels) {
    s16 ReadAll__6WheelsFv(Wheels *wheels);
    LGWheels *lg = wheels;
    s16 chan;
    s32 i;

    chan = ReadAll__6WheelsFv((Wheels *)lg->wheels);
    memcpy(lg, lg->wheels, 0x28);
    if (chan != -1) {
        for (i = 0; i < 8; i++) {
            lg->condition.effectIds[chan][i] = 0xFFFFFFFF;
            lg->condition.playing[chan][i] = 0;
            lg->constant.effectIds[chan][i] = 0xFFFFFFFF;
            lg->constant.playing[chan][i] = 0;
            lg->periodic.effectIds[chan][i] = 0xFFFFFFFF;
            lg->periodic.playing[chan][i] = 0;
            lg->ramp.effectIds[chan][i] = 0xFFFFFFFF;
            lg->ramp.playing[chan][i] = 0;
        }
    }
}

u8 SameSpringForceParams__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient);
void PlaySpringForce__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient) {
    LGWheels *lg = wheels;
    u8 *p;
    s32 Start__5ForceFll();

    if (IsOpened__6WheelsFl((Wheels *)lg->wheels, chan)) {
        if (lg->condition.playing[chan][0]) {
            if (!SameSpringForceParams__8LGWheelsFlScUcs(wheels, chan, offset, saturation, coefficient)) {
                if (UpdateForce__9ConditionFllUcUlUlScUcUcUcss(&lg->condition, chan, 0, 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient) >= 0) {
                    p = (u8 *)wheels + chan * 4;
                    p[0x13CE] = offset;
                    p[0x13CF] = saturation;
                    *(s16 *)(p + 0x13D0) = coefficient;
                }
            }
        } else if (lg->condition.effectIds[chan][0] == 0xFFFFFFFF) {
            if (DownloadForce__9ConditionFllRUlUcUlUlScUcUcUcss(&lg->condition, chan, 0, &lg->overallGainId[chan], 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient) >= 0) {
                p = (u8 *)wheels + chan * 4;
                p[0x13CE] = offset;
                p[0x13CF] = saturation;
                *(s16 *)(p + 0x13D0) = coefficient;
            }
            Start__5ForceFll(&lg->condition, chan, 0);
        } else if (SameSpringForceParams__8LGWheelsFlScUcs(wheels, chan, offset, saturation, coefficient)) {
            Start__5ForceFll(&lg->condition, chan, 0);
        } else {
            if (UpdateForce__9ConditionFllUcUlUlScUcUcUcss(&lg->condition, chan, 0, 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient) >= 0) {
                p = (u8 *)wheels + chan * 4;
                p[0x13CE] = offset;
                p[0x13CF] = saturation;
                *(s16 *)(p + 0x13D0) = coefficient;
            }
            Start__5ForceFll(&lg->condition, chan, 0);
        }
    } else {
        OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
    }
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

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
#pragma dont_inline on
void PlayDamperForce__8LGWheelsFlUcs(void *wheels, s32 chan, u8 coefficient, s16 offset) {
    LGWheels *lg = wheels;
    u8 *p;

    if (lg->condition.playing[chan][2]) {
        return;
    }
    if (IsOpened__6WheelsFl((Wheels *)lg->wheels, chan)) {
        if (lg->condition.playing[chan][1]) {
            if (!SameDamperForceParams__8LGWheelsFlUcs(wheels, chan, coefficient, offset)) {
                if (UpdateForce__9ConditionFllUcUlUlScUcUcUcss(&lg->condition, chan, 1, 8, -1, 0, 0, 0, coefficient, coefficient, offset, offset) >= 0) {
                    p = (u8 *)wheels + chan * 4;
                    p[0x13EE] = coefficient;
                    *(s16 *)(p + 0x13F0) = offset;
                }
            }
        } else if (lg->condition.effectIds[chan][1] == 0xFFFFFFFF) {
            if (DownloadForce__9ConditionFllRUlUcUlUlScUcUcUcss(&lg->condition, chan, 1, &lg->overallGainId[chan], 8, -1, 0, 0, 0, coefficient, coefficient, offset, offset) >= 0) {
                p = (u8 *)wheels + chan * 4;
                p[0x13EE] = coefficient;
                *(s16 *)(p + 0x13F0) = offset;
            }
            Start__5ForceFll(&lg->condition, chan, 1);
        } else if (SameDamperForceParams__8LGWheelsFlUcs(wheels, chan, coefficient, offset)) {
            Start__5ForceFll(&lg->condition, chan, 1);
        } else {
            if (UpdateForce__9ConditionFllUcUlUlScUcUcUcss(&lg->condition, chan, 1, 8, -1, 0, 0, 0, coefficient, coefficient, offset, offset) >= 0) {
                p = (u8 *)wheels + chan * 4;
                p[0x13EE] = coefficient;
                *(s16 *)(p + 0x13F0) = offset;
            }
            Start__5ForceFll(&lg->condition, chan, 1);
        }
    } else {
        OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
    }
}
#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
#pragma dont_inline on
void PlayConstantForce__8LGWheelsFlsUs(void *wheels, s32 chan, s16 magnitude, u16 direction) {
    LGWheels *lg = wheels;
    u8 *p;

    if (IsOpened__6WheelsFl((Wheels *)lg->wheels, chan)) {
        if (lg->constant.playing[chan][0]) {
            if (!SameConstantForceParams__8LGWheelsFlsUs(wheels, chan, magnitude, direction)) {
                if (UpdateForce__8ConstantFllUlUlsUsUlUlUcUc(&lg->constant, chan, 0, -1, 0, magnitude, direction, 0, 0, 0, 0) >= 0) {
                    p = (u8 *)wheels + chan * 4;
                    *(s16 *)(p + 0x13DE) = magnitude;
                    *(u16 *)(p + 0x13E0) = direction;
                }
            }
        } else if (lg->constant.effectIds[chan][0] == 0xFFFFFFFF) {
            if (DownloadForce__8ConstantFllRUlUlUlsUsUlUlUcUc(&lg->constant, chan, 0, &lg->overallGainId[chan], -1, 0, magnitude, direction, 0, 0, 0, 0) >= 0) {
                p = (u8 *)wheels + chan * 4;
                *(s16 *)(p + 0x13DE) = magnitude;
                *(u16 *)(p + 0x13E0) = direction;
            }
            Start__5ForceFll(&lg->constant, chan, 0);
        } else if (SameConstantForceParams__8LGWheelsFlsUs(wheels, chan, magnitude, direction)) {
            Start__5ForceFll(&lg->constant, chan, 0);
        } else {
            if (UpdateForce__8ConstantFllUlUlsUsUlUlUcUc(&lg->constant, chan, 0, -1, 0, magnitude, direction, 0, 0, 0, 0) >= 0) {
                p = (u8 *)wheels + chan * 4;
                *(s16 *)(p + 0x13DE) = magnitude;
                *(u16 *)(p + 0x13E0) = direction;
            }
            Start__5ForceFll(&lg->constant, chan, 0);
        }
    } else {
        OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
    }
}
#pragma pop

s32 DownloadForce__8PeriodicFllRUlUcUlUlUcUsUsUssUlUlUcUc(Force *force, s32 chan, s32 effect, u32 *device, u8 type, u32 duration, u32 startDelay, u8 magnitude, u16 direction, u16 period, u16 phase, s16 offset, u32 attackTime, u32 fadeTime, u8 attackLevel, u8 fadeLevel);
s32 UpdateForce__8PeriodicFllUcUlUlUcUsUsUssUlUlUcUc(Force *force, s32 chan, s32 effect, u8 type, u32 duration, u32 startDelay, u8 axis, u16 magnitude, u16 offset, u16 phase, s16 period, u32 attackTime, u32 fadeTime, u8 attackLevel, u8 fadeLevel);
#pragma push
#pragma readonly_strings on
#pragma dont_inline on
void PlayFrontalCollisionForce__8LGWheelsFlUc(LGWheels *lg, s32 chan, u8 force) {
    s32 Start__5ForceFll();

    if (IsOpened__6WheelsFl((Wheels *)lg->wheels, chan)) {
        if (lg->periodic.playing[chan][0]) {
            if (SameFrontalCollisionForceParams__8LGWheelsFls(lg, chan, force)) {
                Start__5ForceFll(&lg->periodic, chan, 0);
            } else {
                if (UpdateForce__8PeriodicFllUcUlUlUcUsUsUssUlUlUcUc(&lg->periodic, chan, 0, 3, 150, 0, force, 90, 75, 0, 0, 0, 20, 0, 0) >= 0) {
                    lg->frontalCollisionParams[chan] = force;
                }
                Start__5ForceFll(&lg->periodic, chan, 0);
            }
        } else if (lg->periodic.effectIds[chan][0] == 0xFFFFFFFF) {
            if (DownloadForce__8PeriodicFllRUlUcUlUlUcUsUsUssUlUlUcUc(&lg->periodic, chan, 0, &lg->overallGainId[chan], 3, 150, 0, force, 90, 75, 0, 0, 0, 20, 0, 0) >= 0) {
                lg->frontalCollisionParams[chan] = force;
            }
            Start__5ForceFll(&lg->periodic, chan, 0);
        } else if (SameFrontalCollisionForceParams__8LGWheelsFls(lg, chan, force)) {
            Start__5ForceFll(&lg->periodic, chan, 0);
        } else {
            if (UpdateForce__8PeriodicFllUcUlUlUcUsUsUssUlUlUcUc(&lg->periodic, chan, 0, 3, 150, 0, force, 90, 75, 0, 0, 0, 20, 0, 0) >= 0) {
                lg->frontalCollisionParams[chan] = force;
            }
            Start__5ForceFll(&lg->periodic, chan, 0);
        }
    } else {
        OSReport("ERROR: trying to play a force on port %d but no wheel opened.\n", chan);
    }
}
#pragma pop
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

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
#pragma dont_inline on
s32 DownloadForce__8PeriodicFllRUlUcUlUlUcUsUsUssUlUlUcUc(Force *force, s32 chan, s32 effect, u32 *device, u8 type, u32 duration, u32 startDelay, u8 magnitude, u16 direction, u16 period, u16 phase, s16 offset, u32 attackTime, u32 fadeTime, u8 attackLevel, u8 fadeLevel) {
    struct {
        u8 type;
        u32 duration;
        u32 startDelay;
        u8 magnitude;
        u16 direction;
        u16 period;
        u16 phase;
        s16 offset;
        u32 attackTime;
        u32 fadeTime;
        u8 attackLevel;
        u8 fadeLevel;
        u8 pad[2];
    } desc;
    s32 result = 0;

    if (force->effectIds[chan][effect] != 0xFFFFFFFF) {
        Destroy__5ForceFll(force, chan, effect);
    }
    if (*device != 0xFFFFFFFF) {
        memset(&desc, 0, sizeof(desc));
        desc.type = type;
        desc.duration = duration;
        desc.startDelay = startDelay;
        desc.magnitude = magnitude;
        desc.direction = direction;
        desc.period = period;
        desc.phase = phase;
        desc.offset = offset;
        desc.attackTime = attackTime;
        desc.fadeTime = fadeTime;
        desc.attackLevel = attackLevel;
        desc.fadeLevel = fadeLevel;
        result = LGDownloadForceEffect((LGDevice *)*device, &force->effectIds[chan][effect], (u8 *)&desc);
        if (result < 0) {
            OSReport("ERROR: DownloadForce(periodic force) on channel %d returned %d\n", chan, result);
            force->effectIds[chan][effect] = 0xFFFFFFFF;
        }
    } else {
        OSReport("ERROR: Trying to download a periodic force to channel %d but wheel has not been opened.\n", chan);
    }
    return result;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
s32 UpdateForce__8PeriodicFllUcUlUlUcUsUsUssUlUlUcUc(Force *force, s32 chan, s32 effect, u8 type, u32 duration, u32 startDelay, u8 axis, u16 magnitude, u16 offset, u16 phase, s16 period, u32 attackTime, u32 fadeTime, u8 attackLevel, u8 fadeLevel) {
    s32 LGUpdateForceEffect(u32 handle, u8 *desc);
    struct {
        u8 type;
        u32 duration;
        u32 startDelay;
        u8 axis;
        u16 magnitude;
        u16 offset;
        u16 phase;
        s16 period;
        u32 attackTime;
        u32 fadeTime;
        u8 attackLevel;
        u8 fadeLevel;
    } desc;
    s32 result;

    memset(&desc, 0, sizeof(desc));
    desc.type = type;
    desc.duration = duration;
    desc.startDelay = startDelay;
    desc.axis = axis;
    desc.magnitude = magnitude;
    desc.offset = offset;
    desc.phase = phase;
    desc.period = period;
    desc.attackTime = attackTime;
    desc.fadeTime = fadeTime;
    desc.attackLevel = attackLevel;
    desc.fadeLevel = fadeLevel;
    result = LGUpdateForceEffect(force->effectIds[chan][effect], (u8 *)&desc);
    if (result < 0) {
        OSReport("ERROR: UpdateForce(periodic force) on channel %d returned %d\n", chan, result);
        force->effectIds[chan][effect] = 0xFFFFFFFF;
    }
    return result;
}
#pragma pop

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

#pragma push
#pragma readonly_strings on
s16 ReadAll__6WheelsFv(Wheels *wheels) {
    s32 LGOpen(s32 chan, u32 *handle);
    s32 LGRead(void *status);
    u8 IsConnected__6WheelsFl(Wheels *wheels, s32 chan);
    s32 i;

    for (i = 0; i < 4; i++) {
        if (IsConnected__6WheelsFl(wheels, i) && *(u32 *)&wheels->unk828[i * 4] == (u32)-1) {
            if (LGOpen(i, (u32 *)&wheels->unk828[i * 4]) < 0) {
                OSReport("ERROR: Could not open wheel on channel %d\n", i);
            } else {
                wheels->status[i].err = 0;
            }
            break;
        }
    }
    memcpy((u8 *)wheels + 0x858, wheels, 0x28);
    LGRead(wheels);
    for (i = 0; i < 4; i++) {
        if (wheels->status[i].err == -1 && *(u32 *)&wheels->unk828[i * 4] != (u32)-1) {
            ((u32 *)wheels->unk828)[i] = -1;
            return i;
        }
    }
    return -1;
}
#pragma pop

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

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
#pragma dont_inline on
s32 DownloadForce__9ConditionFllRUlUcUlUlScUcUcUcss(Force *force, s32 chan, s32 effect, u32 *device, u8 type, u32 duration, u32 startDelay, s8 offset, u8 deadband, u8 saturationPos, u8 saturationNeg, s16 coefficientPos, s16 coefficientNeg) {
    struct {
        u8 type;
        u32 duration;
        u32 startDelay;
        struct {
            s8 offset;
            u8 deadband;
            u8 saturationPos;
            u8 saturationNeg;
            s16 coefficientPos;
            s16 coefficientNeg;
        } condition[2];
        u8 pad[8];
    } desc;
    s32 result = 0;

    if (force->effectIds[chan][effect] != 0xFFFFFFFF) {
        Destroy__5ForceFll(force, chan, effect);
    }
    if (*device != 0xFFFFFFFF) {
        memset(&desc, 0, sizeof(desc));
        desc.type = type;
        desc.duration = duration;
        desc.startDelay = startDelay;
        desc.condition[0].offset = offset;
        desc.condition[0].deadband = deadband;
        desc.condition[0].saturationPos = saturationPos;
        desc.condition[0].saturationNeg = saturationNeg;
        desc.condition[0].coefficientPos = coefficientPos;
        desc.condition[0].coefficientNeg = coefficientNeg;
        desc.condition[1].offset = offset;
        desc.condition[1].deadband = deadband;
        desc.condition[1].saturationPos = saturationPos;
        desc.condition[1].saturationNeg = saturationNeg;
        desc.condition[1].coefficientPos = coefficientPos;
        desc.condition[1].coefficientNeg = coefficientNeg;
        result = LGDownloadForceEffect((LGDevice *)*device, &force->effectIds[chan][effect], (u8 *)&desc);
        if (result < 0) {
            OSReport("ERROR: DownloadForce(condition force) on channel %d returned %d\n", chan, result);
            force->effectIds[chan][effect] = 0xFFFFFFFF;
        }
    } else {
        OSReport("ERROR: Trying to download a condition force to channel %d but wheel has not been opened.\n", chan);
    }
    return result;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
#pragma readonly_strings on
s32 UpdateForce__9ConditionFllUcUlUlScUcUcUcss(Force *force, s32 chan, s32 effect, u8 type, u32 duration, u32 startDelay, s8 offset, u8 deadband, u8 posSaturation, u8 negSaturation, s16 posCoefficient, s16 negCoefficient)
{
  s32 LGUpdateForceEffect(u32 handle, u8 *desc);
  struct 
  {
    u8 type;
    u32 duration;
    u32 startDelay;
    struct 
    {
      s8 offset;
      u8 deadband;
      u8 posSaturation;
      u8 negSaturation;
      s16 posCoefficient;
      s16 negCoefficient;
    } condition[2];
    u8 pad[8];
  } desc;
  s32 result;
  memset(&desc, 0, sizeof(desc));
  desc.type = type;
  desc.duration = duration;
  desc.startDelay = startDelay;
  desc.condition[0].offset = offset;
  desc.condition[0].deadband = deadband;
  desc.condition[0].posSaturation = posSaturation;
  desc.condition[0].negSaturation = negSaturation;
  desc.condition[0].posCoefficient = (long long) posCoefficient;
  desc.condition[0].negCoefficient = negCoefficient;
  desc.condition[1].offset = offset;
  desc.condition[1].deadband = deadband;
  desc.condition[1].posSaturation = posSaturation;
  desc.condition[1].negSaturation = negSaturation;
  desc.condition[1].posCoefficient = posCoefficient;
  desc.condition[1].negCoefficient = negCoefficient;
  result = LGUpdateForceEffect(force->effectIds[chan][effect], (u8 *) (&desc));
  if (result < 0)
  {
    OSReport("ERROR: UpdateForce(condition force) on channel %d returned %d\n", chan, result);
    force->effectIds[chan][effect] = 0xFFFFFFFF;
  }
  return result;
}
#pragma pop

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

