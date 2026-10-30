#include "common.h"


s8 Player_IsHuman(void *player) {
    extern void Player_DefaultControlReader(void);
    s32 isHuman = 0;

    if (player != NULL && *(void **)((u8 *)player + 0x20) == Player_DefaultControlReader) {
        isHuman = 1;
    }
    return isHuman;
}

void Player_Init(void *player, s32 arg1);
void PlayerHuman_Init(void *player) {
    typedef struct PlayerHuman {
        u8 pad0[0x6C];
        u32 flags;
        u8 pad70[0xC8 - 0x70];
        s32 wheelChannel;
        u8 padCC[0x114 - 0xCC];
        void *bindings;
        u8 bindingData[4];
    } PlayerHuman;
    void PlayerHuman_SetDefaultControls(void *player, s32 arg);
    PlayerHuman *human = player;

    Player_Init(player, 0);
    human->wheelChannel = 0;
    PlayerHuman_SetDefaultControls(player, 0);
    human->flags = 0;
    human->bindings = human->bindingData;
}

void Player_Init(void *player, s32 arg1) {
    s32 i;

    Control_ClearControl(player);
    M2C_FIELD(player, u8 *, 0x40) = 0;
    M2C_FIELD(player, u8 *, 0x31) = 0;
    M2C_FIELD(player, s32 *, 0x3C) = 0;
    M2C_FIELD(player, s32 *, 0x68) = -1;
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    memset((u8 *)player + 0x70, 0, 0x38);
    M2C_FIELD(player, u8 *, 0x71) = 1;
    M2C_FIELD(player, s32 *, 0x34) = 0;
    for (i = 0; i < 8; i++) {
        ((s16 *)((u8 *)player + 0x94))[i] = 30;
    }
    M2C_FIELD(player, s16 *, 0xA2) = 0;
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    M2C_FIELD(player, s16 *, 0xAC) = -1;
    M2C_FIELD(player, s16 *, 0xAE) = -1;
}

void Player_SetName(void *player, s32 port, const char *name) {
    char *strcpy(char *dst, const char *src);
    s32 Input_GetPlayerDevice(s32 port);

    strcpy((char *)player + 0x40, name);
    M2C_FIELD(player, s32 *, 0x60) = port;
    if (Player_IsHuman(player)) {
        M2C_FIELD(player, s32 *, 0xC8) = Input_GetPlayerDevice(M2C_FIELD(player, s32 *, 0x60));
    }
}

void Player_RefreshInterface(void *arg0) {
    s32 InterfaceManager_Create(u8 *script, void *player, s32 arg2);
    void InterfaceManager_ReleaseInterface(u32 index);
    void InterfaceManager_SetOffset(s32 index, f32 x, f32 y);
    typedef struct Thing {
        u8 pad0[0x8C];
        void (**vtable)();
    } Thing;
    typedef struct Player {
        u8 pad0[0x2C];
        Thing *thing;
        u8 pad30[0x68 - 0x30];
        u32 interfaceIndex;
        u32 flags;
        u8 pad70;
        u8 nameDirty;
    } Player;
    struct {
        u32 unk0;
        u8 *script;
        u32 x;
        u32 y;
        u8 unk10[0xC0];
    } info;
    Player *player = arg0;
    Thing *thing;

    if (player->thing != NULL && (player->flags & 0x10)) {
        memset(&info, 0, sizeof(info));
        player->flags |= 0x200;
        thing = player->thing;
        ((void (*)(Thing *, void *))thing->vtable[8])(thing, &info);
        if (player->interfaceIndex < 0xFFFFFFFF) {
            InterfaceManager_ReleaseInterface(player->interfaceIndex);
            player->interfaceIndex = 0xFFFFFFFF;
        }
        if (info.script != NULL) {
            player->interfaceIndex = InterfaceManager_Create(info.script, player, 0);
            InterfaceManager_SetOffset(player->interfaceIndex, info.x, info.y);
            player->nameDirty = 1;
        }
        player->flags &= ~0x200;
    }
}

void Player_ReleaseInterface(void *player) {
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 index = M2C_FIELD(player, u32 *, 0x68);

    if (index != 0xFFFFFFFF) {
        InterfaceManager_ReleaseInterface(index);
    }
}

