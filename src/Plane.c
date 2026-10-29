#include "common.h"

f32 Distance_Plane(Vector4 *plane, Vector4 *point) {
    return plane->w + (plane->z * point->z + (plane->x * point->x + plane->y * point->y));
}
