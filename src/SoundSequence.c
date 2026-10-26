#include "common.h"

typedef struct SoundSequence {
    u8 pad0[0x74];
    u32 flags;    /* 0x74 */
    u8 pad78[0x38];
    u8 *params;   /* 0xB0 */
} SoundSequence;

u32 ThingManager_GetActiveSoundSequences(u8 **list, u32 max);

s32 SoundSeq_Init(u8 *seq, u8 *data) {
    extern Matrix4 g_identity;
    s32 Thing_Init(void *thing, s32 arg1);
    u32 ThingManager_GetActiveSoundSequences(u8 **list, u32 max);
    s32 RandomAdd_s32(s32 base, s32 range);
    u8 *list[0x80];
    u8 *params;
    s16 count;
    u32 n;

    Thing_Init(seq, 0);
    *(Matrix4 *)(seq + 0x10) = g_identity;
    *(Vector4 *)(seq + 0x40) = *(Vector4 *)data;
    *(u8 **)(seq + 0xB0) = *(u8 **)(data + 0x10);
    *(s32 *)(seq + 0xC8) = 0;
    params = *(u8 **)(seq + 0xB0);
    if (*(u8 *)(params + 5) != 0) {
        count = 0;
        n = ThingManager_GetActiveSoundSequences(list, 0x80);
        while (n != 0) {
            n--;
            if (list[n] != seq && *(u8 **)(list[n] + 0xB0) == *(u8 **)(seq + 0xB0)) {
                count++;
            }
        }
        params = *(u8 **)(seq + 0xB0);
        if (count >= *(u8 *)(params + 5)) {
            *(s32 *)(seq + 0xC4) = -1;
        } else {
            *(s32 *)(seq + 0xC4) = RandomAdd_s32(*(u32 *)params, *(u16 *)(params + 6));
        }
    } else {
        *(s32 *)(seq + 0xC4) = RandomAdd_s32(*(u32 *)params, *(u16 *)(params + 6));
    }
    if (*(u32 *)*(u8 **)(seq + 0xB0) == 0) {
        *(u32 *)(seq + 0x74) |= 0x10000000;
    }
    *(f32 *)(seq + 0xCC) = 1.0f;
    params = *(u8 **)(seq + 0xB0);
    if (*(u32 *)params != 0 && *(u16 *)(params + 6) != 0) {
        *(f32 *)(seq + 0xCC) = 1.0f / ((f32)*(s32 *)(seq + 0xC4) / (f32)*(u32 *)params);
    }
    *(u8 *)(seq + 0xC0) = 1;
    *(u32 *)(seq + 0x74) |= 0xE0202;
    if (*(s8 *)(data + 0x14) != 0) {
        *(u32 *)(seq + 0x74) |= 0x10;
    }
    *(s32 *)(seq + 0xD0) = -1;
    *(s32 *)(seq + 0xD4) = -1;
    *(s32 *)(seq + 0xD8) = -1;
    *(s32 *)(seq + 0xDC) = -1;
    return 1;
}

s32 SoundSeq_Invalidate(SoundSequence *seq, u32 start, u32 end)
{
    if ((u32)seq->params >= start && (u32)seq->params < end) {
        return 1;
    }
    return 0;
}

s32 SoundSequence_Play(s8 *seq, Vector4 *pos) {
    u8 PlayerManager_PlayerHumanCount(void);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    extern Vector4 g_zeroVector;
    extern s8 g_bFreezeWorld;
    Vector4 p;
    struct { Vector4 pos; s8 *seq; u8 flag; } params;
    u8 *thing;

    if (!seq[4] && PlayerManager_PlayerHumanCount() > 1) {
        return -1;
    }
    if (pos != NULL) {
        p = *pos;
    } else {
        p = g_zeroVector;
    }
    params.pos = p;
    params.seq = seq;
    params.flag = 0;
    thing = ThingManager_DARYL_CreateThing(0x10, &params);
    if (thing != NULL) {
        if (g_bFreezeWorld) {
            *(u32 *)(thing + 0x74) |= 0x100000;
        }
        return (s32)thing;
    }
    return -1;
}

s32 SoundSequence_PlayQueued(s8 *seq, Vector4 *pos) {
    u32 ThingManager_GetActiveSoundSequences(u8 **list, u32 max);
    u8 *list[0x80];
    s32 thing;
    s8 found;
    u32 n;

    thing = SoundSequence_Play(seq, pos);
    if ((u32)thing != 0xFFFFFFFF) {
        found = 0;
        n = ThingManager_GetActiveSoundSequences(list, 0x80);
        while (n != 0) {
            n--;
            if ((*(u32 *)(list[n] + 0x74) & 0x40000000) && *(u8 **)(list[n] + 0xc8) == NULL) {
                found = 1;
                break;
            }
        }
        if (found) {
            *(s32 *)(list[n] + 0xc8) = thing;
            *(u32 *)((u8 *)thing + 0x74) |= 0x80000000;
        }
        *(u32 *)((u8 *)thing + 0x74) |= 0x40000000;
        *(u8 **)((u8 *)thing + 0xc8) = NULL;
    }
    return thing;
}

void SoundSequence_ClearQueue(void) {
    u32 ThingManager_GetActiveSoundSequences(u8 **list, u32 max);
    u8 *list[128];
    u32 count;

    count = ThingManager_GetActiveSoundSequences(list, 128);
    while (count != 0) {
        count--;
        if (M2C_FIELD(list[count], u32 *, 0x74) & 0x40000000) {
            M2C_FIELD(list[count], u32 *, 0x74) &= ~0x40000000;
            M2C_FIELD(list[count], u8 **, 0xC8) = NULL;
            SoundSeq_Release(list[count]);
        }
    }
}

s32 SoundSequence_GetTotalElementsInQueue(void)
{
    u8 *list[128];
    u32 count;
    s32 total;

    total = 0;
    count = ThingManager_GetActiveSoundSequences(list, 128);
    while (count != 0) {
        count--;
        if (((SoundSequence *)list[count])->flags & 0x40000000) {
            total++;
        }
    }
    return total;
}

