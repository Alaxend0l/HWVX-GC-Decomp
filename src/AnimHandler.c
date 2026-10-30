#include "common.h"

#pragma use_lmw_stmw on

void TEST_AnimTraverseHeirarchyToMatricesWorld(Vector4 *quat, void *pos, void *frames, s32 skel, void *matrices, f32 time);

void MotionState_Init(void *arg0, s8 reset) {
    typedef struct {
        u8 pad0[0x14];
        u32 *skel;
        u8 pad18[4];
        u8 loop[3];
        u8 pad1F;
        f32 *time[3];
    } Anim;
    typedef struct {
        Anim *anim;
        u8 mode;
        u8 unk5;
        u8 unk6;
        u8 unk7;
        u8 pad8[4];
        f32 time;
    } MotionState;
    typedef struct {
        Anim *anim;
        u8 mode;
        u8 unk5;
        u8 unk6;
        u8 pad7;
        f32 time;
        f32 blend;
        u8 pad10[0x10];
    } MotionHistory;
    typedef struct {
        MotionState cur;
        u32 matrices;
        u8 pad14[4];
        MotionHistory hist[1];
    } Motion;
    void MotionState_ReleaseSlavesByType(s32 state, u8 type);
    u32 MatrixCache_Get(u32 size, void *mtx);
    Motion *motion = arg0;
    s32 i;
    u32 *skel;

    motion->cur.unk5 = 0;
    motion->cur.unk6 = 0;
    motion->cur.mode = 0;
    motion->cur.mode = 0;
    if (motion->cur.anim == NULL) {
        return;
    }
    if (!motion->cur.anim->loop[0]) {
        motion->cur.mode++;
        if (!motion->cur.anim->loop[1]) {
            motion->cur.mode++;
            if (motion->cur.anim->loop[2]) {
                motion->cur.time = *motion->cur.anim->time[2];
            }
        } else {
            motion->cur.time = *motion->cur.anim->time[1];
        }
    } else {
        motion->cur.time = *motion->cur.anim->time[0];
    }
    for (;;) {
        switch (motion->cur.mode) {
        case 0:
            if (motion->cur.anim->loop[0]) {
                goto done;
            }
            motion->cur.mode = 1;
        case 1:
            if (motion->cur.anim->loop[1]) {
                goto done;
            }
            motion->cur.mode = 2;
        case 2:
            if (motion->cur.anim->loop[2]) {
                goto done;
            }
            motion->cur.mode = 0;
        }
    }
done:
    if (reset) {
        for (i = 0; i > 0; i--) {
            memcpy(&motion->hist[i], &motion->hist[i - 1], sizeof(MotionHistory));
        }
        motion->hist[0].blend = 0.0f;
    } else {
        motion->hist[0].blend = 1.0f;
    }
    motion->hist[0].anim = motion->cur.anim;
    motion->hist[0].mode = motion->cur.mode;
    motion->hist[0].unk5 = motion->cur.unk5;
    motion->hist[0].unk6 = motion->cur.unk6;
    motion->hist[0].time = motion->cur.time;
    skel = motion->cur.anim->skel;
    if (motion->cur.unk7 < *skel) {
        if (motion->cur.unk7) {
            MatrixCache_Free(motion->matrices);
        }
        motion->matrices = MatrixCache_Get(*skel, motion);
        motion->cur.unk7 = *skel;
    }
    MotionState_ReleaseSlavesByType((s32)motion, 0xFF);
}

