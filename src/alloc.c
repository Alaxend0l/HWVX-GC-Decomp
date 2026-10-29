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

void deallocate_from_fixed_pools(void *pool, void *ptr, u32 size);
void Block_link(Block *ths, SubBlock *sb);
void __sys_free(void *ptr);
void free(void *ptr) {
    void *pool;
    u32 size;
    Block *bp;
    Block *n;
    SubBlock *sb;
    s32 empty;

    pool = get_malloc_pool();
    if (ptr == NULL) {
        return;
    }
    size = !(*(u32 *)((u8 *)ptr - 4) & 1) ? (*(FixBlock **)((u8 *)ptr - 4))->client_size_
                                          : (*(u32 *)((u8 *)ptr - 8) & ~7) - 8;
    if (size <= 0x44) {
        deallocate_from_fixed_pools(pool, ptr, size);
        return;
    }
    sb = (SubBlock *)((u8 *)ptr - 8);
    bp = (Block *)((u32)sb->bp_ & ~1);
    Block_link(bp, sb);
    sb = (SubBlock *)((u8 *)bp + 16);
    empty = 0;
    if (!(sb->size_ & 2) && (sb->size_ & ~7) == (bp->size_ & ~7) - 24) {
        empty = 1;
    }
    if (empty) {
        n = bp->next_;
        if (n == bp) {
            n = NULL;
        }
        if (*(Block **)pool == bp) {
            *(Block **)pool = n;
        }
        if (n != NULL) {
            n->prev_ = bp->prev_;
            n->prev_->next_ = n;
        }
        bp->prev_ = bp->next_ = NULL;
        __sys_free(bp);
    }
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

