#include "common.h"


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