void MotionState_ReleaseSlavesByType(s32 state, u8 type) {
    void ThingManagerList_Remove(void **list, void *thing);
    void ThingManagerList_Append(void **list, void *thing);
    typedef struct SlaveThing {
        u8 pad0[0x74];
        u32 flags;
        u8 pad78[0x83 - 0x78];
        u8 type;
        u8 pad84[0x8C - 0x84];
        void (**vtbl)(struct SlaveThing *);
        u8 pad90[0xA4 - 0x90];
        struct SlaveThing *next;
        s32 pool;
    } SlaveThing;
    SlaveThing *thing;
    SlaveThing *next;

    thing = *(SlaveThing **)(state + 0x14);
    while (thing != NULL) {
        next = thing->next;
        if (thing->type == type) {
            thing->vtbl[1](thing);
            ThingManagerList_Remove((void **)(state + 0x14), thing);
            thing->flags &= ~0x200000;
            thing->flags |= 0x1000;
            ThingManagerList_Append(&g_thingManager_DARYL.freeList[thing->pool], thing);
        }
        thing = next;
    }
}

void MotionState_Release(s32 state) {
    M2C_UNK ThingManagerList_EmancipateSlaves(s32, s32);
    M2C_UNK ThingManagerList_ReleaseAll(s32);
    M2C_UNK ThingManagerList_RemoveReleasedThings(s32);
    if (M2C_FIELD(state, u32 *, 0x10) != 0) {
        ThingManagerList_EmancipateSlaves(M2C_FIELD(state, u32 *, 0x10), state + 0x14);
        MatrixCache_Free(M2C_FIELD(state, u32 *, 0x10));
        M2C_FIELD(state, u32 *, 0x10) = 0;
        M2C_FIELD(state, u8 *, 0x7) = 0;
    }
    ThingManagerList_ReleaseAll(state + 0x14);
    ThingManagerList_RemoveReleasedThings(state + 0x14);
}

void MotionState_ExtractBlendedKeyFrame(u8 *state, void *keyFrame) {
    void MemCpy32(void *dst, void *src, u32 size);
    void Gfx_AnimExtractKeyFrame(void *anim, f32 time, void *keyFrame, u32 count);
    u8 *anim;

    anim = M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), u8 **, 0x14);
    MemCpy32(keyFrame, M2C_FIELD(anim, void **, 0x10), M2C_FIELD(anim, u32 *, 0x0) << 5);
    M2C_FIELD(state, f32 *, 0x24) = 1.0f;
    Gfx_AnimExtractKeyFrame(M2C_FIELD(M2C_FIELD(state, u8 **, 0x18), void **, 0x10),
                            M2C_FIELD(state, f32 *, 0x20), keyFrame, M2C_FIELD(anim, u32 *, 0x0));
}

void MotionState_Draw(void *state) {
    void Gfx_DrawAnimGeometryAux(void *geom, void *mtx, s32 skel, s32 arg3);
    void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);
    void MotionState_DrawSlaves(void *state, s32 arg1);
    u32 i;
    s32 *skel;

    skel = M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), s32 **, 0x14);
    for (i = 0; i < M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), u32 *, 0x0); i++) {
        Gfx_DrawAnimGeometryAux(M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), void ***, 0x4)[i],
                                M2C_FIELD(state, void **, 0x10), *skel, 0);
    }
    for (i = 0; i < M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), u32 *, 0x8); i++) {
        u8 *e = M2C_FIELD(M2C_FIELD(state, u8 **, 0x0), u8 **, 0xC) + i * 8;
        Gfx_DrawGeometry_MatrixNotCached(M2C_FIELD(e, void **, 0x0),
                                         M2C_FIELD(state, u8 **, 0x10) + (M2C_FIELD(e, u16 *, 0x4) << 6));
    }
    MotionState_DrawSlaves(state, 0);
}

