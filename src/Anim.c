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
