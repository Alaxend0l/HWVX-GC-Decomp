/* Basic types for this decompilation (CodeWarrior conventions). Agents add
 * structs and prototypes here or in new headers next to it. */
#ifndef COMMON_H
#define COMMON_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long s32;
typedef unsigned long u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef int BOOL;
typedef struct Vector4 {
  f32 x;
  f32 y;
  f32 z;
  f32 w;
} Vector4;
typedef struct Quaternion {
  f32 x;
  f32 y;
  f32 z;
  f32 w;
} Quaternion;
typedef struct Matrix4 {
  Vector4 v[4];
} Matrix4;
#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct GameInfo {
  const char *a;
  const char *b;
  const char *c;
  const char *d;
  const char *e;
  u8 unk14[0xC];
} GameInfo;

typedef struct WorldData {
  void *world;
  s32 unk4;
  s32 unk8[4];
} WorldData;

typedef struct Camera {
  u8 pad0[0xB4];
  u32 *mode;
  f32 modeTime;
  u8 pad1[0x190 - 0xBC];
} Camera;

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
  f32 a[3];
  f32 k[3];
  f32 lpos[3];
  f32 ldir[3];
} GXLightObjInt;

typedef struct ShaderHolderGX {
  void *gx;
  void *shader;
} ShaderHolderGX;

typedef struct ShaderDef {
  u8 pad0[4];
  u32 numColorKeys;
  u8 pad8[4];
  u32 numPointKeys;
  u8 pad10[4];
  u32 *textureKeys;
} ShaderDef;

typedef struct Shader {
  u8 pad0[0x2C];
  u8 unk2C;
  u8 pad2D[0x38 - 0x2D];
  u8 unk38;
  u8 pad39[0x58 - 0x39];
  u8 unk58;
  u8 pad59[0x5C - 0x59];
  ShaderDef *def;
  u8 hasColorKey;
  u8 hasTextureKey;
  u8 hasPointKey;
  u8 pad63;
  u32 refCount;
  u8 pad68[4];
} Shader;

typedef struct ShaderHolder {
  void *shaders[0x40];
  Shader slots[0x40];
  ShaderHolderGX gx[0x80];
  u32 numShaders;
  u32 numGX;
} ShaderHolder;

typedef struct DividingAxis {
  f32 dir;
  f32 minA;
  f32 maxA;
  f32 minB;
  f32 maxB;
  f32 tEnter;
  f32 tExit;
} DividingAxis;

typedef struct DVDQueue {
  struct DVDQueue *next;
  struct DVDQueue *prev;
} DVDQueue;

typedef struct TextureKeyAnim {
  u32 numKeys;
  u32 *keys;
  u32 frameTime;
} TextureKeyAnim;

typedef struct TextureKeyState {
  u16 texture;
  u16 key;
  f32 time;
  u8 reset;
} TextureKeyState;

typedef struct CollisionTri {
  Vector4 pos;    /* 0x00 */
  Vector4 normal; /* 0x10 */
  u32 triId1;     /* 0x20 */
  u32 triId0;     /* 0x24 */
  u32 time;       /* 0x28 */
  u8 pad2C[4];    /* 0x2C */
} CollisionTri;

typedef struct OBox {
  Vector4 v[3];
} OBox;

typedef struct ConvexHull {
  u8 pad0[0x3D0];
  Vector4 point; /* 0x3D0 */
} ConvexHull;

typedef struct DestructibleInfo {
  f32 inertia[3];    /* 0x00 */
  u8 pad0C[4];       /* 0x0C */
  u8 type;           /* 0x10 */
  u8 takesDamage;    /* 0x11 */
  u8 pad12[6];       /* 0x12 */
  s32 armour;        /* 0x18 */
  u8 pad1C[4];       /* 0x1C */
  f32 mass;          /* 0x20 */
  u8 pad24[0x0C];    /* 0x24 */
  s32 surfaceScript; /* 0x30 */
  u8 pad34[0x10];    /* 0x34 */
  OBox *box;         /* 0x44 */
  ConvexHull *hull;  /* 0x48 */
} DestructibleInfo;

typedef struct ThingRecord {
  u8 pad00[0x10];
  DestructibleInfo *info; /* 0x10 */
} ThingRecord;

