#include "common.h"

typedef struct WorldData
{
    void *world;
    s32 unk4;
    s32 unk8[4];
} WorldData;
extern WorldData g_worldData;


void InterfaceManager_Init(void) {
    extern u8 g_interface[][0x170];
    extern s32 g_TargetArrow[7];
    extern void *g_pInterfaceToEnd;
    extern s32 g_numInterfaceObjectsAllocated;
    extern s32 g_numInterfaceObjectAllocations;
void *memset(void *dst, s32 val, u32 n);
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

s32 InterfaceManager_Create(u8 *header, u8 *owner, void (*callback)(void *msg)) {
    typedef struct {
        s8 isVar;
        u8 pad1;
        u16 value;
    } CondValue;
    typedef struct {
        s32 id;
        CondValue *val;
    } CodeVar;
    typedef struct {
        u8 type;
        u8 pad1[3];
        s32 min;
        s32 max;
        s32 offset;
        CodeVar *data;
        u8 pad14[0x34 - 0x14];
    } Variable;
    typedef struct {
        u32 count;
        Variable *vars;
    } VariableList;
    typedef struct {
        u8 type;
        u8 pad1[3];
        u32 value;
        u8 pad8[0xC - 0x8];
    } Property;
    typedef struct {
        u16 id;
        u8 pad2[0xC - 0x2];
        u32 numObjects;
        u8 *objects;
        u32 numProps;
        Property *props;
        VariableList *vars;
        u8 pad20[0x34 - 0x20];
        u32 layer;
        u8 pad38[0x3C - 0x38];
        u8 mode;
    } Header;
    typedef struct {
        u8 id;
        u8 pad1[0x10 - 0x1];
        u32 unk10;
        u32 *unk14;
    } ObjectData;
    typedef struct {
        u8 pad0[0x8];
        u8 active;
        u8 pad9[0x44 - 0x9];
        ObjectData *data;
    } Object;
    typedef struct {
        u8 pad0[0x25C];
        Object *object;
        u8 pad260[0x300 - 0x260];
    } Slot;
    typedef struct {
        u32 count;
        u32 state;
        u8 *owner;
        Slot *selected;
        u8 pad10[0x14 - 0x10];
        s32 unk14;
        f32 unk18;
        Property *unk1C;
        Header *header;
        u32 unk24;
        u8 pad28[0x7D - 0x28];
        u8 index;
        u8 unk7E;
        u8 unk7F;
        u8 unk80;
        u8 unk81;
        u8 unk82;
        u8 paused;
        u8 unk84;
        u8 unk85;
        u8 pad86[0x88 - 0x86];
        Slot *slots;
        s32 mode;
        void (*callback)(void *msg);
        u32 unk94;
        s32 vars[30];
        u32 numVars;
        u8 player;
        u8 unk115;
        u8 unk116[0x40];
        u8 pad156[0x158 - 0x156];
        u32 unk158;
        u8 pad15C[0x160 - 0x15C];
        u8 unk160;
        u8 unk161;
        u8 pad162[0x164 - 0x162];
        f32 unk164;
        u8 unk168;
        u8 pad169[0x170 - 0x169];
    } Interface;
    typedef struct {
        u8 pad0[2];
        u8 id;
        u8 pad3;
        f32 value;
    } WorldVar;
    typedef struct {
        u8 pad0[0x48];
        u32 numVars;
        WorldVar **vars;
    } World;
    typedef struct {
        u32 index;
        u16 type;
        u16 id;
        u8 pad8[0xA - 0x8];
        u16 result;
        u8 padC[0x28 - 0xC];
        u8 unk28;
        u8 pad29[0x3C - 0x29];
        s32 *vars;
        u8 pad40[0x48 - 0x40];
    } InterfaceMessage;
    extern Interface g_interface[];
    extern Header *g_pInterfaceToEnd;
    extern s32 g_playerIndex;
    extern s8 g_bMemoryCardHWHack;
    void Trigger_InterfaceEnd(void *header);
    void *InterfaceManager_GetInterfaceObjects(u32 num);
    void *memset(void *dst, int c, u32 n);
    void InterfaceManager_AddInterfaceToRender(s32 index, u32 layer);
    s32 GameSettings_GetGameType(void);
    s32 StateManager_CurrState(void);
    void GameSettings_SetDefaultControllerState(void);
    void GSI_VehicleItemSelection(void *msg);
    void ArmageddonDropShip(void *msg);
    void TitleScreen_Interface(void *msg);
    void InterfaceManager_AddObject(void *iface, void *obj);
    void InterfaceManager_SetLoadState(void *iface, s32 state);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void GameSettings_GetSaveGameName(void *name);
    void InterfaceManager_PreprocessInterface(void *iface);
    InterfaceMessage msg;
    Header *hdr = (Header *)header;
    s32 index;
    u32 minId;
    Interface *iface;
    u32 i;
    u32 j;
    Slot *slot;
    CodeVar *cv;

    index = -1;
    minId = -1;
    iface = NULL;
    if (hdr == NULL) {
        return -1;
    }
    if (hdr == g_pInterfaceToEnd) {
        g_pInterfaceToEnd = NULL;
        Trigger_InterfaceEnd(hdr);
        return -1;
    }
    for (i = 0; i < 0x12; i++) {
        if (g_interface[i].count == 0) {
            index = i;
            iface = &g_interface[i];
            break;
        }
    }
    if (iface == NULL) {
        return -1;
    }
    iface->slots = InterfaceManager_GetInterfaceObjects(hdr->numObjects);
    if (iface->slots == NULL) {
        return -1;
    }
    iface->paused = 0;
    iface->count = 0;
    iface->owner = owner;
    iface->selected = NULL;
    iface->header = hdr;
    iface->unk24 = 0;
    iface->callback = callback;
    iface->unk14 = -1;
    iface->unk7E = 0;
    iface->unk7F = 0;
    iface->unk115 = 0;
    iface->unk84 = 0;
    iface->unk80 = 0;
    iface->unk81 = 0;
    iface->unk82 = 0;
    iface->unk1C = NULL;
    iface->unk18 = 0.0f;
    iface->unk94 = 0;
    iface->unk85 = 0;
    iface->unk158 = 0;
    iface->player = g_playerIndex;
    iface->index = index;
    iface->unk160 = 0;
    iface->unk161 = 0;
    iface->unk164 = 500.0f;
    iface->unk168 = 0;
    switch (hdr->mode) {
    case 1:
        iface->mode = 0;
        break;
    case 2:
        iface->mode = 1;
        break;
    case 3:
        iface->mode = 2;
        break;
    case 4:
        iface->mode = 3;
        break;
    case 0:
    default:
        iface->mode = -1;
        break;
    }
    memset(iface->unk116, 0, 0x40);
    if (hdr->layer == 0) {
        hdr->layer = 5;
    }
    InterfaceManager_AddInterfaceToRender(index, hdr->layer);
    switch (hdr->id) {
    case 2:
        if (GameSettings_GetGameType() == 0) {
            iface->callback = GSI_VehicleItemSelection;
        } else if (GameSettings_GetGameType() == 1) {
            if (StateManager_CurrState() == 2) {
                iface->callback = GSI_VehicleItemSelection;
            } else {
                iface->callback = ArmageddonDropShip;
            }
        }
        GameSettings_SetDefaultControllerState();
        break;
    case 3:
        if (GameSettings_GetGameType() == 0) {
            iface->callback = GSI_VehicleItemSelection;
        } else if (GameSettings_GetGameType() == 1) {
            iface->callback = ArmageddonDropShip;
        }
        GameSettings_SetDefaultControllerState();
        break;
    case 8:
        iface->callback = TitleScreen_Interface;
        break;
    }
    for (i = 0; i < hdr->numObjects; i++) {
        InterfaceManager_AddObject(iface, hdr->objects + i * 0x6C);
    }
    InterfaceManager_SetLoadState(iface, 0);
    for (i = 0; i < iface->header->numProps; i++) {
        if (iface->header->props[i].type == 0x1E) {
            iface->unk1C = &iface->header->props[i];
            iface->unk18 = iface->header->props[i].value;
            break;
        }
    }
    for (i = 0; i < iface->count; i++) {
        slot = &iface->slots[i];
        if (slot->object->active && slot->object->data->id != 0 && slot->object->data->id < minId) {
            minId = slot->object->data->id;
            iface->selected = slot;
        }
    }
    for (i = 0, iface->numVars = 0; i < hdr->vars->count && i < 30; i++, iface->numVars++) {
        switch (hdr->vars->vars[i].type) {
        case 0:
            iface->vars[i] = hdr->vars->vars[i].offset;
            break;
        case 1:
            cv = hdr->vars->vars[i].data;
            if (cv->val->isVar) {
                iface->vars[i] = GameSettings_GetCodeVariable(cv->id, iface->vars[cv->val->value]);
            } else {
                iface->vars[i] = GameSettings_GetCodeVariable(cv->id, cv->val->value);
            }
            iface->vars[i] += hdr->vars->vars[i].offset;
            if (iface->vars[i] > hdr->vars->vars[i].max) {
                iface->vars[i] = iface->vars[i] - hdr->vars->vars[i].max - 1 + hdr->vars->vars[i].min;
            } else if (iface->vars[i] < hdr->vars->vars[i].min) {
                iface->vars[i] = hdr->vars->vars[i].max - (hdr->vars->vars[i].min - iface->vars[i] - 1);
            }
            break;
        case 2:
            if (owner != NULL) {
                iface->vars[i] = *(s32 *)(owner + 0x60);
            } else {
                iface->vars[i] = g_playerIndex;
            }
            break;
        case 3:
            if (iface->selected != NULL) {
                iface->vars[i] = iface->selected->object->data->id;
            } else {
                iface->vars[i] = 0;
            }
            break;
        case 4:
            if ((World *)g_worldData.world != NULL) {
                iface->vars[i] = 0;
                for (j = 0; j < ((World *)g_worldData.world)->numVars; j++) {
                    if (((World *)g_worldData.world)->vars[j]->id == hdr->vars->vars[i].offset) {
                        iface->vars[i] = ((World *)g_worldData.world)->vars[j]->value;
                    }
                }
            }
            break;
        }
    }
    if (hdr->id == 7) {
        GameSettings_GetSaveGameName(iface->vars);
    }
    if (iface->callback != NULL) {
        msg.index = iface->index;
        msg.type = 3;
        msg.id = hdr->id;
        msg.result = 0;
        msg.unk28 = 0;
        msg.vars = iface->vars;
        iface->callback(&msg);
        if (msg.result != 0) {
            for (i = 0; i < iface->count; i++) {
                slot = &iface->slots[i];
                if (slot->object->active && slot->object->data->id == msg.result) {
                    iface->selected = slot;
                }
            }
        }
    }
    InterfaceManager_PreprocessInterface(iface);
    if (g_bMemoryCardHWHack && iface->count == 14 && iface->slots[9].object->active &&
        iface->slots[9].object->data->id == 6 && iface->slots[9].object->data->unk10 == 1) {
        g_bMemoryCardHWHack = 0;
        iface->slots[9].object->data->unk14[1] = 100;
    }
    return index;
}

void InterfaceManager_PreprocessInterface(u8 *iface) {
    typedef struct MusicInfo {
        char *name;
        u32 id;
    } MusicInfo;
    typedef struct InterfaceHeader {
        u8 pad0[0x30];
        MusicInfo *music;
        u8 pad34[0x38 - 0x34];
        f32 volume;
    } InterfaceHeader;
    typedef struct FontInfo {
        u8 pad0[0x8];
        u8 *name;
        u8 padC[0x28 - 0xC];
        s32 id;
    } FontInfo;
    typedef struct ResourceEntry {
        u8 pad0[0x8];
        u32 soundID;
        char *soundName;
        u8 pad10[0x14 - 0x10];
        FontInfo *font;
        u8 pad18[0x20 - 0x18];
    } ResourceEntry;
    typedef struct ResourceList {
        u32 count;
        ResourceEntry *entries;
    } ResourceList;
    typedef struct SoundPack {
        s32 id;
        u8 *data;
    } SoundPack;
    typedef struct InterfaceSlot {
        u8 pad0[0x245];
        u8 type;
        u8 pad246[0x25C - 0x246];
        u8 *object;
        u8 pad260[0x300 - 0x260];
    } InterfaceSlot;
    typedef struct WorldObjective {
        u32 unk0;
        u8 *info;
    } WorldObjective;
    typedef struct World {
        u8 pad0[0x58];
        u32 numObjectives;
        WorldObjective *objectives;
    } World;
    extern char *g_curMusicPackName;
    extern u32 g_curMusicPackID;
    extern u32 g_numInterfaceDialogSoundPacks;
    extern u32 g_curInterfaceDialogSoundPack;
    extern u32 g_curInterfaceDialogSoundIndex;
    extern SoundPack g_interfaceDialogSoundPacks[15];
    void SongManager_StopBKSong(void);
    void SongManager_PlaySong(void);
    void SetMusicVol(u32 vol);
    s32 FileStreaming_GetPackID(char *name, s32 type, u32 id);
    s32 FileStreaming_ReleaseDataPack(u32 index, s32 type);
    void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
    s32 InterfaceManager_AddFont(u8 *font);
    void InterfaceManager_SetObjectText(u8 *obj, char **text);
    void InterfaceManager_SetMissionObjectiveText(u8 *obj, void *objective);
    u32 i;
    u32 total;
    ResourceList *list;
    InterfaceSlot *slot;
    s32 pack;
    u8 *data;
    SoundPack *sp;
    World *world;
    u32 k;
    u8 *info;
    WorldObjective *objective;
    u32 id;

    if ((*(InterfaceHeader **)(iface + 0x20))->music != NULL && (*(InterfaceHeader **)(iface + 0x20))->music->name != NULL) {
        if (g_curMusicPackName == NULL || strcmp(g_curMusicPackName, (*(InterfaceHeader **)(iface + 0x20))->music->name) != 0
            || g_curMusicPackID != (*(InterfaceHeader **)(iface + 0x20))->music->id) {
            SongManager_StopBKSong();
            if (g_curMusicPackName != NULL) {
                FileStreaming_ReleaseDataPack(FileStreaming_GetPackID(g_curMusicPackName, 10, g_curMusicPackID), 10);
            }
            FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack((*(InterfaceHeader **)(iface + 0x20))->music->name, 10, (*(InterfaceHeader **)(iface + 0x20))->music->id, 0), 10, 1);
            SongManager_PlaySong();
            SetMusicVol(32767.0f * (*(InterfaceHeader **)(iface + 0x20))->volume);
            g_curMusicPackName = (*(InterfaceHeader **)(iface + 0x20))->music->name;
            g_curMusicPackID = (*(InterfaceHeader **)(iface + 0x20))->music->id;
        }
    }
    g_numInterfaceDialogSoundPacks = 0;
    g_curInterfaceDialogSoundPack = 0;
    g_curInterfaceDialogSoundIndex = 0;
    if (*(u8 **)(iface + 0x158) != NULL) {
        list = *(ResourceList **)(*(u8 **)(iface + 0x158) + 0x258);
        total = 0;
        for (i = 0; i < list->count; i++) {
            if (list->entries[i].font != NULL) {
                if (list->entries[i].font->name == NULL) {
                    continue;
                }
                list->entries[i].font->id = InterfaceManager_AddFont(list->entries[i].font->name);
            }
            if (list->entries[i].soundName != NULL && g_numInterfaceDialogSoundPacks < 15 && total < 1000000) {
                pack = FileStreaming_LoadDataPack(list->entries[i].soundName, 9, list->entries[i].soundID, 0);
                data = FileStreaming_InitializeDataPack(pack, 9, 1);
                sp = &g_interfaceDialogSoundPacks[g_numInterfaceDialogSoundPacks];
                g_numInterfaceDialogSoundPacks++;
                sp->id = pack;
                sp->data = data;
                total += *(u32 *)(data + 0x90);
                g_curInterfaceDialogSoundIndex = i + 1;
            }
        }
    }
    g_curInterfaceDialogSoundPack = 0;
    for (i = 0; i < *(u32 *)iface; i++) {
        slot = &(*(InterfaceSlot **)(iface + 0x88))[i];
        switch (slot->type) {
        case 0x34:
            id = *(u16 *)(slot->object + 0xA);
            world = g_worldData.world;
            if (world == NULL) {
                break;
            }
            for (k = 0; k < world->numObjectives; k++) {
                objective = &world->objectives[k];
                info = objective->info;
                if (info[2] == id) {
                    if (info[1] == 0x17) {
                        InterfaceManager_SetMissionObjectiveText((u8 *)slot, objective);
                    } else {
                        InterfaceManager_SetObjectText((u8 *)slot, NULL);
                    }
                    break;
                }
            }
            break;
        }
    }
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

void InterfaceManager_Render(void) {
    extern u8 g_interface[][0x170];
    extern u32 g_interfaceRenderOrder[0x12];
    extern s32 g_numInterfacesToRender;
    void InterfaceManager_RenderInterface(u8 *iface);
    u32 i;
    u8 *iface;

    for (i = 0; i < g_numInterfacesToRender; i++) {
        iface = g_interface[g_interfaceRenderOrder[i]];
        if (*(void **)iface != NULL && (*(u32 *)(iface + 4) != 1 || *(u32 *)(iface + 0x10) != 0) && iface[0x7F] == 0) {
            InterfaceManager_RenderInterface(iface);
        }
    }
}

void InterfaceManager_DrawObject_Standard_Image(u8 *obj) {
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
    Rect *Gfx_Add2DRectangle(void);
    void Gfx_Add2DRectangleBack(Rect *rect);
    void SinCos(f32 *s, f32 *c, f32 angle);
    Rect local;
    Rect *rect;
    f32 x0, y0;
    f32 s, c;
    f32 tx, ty;
    u8 *anchor;

    if (*(u16 *)(obj + 0x24A) == 0xFFFF) {
        return;
    }
    if (*(f32 *)(obj + 0x68) != 0.0f) {
        rect = &local;
    } else {
        rect = Gfx_Add2DRectangle();
    }
    rect->blendMode = obj[0x241];
    rect->tex = *(u16 *)(obj + 0x24A);
    anchor = *(u8 **)(obj + 0x25C);
    x0 = 0.5f * *(f32 *)(obj + 0x30) + 0.5f * (*(f32 *)(anchor + 0x30) * *(f32 *)(obj + 0x30));
    y0 = 0.5f * *(f32 *)(obj + 0x34) + 0.5f * (*(f32 *)(anchor + 0x34) * *(f32 *)(obj + 0x34));
    x0 = -x0;
    y0 = -y0;
    rect->v[0].x = x0;
    rect->v[0].y = y0;
    rect->v[1].x = x0 + *(f32 *)(obj + 0x30);
    rect->v[1].y = y0;
    rect->v[2].x = x0 + *(f32 *)(obj + 0x30);
    rect->v[2].y = y0 + *(f32 *)(obj + 0x34);
    rect->v[3].x = x0;
    rect->v[3].y = y0 + *(f32 *)(obj + 0x34);
    rect->v[0].s = *(f32 *)(obj + 0xC0);
    rect->v[0].t = *(f32 *)(obj + 0xDC);
    rect->v[1].s = *(f32 *)(obj + 0xD8);
    rect->v[1].t = *(f32 *)(obj + 0xDC);
    rect->v[2].s = *(f32 *)(obj + 0xD8);
    rect->v[2].t = *(f32 *)(obj + 0xC4);
    rect->v[3].s = *(f32 *)(obj + 0xC0);
    rect->v[3].t = *(f32 *)(obj + 0xC4);
    rect->v[0].color = *(u32 *)(obj + 0x24);
    rect->v[1].color = *(u32 *)(obj + 0x24);
    rect->v[2].color = *(u32 *)(obj + 0x24);
    rect->v[3].color = *(u32 *)(obj + 0x24);
    if (*(f32 *)(obj + 0x90)) {
        SinCos(&s, &c, *(f32 *)(obj + 0x90));
        tx = rect->v[0].x;
        ty = rect->v[0].y;
        rect->v[0].x = *(f32 *)(obj + 0x60) + (tx * c - ty * s);
        rect->v[0].y = *(f32 *)(obj + 0x64) + (tx * s + ty * c);
        tx = rect->v[1].x;
        ty = rect->v[1].y;
        rect->v[1].x = *(f32 *)(obj + 0x60) + (tx * c - ty * s);
        rect->v[1].y = *(f32 *)(obj + 0x64) + (tx * s + ty * c);
        tx = rect->v[2].x;
        ty = rect->v[2].y;
        rect->v[2].x = *(f32 *)(obj + 0x60) + (tx * c - ty * s);
        rect->v[2].y = *(f32 *)(obj + 0x64) + (tx * s + ty * c);
        tx = rect->v[3].x;
        ty = rect->v[3].y;
        rect->v[3].x = *(f32 *)(obj + 0x60) + (tx * c - ty * s);
        rect->v[3].y = *(f32 *)(obj + 0x64) + (tx * s + ty * c);
    } else {
        rect->v[0].x += *(f32 *)(obj + 0x60);
        rect->v[0].y += *(f32 *)(obj + 0x64);
        rect->v[1].x += *(f32 *)(obj + 0x60);
        rect->v[1].y += *(f32 *)(obj + 0x64);
        rect->v[2].x += *(f32 *)(obj + 0x60);
        rect->v[2].y += *(f32 *)(obj + 0x64);
        rect->v[3].x += *(f32 *)(obj + 0x60);
        rect->v[3].y += *(f32 *)(obj + 0x64);
    }
    if (*(f32 *)(obj + 0x68) != 0.0f) {
        Gfx_Add2DRectangleBack(rect);
    }
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

#pragma push
#pragma use_lmw_stmw on
s32 InterfaceManager_GetTargetArrowNearestNavPoint(void)
{
    typedef struct TargetArrow
    {
        s32 pad[4];
        s32 nearest;
        s32 timer;
        s32 flag;
    } TargetArrow;
    extern TargetArrow g_TargetArrow;
    s32 NavHolder_FindIndexOfNearestNavPoint(void *pos, s32 hint);

    if (g_TargetArrow.nearest != -1 && g_TargetArrow.timer <= 0)
        g_TargetArrow.nearest = NavHolder_FindIndexOfNearestNavPoint((void *)g_TargetArrow.pad[0], g_TargetArrow.nearest), g_TargetArrow.timer = 300;

    return g_TargetArrow.nearest;
}
#pragma pop
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

void DropShip_StartChildInterface(s32 index, u32 id) {
    typedef struct DropShipSlot {
        u8 pad0[0x243];
        u8 type;
        u8 pad244[0x25C - 0x244];
        u8 *anim;
        u8 pad260[0x270 - 0x260];
        s32 child;
        u8 pad274[0x300 - 0x274];
    } DropShipSlot;
    typedef struct Interface {
        u32 count;
        u32 loadState;
        u8 pad8[0x83 - 0x8];
        u8 flag83;
        u8 pad84[0x88 - 0x84];
        DropShipSlot *slots;
        u8 pad8C[0x170 - 0x8C];
    } Interface;
    extern Interface g_interface[];
    void InterfaceManager_SetLoadState(Interface *iface, s32 state);
    u16 animId;
    u32 i;
    Interface *iface;

    iface = &g_interface[index];
    for (i = 0; i < iface->count; i++) {
        if (iface->slots[i].type == 6) {
            animId = *(u16 *)(iface->slots[i].anim + 0xA);
            if ((animId == id) && (g_interface[iface->slots[i].child].loadState != 0)) {
                g_interface[iface->slots[i].child].flag83 = 0;
                InterfaceManager_SetLoadState(&g_interface[iface->slots[i].child], 0);
            } else {
                g_interface[iface->slots[i].child].flag83 = 1;
                g_interface[iface->slots[i].child].loadState = 2;
            }
        }
    }
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

void DropShip_PullCameraBack(s32 index) {
    typedef struct ObjectEntry {
        u8 type;
        u8 pad1[3];
        s32 value;
        u8 pad8[4];
    } ObjectEntry;
    typedef struct ObjectInfo {
        u8 pad0[0x64];
        u32 count;
        u8 pad68[4];
    } ObjectInfo;
    typedef struct InterfaceSlot {
        u8 pad0[0x245];
        u8 type;
        u8 pad246[0x25C - 0x246];
        u8 *object;
        u8 pad260[0x300 - 0x260];
    } InterfaceSlot;
    typedef struct Interface {
        u32 count;
        u8 pad4[0x18 - 0x4];
        f32 camDistance;
        u8 *ptr1C;
        u8 pad20[0x88 - 0x20];
        InterfaceSlot *slots;
        u8 pad8C[0x170 - 0x8C];
    } Interface;
    extern Interface g_interface[];
    void InterfaceManager_SetObjectState(Interface *iface, InterfaceSlot *slot, s32 state, s32 arg3);
    ObjectInfo *info;
    u32 i;
    u32 j;
    Interface *iface;

    iface = &g_interface[index];
    for (i = 0; i < iface->count; i++) {
        if (iface->slots[i].type == 4) {
            InterfaceManager_SetObjectState(iface, &iface->slots[i], 2, 0);
            info = (ObjectInfo *)iface->slots[i].object;
            for (j = 0; j < info->count; j++) {
                if ((*(ObjectEntry **)(iface->slots[i].object + 0x68))[j].type == 2) {
                    iface->camDistance = (f32)(*(ObjectEntry **)(iface->slots[i].object + 0x68))[j].value;
                    break;
                }
            }
            *(u16 *)(*(u8 **)(iface->ptr1C + 8) + 6) = 9;
        }
    }
}

f32 DropShip_GetLRTime(s32 index)
{
    typedef struct DropShipSlot
    {
        u8 pad0[0x245];
        u8 state;
        u8 pad246[0x25C - 0x246];
        u8 *anim;
        u8 pad260[0x300 - 0x260];
    } DropShipSlot;
    extern u8 g_interface[][0x170];
    u32 i;
    u8 *iface = g_interface[index];

    for (i = 0; i < *(u32 *)iface; i++)
    {
        if ((*(DropShipSlot **)(iface + 0x88))[i].state == 5)
        {
            return (f32)*(u16 *)((*(DropShipSlot **)(iface + 0x88))[i].anim + 0xA);
        }
    }
    return 0.0f;
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

#pragma push
#pragma use_lmw_stmw on
void InterfaceManager_RestoreMissionObjString(u8 *mgr)
{
    u32 strlen(const char *s);
    char *strcpy(char *dst, const char *src);
    u32 i;
    u32 id;
    u8 *obj;
    char **str;
    f32 zero;

    if (*(u32 *)((char *)mgr + 0x25c) == 0)
    {
        return;
    }
    id = *(u16 *)((char *)*(u8 **)((char *)mgr + 0x25c) + 0xa);
    if (strlen((char *)(mgr + 0x290)) == 0)
    {
        return;
    }
    if (g_worldData.world == NULL)
    {
        return;
    }
    zero = 0.0f;
    for (i = 0; i < *(u32 *)((char *)g_worldData.world + 0x58); i++)
    {
        obj = *(u8 **)((char *)*(u8 **)((char *)g_worldData.world + 0x5c) + i * 8 + 4);
        if (obj[2] == id)
        {
            if (*(f32 *)((char *)*(f32 **)((char *)obj + 0xc) + 4) != zero)
            {
                str = *(char ***)((char *)obj + 0x18);
            }
            else
            {
                str = *(char ***)((char *)obj + 0x14);
            }
            strcpy(*str, (char *)(mgr + 0x290));
        }
    }
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