extern u8 g_carAVTable[];
extern u8 g_creatureVTable[];
void CarA_SetSlavePlayers(void *car);
void Thing_SetSlavePlayers(void *thing);
void AI_Car_Reset(void *player);
void Player_SetThing(void *player, void *thing) {
    s32 isCreature;

    *(void **)((u8 *)player + 0x2C) = thing;
    if (thing != NULL) {
        *(void **)(*(u8 **)((u8 *)player + 0x2C) + 0x94) = player;
        *(u32 *)((u8 *)player + 0x38) = *(u32 *)((u8 *)thing + 0xA8);
        if (*(void **)((u8 *)thing + 0x8C) == g_carAVTable) {
            *(u32 *)((u8 *)player + 0x3C) = *(u32 *)((u8 *)thing + 0x3BC);
            CarA_SetSlavePlayers(thing);
            if (!Player_IsHuman(player)) {
                AI_Car_Reset(player);
            }
        } else if (*(void **)((u8 *)thing + 0x8C) == g_creatureVTable) {
            *(u32 *)((u8 *)player + 0x3C) = *(u32 *)((u8 *)thing + 0xB8);
            Thing_SetSlavePlayers(thing);
        }
        if (thing != NULL) {
            Player_RefreshInterface(player);
        }
    }
}

s32 Player_InputHeartbeat(void *arg0) {
    return M2C_FIELD(arg0, s8 (**)(), 0x20)() != 0;
}

void Player_GetInterfaceInfo(void *arg0, void *arg1) {
    int sprintf(char *buf, const char *fmt, ...);
    typedef struct Thing {
        u8 pad0[0x8C];
        void (**vtable)();
    } Thing;
    typedef struct Player {
        u8 pad0[0x2C];
        Thing *thing;
        u8 attached;
        u8 pad31[0x71 - 0x31];
        u8 nameDirty;
        u8 pad72[0x7C - 0x72];
        s32 score;
    } Player;
    typedef struct InterfaceInfo {
        u8 pad0[0x74];
        char text[0xC];
        u8 state;
    } InterfaceInfo;
    Player *player = arg0;
    InterfaceInfo *info = arg1;
    Thing *thing;

    if (player->thing != NULL) {
        info->state = 0;
        thing = player->thing;
        ((void (*)(Thing *, InterfaceInfo *))thing->vtable[8])(thing, info);
    } else {
        info->state = 2;
        if (player->attached != 0) {
            info->state = 1;
            player->attached = 0;
        }
    }
    if (player->nameDirty != 0) {
        sprintf(info->text, "%d", player->score);
        player->nameDirty = 0;
    }
}

void Player_DetachThingDeath(void *arg0) {
    void *temp_r5;

    temp_r5 = M2C_FIELD(arg0, void **, 0x2C);
    M2C_FIELD(arg0, void **, 0x2C) = NULL;
    M2C_FIELD(arg0, s8 *, 0x30) = 1;
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, s32 *, 0x94) = 0;
    }
}

void PlayerHuman_Heartbeat(void *player) {
    void ForceFeedback_GetForce(void *ff, u8 *a, u8 *b, s32 *c);
    void Input_SetAlternativeForceFeedback(s32 input, u8 a, u8 b);
    void Player_Heartbeat(void *player);
    s32 c;
    u8 a;
    u8 b;
    void *car;

    car = M2C_FIELD(player, void **, 0x2C);
    if (car != NULL && !(M2C_FIELD(car, u32 *, 0x74) & 2)) {
        ForceFeedback_GetForce((u8 *)car + 0x40, &a, &b, &c);
        Input_SetAlternativeForceFeedback(M2C_FIELD(player, s32 *, 0xC8), a, b);
    }
    Player_Heartbeat(player);
}