void MotionState_Draw2(void *state, s32 mtx, u8 *matrices) {
    typedef struct GeomEntry {
        void *geom;
        u16 matrix;
        u16 pad6;
    } GeomEntry;
    typedef struct AnimData {
        u32 numAux;
        void **aux;
        u32 numGeom;
        GeomEntry *geom;
        u8 pad10[4];
        s32 *skel;
    } AnimData;
    typedef struct DrawState {
        AnimData *anim;
        u8 pad4[0xC];
        u8 *cached;
    } DrawState;
    void MotionState_ExtractBlendedKeyFrame(u8 *state, void *keyFrame);
    void Gfx_DrawAnimGeometryAux(void *geom, void *mtx, s32 skel, s32 arg3);
    void Gfx_DrawGeometry(void *geom, void *mtx);
    void MotionState_DrawSlaves(void *state, s32 arg1);
    void TEST_AnimTraverseHeirarchyToMatricesWorld(Vector4 *quat, void *pos, void *frames, s32 skel, void *matrices, f32 time);
    DrawState *ds = state;
    s32 *skel;
    u8 *cached;
    u32 i;
    GeomEntry *e;
    Vector4 quat;
    Matrix4 out;
    u8 frames[0x800];

    skel = ds->anim->skel;
    if (ds->cached == NULL) {
        MotionState_ExtractBlendedKeyFrame(state, frames);
        SubMatrixToQuaternion((void *)mtx, &quat);
        TEST_AnimTraverseHeirarchyToMatricesWorld(&quat, (void *)(mtx + 0x30), frames, *skel, matrices, 1.0f);
        for (i = 0; i < ds->anim->numAux; i++) {
            Gfx_DrawAnimGeometryAux(ds->anim->aux[i], matrices, *skel, 0);
        }
        for (i = 0; i < ds->anim->numGeom; i++) {
            e = &ds->anim->geom[i];
            Gfx_DrawGeometry(e->geom, matrices + (e->matrix << 6));
        }
        MotionState_DrawSlaves(state, 0);
    } else {
        cached = ds->cached;
        for (i = 0; i < ds->anim->numAux; i++) {
            Gfx_DrawAnimGeometryAux(ds->anim->aux[i], cached, *skel, mtx);
        }
        for (i = 0; i < ds->anim->numGeom; i++) {
            void *geom;
            e = &ds->anim->geom[i];
            geom = e->geom;
            MultiplyMatrix((Matrix4 *)(cached + (e->matrix << 6)), (void *)mtx, &out);
            Gfx_DrawGeometry(geom, &out);
        }
        MotionState_DrawSlaves(state, mtx);
    }
}

void MotionState_InitAllSlavesNow(s32 state, void *slaves, u8 player) {
    typedef struct SlaveTrigger {
        u8 pad0[0x20];
    } SlaveTrigger;
    typedef struct SlaveList {
        u32 count;
        SlaveTrigger *triggers;
    } SlaveList;
    extern Vector4 g_zeroVector;
    void ThingManagerList_TriggerSlaves(void **list, u8 *trigger, Vector4 *a, Vector4 *b, Vector4 *c, u8 player);
    SlaveList *list = slaves;
    u32 i;

    MotionState_ReleaseSlavesByType(state, player);
    for (i = 0; i < list->count; i++) {
        Vector4 dir = {0.0f, 0.0f, 0.0f, 1.0f};

        ThingManagerList_TriggerSlaves((void **)(state + 0x14), (u8 *)&list->triggers[i], (Vector4 *)&list->triggers[i], &dir, &g_zeroVector, player);
    }
}

