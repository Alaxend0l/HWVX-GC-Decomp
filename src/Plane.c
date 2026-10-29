#include "common.h"

void Reverse_Plane(Vector4 *plane) {
    plane->x *= -1.0f;
    plane->y *= -1.0f;
    plane->z *= -1.0f;
    plane->w *= -1.0f;
}

f32 Distance_Plane(Vector4 *plane, Vector4 *point) {
    return plane->w + (plane->z * point->z + (plane->x * point->x + plane->y * point->y));
}
