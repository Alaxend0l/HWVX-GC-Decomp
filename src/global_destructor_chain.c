#include "common.h"

/* One link of the C++ runtime's global destructor chain: the object whose
 * destructor has to run, the destructor itself, and the next link. */
struct DestructNode {
    struct DestructNode *next;        /* 0x00 */
    void (*destructor)(void *, s32);  /* 0x04 */
    void *object;                     /* 0x08 */
};

extern struct DestructNode *__global_destructor_chain;