void Motion_BlendHeartbeat_FPS(void *layer, s32 mode, void *anim, f32 fps) {
    extern f32 g_fDeltaTimeSec;
    typedef struct {
        f32 start;
        f32 end;
    } MotionRange;
    typedef struct {
        u8 unk0[0x1D];
        u8 loopMode;
        u8 pingPong;
        u8 maxLoops;
        MotionRange *intro;
        MotionRange *loop;
        MotionRange *outro;
    } MotionAnim;
    typedef struct {
        MotionAnim *anim;
        u8 phase;
        u8 loops;
        s8 reverse;
        f32 time;
        f32 weight;
        f32 prevTime;
        f32 rawTime;
        s32 done;
    } MotionLayer;
    MotionLayer *l = layer;
    f32 over;
    f32 step;
    f32 end;
    f32 start;
    f32 raw;
    f32 prev;
    f32 time;
    u8 wrapped;
    u8 oldPhase;
    s8 type;
    s8 reverse;

    l->done = 0;
    raw = 0.0f;
    type = 0;
    type |= 1;
    start = raw;
    end = raw;
    time = l->time;
    prev = time;
    reverse = l->reverse;
    over = -1.0f;
    wrapped = 0;
    switch (l->phase) {
    case 0:
        start = l->anim->intro->start;
        end = l->anim->intro->end;
        break;
    case 1:
        start = l->anim->loop->start;
        end = l->anim->loop->end;
        switch (l->anim->loopMode) {
        case 1:
            if (l->anim->maxLoops == 0 || l->loops < l->anim->maxLoops) {
                type |= 2;
            }
            break;
        case 2:
            type |= 4;
            break;
        }
        break;
    case 2:
        start = l->anim->outro->start;
        end = l->anim->outro->end;
        break;
    }
    step = fps * g_fDeltaTimeSec;
    if (reverse) {
        switch (type) {
        case 3:
            break;
        case 4:
            raw = time += step;
            if (time > end) {
                over = time - end;
                reverse = 0;
                time = end;
                wrapped = 1;
            }
            break;
        case 5:
            raw = time -= step;
            if (time < start) {
                over = start - time;
                reverse = 0;
                time = start;
                wrapped = 1;
            }
            break;
        case 6:
            raw = time += step;
            if (time > end) {
                over = time - end;
                reverse = 0;
                wrapped = 1;
                time = 2.0f * end - time;
            }
            break;
        case 7:
            raw = time -= step;
            if (time < start) {
                over = start - time;
                reverse = 0;
                wrapped = 1;
                time = 2.0f * start - time;
            }
            break;
        }
    } else {
        if (type == 0) {
            raw = time -= step;
            if (time < start) {
                over = start - time;
                wrapped = 1;
                time = start;
            }
        } else if (type == 1) {
            raw = time += step;
            if (time > end) {
                over = time - end;
                wrapped = 1;
                time = end;
            }
        }
    }
    l->prevTime = prev;
    l->rawTime = raw;
    oldPhase = l->phase;
    l->time = time;
    l->reverse = reverse;
    switch (l->phase) {
    case 0:
        if (wrapped) {
            if (l->anim->loopMode & ((u8)mode != 0)) {
                l->phase = 1;
            } else if (l->anim->pingPong & ((u8)mode > 1)) {
                l->phase = 2;
            }
        }
        break;
    case 1:
        l->loops += wrapped;
        if (l->anim->maxLoops != 0 && l->loops >= l->anim->maxLoops) {
            l->loops = l->anim->maxLoops;
            if (l->anim->pingPong & ((u8)mode > 1)) {
                l->phase = 2;
            } else {
                l->time = l->anim->loop->end;
                l->done = 1;
            }
        }
        break;
    case 2:
        if (wrapped) {
            l->done = 1;
        }
        break;
    }
    if (l->phase != oldPhase) {
        switch (l->phase) {
        case 0:
            l->time = l->anim->intro->start + over;
            break;
        case 1:
            l->time = l->anim->loop->start + over;
            break;
        case 2:
            l->time = l->anim->outro->start + over;
            break;
        }
    }
}

