#include "common.h"


s32 Item_Init(u8 *item, u8 *data) {
    void QuaternionToSubMatrix(void *quat, void *mtx);
    f32 RandomAdd(f32 base, f32 range);
    void GenerateQuaternionXYZ(Vector4 *euler, Vector4 *quat);
    void GenerateSpinAngleFromQuaternion(Vector4 *quat, void *axis, f32 *angle);
    s32 NavHolder_FindIndexOfNearestNavPoint(void *pos, s32 hint);
    Vector4 euler;
    Vector4 quat;
    u8 *info;

    Thing_Init(item, (s32)data);
    M2C_FIELD(item, s32 *, 0xD4) = -1;
    M2C_FIELD(item, s32 *, 0xD0) = 0;
    M2C_FIELD(item, u16 *, 0x80) = 2;
    M2C_FIELD(item, f32 *, 0x70) = M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0xC);
    M2C_FIELD(item, f32 *, 0xE8) = M2C_FIELD(item, f32 *, 0x70) * M2C_FIELD(item, f32 *, 0x70);
    M2C_FIELD(item, Vector4 *, 0x40) = M2C_FIELD(data, Vector4 *, 0x10);
    M2C_FIELD(item, f32 *, 0x4C) = 1.0f;
    M2C_FIELD(item, Vector4 *, 0x0) = M2C_FIELD(data, Vector4 *, 0x20);
    M2C_FIELD(item, Vector4 *, 0x50) = M2C_FIELD(data, Vector4 *, 0x0);
    M2C_FIELD(item, s32 *, 0xEC) = 0;
    QuaternionToSubMatrix(item + 0x50, item + 0x10);
    M2C_FIELD(item, f32 *, 0xC8) = M2C_FIELD(data, f32 *, 0x34);
    M2C_FIELD(item, u8 *, 0xE0) = M2C_FIELD(data, u8 *, 0x38);
    if (M2C_FIELD(item, f32 *, 0xC8) == 0.0f) {
        M2C_FIELD(item, f32 *, 0xC8) = 50.0f;
    }
    M2C_FIELD(item, s32 *, 0xD8) = 0;
    M2C_FIELD(item, u32 *, 0x74) |= 0x1000000;
    M2C_FIELD(item, u8 **, 0xC0) = M2C_FIELD(data, u8 **, 0x30);
    info = M2C_FIELD(M2C_FIELD(item, u8 **, 0xC0), u8 **, 0x30);
    if (info != NULL) {
        M2C_FIELD(item, s16 *, 0xCE) = M2C_FIELD(info, s32 *, 0x20);
    }
    euler.x = 0.0f;
    euler.y = 6.2831855f * RandomAdd(M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0x18), M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0x1C));
    euler.z = 0.0f;
    GenerateQuaternionXYZ(&euler, &quat);
    GenerateSpinAngleFromQuaternion(&quat, item + 0xB0, (f32 *)(item + 0xC4));
    M2C_FIELD(item, s16 *, 0xCC) = 1000;
    Thing_InitAllSlavesNow(item, M2C_FIELD(item, u8 **, 0xC0) + 0x38, 0xFF);
    M2C_FIELD(item, u32 *, 0x74) |= 8;
    M2C_FIELD(item, s32 *, 0xD4) = NavHolder_FindIndexOfNearestNavPoint(item + 0x40, -1);
    Thing_AddToGrid(item, 3);
    return 1;
}

void Item_Release(void *item) {
    extern u8 g_itemVTable[];
    void Thing_Release(s32 thing);
    void *slot;

    if (M2C_FIELD(item, void **, 0x8C) == g_itemVTable) {
        slot = M2C_FIELD(item, void **, 0x90);
        if (slot != NULL && M2C_FIELD(slot, void **, 0x24) != NULL) {
            M2C_FIELD(item, void **, 0x90) = NULL;
            M2C_FIELD(slot, void **, 0x24) = NULL;
        }
    }
    Thing_Release((s32)item);
}

s32 Item_PreMove(void) {
    return 1;
}

