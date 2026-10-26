#include "common.h"

f32 Projectile_GetMass(void * arg0)
{
    void * temp;

    temp = ((void **)arg0)[0x48];
    return ((f32 *)temp)[0xb];
}