void MotionState_DrawSlaves(void *state, s32 arg1) {
    extern Matrix4 g_identity;
    void Thing_DrawSlaves(void *thing);
    void CopyVector(Vector4 *dst, Vector4 *src);
    void CopyMatrix(void *dst, void *src);
    void MultiplyMatrix(Matrix4 *a, void *b, Matrix4 *out);
    typedef struct SlaveThing {
        u8 pad0[0x10];
        Matrix4 mtx;
        Vector4 quat;
        u8 pad60[0x74 - 0x60];
        u32 flags;
        u8 pad78[0x82 - 0x78];
        u8 bone;
        u8 pad83[0x8C - 0x83];
        void (**vtbl)();
        u8 pad90[0xA4 - 0x90];
        struct SlaveThing *next;
    } SlaveThing;
    typedef struct {
        u8 pad0[0x10];
        Matrix4 *matrices;
        SlaveThing *slaves;
    } State;
    Matrix4 *matrix = (Matrix4 *)arg1;
    SlaveThing *slave;
    Vector4 quat __attribute__((aligned(16)));
    Matrix4 mtx __attribute__((aligned(16)));
    Matrix4 out __attribute__((aligned(16)));

    slave = ((State *)state)->slaves;
    if (matrix != NULL && matrix != &g_identity) {
        for (; slave != NULL; slave = slave->next) {
            if (slave->flags & 0x200200) {
                continue;
            }
            CopyVector(&quat, &slave->quat);
            CopyMatrix(&mtx, &slave->mtx);
            MultiplyMatrix(&((State *)state)->matrices[slave->bone], matrix, &out);
            slave->vtbl[9](slave, &out);
            slave->vtbl[4](slave);
            Thing_DrawSlaves(slave);
            CopyVector(&slave->quat, &quat);
            CopyMatrix(&slave->mtx, &mtx);
        }
    } else {
        for (; slave != NULL; slave = slave->next) {
            if (slave->flags & 0x200200) {
                continue;
            }
            CopyVector(&quat, &slave->quat);
            CopyMatrix(&mtx, &slave->mtx);
            slave->vtbl[9](slave, &((State *)state)->matrices[slave->bone]);
            slave->vtbl[4](slave);
            Thing_DrawSlaves(slave);
            CopyVector(&slave->quat, &quat);
            CopyMatrix(&slave->mtx, &mtx);
        }
    }
}

void MotionState_DrawSlavesAttachCoronaOnly(void *state, s32 arg1) {
    extern Matrix4 g_identity;
    extern u8 g_attachVTable[];
    void Thing_DrawSlaves(void *thing);
    void CopyVector(Vector4 *dst, Vector4 *src);
    void CopyMatrix(void *dst, void *src);
    void MultiplyMatrix(Matrix4 *a, void *b, Matrix4 *out);
    typedef struct SlaveThing {
        u8 pad0[0x10];
        Matrix4 mtx;
        Vector4 quat;
        u8 pad60[0x74 - 0x60];
        u32 flags;
        u8 pad78[0x82 - 0x78];
        u8 bone;
        u8 pad83[0x8C - 0x83];
        void (**vtbl)();
        u8 pad90[0xA4 - 0x90];
        struct SlaveThing *next;
    } SlaveThing;
    typedef struct {
        u8 pad0[0x10];
        Matrix4 *matrices;
        SlaveThing *slaves;
    } State;
    Matrix4 *matrix = (Matrix4 *)arg1;
    SlaveThing *slave;
    Vector4 quat __attribute__((aligned(16)));
    Matrix4 mtx __attribute__((aligned(16)));
    Matrix4 out __attribute__((aligned(16)));

    slave = ((State *)state)->slaves;
    if (matrix != NULL && matrix != &g_identity) {
        for (; slave != NULL; slave = slave->next) {
            if (!(slave->flags & 0x200200) && slave->vtbl == (void *)g_attachVTable) {
                CopyVector(&quat, &slave->quat);
                CopyMatrix(&mtx, &slave->mtx);
                MultiplyMatrix(&((State *)state)->matrices[slave->bone], matrix, &out);
                slave->vtbl[9](slave, &out);
                slave->vtbl[4](slave);
                Thing_DrawSlaves(slave);
                CopyVector(&slave->quat, &quat);
                CopyMatrix(&slave->mtx, &mtx);
            }
        }
    } else {
        for (; slave != NULL; slave = slave->next) {
            if (!(slave->flags & 0x200200) && slave->vtbl == (void *)g_attachVTable) {
                CopyVector(&quat, &slave->quat);
                CopyMatrix(&mtx, &slave->mtx);
                slave->vtbl[9](slave, &((State *)state)->matrices[slave->bone]);
                slave->vtbl[4](slave);
                Thing_DrawSlaves(slave);
                CopyVector(&slave->quat, &quat);
                CopyMatrix(&slave->mtx, &mtx);
            }
        }
    }
}

