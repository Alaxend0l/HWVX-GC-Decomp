#include "common.h"


void InterfaceManager_Init(void) {
    extern u8 g_interface[][0x170];
    extern s32 g_TargetArrow[7];
    extern void *g_pInterfaceToEnd;
    extern s32 g_numInterfaceObjectsAllocated;
    extern s32 g_numInterfaceObjectAllocations;
    extern s32 g_numInterfacesToRender;
    extern void *g_playerMessage1;
    extern void *g_playerMessage2;
    extern void *g_playerMessage3;
    u32 i;

    g_numInterfaceObjectsAllocated = 0;
    g_numInterfaceObjectAllocations = 0;
    g_numInterfacesToRender = 0;
    g_pInterfaceToEnd = NULL;
    for (i = 0; i < 0x12; i++) {
        *(void **)g_interface[i] = NULL;
        g_interface[i][0x7D] = i;
    }
    g_TargetArrow[0] = 0;
    g_playerMessage1 = NULL;
    g_TargetArrow[1] = 0;
    g_TargetArrow[3] = 0;
    g_TargetArrow[4] = -1;
    g_TargetArrow[5] = 0;
    g_playerMessage2 = NULL;
    g_playerMessage3 = NULL;
}

void InterfaceManager_SetRadarBlip(u16 arg0) {
    extern s32 g_radarTextureIndex;
    g_radarTextureIndex = (s32) arg0;
}

void InterfaceManager_SetInput(s32 index, s32 input) {
    typedef struct InterfaceSlot {
        u8 pad0[0x25C];
        u8 *object;
        u8 pad260[0x270 - 0x260];
        s32 child;
        u8 pad274[0x300 - 0x274];
    } InterfaceSlot;
    typedef struct Interface {
        u32 count;
        u8 pad4[0x84 - 0x4];
        u8 hasInput;
        u8 pad85[0x88 - 0x85];
        InterfaceSlot *slots;
        s32 input;
        u8 pad90[0x170 - 0x90];
    } Interface;
    extern Interface g_interface[];
    u32 i;

    g_interface[index].input = input;
    g_interface[index].hasInput = 1;
    for (i = 0; i < g_interface[index].count; i++) {
        switch (g_interface[index].slots[i].object[1]) {
        case 6:
        case 7:
            if (g_interface[index].slots[i].object[0] != 3) {
                InterfaceManager_SetInput(g_interface[index].slots[i].child, input);
            }
            break;
        }
    }
}

