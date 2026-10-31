#include "common.h"

extern u8 *g_StreamingDataPacks[11];
extern u32 g_MaxStreamingDataPacks[11];
extern s32 g_numQuadPacksInMemory;
extern s32 g_numQuadPacksToLoad;
extern s32 g_itemTimer;
extern void *g_pDataPackToLoad;
void *Allocate(u32 size, s32 type, const char *name);
void memset(void *dst, s32 c, u32 n);
void Deallocate(void *alloc);
s32 FileStreaming_ReleaseAllDataPacks(s32 type, s8 wait);
extern s8 g_bLoadingThreadRunning;
extern s8 g_bLoadQuadrants;
extern s8 g_bLoadDataPack;
extern s32 g_numDataPacksToRelease;
extern s32 g_numQuadPacksToRelease;
extern void *g_pPacksToBeReleased[10];
extern void *g_pQuadPackToBeReleased[11];
extern s32 g_dataPackToLoadType;
extern u8 *g_LoadedQuadrants[11];
extern void *g_pWorldQuadrant;
extern s32 g_quadFileID;
s32 FileStreaming_GetPackID(char *name, s32 type, u32 id);
void FileStreaming_SetActiveQuadrants(void **quads, u32 count);
void File_Close(s32 file);
s32 FileStreaming_ReleaseDataPack(u32 index, s32 type);
void ThingManager_Invalidate(u32 start, u32 end);
extern void (*DestroyMusic)(void);
void FileStreaming_HeartBeat(void);
void LoadDataPack(void);
void FileStreaming_LoadingThread(void);
void DataPack_LoadFile(s32 file, void *pack, s32 type, s32 size, s32 *out);
void DataPack_ReleaseDataPack(void *pack);
void DataPack_ReleaseQuadPack(void *pack);
void LoadQuadrants(s8 refresh);
void EndThread(void);

extern s32 g_loadingThreadID;
extern void *g_QuadrantsToLoad[11];
s32 RunThread(void *func, s32 priority);
void DataPack_RefreshQuadPackPtrs(s32 pack, void *quad);
void FileStreaming_LoadQuadPacks(void **quads, u32 count, s8 threaded);

typedef struct ItemInfo {
    u8 pad0[0x10];
    f32 height;
    u8 pad14[0x28 - 0x14];
    u32 effect;
} ItemInfo;

typedef struct ItemThing {
    u8 pad0[0x74];
    u32 flags;
    u8 pad78[0x8C - 0x78];
    void (**vtbl)(struct ItemThing *);
    void *owner;
    u8 pad94[0xD0 - 0x94];
    void *respawnItem;
} ItemThing;

typedef struct QuadItem {
    Vector4 pos;
    u8 pad10[0x4];
    s32 respawn;
    s32 timer;
    ItemInfo *info;
    f32 value;
    ItemThing *thing;
    u32 type;
    u8 pad2C[0x34 - 0x2C];
    u8 hasValue;
    u8 subtype;
    u8 pad36[0x38 - 0x36];
    s8 spawned;
    u8 defType;
    u8 pad3A[0x40 - 0x3A];
} QuadItem;

typedef struct QuadStatic {
    u8 pad0[0x24];
    u8 *info;
    u8 pad28[0x2C - 0x28];
    ItemThing *thing;
    s32 active;
    u8 pad34[0x36 - 0x34];
    u8 defActive;
    u8 pad37[0x40 - 0x37];
} QuadStatic;

typedef struct QuadDynamic {
    u8 pad0[0x30];
    ItemThing *thing;
    s32 active;
    u8 pad38[0x39 - 0x38];
    u8 defActive;
    u8 pad3A[0x40 - 0x3A];
} QuadDynamic;

typedef struct QuadItemList {
    s8 dirty;
    u32 numStatics;
    QuadStatic *statics;
    u32 numDynamics;
    QuadDynamic *dynamics;
    u32 count;
    QuadItem *items;
} QuadItemList;

typedef struct QuadrantEntry {
    u8 pad0[0x30];
    QuadItemList *data;
    u8 pad34[0x50 - 0x34];
} QuadrantEntry;

typedef struct QuadrantWorld {
    u8 pad[0x20];
    u32 numQuads;
    QuadrantEntry *quads;
} QuadrantWorld;

s8 Trigger_IsTACSystemPaused(void);
void Trigger_PauseTACSystem(s8 paused);

