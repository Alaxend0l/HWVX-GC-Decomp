#include "common.h"


s32 GetType_(u8 *chunk);
s32 GetID_(u8 *chunk);
/* A chunk's four-character id (bytes 0-3) as a big-endian word. Hand-written
 * assembly in the original, as GetType_; the frame is spelled out (this
 * unit's compiler doesn't store the arguments on fralloc). */
asm s32 GetID_(u8 *chunk) {
    fralloc
    lbz r5, 0(r3)
    lbz r6, 1(r3)
    slwi r5, r5, 8
    or r5, r5, r6
    lbz r6, 2(r3)
    slwi r5, r5, 8
    or r5, r5, r6
    lbz r6, 3(r3)
    slwi r5, r5, 8
    or r3, r5, r6
    frfree
    blr
}

/* A RIFF chunk's type (bytes 8-11) as a big-endian word. */
asm s32 GetType_(u8 *chunk) {
    fralloc
    lbz r4, 8(r3)
    lbz r5, 9(r3)
    slwi r4, r4, 8
    or r4, r4, r5
    lbz r5, 10(r3)
    slwi r4, r4, 8
    or r4, r4, r5
    lbz r5, 11(r3)
    slwi r4, r4, 8
    or r3, r4, r5
    frfree
    blr
}

/* The original calls these rather than inlining them. */
#pragma push
#pragma dont_inline on
s8 CmpID_(u8 *chunk, s32 type) {
    s8 same = GetID_(chunk) == type;
    return same;
}

s8 CmpType_(u8 *chunk, s32 type) {
    s8 same = GetType_(chunk) == type;
    return same;
}
#pragma pop