void InterfaceManager_ReleaseInterface(u32 index) {
    typedef struct {
        u32 index;
        u16 type;
        u16 id;
        s16 unk10;
        s16 unk12;
        u8 pad14[0x18];
        s32 unk2C;
        s8 unk30;
        u8 pad31[0x1F];
    } InterfaceMessage;
    typedef struct Interface {
        u32 count;
        u8 pad4[0x20 - 0x4];
        u8 *header;
        u8 pad24[0x7D - 0x24];
        u8 id;
        u8 pad7E[0x88 - 0x7E];
        u8 *slots;
        u8 pad8C[0x90 - 0x8C];
        void (*callback)(InterfaceMessage *);
        u8 pad94[0x98 - 0x94];
        u32 values[(0x170 - 0x98) / 4];
    } Interface;
    extern Interface g_interface[];
    extern s8 g_bUnloadMusicPack;
    extern char *g_curMusicPackName;
    extern s32 g_curMusicPackID;
    void InterfaceManager_RemoveInterfaceFromRender(u32 id);
    void InterfaceManager_RestoreMissionObjString(u8 *mgr);
    void InterfaceHasEnded(u32 handle);
    void Trigger_InterfaceEnd(void *header);
    void Gfx_FontManager_ReleaseFont(u8 font);
    void SoundSequence_Stop(s32 seq);
    void FileStreaming_ReleaseAllDataPacks(s32 type, s32 arg1);
    s32 FileStreaming_GetPackID(char *name, s32 type, s32 id);
    void SongManager_StopBKSong(void);
    u32 i;
    u8 *entry;
    u8 *slot;
    Interface *iface;

    if (index >= 0x12 || (iface = &g_interface[index])->count == 0) {
        return;
    }
    InterfaceManager_RemoveInterfaceFromRender(index);
    if (iface->callback != NULL) {
        InterfaceMessage msg;

        msg.index = iface->id;
        msg.type = 5;
        msg.id = 0;
        msg.unk10 = 0;
        msg.unk2C = 0;
        msg.unk12 = 0;
        msg.unk30 = 0;
        iface->callback(&msg);
    }
    for (i = 0; i < iface->count; i++) {
        slot = iface->slots + i * 0x300;
        if (*(u32 *)(slot + 0x26C) != 0 && (slot[0x243] == 4 || slot[0x243] == 3)) {
            *(u32 *)(slot + 0x26C) = 0;
            slot[0x244] = 0;
        } else if (slot[0x243] == 6) {
            if (&g_interface[*(u32 *)(slot + 0x270)] != iface) {
                InterfaceManager_ReleaseInterface(*(u32 *)(slot + 0x270));
            }
        } else if (slot[0x243] == 5) {
            Gfx_FontManager_ReleaseFont(slot[0x1B0]);
        } else if (slot[0x243] == 7) {
            if (*(u32 *)(slot + 0x2D0) != 0) {
                SoundSequence_Stop(*(s32 *)(slot + 0x2D4));
            }
            FileStreaming_ReleaseAllDataPacks(9, 1);
        }
        if (slot[0x245] == 0x34) {
            InterfaceManager_RestoreMissionObjString(slot);
        }
    }
    for (i = 0; i < **(u32 **)(iface->header + 0x1C); i++) {
        entry = *(u8 **)(*(u8 **)(iface->header + 0x1C) + 4) + i * 0x34;
        if (entry[0] == 0 && entry[2] != 0) {
            *(u32 *)(entry + 0xC) = iface->values[i];
        }
    }
    if (g_bUnloadMusicPack && g_curMusicPackName != NULL) {
        SongManager_StopBKSong();
        FileStreaming_GetPackID(g_curMusicPackName, 10, g_curMusicPackID);
        FileStreaming_ReleaseAllDataPacks(10, 1);
        g_curMusicPackName = NULL;
    }
    Trigger_InterfaceEnd(iface->header);
    InterfaceHasEnded(index);
    InterfaceManager_ReleaseInterfaceObjects(g_interface[index].slots);
    iface->count = 0;
}

void InterfaceManager_ReleaseAllInterfaces(void) {
    extern u8 g_interface[][0x170];
    extern s32 g_TargetArrow[7];
    extern void *g_pInterfaceToEnd;
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL) {
            InterfaceManager_ReleaseInterface(i);
        }
    }
    g_TargetArrow[0] = 0;
    g_pInterfaceToEnd = NULL;
    g_TargetArrow[1] = 0;
    g_TargetArrow[3] = 0;
    g_TargetArrow[4] = -1;
    g_TargetArrow[5] = 0;
}

void InterfaceManager_ReleaseAllUnpausedInterfaces(void) {
    extern u8 g_interface[][0x170];
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL && g_interface[i][0x83] == 0) {
            InterfaceManager_ReleaseInterface(i);
        }
    }
}

void InterfaceManager_SetCallBackRoutine(s32 index, void *callback) {
    extern u8 g_interface[][0x170];
    *(void **)(g_interface[index] + 0x90) = callback;
}

void InterfaceManager_SetInfoCallBackRoutine(s32 index, void *callback) {
    extern u8 g_interface[][0x170];
    *(void **)(g_interface[index] + 0x94) = callback;
}

s32 InterfaceManager_GetTotalActiveInterfaces(void) {
    extern u8 g_interface[][0x170];
    s32 count = 0;
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL) {
            count++;
        }
    }
    return count;
}