typedef struct Destructible {
  Vector4 pos;         /* 0x00 */
  f32 matrix[12];      /* 0x10 */
  Vector4 offsetPos;   /* 0x40 */
  u8 pad50[0x24];      /* 0x50 */
  u32 flags;           /* 0x74 */
  u8 pad78[0x48];      /* 0x78 */
  ThingRecord *record; /* 0xC0 */
  u8 padC4[8];         /* 0xC4 */
  ConvexHull *hull;    /* 0xCC */
  OBox *orientedBox;   /* 0xD0 */
} Destructible;

typedef struct OConvexHull {
  Vector4 verts[32];        /* 0x00 */
  Vector4 planes[16];       /* 0x200 */
  void *hull;               /* 0x300 */
  u8 pad304[0x3E0 - 0x304]; /* 0x304 */
} OConvexHull;

typedef struct CollisionPair {
  u8 pad00[0x20]; /* 0x00 */
  u32 objectA;    /* 0x20 */
  u32 objectB;    /* 0x24 */
  s32 time;       /* 0x28 */
  u8 pad2C[4];    /* 0x2C */
} CollisionPair;

typedef struct Creature {
  u8 unk0[0x10];
  u8 mtx[0x40 - 0x10];
  Vector4 pos;
  Vector4 quat;
  f32 unk60;
  u8 unk64[0x70 - 0x64];
  f32 unk70;
  u32 flags;
  u32 unk78;
  u8 unk7C[0x80 - 0x7C];
  s16 unk80;
  u8 unk82[0xB0 - 0x82];
  f32 unkB0;
  f32 unkB4;
  u8 *anims;
  u8 unkBC[0xBF - 0xBC];
  u8 unkBF;
  f32 unkC0;
  u8 unkC4[0xCC - 0xC4];
  u8 motion[0x130 - 0xCC];
  f32 unk130;
  u8 unk134[0x190 - 0x134];
  Vector4 startPos;
  u8 unk1A0[0x1B0 - 0x1A0];
  s32 unk1B0;
  s32 unk1B4;
  s32 unk1B8;
  f32 mass;
  f32 unk1C0;
  u8 unk1C4[0x1C8 - 0x1C4];
  s32 unk1C8;
} Creature;

typedef struct InterfaceCallBackData {
  u32 data[17];
} InterfaceCallBackData;

typedef struct InterfacePackEntry {
  char *name;
  u32 id;
  u32 unk8;
} InterfacePackEntry;

typedef struct PlayerLink {
  u8 unk0[0x20];
  void (*controlReader)();
  void (*init)();
  void (*heartbeat)();
  u8 unk2C[0xB0 - 0x2C];
  struct PlayerLink *next;
  struct PlayerLink *prev;
} PlayerLink;

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

typedef struct FileTOCEntry {
  u32 version;
  u32 type;
  u32 id;
  u8 unkC[0x14 - 0xC];
  u32 offset;
} FileTOCEntry;

typedef struct FileTOCCacheEntry {
  FileTOCEntry entries[50];
  u32 count;
  char name[0x4C];
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


typedef struct SharedSlaveHolder {
  void *key;
  void *slaves;
} SharedSlaveHolder;

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

typedef struct NavLink {
  f32 pos[3];
  u8 pad0C[4];
  u32 numPrev;
  u8 pad14[4];
  s32 *prev;
  u32 numNext;
  s32 *next;
  u8 pad24[4];
  s32 index;
  u8 pad2C[0x24];
  u8 enabled;
} NavLink;

typedef struct CarPart {
  u8 flags;
  u8 xform;
  u8 pad2[2];
  u16 mtxIndex;
  u8 pad6[2];
  void *geom;
  Vector4 *offset;
  u8 pad10[0xC];
} CarPart;
typedef struct CarPartList {
  u32 count;
  CarPart *parts;
} CarPartList;
typedef struct CarTrailBone {
  f32 offset;
  f32 width0;
  f32 width1;
  u8 padC[0x3F - 0xC];
  u8 parent;
  u16 bone;
  u8 pad42[0x50 - 0x42];
} CarTrailBone;

typedef struct CarNavPath {
  u8 pad[0x568];
  u8 *path[7];
} CarNavPath;

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

typedef struct CodeVariables {
  s32 playerVehicle[4];
  s32 playerItem[4];
  s32 controllerState[4];
  s32 unk30;
  s32 unk34[4];
  s32 unk44[4];
  s32 unk54[4];
  u8 unk64[4];
  f32 unk68[30];
  s32 unkE0;
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
  u8 unk112[0x114 - 0x112];
  char *device;
  s32 fileIndex;
  u8 unk11C[3];
  s8 bOverwritingFile;
  u8 unk120[0x124 - 0x120];
  s32 unk124;
  u8 unk128[0x134 - 0x128];
} CodeVariables;

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
  u8 alarm[4][0x28];
  u8 unk120[0x140 - 0x120];
  s64 xferTime[4];
  u8 unk160[0x1B0 - 0x160];
  BOOL inputBufferValid[4];
  u32 inputBuffer[4][2];
  u32 inputBufferVcount[4];
} SIPacketBss;

