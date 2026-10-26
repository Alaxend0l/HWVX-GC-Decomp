#include "common.h"

/* One link of the C++ runtime's global destructor chain: the object whose
 * destructor has to run, the destructor itself, and the next link. */
struct DestructNode {
    struct DestructNode *next;        /* 0x00 */
    void (*destructor)();             /* 0x04 arity unknown to the chain: it calls it old-style */
    void *object;                     /* 0x08 */
};

extern struct DestructNode *__global_destructor_chain;

void __destroy_global_chain(void) {
    struct DestructNode *node;

    while ((node = __global_destructor_chain) != NULL) {
        __global_destructor_chain = node->next;
        (*node->destructor)(node->object, -1);
    }
}
