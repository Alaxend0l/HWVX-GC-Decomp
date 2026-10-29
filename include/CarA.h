/* Layout of the CarA vehicle object (the Thing-derived class behind every
 * driveable car), reconstructed from the raw-offset accesses in src/CarA.c,
 * src/CarA_Suspension.c and src/AI_CarA.c (vehicle side, reached through
 * *(u8 **)(aiCar + 0x2C)).  sizeof(CarA) is exactly 0x560; the CarNavPath
 * hack in CarA_Release shows path[] lives at 0x568, i.e. outside this block. */
#ifndef CARA_H
#define CARA_H

#include "common.h"

/* The MotionState base subobject, embedded in CarA at 0x330: CarA_SlaveWeaponStop
 * passes both the subobject itself and its slaves list head, so the two really
 * are one and the same object. */
typedef struct MotionState {
  Model *model;            /* 0x00 */
  u8 pad[0xC];             /* 0x04 */
  Matrix4 *animMatrices;   /* 0x10 */
  u8 *slaves;              /* 0x14 linked list, next node at +0xA4 */
} MotionState;

/* The player object a car is driven by (CarA.player): its controller state
 * sits at the start, read by CarA_PreMove_Control_HW. */
typedef struct Player {
  s8 stickX;                /* 0x00 */
  s8 stickY;                /* 0x01 */
  u8 pad02[0xE];            /* 0x02 */
  u8 button10;              /* 0x10 */
  u8 pad11;                 /* 0x11 */
  u8 throttle;              /* 0x12 */
  u8 brake;                 /* 0x13 */
  u8 pad14;                 /* 0x14 */
  u8 button15;              /* 0x15 */
  u8 pad16[4];              /* 0x16 */
  u8 button1A;              /* 0x1A */
  u8 pad1B[0x11];           /* 0x1B */
  void *vehicle;            /* 0x2C the Thing the player drives */
} Player;

typedef struct CarAEngine {
  u8 pad00[0x14];           /* 0x00 */
  f32 unk14;                /* 0x14 */
  u32 unk18;                /* 0x18 */
  u8 pad1C[0x28];           /* 0x1C */
  f32 unk44;                /* 0x44 */
} CarAEngine;

typedef struct CarAAccel {
  u8 pad00[0x18];           /* 0x00 */
  f32 unk18;                /* 0x18 */
  f32 unk1C;                /* 0x1C */
  f32 unk20;                /* 0x20 */
} CarAAccel;

typedef struct CarAStuntInfo {
  u8 pad00[0x28];           /* 0x00 */
  s32 unk28;                /* 0x28 */
} CarAStuntInfo;

typedef struct CarAGear {
  f32 ratio;                /* 0x00 */
  f32 unk4;                 /* 0x04 */
} CarAGear;

/* The car's config/vehicle-interface container (CarA.vehicleIface). */
typedef struct CarAInfo {
  u8 pad00[0x1C];           /* 0x00 */
  f32 unk1C;                /* 0x1C */
  f32 unk20;                /* 0x20 */
  f32 unk24;                /* 0x24 */
  u8 pad28[8];              /* 0x28 */
  f32 unk30;                /* 0x30 */
  u8 pad34[0x4C];           /* 0x34 */
  CarAEngine *engine;       /* 0x80 */
  u8 pad84[4];              /* 0x84 */
  CarAAccel *accel;         /* 0x88 */
  CarAStuntInfo *stunt;     /* 0x8C */
  u8 pad90[0x48];           /* 0x90 */
  u32 numGears;             /* 0xD8 */
  CarAGear *gears;          /* 0xDC torque curve table */
  u32 numSlaves;            /* 0xE0 */
  u8 *slaves;               /* 0xE4 */
  u8 padE8[0x28];           /* 0xE8 */
  u32 numShiftSounds;       /* 0x110 */
  u32 *shiftSounds;         /* 0x114 */
} CarAInfo;

