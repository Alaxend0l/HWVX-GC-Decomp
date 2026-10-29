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
    struct FixSubBlock *start_;
    u32 n_allocated_;
} FixBlock;

typedef struct FixSubBlock {
    FixBlock *block_;
    struct FixSubBlock *next_;
} FixSubBlock;

typedef struct FixStart {
    FixBlock *tail_;
    FixBlock *head_;
} FixStart;

typedef struct mem_pool_obj {
    Block *start_;
    FixStart fix_start[6];
} mem_pool_obj;

extern const u32 fix_pool_sizes[6];

void *Block_subBlock(Block *ths, u32 size);
void FixBlock_construct(FixBlock *ths, FixBlock *prev, FixBlock *next, s32 index, FixSubBlock *chunk, u32 chunk_size);
void *soft_allocate_from_var_pools(Block **pool_obj, u32 size, u32 *max_size);


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

#pragma use_lmw_stmw on
void *allocate_from_fixed_pools(void *pool, u32 size) {
    u32 i = 0;
    FixSubBlock *p;
    FixStart *fs;

    while (size > fix_pool_sizes[i]) {
        ++i;
    }
    fs = &((mem_pool_obj *)pool)->fix_start[i];
    if (fs->head_ == NULL || fs->head_->start_ == NULL) {
        u32 size_received;
        u32 n;
        void *newblock;
        u32 nsave;
        u32 u_size;

        n = 0xFEC / (fix_pool_sizes[i] + sizeof(void *));
        if (n > 0x100) {
            n = 0x100;
        }
        nsave = n;
        while (n >= 10) {
            newblock = soft_allocate_from_var_pools(pool, n * (fix_pool_sizes[i] + sizeof(void *)) + sizeof(FixBlock), &size_received);
            if (newblock != NULL) {
                break;
            }
            if (size_received > sizeof(FixBlock)) {
                n = (size_received - sizeof(FixBlock)) / (fix_pool_sizes[i] + sizeof(void *));
            } else {
                n = 0;
            }
        }
        if (newblock == NULL && n < nsave) {
            newblock = allocate_from_var_pools(pool, nsave * (fix_pool_sizes[i] + sizeof(void *)) + sizeof(FixBlock));
            if (newblock == NULL) {
                return NULL;
            }
        }
        u_size = !(*(u32 *)((u8 *)newblock - 4) & 1) ? (*(FixBlock **)((u8 *)newblock - 4))->client_size_
                                                    : (*(u32 *)((u8 *)newblock - 8) & ~7) - 8;
        if (fs->head_ == NULL) {
            fs->head_ = newblock;
            fs->tail_ = newblock;
        }
        FixBlock_construct(newblock, fs->tail_, fs->head_, i, (FixSubBlock *)((u8 *)newblock + sizeof(FixBlock)), u_size - sizeof(FixBlock));
        fs->head_ = newblock;
    }
    p = fs->head_->start_;
    fs->head_->start_ = p->next_;
    ++fs->head_->n_allocated_;
    if (fs->head_->start_ == NULL) {
        fs->head_ = fs->head_->next_;
        fs->tail_ = fs->tail_->next_;
    }
    return (u8 *)p + 4;
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

