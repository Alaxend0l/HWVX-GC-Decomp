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

static inline void SubBlock_construct(SubBlock *ths, u32 size, Block *bp, int prev_alloc, int this_alloc) {
    ths->bp_ = (Block *)((u32)bp | 1);
    ths->size_ = size;
    if (prev_alloc) {
        ths->size_ |= 4;
    }
    if (this_alloc) {
        ths->size_ |= 2;
        *(u32 *)((u8 *)ths + size) |= 4;
    } else {
        *(u32 *)((u8 *)ths + size - sizeof(u32)) = size;
    }
}

static inline SubBlock *SubBlock_split(SubBlock *ths, u32 sz) {
    u32 origsize = ths->size_ & ~7;
    int is_free = !(ths->size_ & 2);
    int prev_alloc = ths->size_ & 4;
    SubBlock *np = (SubBlock *)((u8 *)ths + sz);
    Block *bp = (Block *)((u32)ths->bp_ & ~1);

    SubBlock_construct(ths, sz, bp, prev_alloc, !is_free);
    SubBlock_construct(np, origsize - sz, bp, !is_free, !is_free);
    if (is_free) {
        np->next_ = ths->next_;
        np->next_->prev_ = np;
        np->prev_ = ths;
        ths->next_ = np;
    }
    return np;
}
void FixBlock_construct(FixBlock *ths, FixBlock *prev, FixBlock *next, s32 index, FixSubBlock *chunk, u32 chunk_size);
void *soft_allocate_from_var_pools(Block **pool_obj, u32 size, u32 *max_size);


void *memset(void *dst, int c, u32 n);
void *allocate_from_fixed_pools(void *pool, u32 size);
void *allocate_from_var_pools(Block **pool_obj, u32 size);
void Block_link(Block *ths, SubBlock *sb);
void *__sys_alloc(u32 size);

static inline void Block_construct(Block *ths, u32 size) {
    SubBlock *sb;

    ths->size_ = size | 3;
    *(u32 *)((u8 *)ths + size - 8) = ths->size_;
    sb = (SubBlock *)((u8 *)ths + 16);
    SubBlock_construct(sb, size - 24, ths, 0, 0);
    ths->max_size_ = size - 24;
    *(SubBlock **)((u8 *)ths + (ths->size_ & ~7) - sizeof(SubBlock *)) = NULL;
    Block_link(ths, sb);
}

static inline Block *link_new_block(Block **pool_obj, u32 size) {
    Block *bp;

    size = (size + 24 + 7) & ~7;
    if (size < 0x10000) {
        size = 0x10000;
    }
    bp = __sys_alloc(size);
    if (bp == NULL) {
        return NULL;
    }
    Block_construct(bp, size);
    if (*pool_obj != NULL) {
        bp->prev_ = (*pool_obj)->prev_;
        bp->prev_->next_ = bp;
        bp->next_ = *pool_obj;
        (*pool_obj)->prev_ = bp;
        *pool_obj = bp;
    } else {
        *pool_obj = bp;
        bp->prev_ = bp;
        bp->next_ = bp;
    }
    return bp;
}

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

void deallocate_from_fixed_pools(void *pool, void *ptr, u32 size) {
    u32 i = 0;
    FixSubBlock *p;
    FixBlock *b;
    FixStart *fs;

    while (size > fix_pool_sizes[i]) {
        ++i;
    }
    fs = &((mem_pool_obj *)pool)->fix_start[i];
    p = (FixSubBlock *)((u8 *)ptr - 4);
    b = p->block_;
    if (b->start_ == NULL && fs->head_ != b) {
        if (fs->tail_ == b) {
            fs->head_ = fs->head_->prev_;
            fs->tail_ = fs->tail_->prev_;
        } else {
            b->prev_->next_ = b->next_;
            b->next_->prev_ = b->prev_;
            b->next_ = fs->head_;
            b->prev_ = b->next_->prev_;
            b->prev_->next_ = b;
            b->next_->prev_ = b;
            fs->head_ = b;
        }
    }
    p->next_ = b->start_;
    b->start_ = p;
    if (--b->n_allocated_ == 0) {
        Block *bp;
        Block *n;
        SubBlock *sb;
        s32 empty;

        if (fs->head_ == b) {
            fs->head_ = b->next_;
        }
        if (fs->tail_ == b) {
            fs->tail_ = b->prev_;
        }
        b->prev_->next_ = b->next_;
        b->next_->prev_ = b->prev_;
        if (fs->head_ == b) {
            fs->head_ = NULL;
        }
        if (fs->tail_ == b) {
            fs->tail_ = NULL;
        }
        sb = (SubBlock *)((u8 *)b - 8);
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
            if (((mem_pool_obj *)pool)->start_ == bp) {
                ((mem_pool_obj *)pool)->start_ = n;
            }
            if (n != NULL) {
                n->prev_ = bp->prev_;
                n->prev_->next_ = n;
            }
            bp->prev_ = bp->next_ = NULL;
            __sys_free(bp);
        }
    }
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