void InterfaceManager_PauseAllInterface(u8 pause) {
    extern u8 g_interface[][0x170];
    u32 i;

    for (i = 0; i < 0x12; i++) {
        g_interface[i][0x83] = pause;
    }
}

void InterfaceManager_SetOffset(s32 index, f32 x, f32 y) {
    extern u8 g_interface[][0x170];
    u8 *iface = g_interface[index];
    iface[0x82] = 1;
    *(f32 *)(iface + 0x60) = x;
    *(f32 *)(iface + 0x64) = y;
}

void InterfaceManager_SetObjectString(u8 *obj, char *str) {
    u32 strlen(const char *s);
    char *strncpy(char *dst, const char *src, u32 n);
    char *strcpy(char *dst, const char *src);
    u8 *info = *(u8 **)(*(u8 **)(obj + 0x258) + 4);

    obj[0x248] = 1;
    obj[0x84] = 1;
    obj[0x2C] = 1;
    obj[0x164] = 1;
    obj[0x194] = 1;
    *(u16 *)(obj + 0x1AA) = 0;
    *(u32 *)(obj + 0x1AC) = *(u32 *)(info + 0x14);
    *(u16 *)(obj + 0x1A8) = 0;
    obj[0x1C0] = 0;
    *(void **)(obj + 0x1A0) = NULL;
    *(char **)(obj + 0x1A4) = NULL;
    obj[0x1B1] = 0;
    if (str != NULL) {
        if (strlen(str) > 0x80) {
            strncpy((char *)(obj + 0x1C0), str, 0x80);
        } else {
            strcpy((char *)(obj + 0x1C0), str);
        }
        *(u16 *)(obj + 0x1A8) = strlen(str);
        if (*(s32 *)(obj + 0x1AC) == 0) {
            *(u16 *)(obj + 0x1AA) = *(u16 *)(obj + 0x1A8);
        }
        if (*(u8 *)(*(u8 **)(obj + 0x258) + 0x12) != 0) {
            obj[0x1B1] = 1;
        }
    }
}

void InterfaceManager_SetObjectText(u8 *obj, char **text) {
    u32 strlen(const char *s);
    char *str;

    if (text != NULL) {
        u8 *info = *(u8 **)(*(u8 **)(obj + 0x258) + 4);
        obj[0x248] = 1;
        obj[0x2C] = 1;
        obj[0x54] = 1;
        obj[0x84] = 1;
        obj[0x164] = 1;
        obj[0x194] = 1;
        *(u16 *)(obj + 0x1AA) = 0;
        *(u32 *)(obj + 0x1AC) = *(u32 *)(info + 0x14);
        *(u16 *)(obj + 0x1A8) = 0;
        obj[0x1C0] = 0;
        *(char ***)(obj + 0x1A0) = text;
        *(char **)(obj + 0x1A4) = NULL;
        obj[0x1B1] = 0;
        str = *text;
        if (str != NULL) {
            *(char **)(obj + 0x1A4) = str;
            *(u16 *)(obj + 0x1A8) = strlen(str);
            if (*(s32 *)(obj + 0x1AC) == 0) {
                *(u16 *)(obj + 0x1AA) = *(u16 *)(obj + 0x1A8);
            }
            if (*(u8 *)(*(u8 **)(obj + 0x258) + 0x12) != 0) {
                obj[0x1B1] = 1;
            }
        }
    }
}

void InterfaceManager_EndInterface(s32 index) {
    extern u8 g_interface[][0x170];
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    u8 *iface = g_interface[index];

    if (iface != NULL) {
        InterfaceManager_SetLoadState(iface, 1);
    }
}

