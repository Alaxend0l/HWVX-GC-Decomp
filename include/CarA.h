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
  u8 *player;               /* 0x94 NULL-checked, copied to slave players */
  u8 pad98[0x28];           /* 0x98 */
  OBox orientedBox;         /* 0xC0 BuildOOBOffset writes it; GetOrientedCollision
                               returns car + 0xC0; GetTargetPos reads its v[0] */
  u8 padD0[0x240];          /* 0xF0 */
  MotionState motionState;   /* 0x330 embedded base subobject: model @0x330,
                               animMatrices @0x340, slave players list @0x344 */
  u8 pad348[0x28];          /* 0x348 */
  void *hull;               /* 0x370 OConvexHullHolder: Released + NULLed in CarA_Release */
  u32 partVisMask;          /* 0x374 bit per car part in the reflection loop */
  u8 pad378[0x18];          /* 0x378 */
  f32 forceAccum[3];        /* 0x390 += force in CarA_ProcessForce */
  u8 pad39C[4];             /* 0x39C */
  f32 torqueAccum[3];       /* 0x3A0 += torque in CarA_ProcessForce */
  u8 pad3AC[4];             /* 0x3AC */
  s32 navPointIndex;        /* 0x3B0 stored as s16 pairs into the player */
  u8 pad3B4[8];             /* 0x3B4 */
  u8 *vehicleIface;         /* 0x3BC config/vehicle-interface container: read as u32
                               by CarA_Invalidate, dereferenced for +0x11 (AI drive
                               mode), +0x54 (reflection geom), +0xA0 (collider),
                               +0xD4 (CarTrailBone), +0xDC (torque curve table) */
  Matrix4 inverseBodyInertia; /* 0x3C0 = g_identity, diagonal set at 0x3C0/0x3D4/0x3E8;
                                 read through (f64 *) casts in the decomp only */
  u8 pad400[4];             /* 0x400 */
  s32 lowSpeedFlag;         /* 0x404 compared < 500 by AI cars */
  u8 pad408[8];             /* 0x408 */
  Vector4 localAxis;        /* 0x410 cross-product operand in CarA_ProcessForce */
  u8 pad420[0x38];          /* 0x420 */
  f32 gradeOrReverse;       /* 0x458 < 0.0f gates AI braking */
  u8 pad45C[8];             /* 0x45C */
  s32 stuckCounter;         /* 0x464 > 600 marks the AI car stuck */
  f32 gripScale;            /* 0x468 *= AI gripScale */
  u8 pad46C[4];             /* 0x46C */
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
  u8 pad4FC[0x24];          /* 0x4FC */
  Vector4 bodyOffset;       /* 0x520 XformVec(worldMatrix, this) -> camera/reflection pos */
  u8 pad530[0x30];          /* 0x530 */
} CarA;

/* Compile-time proof that sizeof(CarA) == 0x560 exactly: the first typedef is
   invalid if the struct is too big, the second if it is too small. */
typedef char CarA_size_too_big[0x560 - sizeof(CarA) + 1];
typedef char CarA_size_too_small[sizeof(CarA) - 0x560 + 1];

#endif /* CARA_H */