/* ThingManager_DARYL_CreateThing(4, ...) parameters. */
typedef struct ItemThingParams {
    Quaternion rot;
    Vector4 pos;
    f32 unk30;
    f32 unk34;
    f32 unk38;
    u32 unk3C;
    ItemInfo *info;
    f32 value;
    s8 subtype;
} ItemThingParams;

/* ThingManager_DARYL_CreateThing(1, ...) parameters. */
typedef struct EffectThingParams {
    Vector4 pos;
    Vector4 animRefPos;
    Vector4 vel;
    Vector4 angVel;
    u32 id;
    void *player;
    f32 unk48;
    f32 unk4C;
} EffectThingParams;

extern s32 g_iDeltaTimeMS;
extern Vector4 g_zeroVector __attribute__((aligned(16)));
void *ThingManager_DARYL_CreateThing(s32 type, void *params);
u32 GameSettings_GetCodeVariable(s32 var, s32 arg);

#pragma push
#pragma use_lmw_stmw on

void FileStreaming_Initialize(void)
{
    u32 i;
    u32 size;

    g_numQuadPacksInMemory = 0;
    g_numQuadPacksToLoad = 0;
    g_itemTimer = 0;
    for (i = 0; i < 11; i++) {
        if (i != 7 && g_MaxStreamingDataPacks[i] != 0) {
            size = g_MaxStreamingDataPacks[i] * 0xA8;
            g_StreamingDataPacks[i] = (u8 *) Allocate(size, 0, "");
            memset(g_StreamingDataPacks[i], 0, size);
        } else {
            g_StreamingDataPacks[i] = NULL;
        }
    }
    g_pDataPackToLoad = NULL;
}

void FileStreaming_Destroy(void)
{
    u32 i;

    for (i = 0; i < 11; i++) {
        FileStreaming_ReleaseAllDataPacks(i, 1);
        Deallocate(g_StreamingDataPacks[i]);
    }
}

u8 IsLoadingThreadRunning(void)
{
    return g_bLoadingThreadRunning;
}

s32 FileStreaming_GetNumAvailableQuadPacks(void)
{
    return 0xB;
}

void FileStreaming_ReleaseQuadPacks(void)
{
    u32 i;

    while (g_bLoadingThreadRunning)
    {
    }
    for (i = 0; i < (u32)g_numQuadPacksInMemory; i++)
    {
        if (g_LoadedQuadrants[i] != NULL && (s8)g_LoadedQuadrants[i][0x34] != 0)
        {
            DataPack_ReleaseQuadPack(g_LoadedQuadrants[i]);
        }
    }
    if (g_pWorldQuadrant != NULL && (s8)((u8 *)g_pWorldQuadrant)[0x34] != 0)
    {
        DataPack_ReleaseQuadPack(g_pWorldQuadrant);
    }
    g_numQuadPacksInMemory = 0;
    g_numQuadPacksToLoad = 0;
    FileStreaming_HeartBeat();
    FileStreaming_HeartBeat();
    FileStreaming_SetActiveQuadrants(0, 0);
    g_itemTimer = 0;
    if (g_quadFileID != -1)
    {
        File_Close(g_quadFileID);
        g_quadFileID = -1;
    }
}

void FileStreaming_InitializeAllQuads(void)
{
    typedef struct Quad
    {
        u32 id;
        u8 pad[0x18];
        u32 unk1C;
        u32 numNeighbors;
        u32 *neighbors;
        u8 pad2[0x28];
    } Quad;
    typedef struct QuadWorld
    {
        u8 pad[0x20];
        u32 numQuads;
        Quad *quads;
    } QuadWorld;
    u32 i;
    u32 j;
    u32 k;
    Quad *quad;

    if (g_worldData.world == NULL)
    {
        return;
    }
    for (i = 0; i < ((QuadWorld *)g_worldData.world)->numQuads; i++)
    {
        quad = &((QuadWorld *)g_worldData.world)->quads[i];
        if (quad->unk1C == 0)
        {
            g_pWorldQuadrant = quad;
        }
        else
        {
            for (j = 0; j < quad->numNeighbors; j++)
            {
                for (k = 0; k < ((QuadWorld *)g_worldData.world)->numQuads; k++)
                {
                    if (k != i && ((QuadWorld *)g_worldData.world)->quads[k].id == quad->neighbors[j])
                    {
                        quad->neighbors[j] = k;
                        break;
                    }
                }
            }
        }
    }
}