#pragma push
#pragma use_lmw_stmw on
void Player_Heartbeat(void *arg0) {
    typedef struct NavPoint {
        Vector4 pos;
        u8 pad10[0x3C];
        s32 next;
        u8 pad50;
        u8 type;
    } NavPoint;
    typedef struct NavPath {
        u8 pad0[0x40];
        f32 length;
    } NavPath;
    typedef struct HBVehicle HBVehicle;
    typedef struct HBVehicleVTable {
        u8 pad0[0x2C];
        s32 (*getNearestNavPoint)(HBVehicle *thing);
        u8 pad30[0x10];
        void *(*getOrientedCollision)(HBVehicle *thing);
    } HBVehicleVTable;
    typedef struct HBItem {
        u8 pad0[0x30];
        struct {
            u8 pad0[0x20];
            s32 count;
        } *info;
    } HBItem;
    typedef struct HBTileCar {
        Vector4 pos;
        f32 rotX;
        f32 rotY;
        f32 rotZ;
        u8 pad1C[8];
        void *aiScripts;
        HBVehicle *creature;
        u32 mode;
        u8 pad30[0x24];
        HBItem *item;
        u8 pad58[4];
        s8 respawn;
    } HBTileCar;
    struct HBVehicle {
        u8 pad0[0x40];
        Vector4 pos;
        u8 pad50[0x24];
        u32 flags;
        u8 pad78[0x14];
        HBVehicleVTable *vtable;
        HBTileCar *tileData;
        u8 pad94[0x344 - 0x94];
        void *slaves;
        u8 pad348[0x55C - 0x348];
        s32 navTimer;
        f32 pathLength;
        f32 navDist;
        NavPoint *navPath[7];
    };
    typedef struct HBPlayer {
        u8 pad0[0xC];
        u32 buttons;
        u8 pad10[0xB];
        u8 active;
        u8 pad1C[0x10];
        HBVehicle *vehicle;
        u8 pad30;
        u8 cameraIndex;
        u8 pad32[2];
        u32 deadTime;
        u32 type;
        void *aiScripts;
        u8 pad40[0x2C];
        u32 flags;
        u8 pad70[0x38];
        HBTileCar *tileCar;
        s16 deathNavPoint;
        s16 respawnNavPoint;
    } HBPlayer;
    typedef struct HBCreateParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        void *aiScripts;
        f32 unk34;
        u32 unk38;
        HBPlayer *player;
    } HBCreateParams;
    extern s32 g_iDeltaTimeMS;
    extern s32 g_TargetArrow[7];
    extern s8 g_bWaitingForRoundReset;
    extern Matrix4 g_identity;
    u32 NavHolder_GetTotalNavPoints(void);
    s32 InterfaceManager_GetTargetArrowNearestNavPoint(void);
    void NavHolder_UseMainRouteOnly(void);
    NavPath *NavHolder_FindPath(s32 from, s32 to);
    void NavHolder_FillPointerArrayPath(NavPath *path, NavPoint **out, s32 count);
    NavPoint *NavHolder_GetNavPoint(s32 index);
    s32 NavHolder_GetFirstWayPoint(void);
    s32 RandomAdd_s32(s32 base, s32 range);
    f32 LinearPointTest(Vector4 *p, NavPoint *a, NavPoint *b);
    f32 PSVECSquareDistance(void *a, void *b);
    f32 Sqrt(f32 x);
    void DistanceXYZ(Vector4 *a, void *b, f32 *out);
    u32 GameSettings_GetCodeVariable(s32 var, s32 arg);
    void PlayerManager_RemovePlayer(void *player);
    void *memcpy(void *dst, const void *src, u32 n);
    void PSVECSubtract(void *a, void *b, void *out);
    f32 PSVECSquareMag(void *v);
    void PSVECNormalize(void *v, void *out);
    void PSVECCrossProduct(void *a, void *b, void *out);
    void SubMatrixToQuaternion(void *mtx, Vector4 *quat);
    void SetOrientation(void *mtx, f32 y, f32 x, f32 z);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    s8 OBoxHitsTerrainAndObjectsZeroTime(void *obox, void *thing, s32 arg);
    u32 ThingManager_GetActiveCars(HBVehicle **cars, s32 max);
    HBTileCar *World_GetTileCar(u32 index);
    u32 World_GetTotalTiles_Car(void);
    void *ThingManagerList_GetByType(void *list, s32 type);
    void WildCardRack_JettisonAll(void *rack);
    void WildCardRack_Add(void *rack, HBItem *item, s32 count);
    HBPlayer *player = arg0;
    HBTileCar *tile;
    HBVehicle *thing;
    NavPoint *b;
    NavPath *path;
    Vector4 *pos;
    NavPoint *a;
    s32 i;
    f32 left;
    f32 dist;
    HBVehicle *cars[8];
    HBCreateParams params __attribute__((aligned(16)));
    Vector4 mtx[4] __attribute__((aligned(16)));

    thing = player->vehicle;
    if (player->buttons & 0x1000) {
        player->cameraIndex++;
    }
    if (player->vehicle == NULL) {
        player->deadTime += g_iDeltaTimeMS;
    } else if (player->type == 9 && (void *)g_TargetArrow[0] != NULL) {
        thing->navTimer -= g_iDeltaTimeMS;
        if (thing->navTimer <= 0 && NavHolder_GetTotalNavPoints() != 0) {
            s32 dest;
            s32 cur;

            dest = InterfaceManager_GetTargetArrowNearestNavPoint();
            cur = thing->vtable->getNearestNavPoint(thing);
            if (dest != -1 && cur != -1) {
                NavHolder_UseMainRouteOnly();
                path = NavHolder_FindPath(cur, dest);
                NavHolder_FillPointerArrayPath(path, thing->navPath, 7);
                if (path != NULL) {
                    thing->pathLength = path->length;
                    thing->navTimer = RandomAdd_s32(900, 100);
                }
            }
        }
        left = thing->pathLength;
        pos = &thing->pos;
        for (i = 0; i < 6; i++) {
            a = thing->navPath[i];
            b = thing->navPath[i + 1];
            if (a == NULL || b == NULL) {
                break;
            }
            if (LinearPointTest(pos, a, b) < 0.0f) {
                break;
            }
            left -= Sqrt(PSVECSquareDistance(a, b));
        }
        if (a != NULL && b != NULL) {
            DistanceXYZ(pos, a, &dist);
            thing->navDist = dist + left;
            if (Player_IsHuman(player)) {
                while (dist < 300.0f) {
                    a = thing->navPath[i];
                    b = thing->navPath[i + 1];
                    if (a == NULL || b == NULL) {
                        if ((NavPoint *)g_TargetArrow[1] != NULL) {
                            g_TargetArrow[2] = g_TargetArrow[1];
                            g_TargetArrow[6] = 0;
                            g_TargetArrow[1] = 0;
                        }
                        break;
                    }
                    dist += Sqrt(PSVECSquareDistance(a, b));
                    if (++i >= 6) {
                        break;
                    }
                }
                if ((NavPoint *)g_TargetArrow[1] != b) {
                    g_TargetArrow[2] = g_TargetArrow[1];
                    g_TargetArrow[6] = 0;
                    g_TargetArrow[1] = (s32)b;
                }
            }
        } else {
            DistanceXYZ(&thing->pos, (void *)g_TargetArrow[0], &thing->navDist);
            if (Player_IsHuman(player)) {
                if ((NavPoint *)g_TargetArrow[1] != NULL) {
                    g_TargetArrow[2] = g_TargetArrow[1];
                    g_TargetArrow[6] = 0;
                    g_TargetArrow[1] = 0;
                }
            }
        }
    } else if (GameSettings_GetCodeVariable(0x12D, 0) >= 3) {
        thing->navTimer -= g_iDeltaTimeMS;
        if (thing->navTimer <= 0 && NavHolder_GetTotalNavPoints() != 0) {
            s32 dest;
            s32 cur;

            cur = thing->vtable->getNearestNavPoint(thing);
            dest = NavHolder_GetFirstWayPoint();
            while (NavHolder_GetNavPoint(dest)->type == 1) {
                dest = NavHolder_GetNavPoint(dest)->next;
            }
            if (dest != -1 && cur != -1) {
                NavHolder_UseMainRouteOnly();
                path = NavHolder_FindPath(cur, dest);
                NavHolder_FillPointerArrayPath(path, thing->navPath, 7);
                if (path != NULL) {
                    thing->pathLength = path->length;
                    thing->navTimer = RandomAdd_s32(900, 100);
                }
            }
        }
    }
    if (player->vehicle == NULL && player->deadTime > 2000 && !(player->flags & 1) && !g_bWaitingForRoundReset &&
        player->active && player->aiScripts != NULL && player->type == 9) {
        tile = player->tileCar;
        if (tile != NULL && !tile->respawn) {
            PlayerManager_RemovePlayer(player);
        } else if (tile != NULL) {
            params.player = player;
            params.aiScripts = tile->aiScripts;
            params.unk34 = 0.0f;
            if (GameSettings_GetCodeVariable(0x12D, 0) >= 3 && player->deathNavPoint != -1) {
                u8 attempt = 0;

                params.pos = NavHolder_GetNavPoint(player->deathNavPoint)->pos;
                memcpy(mtx, &g_identity, sizeof(mtx));
                if (player->respawnNavPoint != -1) {
                    a = NavHolder_GetNavPoint(player->deathNavPoint);
                    PSVECSubtract(NavHolder_GetNavPoint(player->respawnNavPoint), a, &mtx[2]);
                    if (PSVECSquareMag(&mtx[2])) {
                        PSVECNormalize(&mtx[2], &mtx[2]);
                    }
                    mtx[1].x = 0.0f;
                    mtx[1].y = 1.0f;
                    mtx[1].z = 0.0f;
                    mtx[1].w = 0.0f;
                    PSVECCrossProduct(&mtx[1], &mtx[2], &mtx[0]);
                    PSVECCrossProduct(&mtx[2], &mtx[0], &mtx[1]);
                    if (PSVECSquareMag(&mtx[0])) {
                        PSVECNormalize(&mtx[0], &mtx[0]);
                    }
                    if (PSVECSquareMag(&mtx[1])) {
                        PSVECNormalize(&mtx[1], &mtx[1]);
                    }
                }
                player->respawnNavPoint = -1;
                player->deathNavPoint = -1;
                params.pos.y += 10.0f;
                SubMatrixToQuaternion(mtx, &params.quat);
                thing = ThingManager_DARYL_CreateThing(9, &params);
                do {
                    thing->pos = params.pos;
                    thing->flags |= 0x80;
                    if (OBoxHitsTerrainAndObjectsZeroTime(thing->vtable->getOrientedCollision(thing), thing, 0)) {
                        switch (attempt++) {
                        case 0:
                            params.pos.z -= 10.0f;
                            break;
                        case 1:
                            params.pos.x += 10.0f;
                            break;
                        case 2:
                            params.pos.z += 10.0f;
                            break;
                        case 3:
                            params.pos.z += 10.0f;
                            break;
                        case 4:
                            params.pos.x -= 10.0f;
                            break;
                        case 5:
                            params.pos.x -= 10.0f;
                            break;
                        case 6:
                            params.pos.z -= 10.0f;
                            break;
                        case 7:
                            params.pos.z -= 10.0f;
                            break;
                        default:
                            attempt = 0;
                            params.pos.x += 10.0f;
                            params.pos.z += 10.0f;
                            thing->pos = params.pos;
                            thing->flags |= 0x80;
                            break;
                        }
                    } else {
                        attempt = 0;
                    }
                } while (attempt != 0);
            } else if (GameSettings_GetCodeVariable(0x12D, 0) == 2) {
                HBTileCar *car;
                HBTileCar *bestTile = NULL;
                u32 count;
                u8 j;
                u8 k;
                f32 nearest;
                f32 best;

                count = ThingManager_GetActiveCars(cars, 8);
                best = 0.0f;
                for (j = 0; j < World_GetTotalTiles_Car(); j++) {
                    car = World_GetTileCar(j);
                    if (car->mode == 1) {
                        nearest = 1e10f;
                        for (k = 0; k < count; k++) {
                            f32 d = PSVECSquareDistance(&cars[k]->pos, car);
                            if (nearest > d) {
                                nearest = d;
                            }
                        }
                        if (best < nearest) {
                            best = nearest;
                            bestTile = car;
                        }
                    }
                }
                params.pos = bestTile->pos;
                params.pos.y += 10.0f;
                SetOrientation(mtx, bestTile->rotY, bestTile->rotX, bestTile->rotZ);
                SubMatrixToQuaternion(mtx, &params.quat);
                thing = ThingManager_DARYL_CreateThing(9, &params);
            } else {
                params.pos = tile->pos;
                params.pos.y += 10.0f;
                SetOrientation(mtx, tile->rotY, tile->rotX, tile->rotZ);
                SubMatrixToQuaternion(mtx, &params.quat);
                thing = ThingManager_DARYL_CreateThing(9, &params);
            }
            player->deadTime = 0;
            tile->creature = thing;
            Player_SetThing(player, thing);
            thing->tileData = tile;
            if (player->tileCar->item != NULL) {
                void *rack = ThingManagerList_GetByType(&player->tileCar->creature->slaves, 8);

                WildCardRack_JettisonAll(rack);
                WildCardRack_Add(rack, player->tileCar->item, player->tileCar->item->info->count);
            }
        }
    }
}
#pragma pop
void PlayerHuman_SetDefaultControls(void *arg0, s32 index) {
    typedef struct { u8 unk0; u8 unk1; u8 keys[14]; } KeyBinding;
    typedef struct { u8 pad0[0xB8]; u8 keys[14]; u8 unkC6; u8 unkC7; } PlayerHuman;
    extern KeyBinding g_keyBinding[9];
    void *memset(void *dst, int c, u32 n);
    PlayerHuman *player = arg0;
    u16 i;

    memset(player->keys, 0, 0x10);
    for (i = 0; i < 14; i++) {
        player->keys[i] = 0xFF;
    }
    index++;
    for (i = 0; i < 14; i++) {
        player->keys[g_keyBinding[0].keys[i]] = g_keyBinding[index].keys[i];
    }
    player->unkC6 = g_keyBinding[index].unk0;
    player->unkC7 = g_keyBinding[index].unk1;
}

