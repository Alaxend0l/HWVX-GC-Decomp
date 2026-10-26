/* Basic types for this decompilation (CodeWarrior conventions). Agents add structs and prototypes here or in
 * new headers next to it. */
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
typedef struct Vector4 { f32 x; f32 y; f32 z; f32 w; } Vector4;
typedef struct Quaternion { f32 x; f32 y; f32 z; f32 w; } Quaternion;
typedef struct Matrix4 { Vector4 v[4]; } Matrix4;
#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL ((void*)0)
#endif

typedef struct WorldData
{
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
    Vector4 pos;                 /* 0x00 */
    Vector4 normal;              /* 0x10 */
    u32 triId1;                  /* 0x20 */
    u32 triId0;                  /* 0x24 */
    u32 time;                    /* 0x28 */
    u8 pad2C[4];                 /* 0x2C */
} CollisionTri;

/* What machine drafts (m2c) use for what they can't tell yet: autopilot tries them as they are. */
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