void InterfaceManager_LinkThingToObject(s32 index, u32 id, void *thing) {
    typedef struct InterfaceSlot {
        u8 pad0[0x25C];
        u8 *object;
        u8 pad260[0x288 - 0x260];
        void *thing;
        u8 pad28C[0x300 - 0x28C];
    } InterfaceSlot;
    extern u8 g_interface[][0x170];
    u8 *iface = g_interface[index];
    u32 i;

    if (iface != NULL) {
        for (i = 0; i < *(u32 *)iface; i++) {
            if (id == *(u32 *)((*(InterfaceSlot **)(iface + 0x88))[i].object + 0x10)) {
                (*(InterfaceSlot **)(iface + 0x88))[i].thing = thing;
            }
        }
    }
}

void InterfaceManager_Heartbeat(void) {
    extern u8 g_interface[][0x170];
    extern s32 g_TargetArrow[7];
    extern f32 g_fDeltaTimeMS;
    extern s32 g_iDeltaTimeMS;
    void InterfaceManager_MoveInterface(void *iface, f32 time);
    typedef struct {
        u32 index;
        u16 type;
        u16 id;
        u8 unk8[0x48];
    } InterfaceMessage;
    f32 dt = g_fDeltaTimeMS;
    u32 i;

    for (i = 0; i < 0x12; i++) {
        if (*(void **)g_interface[i] != NULL && g_interface[i][0x83] == 0 && g_interface[i][0x85] == 0) {
            void (*callback)(InterfaceMessage *) = *(void (**)(InterfaceMessage *))(g_interface[i] + 0x90);

            if (callback != NULL) {
                InterfaceMessage msg;

                msg.index = g_interface[i][0x7D];
                msg.type = 0xC;
                msg.id = **(u16 **)(g_interface[i] + 0x20);
                callback(&msg);
            }
            InterfaceManager_MoveInterface(g_interface[i], dt);
        }
    }
    if (g_TargetArrow[5] > 0) {
        g_TargetArrow[5] -= g_iDeltaTimeMS;
    }
    if ((u8 *)g_TargetArrow[3] != NULL) {
        u8 *info = *(u8 **)(g_TargetArrow[3] + 0x94);

        if (info != NULL && *(u8 **)(info + 0x2C) != NULL) {
            g_TargetArrow[0] = (s32)(*(u8 **)(info + 0x2C) + 0x40);
        } else {
            g_TargetArrow[0] = g_TargetArrow[3] + 0x40;
        }
    }
}

void InterfaceManager_SetObjectReactive(u8 *iface, u32 onId, u32 offId) {
    u32 i;
    u32 j;
    u8 *obj;
    u8 *info;

    for (i = 0; i < *(u32 *)iface; i++) {
        obj = *(u8 **)(iface + 0x88) + i * 0x300;
        if (*(u8 *)(*(u8 **)(obj + 0x25C) + 8) != 0 && *(u32 *)(obj + 0x254) != 0) {
            if (**(u8 **)(*(u8 **)(obj + 0x25C) + 0x44) == onId) {
                InterfaceManager_SetObjectState(iface, obj, 1, 0);
            }
            if (**(u8 **)(*(u8 **)(obj + 0x25C) + 0x44) == offId) {
                InterfaceManager_SetObjectState(iface, obj, 2, 0);
                info = *(u8 **)(*(u8 **)(obj + 0x25C) + 0x44);
                for (j = 0; j < *(u32 *)(info + 0x10); j++) {
                    if (*(u32 *)(*(u8 **)(info + 0x14) + 4) != 0) {
                        obj[0x247] = 0;
                        *(u32 *)(obj + 0x274) = *(u32 *)(*(u8 **)(*(u8 **)(*(u8 **)(obj + 0x25C) + 0x44) + 0x14) + 4);
                        break;
                    }
                }
            }
        }
    }
}

void InterfaceManager_SetControllerConfigInputString(void) {

}

s32 ActionInterfacePlay(s32 *arg0) {
    M2C_UNK InterfaceManager_Create(s32, M2C_UNK, M2C_UNK);
    InterfaceManager_Create(*arg0, 0, 0);
    return 1;
}

