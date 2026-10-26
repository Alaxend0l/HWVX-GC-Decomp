#include "common.h"

void MotionState_Draw(void *motionState);
void MotionState_Release(void *motionState);
void Thing_Release(void *thing);
f32 RandomAdd(f32 base, f32 range);

/* parameter 2 of Creature_Init: the spawn description it copies out of */
typedef struct CreatureInit
{
    Vector4 pos;
    f32 jitterBase;
    u8 *anims;
} CreatureInit;


void Creature_GetTargetPos(Creature *creature, Vector4 *pos) {
    pos->x = creature->pos.x;
    pos->y = 4.0f + creature->pos.y;
    pos->z = creature->pos.z;
    pos->w = 1.0f;
}

f32 Creature_GetMass(Creature *creature) {
    return creature->mass;
}

void MotionState_Init(void *motion, s8 reset);
void Creature_MotionStateInitDeath(Creature *creature) {
    u8 *motion;
    s32 state;
    u32 index;
    u8 *entry;
    u8 *anims;

    motion = creature->motion;
    index = *(u32 *)((u8 *)creature + 0x1CC);
    state = 0xFFFF;
    if (creature->unk1B4 == 0) {
        anims = creature->anims;
        if (*(u32 **)(anims + 0x30) != NULL) {
            entry = (u8 *)*(u32 **)(anims + 0x30) + index * 0x60;
            if (index >= *(u32 *)(anims + 0x2C) || *(u32 *)entry == 0) {
                entry = (u8 *)*(u32 **)(anims + 0x18);
                state = 0;
                index = 0;
            }
            *(u32 *)((u8 *)creature + 0xCC) = *(u32 *)entry;
            MotionState_Init(motion, 0);
            *(s32 *)((u8 *)creature + 0xC4) = state;
            *(s32 *)((u8 *)creature + 0x114) = index;
            *(u32 *)((u8 *)creature + 0x118) = (u32)entry;
            creature->flags |= 0x20;
            if (*(u8 **)((u8 *)creature + 0x90) != NULL) {
                *(s32 *)(*(u8 **)((u8 *)creature + 0x90) + 0x1C) = 0;
            }
            *(u32 *)((u8 *)creature + 0x1CC) = 0;
        }
    }
}

void Thing_Init(void *a, s32 b);
void Thing_AddToGrid(void *a, s32 b);
void Creature_MotionStateInit(void *a, s32 b, s32 c, s32 d);
void MotionState_BuildMatricesWorld(void *a, void *b, void *c, f32 d);
void QuaternionToSubMatrix(Vector4 *q, void *m);
s32 Creature_Init(Creature *creature, CreatureInit *init)
{
    Thing_Init(creature, 0);
    init = init;
    creature->anims = init->anims;
    creature->pos = init->pos;
    creature->startPos = creature->pos;
    creature->mass = RandomAdd(0.002f, 0.001f);
    creature->unk1B8 = 0;
    creature->unk1C8 = 0;
    QuaternionToSubMatrix(&creature->quat, creature->mtx);
    creature->unk80 = 0;
    creature->unk70 = 11.0f;
    creature->unkB0 = init->jitterBase;
    creature->unkB4 = 0.0f;
    creature->unkC0 = 0.0f;
    creature->unk1B4 = 0;
    creature->unk78 = *(u32 *)(creature->anims + 4);
    Creature_MotionStateInit(creature, 0, 0, 0);
    creature->unkBF = 0;
    creature->unk130 = 0.03f;
    creature->unk1B0 = -1;
    creature->unk1C0 = 1.0f;
    Thing_AddToGrid(creature, 6);
    MotionState_BuildMatricesWorld(&creature->quat, &creature->pos, creature->motion, 1.0f);
    return 1;
}

void Creature_Release(Creature *creature) {
    MotionState_Release(creature->motion);
    Thing_Release(creature);
}

void Creature_MicroStep(void *arg0, f32 dt) {
    void AccumulateVector(void *arg0, void *arg1, void *arg2, f32 farg0);
    struct CreatureModel {
        u8 unk0[0x14];
        u32 *numBones;
    };
    struct Matrix4 {
        Vector4 v[4];
    };
    struct Creature {
        u8 unk0[0x40];
        Vector4 pos;
        u8 unk50[0x74 - 0x50];
        u32 flags;
        u8 unk78[0xCC - 0x78];
        struct CreatureModel *model;
        u8 unkD0[0xDC - 0xD0];
        struct Matrix4 *bones;
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

void Creature_Draw(Creature *creature) {
    if (creature->unk1C0 > 0.0f) {
        MotionState_Draw(creature->motion);
    }
    creature->flags |= 0x4000;
}

u32 Creature_Invalidate(Creature *creature, u32 start, u32 end) {
    if ((u32)creature->anims >= start && (u32)creature->anims < end) {
        return 1;
    }
    return 0;
}
