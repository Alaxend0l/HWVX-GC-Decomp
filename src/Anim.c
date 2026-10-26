#include "common.h"


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
