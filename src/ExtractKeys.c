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