typedef struct CarA {
  Vector4 velocity;         /* 0x00 carVel = *(Vector4 *)car; read as u32 at 0x8 (scale) */
  Matrix4 worldMatrix;      /* 0x10 mtx = *(Matrix4 *)(car + 0x10);
                               v[1] = up normal  @ 0x20 (impact dot product),
                               v[2] = forward   @ 0x30 (read as Quaternion by AI cars),
                               v[3] = position  @ 0x40 */
  Vector4 animRefPos;       /* 0x50 fed to TEST_AnimTraverseHeirarchyToMatricesWorld */
  u8 pad60[0x14];           /* 0x60 */
  u32 collDirtyFlags;       /* 0x74 &= ~0x180 when the oriented box is rebuilt */
  s32 speed;                /* 0x78 scaled by AI speedScale */
  u8 pad7C[0x18];           /* 0x7C */
  Player *player;           /* 0x94 NULL-checked, copied to slave players */
  u8 pad98[0x28];           /* 0x98 */
  OBox orientedBox;         /* 0xC0 BuildOOBOffset writes it; GetOrientedCollision
                               returns car + 0xC0; GetTargetPos reads its v[0] */
  u8 padD0[0x240];          /* 0xF0 */
  MotionState motionState;   /* 0x330 embedded base subobject: model @0x330,
                               animMatrices @0x340, slave players list @0x344 */
  u8 pad348[0x28];          /* 0x348 */
  void *hull;               /* 0x370 OConvexHullHolder: Released + NULLed in CarA_Release */
  u32 partVisMask;          /* 0x374 bit per car part in the reflection loop */
  f32 unk378;               /* 0x378 */
  u8 pad37C[0x14];          /* 0x37C */
  f32 forceAccum[3];        /* 0x390 += force in CarA_ProcessForce */
  u8 pad39C[4];             /* 0x39C */
  f32 torqueAccum[3];       /* 0x3A0 += torque in CarA_ProcessForce */
  u8 pad3AC[4];             /* 0x3AC */
  s32 navPointIndex;        /* 0x3B0 stored as s16 pairs into the player */
  u8 pad3B4[8];             /* 0x3B4 */
  CarAInfo *vehicleIface;   /* 0x3BC config/vehicle-interface container: read as u32
                               by CarA_Invalidate, dereferenced for +0x11 (AI drive
                               mode), +0x54 (reflection geom), +0xA0 (collider),
                               +0xD4 (CarTrailBone), +0xDC (torque curve table) */
  Matrix4 inverseBodyInertia; /* 0x3C0 = g_identity, diagonal set at 0x3C0/0x3D4/0x3E8;
                                 read through (f64 *) casts in the decomp only */
  u8 pad400[4];             /* 0x400 */
  s32 lowSpeedFlag;         /* 0x404 compared < 500 by AI cars */
  u8 pad408[8];             /* 0x408 */
  Vector4 localAxis;        /* 0x410 cross-product operand in CarA_ProcessForce */
  u8 pad420[0x30];          /* 0x420 */
  u32 reverseTimer;         /* 0x450 ms the brake is held at standstill */
  u8 pad454[4];             /* 0x454 */
  f32 gradeOrReverse;       /* 0x458 < 0.0f gates AI braking */
  u8 pad45C[4];             /* 0x45C */
  s32 unk460;               /* 0x460 */
  s32 stuckCounter;         /* 0x464 > 600 marks the AI car stuck */
  f32 gripScale;            /* 0x468 *= AI gripScale */
  s32 slaveWeaponRpm[1];      /* 0x46C indexed by weapon slot (0..2) in CarA_SlaveWeaponStop,
                                which writes 10001, just past the 10000 rpm test on the word
                                that follows; only slot 0 is sized here */
  s32 engineRpmState;       /* 0x470 compared < 10000 in GetTorqueAtWheels */
  u8 pad474[0x18];          /* 0x474 */
  u32 transmissionFlags;    /* 0x48C & 0x1000 selects 4WD torque split */
  u8 pad490[0x30];          /* 0x490 */
  f32 torquePolyCoeffs[4];  /* 0x4C0 Horner polynomial in rpm */
  u8 pad4D0[0x14];          /* 0x4D0 */
  f32 rpm;                  /* 0x4E4 */
  u8 curveTableIdx;         /* 0x4E8 index into the interface's curve table */
  u8 pad4E9[7];             /* 0x4E9 */
  s32 soundIds[3];          /* 0x4F0 SoundSource ids, -1 == none; stopped + reset in Release */
  f32 unk4FC;               /* 0x4FC */
  u8 pad500[0x20];          /* 0x500 */
  Vector4 bodyOffset;       /* 0x520 XformVec(worldMatrix, this) -> camera/reflection pos */
  u8 pad530[0x30];          /* 0x530 */
} CarA;

/* Compile-time proof that sizeof(CarA) == 0x560 exactly: the first typedef is
   invalid if the struct is too big, the second if it is too small. */
typedef char CarA_size_too_big[0x560 - sizeof(CarA) + 1];
typedef char CarA_size_too_small[sizeof(CarA) - 0x560 + 1];

#endif /* CARA_H */
