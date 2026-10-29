#include "common.h"

typedef struct BSPNode {
    Vector4 plane;
    u8 type;
    u8 pad;
    u16 next;
    u8 pad14[0xC];
} BSPNode;

f32 Distance_Plane(void *plane, void *point);