s32 ActionHUDChangeMode(u32 *action) {
    extern u8 g_cameraManager[];
    void Player_RefreshInterface(void *player);
    void InterfaceManager_ReleaseInterface(u32 index);
    s32 i;

    for (i = 0; i < g_cameraManager[0x640]; i++) {
        if (action[1] == 1) {
            Player_RefreshInterface(M2C_FIELD(g_cameraManager + i * 0x190, void **, 0));
        } else {
            InterfaceManager_ReleaseInterface(M2C_FIELD(M2C_FIELD(g_cameraManager + i * 0x190, u8 **, 0), u32 *, 0x68));
            M2C_FIELD(M2C_FIELD(g_cameraManager + i * 0x190, u8 **, 0), s32 *, 0x68) = -1;
        }
    }
    return 1;
}

void InterfaceManager_DrawMap(u8 *obj, u8 *iface) {
    typedef struct RectVtx {
        f32 x;
        f32 y;
        f32 z;
        f32 pad;
        f32 s;
        f32 t;
        u32 color;
        u32 pad2;
    } RectVtx;
    typedef struct Rect {
        RectVtx v[4];
        u16 tex;
        u8 blendMode;
        u8 window;
    } Rect;
    typedef struct Colour {
        u8 r, g, b, a;
    } Colour;
    Rect *Gfx_Add2DRectangle(void);
    f32 ArcTan(f32 y, f32 x);
    void SinCosRad(f32 *s, f32 *c, f32 rad);
    Colour colour;
    f32 s;
    f32 c;
    Rect *rect;
    f32 x, z;
    f32 scaleX, scaleZ;
    f32 offsetX, offsetZ;
    f32 halfW, halfH;
    u8 *player;

    if (*(u8 **)(iface + 8) != NULL && *(u8 **)(*(u8 **)(iface + 8) + 0x2C) != NULL &&
        *(u32 *)((u8 *)World_GetGameData() + 0x78) != 0) {
        x = *(f32 *)(*(u8 **)(*(u8 **)(iface + 8) + 0x2C) + 0x40);
        z = *(f32 *)(*(u8 **)(*(u8 **)(iface + 8) + 0x2C) + 0x48);
    } else {
        return;
    }
    scaleX = *(f32 *)((u8 *)World_GetGameData() + 0x4);
    scaleZ = *(f32 *)((u8 *)World_GetGameData() + 0x8);
    offsetX = *(f32 *)((u8 *)World_GetGameData() + 0x58);
    offsetZ = *(f32 *)((u8 *)World_GetGameData() + 0x5C);
    halfW = *(f32 *)((u8 *)World_GetGameData() + 0x60);
    halfH = *(f32 *)((u8 *)World_GetGameData() + 0x64);
    rect = Gfx_Add2DRectangle();
    rect->blendMode = 2;
    rect->tex = *(u32 *)((u8 *)World_GetGameData() + 0x78);
    rect->v[0].x = *(f32 *)(obj + 0x60);
    rect->v[0].y = *(f32 *)(obj + 0x64);
    rect->v[1].x = *(f32 *)(obj + 0x60) + *(f32 *)(obj + 0x30);
    rect->v[1].y = *(f32 *)(obj + 0x64);
    rect->v[2].x = *(f32 *)(obj + 0x60) + *(f32 *)(obj + 0x30);
    rect->v[2].y = *(f32 *)(obj + 0x64) + *(f32 *)(obj + 0x34);
    rect->v[3].x = *(f32 *)(obj + 0x60);
    rect->v[3].y = *(f32 *)(obj + 0x64) + *(f32 *)(obj + 0x34);
    x = offsetX + x / scaleX;
    z = offsetZ + -z / scaleZ;
    player = *(u8 **)(*(u8 **)(iface + 8) + 0x2C);
    SinCosRad(&s, &c, 1.5707964f + ArcTan(*(f32 *)(player + 0x38), *(f32 *)(player + 0x30)));
    rect->v[0].s = x - halfW * c;
    rect->v[0].t = halfH * s + z;
    rect->v[1].s = halfW * s + x;
    rect->v[1].t = halfH * c + z;
    rect->v[2].s = halfW * c + x;
    rect->v[2].t = z - halfH * s;
    rect->v[3].s = x - halfW * s;
    rect->v[3].t = z - halfH * c;
    rect->v[0].color = *(u32 *)(obj + 0x24);
    rect->v[1].color = *(u32 *)(obj + 0x24);
    rect->v[2].color = *(u32 *)(obj + 0x24);
    rect->v[3].color = *(u32 *)(obj + 0x24);
    rect = Gfx_Add2DRectangle();
    rect->blendMode = 2;
    rect->tex = 0xFFFF;
    colour.r = colour.g = colour.b = 0xFF;
    colour.a = 0xFF;
    rect->v[0].x = *(f32 *)(obj + 0x30) / 2.0f + *(f32 *)(obj + 0x60) - 1.0f;
    rect->v[0].y = *(f32 *)(obj + 0x34) / 2.0f + *(f32 *)(obj + 0x64) - 1.0f;
    rect->v[1].x = 1.0f + (*(f32 *)(obj + 0x30) / 2.0f + *(f32 *)(obj + 0x60));
    rect->v[1].y = *(f32 *)(obj + 0x34) / 2.0f + *(f32 *)(obj + 0x64) - 1.0f;
    rect->v[2].x = 1.0f + (*(f32 *)(obj + 0x30) / 2.0f + *(f32 *)(obj + 0x60));
    rect->v[2].y = 1.0f + (*(f32 *)(obj + 0x34) / 2.0f + *(f32 *)(obj + 0x64));
    rect->v[3].x = *(f32 *)(obj + 0x30) / 2.0f + *(f32 *)(obj + 0x60) - 1.0f;
    rect->v[3].y = 1.0f + (*(f32 *)(obj + 0x34) / 2.0f + *(f32 *)(obj + 0x64));
    rect->v[0].s = 0.0f;
    rect->v[0].t = 1.0f;
    rect->v[1].s = 1.0f;
    rect->v[1].t = 1.0f;
    rect->v[2].s = 1.0f;
    rect->v[2].t = 0.0f;
    rect->v[3].s = 0.0f;
    rect->v[3].t = 0.0f;
    rect->v[0].color = *(u32 *)&colour;
    rect->v[1].color = *(u32 *)&colour;
    rect->v[2].color = *(u32 *)&colour;
    rect->v[3].color = *(u32 *)&colour;
}