void PlayerHuman_SetControlBinding(void *arg0, void *arg1) {
    typedef struct { u8 unk0; u8 unk1; u8 keys[14]; } KeyBinding;
    typedef struct { u8 pad0[0xB8]; u8 keys[14]; u8 unkC6; u8 unkC7; } PlayerHuman;
    extern KeyBinding g_keyBinding[9];
    void *memset(void *dst, int c, u32 n);
    PlayerHuman *player = arg0;
    KeyBinding *binding = arg1;
    u16 i;

    memset(player->keys, 0, 0x10);
    for (i = 0; i < 14; i++) {
        player->keys[i] = 0xFF;
    }
    for (i = 0; i < 14; i++) {
        player->keys[g_keyBinding[0].keys[i]] = binding->keys[i];
    }
    player->unkC6 = binding->unk0;
    player->unkC7 = binding->unk1;
}

u8 Input_KeyDown(s32 device, u8 key);
s8 Input_KeyPress(s32 device, u8 key);

typedef struct PlayerHumanKeys {
    u8 pad0[0xB8];
    u8 keys[14];
    u8 stickMode;
    u8 substickMode;
    s32 device;
} PlayerHumanKeys;

static inline u8 PlayerHuman_KeyDown(PlayerHumanKeys *player, s32 key) {
    return Input_KeyDown(player->device, player->keys[key]);
}

