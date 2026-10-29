

#include "common.h"

extern u8 g_CurrentViews;

s32 Gfx_GetCameraPositions(Vector4 *positions);
f32 PSVECSquareDistance(f32 *a, f32 *b);

void Thing_Release(void *);
s32 EventSeq_Init(u8 *seq, u8 *data)
{
    void QuaternionToSubMatrix(void *quat, void *mtx);

    Thing_Init(seq, data);
    *(Vector4 *)(seq + 0x0) = *(Vector4 *)(data + 0x20);
    *(Vector4 *)(seq + 0x50) = *(Vector4 *)(data + 0x10);
    *(s32 *)(seq + 0x94) = *(s32 *)(data + 0x44);
    *(Vector4 *)(seq + 0xC0) = *(Vector4 *)(data + 0x30);
    *(f32 *)(seq + 0xF8) = *(f32 *)(data + 0x48);
    QuaternionToSubMatrix(seq + 0x50, seq + 0x10);
    *(Vector4 *)(seq + 0x40) = *(Vector4 *)(data + 0x0);
    *(s32 *)(seq + 0xD0) = *(s32 *)(data + 0x40);
    *(s32 *)(seq + 0xDC) = 0;
    *(s32 *)(seq + 0xE0) = 0;
    *(s32 *)(seq + 0xD4) = 0;
    *(u8 *)(seq + 0xF0) = 0;
    *(u16 *)(seq + 0x80) = 0;
    *(Vector4 *)(seq + 0x50) = *(Vector4 *)(data + 0x10);
    *(u32 *)(seq + 0x74) |= 0x80202;
    return 1;
}

void EventSeq_Release(void *arg0)
{
    Thing_Release(arg0);
}

void EventSeq_Advanced_GetOffset(u8 *seq, void *arg1, Vector4 *out) {
    typedef struct {
        f32 base;
        f32 range;
        f32 scale;
        f32 speed;
        u8 axis;
        u8 damped;
        u8 spin;
    } EventSeqCircle;
    typedef struct {
        f32 base;
        f32 range;
        u8 pad8;
        u8 type;
        u8 padA[2];
        void *data;
    } EventSeqAdvanced;
    f32 RandomAdd(f32 base, f32 range);
    void RandomNumberGenerator_GetRandomVector(Vector4 *out);
    void SinCos(f32 *s, f32 *c, f32 angle);
    f32 scale;

    switch (seq[0x21]) {
    case 1: {
        f32 *range = *(f32 **)(seq + 0x28);

        out->x = RandomAdd(range[0], range[1]);
        out->y = RandomAdd(range[2], range[3]);
        out->z = RandomAdd(range[4], range[5]);
        break;
    }
    case 2: {
        EventSeqAdvanced *adv = *(EventSeqAdvanced **)(seq + 0x28);

        scale = RandomAdd(adv->base, adv->range);
        switch (adv->type) {
        case 0: {
            Vector4 *dir = adv->data;

            RandomNumberGenerator_GetRandomVector(out);
            out->x *= scale * dir->x;
            out->y *= scale * dir->y;
            out->z *= scale * dir->z;
            break;
        }
        case 1: {
            Vector4 *dir = adv->data;
            Vector4 mul __attribute__((aligned(16))) = {1.0f, 1.0f, 1.0f, 0.0f};

            RandomNumberGenerator_GetRandomVector(out);
            switch (((u8 *)dir)[0x10]) {
            case 4:
                if (out->x < 0.0f) {
                    out->x *= -1.0f;
                }
                mul.x = dir->x;
                break;
            case 5:
                if (out->x > 0.0f) {
                    out->x *= -1.0f;
                }
                mul.x = dir->x;
                break;
            case 2:
                if (out->y < 0.0f) {
                    out->y *= -1.0f;
                }
                mul.y = dir->x;
                break;
            case 3:
                if (out->y > 0.0f) {
                    out->y *= -1.0f;
                }
                mul.y = dir->x;
                break;
            case 0:
                if (out->z < 0.0f) {
                    out->z *= -1.0f;
                }
                mul.z = dir->x;
                break;
            case 1:
                if (out->z > 0.0f) {
                    out->z *= -1.0f;
                }
                mul.z = dir->x;
                break;
            }
            out->x *= scale * mul.x;
            out->y *= scale * mul.y;
            out->z *= scale * mul.z;
            break;
        }
        case 2: {
            EventSeqCircle *circle = adv->data;
            f32 height = RandomAdd(circle->base, circle->range);
            f32 t = 1.0f;
            f32 angle;
            f32 s;
            f32 c;

            if (circle->range) {
                if (circle->damped && circle->scale) {
                    t = 1.0f - (height - circle->base) / (circle->range * circle->scale);
                    scale *= t;
                } else {
                    t = 1.0f - (height - circle->base) / circle->range;
                }
            }
            if (circle->spin) {
                if (circle->damped) {
                    angle = t * (360.0f * circle->speed * circle->scale);
                } else {
                    angle = t * (360.0f * circle->speed);
                }
            } else {
                angle = RandomAdd(0.0f, 360.0f);
            }
            SinCos(&s, &c, angle);
            switch (circle->axis) {
            case 0:
                out->x = height;
                out->y = scale * c;
                out->z = scale * s;
                break;
            case 1:
                out->x = scale * c;
                out->y = height;
                out->z = scale * s;
                break;
            case 2:
                out->x = scale * c;
                out->y = scale * s;
                out->z = height;
                break;
            }
            break;
        }
        }
        break;
    }
    default:
        out->w = out->x = out->y = out->z = 0.0f;
        break;
    }
}

s32 EventSeq_Move(void *thing) {
    return 0;
}

s32 EventSeq_InCameraRange(u8 *seq, f32 range)
{
    Vector4 positions[5] __attribute__((aligned(16)));
    Vector4 *pos;
    f32 dist;
    u32 count;
    u32 i;

    if (range == 0.0f)
    {
        return 1;
    }
    if (*(s8 *)(seq + 0xF1))
    {
        count = Gfx_GetCameraPositions(positions);
        *(f32 *)(seq + 0xF4) = PSVECSquareDistance((f32 *)(seq + 0x40), (f32 *)&positions[0]);
        if (count > 1)
        {
            for (i = 0, pos = positions; i < 5; i++, pos++)
            {
                if (g_CurrentViews & (1 << i))
                {
                    dist = PSVECSquareDistance((f32 *)(seq + 0x40), (f32 *)pos);
                    if (dist < *(f32 *)(seq + 0xF4))
                    {
                        *(f32 *)(seq + 0xF4) = dist;
                    }
                }
            }
        }
        *(s8 *)(seq + 0xF1) = 0;
    }
    return range * range > *(f32 *)(seq + 0xF4);
}

s32 EventSeq_Invalidate(void *arg0, u32 arg1, u32 arg2)
{
    u32 field;

    field = M2C_FIELD(arg0, u32 *, 0xD0);
    if (field >= arg1 && field < arg2)
    {
        return 1;
    }
    return 0;
}