s32 DropShip_IsChildInterfaceReady(s32 index) {
    typedef struct DropShipSlot {
        u8 pad0[0x243];
        u8 type;
        u8 pad244[0x270 - 0x244];
        s32 child;
        u8 pad274[0x300 - 0x274];
    } DropShipSlot;
    extern u8 g_interface[][0x170];
    u32 i;
    u8 *iface = g_interface[index];

    for (i = 0; i < *(u32 *)iface; i++) {
        if ((*(DropShipSlot **)(iface + 0x88))[i].type == 6) {
            u8 *child = g_interface[(*(DropShipSlot **)(iface + 0x88))[i].child];

            if (child[0x83] == 0 &&
                (*(u32 *)(child + 4) == 0 ||
                 (*(u32 *)(child + 4) == 1 && *(u32 *)(child + 0x10) != 0))) {
                return 0;
            }
        }
    }
    return 1;
}

#pragma push
#pragma use_lmw_stmw on
void DropShip_EndChildInterface(s32 index, u32 id)
{
    typedef struct DropShipSlot
    {
        u8 pad0[0x243];
        u8 type;
        u8 pad244[0x25C - 0x244];
        u8 *anim;
        u8 pad260[0x270 - 0x260];
        s32 child;
        u8 pad274[0x300 - 0x274];
    } DropShipSlot;
    extern u8 g_interface[][0x170];
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    u32 i;
    u8 *iface = g_interface[index];
    DropShipSlot *slot;

    for (i = 0; i < *(u32 *)iface; i++)
    {
        slot = &(*(DropShipSlot **)(iface + 0x88))[i];
        if (slot->type == 6 && *(u16 *)(slot->anim + 0xA) == id &&
            *(u32 *)(g_interface[slot->child] + 4) != 1)
        {
            InterfaceManager_SetLoadState(g_interface[slot->child], 1);
        }
    }
}
#pragma pop
void DropShip_EndInterface(s32 index) {
    extern u8 g_interface[][0x170];
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    void InterfaceManager_MoveInterface(void *iface, f32 time);
    u8 *iface = g_interface[index];

    *(u16 *)(*(u32 *)(*(u32 *)(iface + 0x1C) + 8) + 6) = 8;
    if (*(u32 *)(iface + 4) != 1) {
        InterfaceManager_SetLoadState(iface, 1);
        InterfaceManager_MoveInterface(iface, 0.0f);
    }
}