#pragma push
#pragma dont_inline on
void MotionState_BuildMatricesWorld(void *arg0, void *arg1, void *motionState, f32 time) {
    u8 frames[0x800];
    void *anim;
    s32 *skel;

    if (M2C_FIELD(motionState, u32 *, 0x10) != 0) {
        anim = M2C_FIELD(motionState, void **, 0);
        skel = M2C_FIELD(anim, s32 **, 0x14);
        MotionState_ExtractBlendedKeyFrame(motionState, frames);
        TEST_AnimTraverseHeirarchyToMatricesWorld(arg0, arg1, frames, *skel, M2C_FIELD(motionState, void **, 0x10), time);
    }
}

void Motion_HeartbeatB2(void *state, Vector4 *vel) {
    void MotionState_BlendTriggerEvents(void *state, void *events, Vector4 *vel);
    void Thing_MoveSlaves(void *thing);
    void CopyVector(Vector4 *dst, Vector4 *src);
    void CopyMatrix(void *dst, void *src);
    typedef struct SlaveThing {
        Vector4 vel;
        Matrix4 mtx;
        Vector4 pos;
        u8 pad60[0x74 - 0x60];
        u32 flags;
        u8 pad78[0x82 - 0x78];
        u8 bone;
        u8 pad83[0x8C - 0x83];
        void (**vtbl)();
        u8 pad90[0xA4 - 0x90];
        struct SlaveThing *next;
    } SlaveThing;
    typedef struct {
        u8 *anim;
        u8 pad4[0x10 - 0x4];
        Matrix4 *matrices;
        SlaveThing *slaves;
    } State;
    Matrix4 *matrices;
    SlaveThing *slave;

    MotionState_BlendTriggerEvents(state, ((State *)state)->anim + 0x2C, vel);
    matrices = ((State *)state)->matrices;
    {
        Vector4 v __attribute__((aligned(16))) = {0};
        Vector4 pos __attribute__((aligned(16)));
        Vector4 oldVel __attribute__((aligned(16)));
        Matrix4 mtx __attribute__((aligned(16)));

        if (vel != NULL) {
            v = *vel;
        }
        for (slave = ((State *)state)->slaves; slave != NULL; slave = slave->next) {
            if (slave->flags & 0x201000) {
                continue;
            }
            if (!(slave->flags & 0x400000)) {
                CopyVector(&pos, &slave->pos);
                CopyMatrix(&mtx, &slave->mtx);
                slave->vtbl[9](slave, &matrices[slave->bone]);
                oldVel = slave->vel;
                slave->vel = v;
                slave->vtbl[3](slave);
                Thing_MoveSlaves(slave);
                CopyVector(&slave->pos, &pos);
                CopyMatrix(&slave->mtx, &mtx);
                CopyVector(&slave->vel, &oldVel);
            } else {
                slave->vtbl[3](slave);
            }
        }
    }
    ThingManagerList_RemoveReleasedThings((s32)state + 0x14);
}

#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 AnimNodes_BlowBranch(u8 *node) {
    s32 count = 1;
    u32 i;
    u8 *child = node + 0x20;

    if (node[0x1F] & 3) {
        return 0;
    }
    node[0x1F] |= 2;
    for (i = 0; i < node[0x1C]; i++) {
        count += AnimNodes_BlowBranch(child);
        child += child[0x1E] << 5;
    }
    return count;
}
#pragma pop