static inline s8 PlayerHuman_KeyPress(PlayerHumanKeys *player, s32 key) {
    return Input_KeyPress(player->device, player->keys[key]);
}

s32 Player_NULL(void) {
    return 0;
}

void PlayerHuman_EvaluateCombo(u8 *player) {
    extern s8 g_bFreezeWorld;
    void TimeHandler_SetSlowMotion(void);
    void CameraManager_ReturnAllToPlayer(void);
    u32 *combo = (u32 *)(player + 0xD8);

    if (!(*(u32 *)(player + 0x6C) & 0x20)) {
        if (*(u32 *)(player + 0xCC) == 13 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x800) &&
            (combo[3] & 4) && (combo[4] & 4) && (combo[5] & 8) && (combo[6] & 8) && (combo[7] & 0x20) &&
            (combo[8] & 0x20) && (combo[9] & 0x800) && (combo[10] & 0x800) && (combo[11] & 0x800)) {
            *(u32 *)(player + 0x6C) |= 0x20;
        }
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 4) && (combo[2] & 4)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 4) && (combo[2] & 8)) {
        player[0x32] = 7;
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 4) && (combo[2] & 0x10)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x10)) {
        player[0x32] = 3;
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x20)) {
        TimeHandler_SetSlowMotion();
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x10) && (combo[2] & 0x10)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x10) && (combo[2] & 0x20)) {
        CameraManager_ReturnAllToPlayer();
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 8) && (combo[2] & 8)) {
    } else if (*(u32 *)(player + 0xCC) == 6 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x800) &&
               (combo[3] & 0x20) && (combo[4] & 0x20)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x20) && (combo[2] & 0x20)) {
        g_bFreezeWorld = !g_bFreezeWorld;
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 4) && (combo[1] & 8) && (combo[2] & 8)) {
        player[0x32] = 8;
    }
}