typedef struct HotWheelsGameData {
  u8 unk0;
  u8 unk1;
  u8 unk2;
  u8 unk3[0xF1];
} HotWheelsGameData;

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

typedef struct GXObject {
    u8 pad0[0x1C];
    u32 count;          /* 0x1C */
    s32 *shaderList;    /* 0x20 */
} GXObject;

typedef struct QuadPack {
    u8 pad00[0x10];
    void *data;         /* 0x10 */
    void *vertices;     /* 0x14 */
    u8 pad18[0x10];
    GXObject *gxObject; /* 0x28 */
    s32 gxBase;         /* 0x2C */
    u8 pad30[0x4];
    u8 flag34;          /* 0x34 */
    u8 pad35[0xC];
    void *alloc;        /* 0x44 */
} QuadPack;

typedef struct DataPackShader {
    s32 shader;             /* 0x00 */
    u8 pad04[0x8];
} DataPackShader;

typedef struct DataPackRef {
    char name[0xC];         /* 0x00 */
    u32 id;                 /* 0x0C */
    struct DataPack *pack;  /* 0x10 */
} DataPackRef;

typedef struct DataPack {
    u8 *base;               /* 0x00 */
    u32 numRelocs;          /* 0x04 */
    s32 *relocs;            /* 0x08 */
    u32 numSoundRelocs;     /* 0x0C */
    u32 *soundRelocs;       /* 0x10 */
    void *palettes;         /* 0x14 */
    u32 numShaders;         /* 0x18 */
    u32 numGX;              /* 0x1C */
    u16 *shaderIDs;         /* 0x20 */
    u32 *gxList;            /* 0x24 */
    u32 dataSize;           /* 0x28 */
    u32 numTextures;        /* 0x2C */
    u16 *textureIDs;        /* 0x30 */
    u32 numSounds;          /* 0x34 */
    u16 *soundIDs;          /* 0x38 */
    void *soundData;        /* 0x3C */
    u8 *soundBase;          /* 0x40 */
    u32 textureData;        /* 0x44 */
    u8 *sfxData;            /* 0x48 */
    void *paletteData;      /* 0x4C */
    DataPackShader *shaders;/* 0x50 */
    u32 numTriggers;        /* 0x54 */
    u8 *triggers;           /* 0x58 */
    void *song;             /* 0x5C */
    void *songBank;         /* 0x60 */
    s8 songLoaded;          /* 0x64 */
    u8 pad65[3];            /* 0x65 */
    DataPackRef refs[2];    /* 0x68 */
    u32 soundDataSize;      /* 0x90 */
} DataPack;

typedef struct QuadPackRef {
    char name[0xC];        /* 0x00 */
    u32 id;                /* 0x0C */
    DataPack *pack;        /* 0x10 */
} QuadPackRef;

/* The quad pack's header carries its pack references inline, at 0x2C. */
typedef struct QuadPackHeader {
    u32 pad00[2];          /* 0x00 */
    u32 numRelocs;         /* 0x08 */
    s32 *relocs;           /* 0x0C */
    u32 pad10[3];          /* 0x10 */
    u32 numShaders;        /* 0x1C */
    s32 *shaderList;       /* 0x20 */
    u32 pad24[2];          /* 0x24 */
    QuadPackRef refs[2];   /* 0x2C */
} QuadPackHeader;

/* What machine drafts (m2c) use for what they can't tell yet: autopilot tries
 * them as they are. */
#ifndef M2C_DEFINES
#define M2C_DEFINES
typedef s32 M2C_UNK;
typedef s8 M2C_UNK8;
typedef s16 M2C_UNK16;
typedef s32 M2C_UNK32;
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) ((type)(expr))
#define M2C_ERROR(desc) (0)
#define M2C_CARRY 0
#define M2C_OVERFLOW(a) (0)
#endif

#endif
