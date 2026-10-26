#include "common.h"


void ThingManagerList_Append(void **arg0, void *arg1) {
    void *temp_r5;

    M2C_FIELD(arg1, void **, 0xA4) = (void *) *arg0;
    M2C_FIELD(arg1, s32 *, 0xA0) = 0;
    temp_r5 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, void **, 0xA0) = arg1;
    }
    *arg0 = arg1;
}

void ThingManagerList_Remove(void **arg0, void *arg1) {
    void *temp_r5;
    void *temp_r5_2;

    temp_r5 = M2C_FIELD(arg1, void **, 0xA0);
    if (temp_r5 == NULL) {
        *arg0 = M2C_FIELD(arg1, void **, 0xA4);
    } else {
        M2C_FIELD(temp_r5, void **, 0xA4) = (void *) M2C_FIELD(arg1, void **, 0xA4);
    }
    temp_r5_2 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5_2 != NULL) {
        M2C_FIELD(temp_r5_2, void **, 0xA0) = (void *) M2C_FIELD(arg1, void **, 0xA0);
        M2C_FIELD(arg1, void **, 0xA4) = NULL;
    }
    M2C_FIELD(arg1, void **, 0xA0) = NULL;
}

void *ThingManagerList_GetByType(void *list, s32 type) {
    void *thing;

    for (thing = *(void **)list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (M2C_FIELD(thing, s32 *, 0xA8) == type) {
            return thing;
        }
    }
    return NULL;
}

s32 ThingManagerList_Get(void *list, void *target) {
    void *thing;

    for (thing = *(void **)list; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        if (target == thing) {
            return 1;
        }
    }
    return 0;
}

void ThingManagerList_ReleaseSlavesBySlaveType(void **list, u8 type) {
    void *thing;
    void *next;
    void **freeList;
    void *head;

    thing = *list;
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        if (M2C_FIELD(thing, u8 *, 0x83) == type) {
            M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *), 4)(thing);
            ThingManagerList_Remove(list, thing);
            M2C_FIELD(thing, u32 *, 0x74) &= ~0x200000;
            M2C_FIELD(thing, u32 *, 0x74) |= 0x1000;
            freeList = &g_thingManager_DARYL.freeList[M2C_FIELD(thing, s32 *, 0xA8)];
            M2C_FIELD(thing, void **, 0xA4) = *freeList;
            M2C_FIELD(thing, void **, 0xA0) = NULL;
            head = M2C_FIELD(thing, void **, 0xA4);
            if (head != NULL) {
                M2C_FIELD(head, void **, 0xA0) = thing;
            }
            *freeList = thing;
        }
        thing = next;
    }
}

void ThingManagerList_ReleaseAll(void **list) {
    void *thing;
    void *next;

    thing = *list;
    while (thing != NULL) {
        next = M2C_FIELD(thing, void **, 0xA4);
        M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *), 4)(thing);
        thing = next;
    }
}

void ThingManager_DARYL_Destroy(void) {
    void *thing;
    u32 i;

    for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
        ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[1])(thing);
    }
    g_thingManager_DARYL.activeList = NULL;
    for (i = 0; i < 18; i++) {
        g_thingManager_DARYL.freeList[i] = NULL;
    }
}

void ThingManager_DARYL_Init(void) {
    M2C_UNK ThingManager_DARYL_Release();
    M2C_UNK MatrixCache_Init();
    ThingManager_DARYL_Release();
    MatrixCache_Init();
}

s32 TriggerPlayerVehicleGear(u32 *params) {
    extern u8 g_cameraManager[];
    u8 *vehicle = M2C_FIELD(*(u8 **)g_cameraManager, u8 **, 0x2C);

    if (vehicle != NULL && vehicle[0x4E8] == params[1]) {
        return 1;
    }
    return 0;
}

s32 TriggerPlayerVehicleInAir(u32 *params) {
    extern u8 g_cameraManager[];
    u8 *vehicle;
    u32 inAir;

    if (*(u8 **)g_cameraManager != NULL) {
        vehicle = M2C_FIELD(*(u8 **)g_cameraManager, u8 **, 0x2C);
    } else {
        vehicle = NULL;
    }
    inAir = params[2];
    if (inAir == 0 && vehicle != NULL && *(s32 *)(vehicle + 0x460) >= (s32)params[1]) {
        params[2] = 1;
        return 1;
    }
    if (inAir != 0 && vehicle != NULL && *(s32 *)(vehicle + 0x460) == 0) {
        params[2] = 0;
        return 1;
    }
    return 0;
}

