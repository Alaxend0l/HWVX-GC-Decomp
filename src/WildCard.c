#include "common.h"

#pragma use_lmw_stmw on


s32 WildCardRack_Init(void *thing, void *data) {
    typedef struct WildCardSlot {
        void *card;
        u8 pad4[0x40 - 0x4];
        u16 unk40;
        u16 unk42;
        u8 pad44[0x48 - 0x44];
        f32 unk48;
        s32 unk4C;
    } WildCardSlot;
    typedef struct WildCardPos {
        s32 v[4];
    } WildCardPos;
    typedef struct WildCardRack {
        u8 pad0[0x40];
        WildCardPos pos;
        u8 pad50[0xB0 - 0x50];
        WildCardSlot slots[12];
        u8 pad470;
        u8 pad471;
        u8 unk472;
        u8 pad473[0x478 - 0x473];
        s32 unk478;
        f32 unk47C;
    } WildCardRack;
    WildCardRack *rack = thing;
    WildCardPos *init = data;
    u32 i;

    Thing_Init(rack, 0);
    rack->pos = *init;
    rack->pad470 = 0;
    rack->unk472 = 0;
    for (i = 0; i < 12; i++) {
        rack->slots[i].unk40 = 0;
        rack->slots[i].card = NULL;
        rack->slots[i].unk42 = 0;
        rack->slots[i].unk48 = 0.0f;
        rack->slots[i].unk4C = 0;
    }
    rack->unk478 = 0;
    rack->unk47C = -1.0f;
    return 0;
}

void WildCardRack_Release(s32 arg0) {
    s32 var_r31;
    u32 var_r30;

    var_r30 = 0U;
    var_r31 = arg0;
    do {
        MotionState_Release(var_r31 + 0xB0);
        var_r30 += 1;
        var_r31 += 0x50;
    } while (var_r30 < 0xCU);
    Thing_Release(arg0);
}

extern Matrix4 g_identity;
void QuaternionToSubMatrix(void *quat, void *mtx);
void MotionState_Draw(void *state);
void MotionState_Draw2(void *state, void *arg1, void *matStack);
void MultiplyMatrix(void *a, void *b, Matrix4 *out);
void WildCardRack_Draw(u8 *rack) {
    typedef struct {
        void *motion;
        u8 pad4[0x40];
        f32 scale;
        u8 pad48[0x8];
    } RackState;
    typedef struct {
        u8 pad0[0xB0];
        RackState states[1];
    } Rack;
    Matrix4 tmp;
    Matrix4 scale;
    Matrix4 trans;
    Matrix4 out;
    f32 s;

    if (((RackState *)(rack + 0xB0))[rack[0x470]].motion != NULL) {
        QuaternionToSubMatrix(rack + 0x50, rack + 0x10);
        s = ((RackState *)(rack + 0xB0))[rack[0x470]].scale;
        if (s < 1.0f) {
            scale = g_identity;
            trans = g_identity;
            scale.v[2].z = s;
            scale.v[1].y = s;
            scale.v[0].x = s;
            tmp = g_identity;
            scale.v[3].x = *(f32 *)(rack + 0x40);
            scale.v[3].y = *(f32 *)(rack + 0x44);
            scale.v[3].z = *(f32 *)(rack + 0x48);
            trans.v[3].x = -*(f32 *)(rack + 0x40);
            trans.v[3].y = -*(f32 *)(rack + 0x44);
            trans.v[3].z = -*(f32 *)(rack + 0x48);
            MultiplyMatrix(&trans, &scale, &out);
            MotionState_Draw2(&((Rack *)rack)->states[rack[0x470]], &out, NULL);
        } else {
            MotionState_Draw(&((Rack *)rack)->states[rack[0x470]]);
        }
    }
}

void WildCardRack_JettisonAll(void *rack) {
    void WildCardRack_Jettison(void *rack, u8 slot);
    u32 i;

    if (rack != NULL) {
        for (i = 0; i < 12; i++) {
            WildCardRack_Jettison(rack, i);
        }
    }
}

void WildCardRack_Jettison(void *thing, u8 slot) {
    typedef struct WildCardSlot {
        void *card;
        u8 pad4[0x44 - 0x4];
        f32 unk44;
        u8 pad48[0x4C - 0x48];
        void *unk4C;
    } WildCardSlot;
    typedef struct WildCardRack {
        u8 pad0[0xB0];
        WildCardSlot slots[12];
        u8 unk470;
    } WildCardRack;
    WildCardRack *rack = thing;
    u32 i;

    if (rack->slots[slot].card != NULL) {
        rack->slots[slot].unk4C = NULL;
        rack->slots[slot].card = NULL;
        for (i = 0; i < 12; i++) {
            if (rack->slots[i].unk4C != NULL && rack->slots[i].card != NULL) {
                rack->unk470 = i;
                rack->slots[i].unk44 = 0.0f;
            }
        }
    }
}

