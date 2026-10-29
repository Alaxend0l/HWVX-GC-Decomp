#include "common.h"

typedef struct Block {
    struct Block *prev_;
    struct Block *next_;
    u32 max_size_;
    u32 size_;
} Block;

typedef struct SubBlock {
    u32 size_;
    Block *bp_;
    struct SubBlock *prev_;
    struct SubBlock *next_;
} SubBlock;

typedef struct FixBlock {
    struct FixBlock *prev_;
    struct FixBlock *next_;
    u32 client_size_;
} FixBlock;

void *Block_subBlock(Block *ths, u32 size);


void *memset(void *dst, int c, u32 n);
void *allocate_from_fixed_pools(void *pool, u32 size);
void *allocate_from_var_pools(void *pool, u32 size);

static inline void *get_malloc_pool(void) {
    static u8 protopool[0x34];
    static u8 init;

    if (!init) {
        memset(protopool, 0, sizeof(protopool));
        init = 1;
    }
    return protopool;
}

void *malloc(u32 size) {
    void *pool;

    pool = get_malloc_pool();
    if (size == 0) {
        return NULL;
    }
    if (size > (u32)-0x31) {
        return NULL;
    }
    if (size <= 0x44) {
        return allocate_from_fixed_pools(pool, size);
    }
    return allocate_from_var_pools(pool, size);
}

void *soft_allocate_from_var_pools(Block **pool_obj, u32 size, u32 *max_size) {
    Block *bp;
    void *ptr;

    size = (size + 8 + 7) & ~7;
    if (size < 0x50) {
        size = 0x50;
    }
    *max_size = 0;
    bp = *pool_obj;
    if (bp == NULL) {
        return NULL;
    }
    do {
        if (size <= bp->max_size_) {
            ptr = Block_subBlock(bp, size);
            if (ptr != NULL) {
                *pool_obj = bp;
                goto found;
            }
        }
        if (bp->max_size_ > 8 && *max_size < bp->max_size_ - 8) {
            *max_size = bp->max_size_ - 8;
        }
        bp = bp->next_;
    } while (bp != *pool_obj);
    return NULL;
found:
    return (u8 *)ptr + 8;
}

