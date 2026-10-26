#include "common.h"

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

void MotionState_Draw(void *motionState);
void MotionState_Release(void *motionState);
void Thing_Release(void *thing);

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