s8 Input_KeyPress(s32 device, u8 key);
u8 Input_KeyDown(s32 device, u8 key);
s8 Wheel_IsConnected(s32 device);
s8 Input_GetAnalog(s32 chan, s16 *x, s16 *y, s16 *cx, s16 *cy);
s8 Timer_Interrupt300(void);
void Dprintf(const char *fmt, ...);
s32 Playback_GetElement(void);
void Playback_PutElement(s32 element);
s32 Player_DefaultControlReader(void *arg0) {
    typedef struct Thing {
        u8 pad0[0x24];
        f32 speed;
    } Thing;
    typedef struct PlayerHuman {
        s8 steer;
        s8 accel;
        s8 steerHi;
        s8 accelHi;
        u8 pad4[0xC - 0x4];
        u32 pressed;
        u8 down[14];
        u8 pad1E[0x2C - 0x1E];
        Thing *thing;
        u8 pad30[0x6C - 0x30];
        u32 flags;
        u8 pad70[0xB8 - 0x70];
        u8 keys[14];
        u8 stickMode;
        u8 substickMode;
        s32 device;
        u32 comboCount;
        u8 padD0[4];
        u32 combo[16];
    } PlayerHuman;
    extern u32 g_playbackStatus;
    PlayerHuman *player = arg0;
    s16 stickX;
    s16 substickX;
    s16 stickY;
    s16 substickY;
    u32 i;

    Control_ClearControl(player);
    if (player->flags & 1) {
        player->down[3] = 0xFF;
        player->down[2] = 0xFF;
        if (player->thing != NULL && player->thing->speed < 0.0f) {
            player->steer = -128;
        }
        return 1;
    }
    if (g_playbackStatus == 2) {
        u32 element = Playback_GetElement();
        player->steer = element;
        player->steerHi = element >> 16;
        element = Playback_GetElement();
        player->accel = element;
        player->accelHi = element >> 16;
    } else {
        Control_ClearControl(player);
        for (i = 0; i < 14; i++) {
            player->down[i] = PlayerHuman_KeyDown((PlayerHumanKeys *)player, i);
            if (PlayerHuman_KeyPress((PlayerHumanKeys *)player, i)) {
                player->pressed |= 1 << i;
            }
        }
        if (player->down[1]) {
            if (player->pressed) {
                player->combo[player->comboCount++] = player->pressed;
                if (player->comboCount >= 16) {
                    player->comboCount = 0;
                }
                PlayerHuman_EvaluateCombo((u8 *)player);
            }
        } else {
            player->comboCount = 0;
        }
        if (!Wheel_IsConnected(player->device) && player->down[2]) {
            player->down[2] = 0xFF;
        }
        if (Input_GetAnalog(player->device, &stickX, &stickY, &substickX, &substickY)) {
            u8 mode = player->stickMode;
            switch (mode) {
            case 0:
                break;
            case 1:
                player->steer = stickX;
                player->accel = stickY;
                break;
            case 2:
                player->steer = stickX;
                player->accel = stickY;
                if (player->accel > 0) {
                    player->down[2] = player->accel << 1;
                } else {
                    player->down[3] = ~player->accel << 1;
                }
                break;
            case 3:
                player->steer = stickX;
                player->accel = stickY;
                if (player->accel > 0) {
                    player->down[2] = player->accel << 1;
                } else {
                    player->down[3] = ~player->accel << 1;
                }
                break;
            case 4:
                player->steer = stickX;
                player->accel = stickY;
                if (player->accel < 0) {
                    player->down[3] = ~player->accel << 1;
                }
                break;
            case 5:
                if (Timer_Interrupt300() && stickY > 0) {
                    player->down[12] = 0xFF;
                }
                break;
            case 6:
                if (Timer_Interrupt300() && stickX > 0) {
                    player->down[1] = 0xFF;
                }
                break;
            default:
                Dprintf("%d\n", mode);
                break;
            }
            switch (player->substickMode) {
            case 0:
                break;
            case 1:
                player->steer = substickX;
                break;
            case 2:
                player->accel = substickY;
                if (player->accel >= 0) {
                    if (!player->down[2]) {
                        player->down[2] = player->accel << 1;
                    }
                } else {
                    if (!player->down[3]) {
                        player->down[3] = ~player->accel << 1;
                    }
                }
                break;
            case 3:
                player->steer = substickX;
                player->accel = substickY;
                if (player->accel > 0) {
                    if (!player->down[2]) {
                        player->down[2] = player->accel << 1;
                    }
                } else {
                    if (!player->down[3]) {
                        player->down[3] = ~player->accel << 1;
                    }
                }
                break;
            case 4:
                player->steer = substickX;
                if (substickY < 0 && !player->down[3]) {
                    player->down[3] = ~substickY << 1;
                }
                break;
            case 5:
                if (Timer_Interrupt300()) {
                    if (substickY > 40 || substickY < -40 || substickX > 40 || substickX < -40) {
                        player->down[12] = 0xFF;
                        player->pressed |= 0x1000;
                    }
                }
                break;
            case 6:
                if (Timer_Interrupt300()) {
                    if (substickY > 40 || substickY < -40 || substickX > 40 || substickX < -40) {
                        player->down[1] = 0xFF;
                        player->pressed |= 2;
                    }
                }
                break;
            }
        }
        if (Input_KeyDown(player->device, player->keys[6])) {
            player->accel = 0x7F;
        }
        if (Input_KeyDown(player->device, player->keys[7])) {
            player->accel = -0x7F;
        }
        if (Input_KeyDown(player->device, player->keys[9])) {
            player->steer = -0x7F;
        }
        if (Input_KeyDown(player->device, player->keys[8])) {
            player->steer = 0x7F;
        }
        if (g_playbackStatus == 1) {
            Playback_PutElement((u16)player->steer + (player->steerHi << 16));
            Playback_PutElement((u16)player->accel + (player->accelHi << 16));
        }
    }
    return 1;
}