void FileStreaming_ResetQuadResources(void)
{
    u32 i;
    u32 j;
    s8 paused;
    QuadStatic *st;
    QuadDynamic *dyn;
    QuadItem *item;
    QuadrantEntry *quad;

    paused = Trigger_IsTACSystemPaused();
    Trigger_PauseTACSystem(1);
    g_LoadedQuadrants[g_numQuadPacksInMemory] = g_pWorldQuadrant;
    for (i = 0; i < g_numQuadPacksInMemory + 1; i++) {
        if (g_LoadedQuadrants[i] != NULL && (*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->dirty) {
            (*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->dirty = 0;
            for (j = 0; j < (*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->numStatics; j++) {
                st = &(*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->statics[j];
                if (st->thing != NULL && st->info[7] == 0) {
                    st->thing->vtbl[1](st->thing);
                    st->thing = NULL;
                    st->active = 1;
                }
            }
            for (j = 0; j < (*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->numDynamics; j++) {
                dyn = &(*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->dynamics[j];
                if (dyn->thing != NULL) {
                    dyn->thing->vtbl[1](dyn->thing);
                    dyn->thing = NULL;
                    dyn->active = 1;
                }
            }
            for (j = 0; j < (*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->count; j++) {
                item = &(*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->items[j];
                if (item->thing != NULL) {
                    item->thing->vtbl[1](item->thing);
                    item->thing = NULL;
                    item->type = 1;
                }
            }
        }
    }
    for (i = 0; i < ((QuadrantWorld *)g_worldData.world)->numQuads; i++) {
        quad = &((QuadrantWorld *)g_worldData.world)->quads[i];
        for (j = 0; j < quad->data->numDynamics; j++) {
            quad->data->dynamics[j].active = quad->data->dynamics[j].defActive;
        }
        for (j = 0; j < quad->data->count; j++) {
            quad->data->items[j].type = quad->data->items[j].defType;
            quad->data->items[j].spawned = 0;
        }
        for (j = 0; j < quad->data->numStatics; j++) {
            quad->data->statics[j].active = quad->data->statics[j].defActive;
        }
    }
    FileStreaming_SetActiveQuadrants((void **)g_LoadedQuadrants, g_numQuadPacksInMemory + 1);
    Trigger_PauseTACSystem(paused);
}

#pragma push
#pragma use_lmw_stmw on
void FileStreaming_SetActiveQuadrants(void **quads, u32 count) {
    typedef struct Thing {
        u8 pad0[0x8C];
        void (**vtbl)(struct Thing *);
        u8 *owner;
    } Thing;
    typedef struct SlaveInfo {
        u8 pad0[0x18];
        u32 *shared;
        u8 pad1C[0x20 - 0x1C];
    } SlaveInfo;
    typedef struct StaticInfo {
        u8 *box;
        u8 noRadius;
        u8 pad5[2];
        u8 isStatic;
        u8 pad8[0x18 - 0x8];
        u32 numSlaves;
        SlaveInfo *slaves;
    } StaticInfo;
    typedef struct StaticEntry {
        Vector4 pos;
        Vector4 rot;
        u8 pad20[0x24 - 0x20];
        StaticInfo *info;
        f32 scale;
        Thing *thing;
        u32 active;
        u8 hasScale;
        s8 respawn;
        u8 pad36[0x3C - 0x36];
        f32 radius;
    } StaticEntry;
    typedef struct DynamicEntry {
        Vector4 pos;
        Vector4 rot;
        u8 pad20[0x24 - 0x20];
        void *info;
        void *corona;
        f32 scale;
        Thing *thing;
        u32 mode;
        u8 hasScale;
        u8 pad39[0x40 - 0x39];
    } DynamicEntry;
    typedef struct ItemEntry {
        u8 pad0[0x18];
        s32 timer;
        u8 *info;
        u8 pad20[0x24 - 0x20];
        Thing *thing;
        u8 pad28[0x38 - 0x28];
        s8 spawned;
        u8 pad39[0x40 - 0x39];
    } ItemEntry;
    typedef struct QuadData {
        s8 dirty;
        u32 numStatics;
        StaticEntry *statics;
        u32 numDynamics;
        DynamicEntry *dynamics;
        u32 numItems;
        ItemEntry *items;
    } QuadData;
    typedef struct Quad {
        u8 pad0[0x1C];
        void *loaded;
        u8 pad20[0x30 - 0x20];
        QuadData *data;
        s8 releasable;
    } Quad;
    typedef struct StaticParams {
        Vector4 pos;
        Vector4 vel;
        Vector4 angVel;
        Vector4 rot;
        StaticInfo *info;
        u32 unk44;
        f32 scale;
        u32 unk4C;
        u32 unk50;
    } StaticParams;
    typedef struct DynamicParams {
        Vector4 pos;
        Vector4 rot;
        u8 pad20[0x30 - 0x20];
        void *info;
        u32 unk34;
        f32 scale;
    } DynamicParams;
    typedef struct ModeAction {
        u32 mode;
        s32 set;
        void *entry;
    } ModeAction;
    void ThingManagerList_RemoveReleasedThings(void **list);
    s32 ActionDestructibleChangeMode(u32 *action);
    s32 ActionFXPointChangeMode(s32 *data);
    s32 CoronaHolder_Add(void *corona, Vector4 *pos);
    s32 SharedSlaveHolder_Get(void *shared);
    f32 GetBoundingBoxRadius(Vector4 *box, Vector4 *offset);
    StaticParams sp __attribute__((aligned(16)));
    Quad *release[12];
    DynamicParams dp __attribute__((aligned(16)));
    ModeAction destAction;
    ModeAction fxAction;
    u32 numRelease;
    u32 i;
    u32 k;
    u32 j;
    Quad *quad;
    s8 paused;

    numRelease = 0;
    paused = Trigger_IsTACSystemPaused();
    for (i = 0; i < g_numQuadPacksInMemory; i++) {
        quad = ((Quad **)g_LoadedQuadrants)[i];
        for (j = 0; j < count; j++) {
            if (quad == ((Quad **)quads)[j]) {
                break;
            }
        }
        if (j == count && quad->loaded != NULL) {
            release[numRelease++] = quad;
        }
    }
    Trigger_PauseTACSystem(1);
    for (i = 0; i < numRelease; i++) {
        quad = release[i];
        if (quad->data->dirty) {
            quad->data->dirty = 0;
            for (j = 0; j < quad->data->numStatics; j++) {
                StaticEntry *st = &release[i]->data->statics[j];

                if (st->thing != NULL && st->info->isStatic == 0) {
                    st->thing->vtbl[1](st->thing);
                    st->thing = NULL;
                }
            }
            for (j = 0; j < release[i]->data->numDynamics; j++) {
                DynamicEntry *dyn = &release[i]->data->dynamics[j];

                if (dyn->thing != NULL) {
                    dyn->thing->vtbl[1](dyn->thing);
                    dyn->thing = NULL;
                }
            }
            for (j = 0; j < release[i]->data->numItems; j++) {
                ItemEntry *item = &release[i]->data->items[j];

                if (item->thing != NULL && item->info[0] != 7) {
                    if (item->timer == 0) {
                        item->spawned = 0;
                    }
                    item->thing->vtbl[1](item->thing);
                    item->thing = NULL;
                }
            }
            if (release[i]->releasable) {
                g_pQuadPackToBeReleased[g_numQuadPacksToRelease++] = release[i];
            }
        }
    }
    ThingManagerList_RemoveReleasedThings((void **)&g_thingManager_DARYL.activeList);
    Trigger_PauseTACSystem(paused);
    g_numQuadPacksInMemory = 0;
    for (i = 0; i < count; i++) {
        if (((Quad **)quads)[i]->loaded != NULL) {
            ((Quad **)g_LoadedQuadrants)[g_numQuadPacksInMemory++] = ((Quad **)quads)[i];
        }
        if (!((Quad **)quads)[i]->data->dirty) {
            ((Quad **)quads)[i]->data->dirty = 1;
            for (j = 0; j < ((Quad **)quads)[i]->data->numStatics; j++) {
                StaticEntry *st = &((Quad **)quads)[i]->data->statics[j];

                if (st->active == 3 && st->respawn) {
                    st->active = 1;
                } else if (st->active == 3) {
                    continue;
                } else if (st->info->isStatic) {
                    for (k = 0; k < st->info->numSlaves; k++) {
                        u32 *shared = st->info->slaves[k].shared;

                        if (shared[0] == 0) {
                            shared[0] = 1;
                            ((u8 *)shared[2])[0x1D] = SharedSlaveHolder_Get(shared);
                        }
                    }
                    if (st->info->noRadius == 0) {
                        st->radius = GetBoundingBoxRadius((Vector4 *)(st->info->box + 0x10), &g_zeroVector);
                    } else {
                        st->radius = 1.0f;
                    }
                    continue;
                }
                sp.unk4C = 0;
                sp.unk50 = 0;
                sp.scale = st->hasScale ? st->scale : 0.0f;
                sp.pos = st->pos;
                sp.info = st->info;
                sp.rot = st->rot;
                sp.angVel = g_zeroVector;
                sp.vel = g_zeroVector;
                st->thing = ThingManager_DARYL_CreateThing(3, &sp);
                if (st->thing != NULL) {
                    st->thing->owner = (u8 *)st;
                    destAction.mode = st->active;
                    destAction.set = 1;
                    destAction.entry = st;
                    ActionDestructibleChangeMode((u32 *)&destAction);
                }
            }
            for (j = 0; j < ((Quad **)quads)[i]->data->numDynamics; j++) {
                DynamicEntry *dyn = &((Quad **)quads)[i]->data->dynamics[j];

                if (dyn->info != NULL) {
                    dp.pos = dyn->pos;
                    dp.info = dyn->info;
                    dp.rot = dyn->rot;
                    dp.scale = dyn->hasScale ? dyn->scale : 0.0f;
                    dp.unk34 = 0;
                    dyn->thing = ThingManager_DARYL_CreateThing(2, &dp);
                }
                if (dyn->corona != NULL) {
                    CoronaHolder_Add(dyn->corona, (Vector4 *)dyn);
                }
                fxAction.mode = dyn->mode;
                fxAction.set = 1;
                fxAction.entry = dyn;
                ActionFXPointChangeMode((s32 *)&fxAction);
            }
        }
    }
}
#pragma pop

#pragma use_lmw_stmw on
void FileStreaming_HeartBeatItems(void)
{
    EffectThingParams p __attribute__((aligned(16)));
    ItemThingParams params __attribute__((aligned(16)));
    u32 i;
    u32 j;
    QuadItem *item;
    ItemThing *thing;

    g_LoadedQuadrants[g_numQuadPacksInMemory] = g_pWorldQuadrant;
    g_itemTimer += g_iDeltaTimeMS;
    if (g_itemTimer > 1500) {
        g_itemTimer -= 1500;
        for (i = 0; i < g_numQuadPacksInMemory + 1; i++) {
            for (j = 0; j < (*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->count; j++) {
                item = &(*(QuadItemList **)(g_LoadedQuadrants[i] + 0x30))->items[j];
                if ((item->timer != 0 || !item->spawned) && item->info != NULL && item->thing == NULL && item->type == 1) {
                    item->timer -= 1500;
                    if (item->timer <= 0) {
                        item->timer = 0;
                        params.pos = item->pos;
                        params.pos.y += item->info->height;
                        params.unk30 = 0.0f;
                        params.unk34 = 0.0f;
                        params.unk38 = 0.0f;
                        params.info = item->info;
                        params.rot.w = 1.0f;
                        params.rot.x = 0.0f;
                        params.rot.y = 0.0f;
                        params.rot.z = 0.0f;
                        if (item->hasValue != 0) {
                            params.value = item->value;
                        }
                        params.subtype = item->subtype;
                        thing = (ItemThing *)ThingManager_DARYL_CreateThing(4, &params);
                        if (thing) {
                            item->spawned = 1;
                            if (GameSettings_GetCodeVariable(0x12D, 0) && !GameSettings_GetCodeVariable(0x136, 0)) {
                                item->respawn = 0;
                            }
                            item->thing = thing;
                            thing->respawnItem = item;
                            thing->owner = item;
                            thing->flags |= 1;
                            if (item->respawn) {
                                thing->respawnItem = item;
                            }
                            if (item->info->effect) {
                                p.id = item->info->effect;
                                p.pos = item->pos;
                                p.animRefPos = *(Vector4 *)&params.rot;
                                p.vel.z = 0.0f;
                                p.vel.y = 0.0f;
                                p.vel.x = 0.0f;
                                p.player = NULL;
                                p.unk48 = 0.0f;
                                p.unk4C = 0.0f;
                                p.angVel = g_zeroVector;
                                ThingManager_DARYL_CreateThing(1, &p);
                            }
                        }
                    }
                }
            }
        }
    }
}
void FileStreaming_LoadQuadPacks(void **quads, u32 count, s8 threaded)
{
    u32 i;
    void *quad;

    if (g_bLoadingThreadRunning || (u32)g_numQuadPacksToRelease != 0)
    {
        return;
    }
    for (i = 0; i < count; i++)
    {
        quad = quads[i];
        if (M2C_FIELD(quad, void **, 0x10) != NULL && (s8)M2C_FIELD(quad, s8 *, 0x34) == 0)
        {
            DataPack_RefreshQuadPackPtrs(g_worldData.unk8[2], quads[i]);
            M2C_FIELD(quads[i], s8 *, 0x34) = 1;
        }
        else if (M2C_FIELD(quad, void **, 0xC) == NULL)
        {
            M2C_FIELD(quad, s8 *, 0x34) = 1;
        }
        else if ((s8)M2C_FIELD(quad, s8 *, 0x34) == 0)
        {
            g_QuadrantsToLoad[g_numQuadPacksToLoad++] = quad;
        }
    }
    if ((u32)g_numQuadPacksToLoad != 0)
    {
        if (threaded)
        {
            g_bLoadQuadrants = 1;
            g_bLoadingThreadRunning = 1;
            g_loadingThreadID = RunThread(FileStreaming_LoadingThread, 0x63);
        }
        else
        {
            LoadQuadrants(1);
        }
    }
}

void FileStreaming_LoadingThread(void)
{
    g_bLoadingThreadRunning = 1;
    if (g_bLoadQuadrants)
    {
        LoadQuadrants(0);
    }
    if (g_bLoadDataPack)
    {
        LoadDataPack();
    }
    g_bLoadQuadrants = 0;
    g_bLoadDataPack = 0;
    g_bLoadingThreadRunning = 0;
    EndThread();
}

void LoadQuadrants(s8 refresh) {
    typedef struct TocEntry {
        u32 version;
        u32 type;
        u32 world;
        u32 id;
        u32 size;
        u32 offset;
    } TocEntry;
    typedef struct Quad {
        u8 pad0[0xC];
        u32 index;
        u8 pad10[0x38 - 0x10];
        u32 id;
        u32 fileOffset;
        u32 size;
        u8 pad44[0x50 - 0x44];
    } Quad;
    typedef struct World {
        u8 pad0[0x20];
        u32 numQuads;
        Quad *quads;
        u8 pad28[0x7C - 0x28];
        char *fileName;
        u8 pad80[0x8C - 0x80];
        char *name;
        u32 id;
    } World;
    typedef struct QuadPack {
        u8 pad0[0x10];
        void *loaded;
        u8 pad14[0x34 - 0x14];
        s8 inMemory;
    } QuadPack;
    s32 File_Open(u32 dev, const char *name, u32 flags, s32 *size);
    s32 File_Read(u32 id, s32 size, void *data);
    void DataPack_LoadQuadPack(s32 file, void *quad, u32 worldID);
    void Dprintf(const char *fmt, ...);
    int sprintf(char *buf, const char *fmt, ...);
    char *strcpy(char *dst, const char *src);
    char *strcat(char *dst, const char *src);
    static TocEntry tocHeader[50];
    char path[0x80];
    char name[0x80];
    char msg[0x100];
    u32 i;
    u32 j;
    s8 error;
    u32 count;
    Quad *quad;
    u32 alt;

    strcpy(name, ((World *)g_worldData.world)->name);
    strcpy(path, ((World *)g_worldData.world)->name);
    strcat(path, ".xgc");
    if (g_quadFileID == -1) {
        error = 0;
        g_quadFileID = File_Open(0, path, 0, NULL);
        if (g_quadFileID != -1) {
            File_Read(g_quadFileID, 0x10, tocHeader);
            if (tocHeader[0].version != 0x020100A0 || tocHeader[0].type != 0x040100AE) {
                error = 1;
            } else if (tocHeader[0].world != 0xB7) {
                Dprintf("\n*The world format is different %s. Packfile:%d,  Code:%d\n", ((World *)g_worldData.world)->fileName, tocHeader[0].world, 0xB7);
                error = 1;
            } else {
                count = tocHeader[0].id;
                File_Read(g_quadFileID, count * sizeof(TocEntry), tocHeader);
                for (i = 0; i < ((World *)g_worldData.world)->numQuads; i++) {
                    quad = &((World *)g_worldData.world)->quads[i];
                    alt = 0;
                    for (j = 0; j < count; j++) {
                        u32 id = tocHeader[j].id;
                        u32 type = tocHeader[j].type;
                        u32 index = id >> 16;

                        if (type == 7 && ((World *)g_worldData.world)->id == tocHeader[j].world &&
                            quad->id == id && index == quad->index) {
                            if (tocHeader[j].version != 0xB7) {
                                Dprintf("\n*The world format is different %s. Packfile:%d,  Code:%d\n", ((World *)g_worldData.world)->name, tocHeader[j].version, 0xB7);
                            } else {
                                quad->size = tocHeader[j].size;
                                quad->fileOffset = (count * sizeof(TocEntry) + 0x10) + tocHeader[j].offset;
                            }
                            break;
                        }
                        if (type == 7 && ((World *)g_worldData.world)->id == tocHeader[j].world &&
                            (u16)quad->id == (u16)id) {
                            alt = index;
                        }
                    }
                    if (j == count) {
                        if (alt == 0) {
                            sprintf(msg, "The quadrant pack could not be found, Pack ID = %d Num GX = %d\nYou need to repack the world using the latest BrainCore\n", (u16)quad->id, quad->index);
                        } else {
                            sprintf(msg, "The quadrant pack could not be found, Pack ID = %d Num GX = %d\nThe quadrant pack contains a pack with the same ID however the Num GX = %d\nYou need to repack the world using the latest BrainCore\n", (u16)quad->id, quad->index, alt);
                        }
                        Dprintf(msg);
                    }
                }
            }
        }
        if (error || g_quadFileID == -1) {
            g_numQuadPacksToLoad = 0;
            return;
        }
    }
    for (i = 0; i < g_numQuadPacksToLoad; i++) {
        if (((QuadPack *)g_QuadrantsToLoad[i])->loaded == NULL) {
            DataPack_LoadQuadPack(g_quadFileID, g_QuadrantsToLoad[i], ((World *)g_worldData.world)->id);
            if (refresh) {
                DataPack_RefreshQuadPackPtrs(g_worldData.unk8[2], g_QuadrantsToLoad[i]);
                ((QuadPack *)g_QuadrantsToLoad[i])->inMemory = 1;
            }
        }
    }
    g_numQuadPacksToLoad = 0;
}

s32 FileStreaming_LoadDataPack(void *name, s32 type, u32 id, s8 threaded) {
    typedef struct DataPack {
        s8 used;
        u8 unk1;
        u8 unk2;
        char *name;
        u32 id;
        u32 refs;
        u8 unk10;
        u8 pad11[0x4C - 0x11];
        void *data;
        u8 pad50[0xA8 - 0x50];
    } DataPack;
    u32 i;
    u32 slot;

    slot = -1;
    if (type >= 11 || (g_bLoadingThreadRunning && threaded)) {
        return -1;
    }
    if (((DataPack **)g_StreamingDataPacks)[type] == NULL) {
        FileStreaming_Initialize();
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++) {
        if (((DataPack **)g_StreamingDataPacks)[type][i].used && id == ((DataPack **)g_StreamingDataPacks)[type][i].id
            && strcmp(name, ((DataPack **)g_StreamingDataPacks)[type][i].name) == 0
            && ((DataPack **)g_StreamingDataPacks)[type][i].refs != 0) {
            if (((DataPack **)g_StreamingDataPacks)[type][i].data == NULL) {
                ((DataPack **)g_StreamingDataPacks)[type][i].used = 0;
                ((DataPack **)g_StreamingDataPacks)[type][i].refs = 0;
                ((DataPack **)g_StreamingDataPacks)[type][i].unk1 = 0;
                ((DataPack **)g_StreamingDataPacks)[type][i].unk2 = 0;
            } else {
                (((DataPack **)g_StreamingDataPacks)[type] + i)->refs++;
                return i;
            }
        }
        if (slot == -1 && !((DataPack **)g_StreamingDataPacks)[type][i].used) {
            slot = i;
        }
    }
    if (slot == -1) {
        return -1;
    }
    ((DataPack **)g_StreamingDataPacks)[type][slot].used = 1;
    (((DataPack **)g_StreamingDataPacks)[type] + slot)->unk1 = 0;
    (((DataPack **)g_StreamingDataPacks)[type] + slot)->unk2 = 0;
    (((DataPack **)g_StreamingDataPacks)[type] + slot)->name = name;
    (((DataPack **)g_StreamingDataPacks)[type] + slot)->id = id;
    (((DataPack **)g_StreamingDataPacks)[type] + slot)->refs = 1;
    (((DataPack **)g_StreamingDataPacks)[type] + slot)->unk10 = 0;
    g_dataPackToLoadType = type;
    g_pDataPackToLoad = ((DataPack **)g_StreamingDataPacks)[type] + slot;
    if (threaded) {
        if (!g_bLoadingThreadRunning) {
            g_bLoadDataPack = 1;
            g_bLoadingThreadRunning = 1;
            g_loadingThreadID = RunThread(FileStreaming_LoadingThread, 0x63);
        }
    } else {
        LoadDataPack();
    }
    return slot;
}

s32 FileStreaming_GetPackID(char *name, s32 type, u32 id)
{
    u8 *pack;
    u32 i;

    if (type >= 11)
    {
        return -1;
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++)
    {
        pack = g_StreamingDataPacks[type];
        if ((s8)pack[i * 0xA8] != 0 && id == M2C_FIELD(pack + i * 0xA8, u32 *, 8))
        {
            if (name == M2C_FIELD(pack + i * 0xA8, char **, 4) || strcmp(name, M2C_FIELD(pack + i * 0xA8, char **, 4)) == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

#pragma push
#pragma use_lmw_stmw on
s32 FileStreaming_ReleaseAllDataPacks(s32 type, s8 wait)
{
    u8 *pack;
    u32 i;

    if (type >= 11)
    {
        return 0;
    }
    for (i = 0; i < g_MaxStreamingDataPacks[type]; i++)
    {
        pack = g_StreamingDataPacks[type];
        if (pack != NULL && (s8)pack[i * 0xA8 + 1] != 0)
        {
            M2C_FIELD(pack + i * 0xA8, s32 *, 0xC) = 1;
            FileStreaming_ReleaseDataPack(i, type);
        }
    }
    if (wait)
    {
        FileStreaming_HeartBeat();
        FileStreaming_HeartBeat();
    }
    return 1;
}

s32 FileStreaming_ReleaseDataPack(u32 index, s32 type)
{
    if (type >= 11 || index >= g_MaxStreamingDataPacks[type])
    {
        return 0;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8] == 0)
    {
        return 1;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8 + 1] == 0)
    {
        g_StreamingDataPacks[type][index * 0xA8] = 0;
        return 1;
    }
    if (--M2C_FIELD(g_StreamingDataPacks[type] + index * 0xA8, s32 *, 0xC) != 0)
    {
        return 1;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8 + 2] == 0)
    {
        *(s32 *)&g_StreamingDataPacks[type][index * 0xA8 + 0x40] = 0;
    }
    ThingManager_Invalidate(M2C_FIELD(g_StreamingDataPacks[type] + index * 0xA8, u32 *, 0x14),
                            M2C_FIELD(g_StreamingDataPacks[type] + index * 0xA8, u32 *, 0x14) +
                                M2C_FIELD(g_StreamingDataPacks[type] + index * 0xA8, u32 *, 0x3C));
    if ((s8)g_StreamingDataPacks[type][index * 0xA8 + 0x78] != 0)
    {
        DestroyMusic();
    }
    g_pPacksToBeReleased[g_numDataPacksToRelease++] = g_StreamingDataPacks[type] + index * 0xA8;
    return 1;
}

#pragma push
#pragma use_lmw_stmw off
void FileStreaming_HeartBeat(void)
{
    u32 i;

    for (i = 0; i < g_numDataPacksToRelease; i++)
    {
        DataPack_ReleaseDataPack((u8 *)g_pPacksToBeReleased[i] + 0x14);
        ((u8 *)g_pPacksToBeReleased[i])[0] = 0;
    }
    g_numDataPacksToRelease = 0;
    for (i = 0; i < g_numQuadPacksToRelease; i++)
    {
        DataPack_ReleaseQuadPack(g_pQuadPackToBeReleased[i]);
    }
    g_numQuadPacksToRelease = 0;
}

#pragma push
#pragma use_lmw_stmw off
void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh)
{
    if (type >= 11 || index >= g_MaxStreamingDataPacks[type])
    {
        return NULL;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8] == 0 || (s8)g_StreamingDataPacks[type][index * 0xA8 + 1] == 0)
    {
        return NULL;
    }
    if ((s8)g_StreamingDataPacks[type][index * 0xA8 + 2] == 0 && refresh)
    {
        DataPack_RefreshPtrs(g_StreamingDataPacks[type] + index * 0xA8 + 0x14);
    }
    g_StreamingDataPacks[type][index * 0xA8 + 2] = 1;
    return g_StreamingDataPacks[type] + index * 0xA8 + 0x14;
}
#pragma pop

#pragma pop

#pragma push
#pragma use_lmw_stmw off
void LoadDataPack(void)
{
    s32 result;
    u8 *pack;

    pack = (u8 *)g_pDataPackToLoad;
    if (pack != NULL)
    {
        g_pDataPackToLoad = NULL;
        pack[1] = 1;
        DataPack_LoadFile(M2C_FIELD(pack, s32 *, 4), pack + 0x14, g_dataPackToLoadType, M2C_FIELD(pack, s32 *, 8), &result);
    }
}
