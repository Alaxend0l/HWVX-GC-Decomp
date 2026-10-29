#include "common.h"

typedef struct Anim {
    u8 pad0[8];
    f32 minTime;
    f32 maxTime;
    u32 numBones;
    u32 numKeysPerFrame;
    u32 *mask;
    u16 *flags;
    f32 *times;
    Vector4 *data;
    u32 numFrames;
    f32 *staticData;
    u32 numCompound;
} Anim;

typedef struct AnimBone {
    Quaternion rot;
    f32 pos[3];
    u32 info;
} AnimBone;

f32 Distance_Plane(void *plane, void *point);

void AnimExtractKeyFrame_NonCompound(void) {

}

#pragma use_lmw_stmw on
void AnimExtractKeyFrame_Compound(Anim *anim, AnimBone *outp, f32 time) {
    Vector4 tv __attribute__((aligned(16)));
    f32 values[448];
    u16 *flags;
    AnimBone *out;
    u32 numBones;
    u32 *mask;
    u32 bits;
    u32 bit;
    f32 *stat;
    f32 *dyn;
    Vector4 *plane;
    u32 i;
    s32 idx;
    u16 f;

    out = outp;
    bits = 0;
    bit = 1;
    flags = anim->flags;
    numBones = anim->numBones;
    mask = anim->mask;
    if (time < anim->minTime) {
        time = anim->minTime;
    }
    if (time > anim->maxTime) {
        time = anim->maxTime;
    }
    if (mask != NULL) {
        bits = *mask;
    }
    stat = anim->staticData;
    dyn = values;
    tv.z = time;
    tv.y = time * time;
    tv.x = tv.y * time;
    tv.w = 1.0f;
    idx = 0;
    for (i = 1; i < anim->numFrames; i++) {
        if (time <= anim->times[i]) {
            idx = i - 1;
            break;
        }
    }
    plane = &anim->data[idx * anim->numCompound];
    for (i = 0; i < anim->numCompound; i++) {
        values[i] = Distance_Plane(&plane[i], &tv);
    }
    for (i = 0; i < numBones; i++) {
        f = *flags;
        if (f & 1) {
            if (bits & bit) {
                out->pos[0] = *dyn++;
            } else {
                out->pos[0] = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        if (f & 2) {
            if (bits & bit) {
                out->pos[1] = *dyn++;
            } else {
                out->pos[1] = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        if (f & 4) {
            if (bits & bit) {
                out->pos[2] = *dyn++;
            } else {
                out->pos[2] = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        if (f & 8) {
            if (bits & bit) {
                out->rot.w = *dyn++;
            } else {
                out->rot.w = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        if (f & 0x10) {
            if (bits & bit) {
                out->rot.x = *dyn++;
            } else {
                out->rot.x = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        if (f & 0x20) {
            if (bits & bit) {
                out->rot.y = *dyn++;
            } else {
                out->rot.y = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        if (f & 0x40) {
            if (bits & bit) {
                out->rot.z = *dyn++;
            } else {
                out->rot.z = *stat++;
            }
            if (!(bit <<= 1)) {
                bits = *++mask;
                bit = 1;
            }
        }
        flags++;
        out++;
    }
}
#pragma use_lmw_stmw off
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