void WildCardRack_Add(void *thing, u8 *item, s16 count) {
    typedef struct WildCard {
        u8 pad0[0x1E];
        u8 unk1E;
        u8 pad1F[0x24 - 0x1F];
        f32 *unk24;
        f32 *unk28;
    } WildCard;
    typedef struct WildCardData {
        WildCard *card;
        u8 pad4[0x28 - 0x4];
        s32 max;
    } WildCardData;
    typedef struct WildCardItem {
        u8 pad0[0x30];
        WildCardData *data;
    } WildCardItem;
    typedef struct WildCardSlot {
        WildCard *card;
        u8 pad4[0x1C - 0x4];
        u8 unk1C;
        u8 unk1D;
        u8 pad1E[0x20 - 0x1E];
        f32 unk20;
        f32 unk24;
        u8 pad28[0x40 - 0x28];
        s16 count;
        u8 pad42[0x44 - 0x42];
        f32 unk44;
        f32 unk48;
        WildCardItem *item;
    } WildCardSlot;
    typedef struct WildCardRack {
        u8 pad0[0x74];
        u32 flags;
        u8 pad78[0xB0 - 0x78];
        WildCardSlot slots[12];
        u8 unk470;
    } WildCardRack;
    WildCardRack *rack = thing;
    WildCardItem *it = (WildCardItem *)item;
    s8 found = 0;
    s8 slot = rack->unk470;
    u32 i;
    WildCardSlot *s;

    if (rack->flags & 0x10) {
        if (rack->slots[rack->unk470].item != NULL) {
            if (rack->slots[rack->unk470].item->data == it->data) {
                if (rack->slots[rack->unk470].count == 0) {
                    rack->slots[rack->unk470].unk1C = 1;
                    rack->slots[rack->unk470].unk20 = rack->slots[rack->unk470].card->unk24[1];
                }
                rack->slots[rack->unk470].count += (s16)count;
                if (rack->slots[rack->unk470].count > (s16)it->data->max) {
                    rack->slots[rack->unk470].count = it->data->max;
                }
                found = 1;
            } else {
                WildCardRack_Jettison(thing, rack->unk470);
            }
        }
    } else {
        for (i = 0; i < 12; i++) {
            if (rack->slots[i].item != NULL && rack->slots[i].item->data == it->data) {
                if (rack->slots[i].count == 0) {
                    rack->slots[i].unk1C = 1;
                    rack->slots[i].unk20 = rack->slots[i].card->unk24[1];
                }
                rack->slots[i].count += (s16)count;
                if (rack->slots[i].count > (s16)it->data->max) {
                    rack->slots[i].count = it->data->max;
                }
                found = 1;
                if (rack->unk470 != i) {
                    rack->unk470 = i;
                    rack->slots[i].unk44 = 0.0f;
                }
                break;
            }
            if (rack->slots[i].item == NULL || rack->slots[i].card == NULL) {
                slot = i;
            }
        }
    }
    if (!found) {
        rack->slots[slot].card = it->data->card;
        s = &rack->slots[slot];
        rack->slots[slot].count = count;
        if (rack->slots[slot].count > (s16)it->data->max) {
            rack->slots[slot].count = it->data->max;
        }
        rack->slots[slot].item = it;
        s->unk24 = 0.0f;
        MotionState_Init(s, 0);
        if (s->card != NULL) {
            if (s->card->unk1E) {
                rack->slots[slot].unk20 = *s->card->unk28;
                rack->slots[slot].unk1C = 2;
            } else {
                rack->slots[slot].unk20 = s->card->unk24[1];
            }
            rack->slots[slot].unk1D = 1;
            rack->slots[slot].unk44 = 0.0f;
            rack->slots[slot].unk48 = 0.0f;
            rack->unk470 = slot;
        }
    }
}

s8 Thing_HasThing(u8 *thing, void *target);
void *ThingManagerList_GetByType(void *list, s32 type);
s32 TriggerItemPickup(u32 *trigger) {
    typedef struct WildCardSlot {
        u8 pad0[0x4C];
        u8 *item;
    } WildCardSlot;
    typedef struct WildCardRack {
        u8 pad0[0xB0];
        WildCardSlot slots[12];
    } WildCardRack;
    u32 i;
    u32 j;
    u32 numItems;
    u8 *item;
    u32 found;
    u8 *thing;
    WildCardRack *rack;
    u32 k;

    numItems = trigger[0] + 1;
    for (i = 0; i < trigger[0]; i++) {
        item = (u8 *)trigger[i + 1];
        for (j = 0; j < trigger[numItems]; j++) {
            found = 0;
            thing = *(u8 **)(trigger[j + numItems + 1] + 0x28);
            if (thing != NULL) {
                if (Thing_HasThing(thing, *(void **)(item + 0x24))) {
                    return 1;
                }
                rack = ThingManagerList_GetByType(thing + 0x344, 8);
                for (k = 0; rack != NULL && k < 12; k++) {
                    if (rack->slots[k].item != NULL &&
                        *(u32 *)(*(u8 **)(item + 0x1C) + 0x30) == *(u32 *)(rack->slots[k].item + 0x30)) {
                        found = 1;
                    }
                }
                if (found) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

s32 WildCardRack_Invalidate(u8 *rack, u32 start, u32 end) {
    u32 i;
    u32 addr;

    for (i = 0; i < 12; i++) {
        addr = *(u32 *)(rack + i * 0x50 + 0xFC);
        if (addr >= start && addr < end) {
            return 1;
        }
    }
    return 0;
}