s32 Player_AIControlReader(s8 *ctrl) {
    extern u32 g_playbackStatus;
    s32 element;

    if (M2C_FIELD(ctrl, u32 *, 0x6C) & 1) {
        return 0;
    }
    if (g_playbackStatus == 2) {
        element = Playback_GetElement();
        ctrl[0] = element;
        ctrl[2] = (s8)(element & 0xFFFF0000) >> 16;
        element = Playback_GetElement();
        ctrl[1] = element;
        ctrl[3] = (s8)(element & 0xFFFF0000) >> 16;
    } else if (g_playbackStatus == 1) {
        Playback_PutElement(ctrl[0] + (ctrl[2] << 16));
        Playback_PutElement(ctrl[1] + (ctrl[3] << 16));
    }
    return 1;
}

s32 ActionVehicleChangeControl(u32 *action) {
    extern void Player_DefaultControlReader(void);
    void Input_SetForceFeedbackActive(s32 input, u8 active);
    u8 *GameSettings_GetControllerSettings(s32 index);
    u8 i;
    u8 *vehicle;
    u8 *player;
    s32 isHuman;

    for (i = 0; i < action[2]; i++) {
        vehicle = *(u8 **)(action[i + 3] + 0x28);
        if (vehicle == NULL) {
            continue;
        }
        player = *(u8 **)(vehicle + 0x94);
        if (player == NULL) {
            continue;
        }
        if (action[0] == 0x22) {
            isHuman = 0;
            if (player != NULL && *(void **)(player + 0x20) == Player_DefaultControlReader) {
                isHuman = 1;
            }
            if (isHuman) {
                Input_SetForceFeedbackActive(*(s32 *)(player + 0xC8), 0);
            }
            *(u32 *)(player + 0x6C) |= 1;
        } else {
            isHuman = 0;
            if (player != NULL && *(void **)(player + 0x20) == Player_DefaultControlReader) {
                isHuman = 1;
            }
            if (isHuman) {
                Input_SetForceFeedbackActive(*(s32 *)(player + 0xC8), *GameSettings_GetControllerSettings(*(s32 *)(player + 0x60)));
            }
            *(u32 *)(player + 0x6C) &= ~1;
        }
    }
    return 1;
}

