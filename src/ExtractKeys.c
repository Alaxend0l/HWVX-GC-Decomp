#include "common.h"

typedef struct TextureKeyAnim {
    u32 numKeys;
    u32 *keys;
    u32 frameTime;
} TextureKeyAnim;

typedef struct TextureKeyState {
    u16 texture;
    u16 key;
    f32 time;
    u8 reset;
} TextureKeyState;

void ExtractF32Key(u8 *track, u8 *state, f32 dt) {
    typedef struct Key {
        f32 value;
        u8 active;
        u8 interp;
        u8 pad[2];
        f32 duration;
    } Key;
    typedef struct KeyTrack {
        u32 count;
        Key *keys;
    } KeyTrack;
    typedef struct KeyState {
        f32 value;
        f32 slope;
        f32 time;
        u16 index;
        u16 current;
        u8 reset;
    } KeyState;
    KeyTrack *t = (KeyTrack *)track;
    KeyState *s = (KeyState *)state;
    u32 reset;
    u16 i;
    Key *keys;
    Key *key;
    f32 f;

    reset = 0;
    if (s->reset) {
        s->reset = 0;
        reset = 1;
        s->index = 0;
        s->time = 0.0f;
    }
    if (s->index >= t->count) {
        return;
    }
    s->time += dt;
    if (reset || s->time >= t->keys[s->index].duration) {
        s->current = 0;
        for (;;) {
            if (s->time < t->keys[s->index].duration) {
                break;
            }
            s->current = s->index;
            s->time -= t->keys[s->index].duration;
            s->index++;
            if (s->index >= t->count) {
                for (i = 0; i < t->count; i++) {
                    if (t->keys[i].active) {
                        s->index = i;
                        break;
                    }
                }
                if (s->index >= t->count) {
                    s->value = t->keys[s->current].value;
                    return;
                }
                break;
            }
        }
        s->slope = t->keys[s->index].value - t->keys[s->current].value;
        key = &t->keys[s->index];
        if (key->interp == 0) {
            f = 0.0f;
            if (key->duration > 0.0f) {
                f = 1.0f / key->duration;
            }
            s->slope *= f;
        }
    }
    keys = t->keys;
    key = &keys[s->index];
    if (key->interp == 0) {
        s->value = s->slope * s->time + keys[s->current].value;
        return;
    }
    f = s->time / key->duration;
    if (key->interp == 1) {
        f = 1.0f - (1.0f - f) * (1.0f - f);
    } else if (key->interp == 2) {
        f = f * f;
    } else {
        f = 0.0f;
    }
    s->value = f * s->slope + keys[s->current].value;
}

void ExtractTextureKey(TextureKeyAnim *anim, TextureKeyState *state, f32 dt) {
    if (state->reset) {
        if (anim->numKeys == 0) {
            state->texture = 0;
            return;
        }
        state->texture = anim->keys[0];
        state->key = 0;
        state->time = 0.0f;
        state->reset = 0;
    }
    state->time += dt;
    if (state->time >= anim->frameTime) {
        state->key++;
        if (state->key == anim->numKeys) {
            state->key = 0;
        }
        state->texture = anim->keys[state->key];
        state->time -= anim->frameTime;
    }
}