void InterfaceManager_SetMissionObjectiveText(u8 *obj, u8 **data) {
    u32 strlen(const char *s);
    char *strcpy(char *dst, const char *src);
    int sprintf(char *buf, const char *fmt, ...);
    f32 GetCodeVariable(s32 var, u8 *thing);
    char **text;
    char buf[16];
    char *str;
    s32 i;
    s32 j;
    s32 len;

    if (*(u8 **)(data[1] + 0xC) == NULL) {
        if (*(char ***)(data[1] + 0x14) != NULL) {
            text = *(char ***)(data[1] + 0x14);
        } else {
            text = *(char ***)(data[1] + 0x18);
        }
    } else if (*(f32 *)(*(u8 **)(data[1] + 0xC) + 4)) {
        text = *(char ***)(data[1] + 0x18);
    } else {
        text = *(char ***)(data[1] + 0x14);
    }
    if (text == NULL) {
        return;
    }
    if (data[1][0] == 0) {
        InterfaceManager_SetObjectText(obj, text);
        return;
    }
    if (data[1][0] == 1) {
        sprintf(buf, "%d", (u32)*(f32 *)(*(u8 **)(data[1] + 0x10) + 4));
    } else if (data[1][0] == 2) {
        u32 ms = *(f32 *)(*(u8 **)(data[1] + 0x10) + 8);
        u32 secs = ms / 1000;
        u32 mins = secs / 60;
        secs %= 60;
        sprintf(buf, "%02d:%02d", mins, secs);
    } else if (data[1][0] == 3) {
        u32 *var = *(u32 **)(data[1] + 0x10);
        u32 value;
        if (var[0] < 10) {
            value = GetCodeVariable(var[0], data[0]);
        } else {
            value = GetCodeVariable(var[0], (u8 *)var[1]);
        }
        sprintf(buf, "%d", value);
    }
    str = *text;
    len = strlen(str);
    j = strlen(buf);
    if (len != strlen((char *)(obj + 0x290))) {
        strcpy((char *)(obj + 0x290), str);
    } else {
        strcpy(str, (char *)(obj + 0x290));
    }
    j--;
    for (i = len - 1; i >= 0; i--) {
        if (str[i] == '#') {
            if (j >= 0) {
                str[i] = buf[j--];
            } else {
                str[i] = ' ';
            }
        }
    }
    InterfaceManager_SetObjectText(obj, text);
}

void InterfaceManager_SetPlayerMessage(void *message, u32 player) {
    extern void *g_playerMessage1;
    extern void *g_playerMessage2;
    extern void *g_playerMessage3;

    if (player == 0) {
        g_playerMessage1 = message;
    } else if (player == 1) {
        g_playerMessage2 = message;
    } else if (player == 2) {
        g_playerMessage3 = message;
    }
}