void FixBlock_construct(FixBlock *ths, FixBlock *prev, FixBlock *next, s32 index, FixSubBlock *chunk, u32 chunk_size) {
    u32 fixSubBlock_size;
    u32 n;
    char *p;
    u32 i;
    char *np;

    ths->prev_ = prev;
    ths->next_ = next;
    prev->next_ = ths;
    next->prev_ = ths;
    ths->client_size_ = fix_pool_sizes[index];
    fixSubBlock_size = fix_pool_sizes[index] + sizeof(void *);
    n = chunk_size / fixSubBlock_size;
    p = (char *)chunk;
    for (i = 0; i < n - 1; ++i) {
        np = p + fixSubBlock_size;
        ((FixSubBlock *)p)->block_ = ths;
        ((FixSubBlock *)p)->next_ = (FixSubBlock *)np;
        p = np;
    }
    ((FixSubBlock *)p)->block_ = ths;
    ((FixSubBlock *)p)->next_ = NULL;
    ths->start_ = chunk;
    ths->n_allocated_ = 0;
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

void *allocate_from_var_pools(Block **pool_obj, u32 size) {
    Block *bp;
    void *ptr;

    size = (size + 8 + 7) & ~7;
    if (size < 0x50) {
        size = 0x50;
    }
    bp = *pool_obj != NULL ? *pool_obj : link_new_block(pool_obj, size);
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
        bp = bp->next_;
    } while (bp != *pool_obj);
    bp = link_new_block(pool_obj, size);
    if (bp == NULL) {
        return NULL;
    }
    ptr = Block_subBlock(bp, size);
found:
    return (u8 *)ptr + 8;
}

static inline SubBlock *SubBlock_merge_prev(SubBlock *ths, SubBlock **start) {
    u32 prevsz;
    SubBlock *p;

    if (!(ths->size_ & 4)) {
        prevsz = *(u32 *)((u8 *)ths - sizeof(u32));
        if (prevsz & 2) {
            return ths;
        }
        p = (SubBlock *)((u8 *)ths - prevsz);
        p->size_ &= ~0xFFFFFFF8;
        p->size_ |= (prevsz + (ths->size_ & 0xFFFFFFF8)) & 0xFFFFFFF8;
        if (!(p->size_ & 2)) {
            *(u32 *)((u8 *)p + (p->size_ & 0xFFFFFFF8) - sizeof(u32)) = p->size_ & 0xFFFFFFF8;
        }
        if (*start == ths) {
            *start = (*start)->next_;
        }
        ths->next_->prev_ = ths->prev_;
        ths->next_->prev_->next_ = ths->next_;
        return p;
    }
    return ths;
}

#pragma push
#pragma auto_inline off
void SubBlock_merge_next(SubBlock *ths, SubBlock **start) {
    u32 this_cur_size;
    SubBlock *next_sub_block;
    SubBlock *after;

    next_sub_block = (SubBlock *)((u8 *)ths + (ths->size_ & 0xFFFFFFF8));
    if (!(next_sub_block->size_ & 2)) {
        this_cur_size = (ths->size_ & 0xFFFFFFF8) + (next_sub_block->size_ & 0xFFFFFFF8);
        ths->size_ &= ~0xFFFFFFF8;
        ths->size_ |= this_cur_size & 0xFFFFFFF8;
        if (!(ths->size_ & 2)) {
            *(u32 *)((u8 *)ths + this_cur_size - sizeof(u32)) = this_cur_size;
        }
        after = (SubBlock *)((u8 *)ths + this_cur_size);
        if (!(ths->size_ & 2)) {
            after->size_ &= ~4;
        } else {
            after->size_ |= 4;
        }
        if (*start == next_sub_block) {
            *start = (*start)->next_;
        }
        if (*start == next_sub_block) {
            *start = NULL;
        }
        next_sub_block->next_->prev_ = next_sub_block->prev_;
        next_sub_block->prev_->next_ = next_sub_block->next_;
    }
}

void *Block_subBlock(Block *ths, u32 size) {
    SubBlock *st;
    SubBlock *sb;
    u32 sb_size;
    u32 max_found;

    st = *(SubBlock **)((u8 *)ths + (ths->size_ & ~7) - sizeof(SubBlock *));
    if (st == NULL) {
        return NULL;
    }
    sb = st;
    sb_size = sb->size_ & ~7;
    max_found = sb_size;
    while (sb_size < size) {
        sb = sb->next_;
        sb_size = sb->size_ & ~7;
        if (max_found < sb_size) {
            max_found = sb_size;
        }
        if (sb == st) {
            ths->max_size_ = max_found;
            return NULL;
        }
    }
    if (sb_size - size >= 0x50) {
        SubBlock_split(sb, size);
    }
    *(SubBlock **)((u8 *)ths + (ths->size_ & ~7) - sizeof(SubBlock *)) = sb->next_;
    sb_size = sb->size_ & ~7;
    sb->size_ |= 2;
    *(u32 *)((u8 *)sb + sb_size) |= 4;
    {
        SubBlock **start = (SubBlock **)((u8 *)ths + (ths->size_ & ~7) - sizeof(SubBlock *));
        if (*start == sb) {
            *start = sb->next_;
        }
        if (*start == sb) {
            *start = NULL;
            ths->max_size_ = 0;
        } else {
            sb->next_->prev_ = sb->prev_;
            sb->prev_->next_ = sb->next_;
        }
    }
    return sb;
}

