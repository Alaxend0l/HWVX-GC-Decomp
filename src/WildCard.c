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

