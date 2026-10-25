#include "common.h"


void MotionState_Init(void *arg0, s8 reset) {
    typedef struct {
        u8 pad0[0x14];
        u32 *skel;
        u8 pad18[4];
        u8 loop[3];
        u8 pad1F;
        f32 *time[3];
    } Anim;
    typedef struct {
        Anim *anim;
        u8 mode;
        u8 unk5;
        u8 unk6;
        u8 unk7;
        u8 pad8[4];
        f32 time;
    } MotionState;
    typedef struct {
        Anim *anim;
        u8 mode;
        u8 unk5;
        u8 unk6;
        u8 pad7;
        f32 time;
        f32 blend;
        u8 pad10[0x10];
    } MotionHistory;
    typedef struct {
        MotionState cur;
        u32 matrices;
        u8 pad14[4];
        MotionHistory hist[1];
    } Motion;
    void MotionState_ReleaseSlavesByType(s32 state, u8 type);
    u32 MatrixCache_Get(u32 size, void *mtx);
    Motion *motion = arg0;
    s32 i;
    u32 *skel;

    motion->cur.unk5 = 0;
    motion->cur.unk6 = 0;
    motion->cur.mode = 0;
    motion->cur.mode = 0;
    if (motion->cur.anim == NULL) {
        return;
    }
    if (!motion->cur.anim->loop[0]) {
        motion->cur.mode++;
        if (!motion->cur.anim->loop[1]) {
            motion->cur.mode++;
            if (motion->cur.anim->loop[2]) {
                motion->cur.time = *motion->cur.anim->time[2];
            }
        } else {
            motion->cur.time = *motion->cur.anim->time[1];
        }
    } else {
        motion->cur.time = *motion->cur.anim->time[0];
    }
    for (;;) {
        switch (motion->cur.mode) {
        case 0:
            if (motion->cur.anim->loop[0]) {
                goto done;
            }
            motion->cur.mode = 1;
        case 1:
            if (motion->cur.anim->loop[1]) {
                goto done;
            }
            motion->cur.mode = 2;
        case 2:
            if (motion->cur.anim->loop[2]) {
                goto done;
            }
            motion->cur.mode = 0;
        }
    }
done:
    if (reset) {
        for (i = 0; i > 0; i--) {
            memcpy(&motion->hist[i], &motion->hist[i - 1], sizeof(MotionHistory));
        }
        motion->hist[0].blend = 0.0f;
    } else {
        motion->hist[0].blend = 1.0f;
    }
    motion->hist[0].anim = motion->cur.anim;
    motion->hist[0].mode = motion->cur.mode;
    motion->hist[0].unk5 = motion->cur.unk5;
    motion->hist[0].unk6 = motion->cur.unk6;
    motion->hist[0].time = motion->cur.time;
    skel = motion->cur.anim->skel;
    if (motion->cur.unk7 < *skel) {
        if (motion->cur.unk7) {
            MatrixCache_Free(motion->matrices);
        }
        motion->matrices = MatrixCache_Get(*skel, motion);
        motion->cur.unk7 = *skel;
    }
    MotionState_ReleaseSlavesByType((s32)motion, 0xFF);
}

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

