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
