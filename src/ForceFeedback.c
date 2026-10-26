#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct ForceFeedback {
    void *effect;                 /* 0x00 */
    u8 point[0x1C - 0x4];         /* 0x04 */
    u8 active;                    /* 0x1C */
    u8 pad1D[0x20 - 0x1D];        /* 0x1D */
    f32 value;                    /* 0x20 */
    u8 pad24[0x30 - 0x24];        /* 0x24 */
    u8 playing;                   /* 0x30 */
    u8 pad31[0x40 - 0x31];        /* 0x31 */
    Vector4 dir;                  /* 0x40 */
    s32 magnitude;                /* 0x50 */
    s32 unk54;                    /* 0x54 */
    s32 unk58;                    /* 0x58 */
    u8 pad5C[0x60 - 0x5C];        /* 0x5C */
} ForceFeedback;

extern ForceFeedback g_forcefeedback[24];

extern f32 g_fDeltaTimeMS;
extern s32 g_iDeltaTimeMS;
void ExtractPointKey(void *keys, void *out, f32 time);
void ExtractF32Key(void *keys, void *out, f32 time);

void ForceFeedback_Init(void)
{
    s32 i;

    for (i = 0; i < 24; i++)
    {
        g_forcefeedback[i].effect = 0;
    }
}

void ForceFeedback_Play(void *effect, Vector4 *dir) {
    u32 i;

    for (i = 0; i < 24; i++) {
        if (g_forcefeedback[i].effect == 0) {
            g_forcefeedback[i].effect = effect;
            g_forcefeedback[i].active = 1;
            g_forcefeedback[i].playing = 1;
            g_forcefeedback[i].dir = *dir;
            g_forcefeedback[i].unk54 = M2C_FIELD(effect, s32 *, 0x14);
            g_forcefeedback[i].unk58 = M2C_FIELD(effect, s32 *, 0x18);
            g_forcefeedback[i].magnitude = M2C_FIELD(effect, s32 *, 0x10);
            return;
        }
    }
}

void ForceFeedback_Heartbeat(void)
{
    ForceFeedback *ff;
    s32 i;

    for (i = 0; i < 24; i++)
    {
        ff = &g_forcefeedback[i];
        if (ff->effect != NULL)
        {
            ExtractPointKey((char *)ff->effect + 0x2C, ff->point, g_fDeltaTimeMS);
            if (ff->unk54 > 0)
            {
                ff->unk54 -= g_iDeltaTimeMS;
            }
            if (ff->unk58 > 0)
            {
                ff->unk58 -= g_iDeltaTimeMS;
                if (ff->unk58 >= 0)
                {
                    ExtractF32Key(*(u8 **)((char *)ff->effect + 0x28), &ff->value, g_fDeltaTimeMS);
                }
            }
            if (ff->magnitude > 0)
            {
                ff->magnitude -= g_iDeltaTimeMS;
                if (ff->magnitude >= 0)
                {
                    ExtractF32Key(*(u8 **)((char *)ff->effect + 0x24), &ff->value, g_fDeltaTimeMS);
                }
            }
        }
        if (ff->unk54 <= 0 && ff->unk58 <= 0 && ff->magnitude <= 0)
        {
            ff->effect = NULL;
        }
    }
}

