#include "common.h"


s32 World_Create(void) {
    extern s32 m_VertIn;
    extern s32 g_drawShadowsFlag;
    m_VertIn = 0;
    g_drawShadowsFlag = 1;
    g_worldData.world = NULL;
    return 1;
}

void World_Destroy(void) {
    if (g_worldData.world != NULL) {
        World_Unload();
    }
}

void World_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void *World_GetTileCar(u32 index) {
    void *world = g_worldData.world;
    if (index < M2C_FIELD(world, u32 *, 0x8)) {
        return M2C_FIELD(world, u8 **, 0xC) + index * 0x60;
    }
    return NULL;
}

void *World_GetNavHolder(void) {
    return M2C_FIELD(g_worldData.world, void **, 0x78);
}

void *World_GetGameData(void) {
    void *world = g_worldData.world;
    if (world != NULL) {
        return M2C_FIELD(world, void **, 0x9C);
    }
    return NULL;
}

s32 ActionItemChangeMode(u32 *action) {
    u32 i;
    u8 *item;
    u8 *obj;

    for (i = 0; i < action[1]; i++) {
        item = (u8 *)action[i + 2];
        if (action[0] == 2) {
            if (*(u32 *)(item + 0x28) == 1) {
                *(u32 *)(item + 0x28) = 0;
                obj = *(u8 **)(item + 0x24);
                if (obj != NULL) {
                    (*(void (**)(void *))(*(u8 **)(obj + 0x8C) + 4))(obj);
                    *(u32 *)(item + 0x18) = *(u32 *)(item + 0x14);
                    *(u8 **)(item + 0x24) = NULL;
                }
            } else {
                *(u32 *)(item + 0x28) = 1;
                *(u32 *)(item + 0x18) = 1;
            }
        } else {
            *(u32 *)(item + 0x28) = action[0];
            if (action[0] == 1) {
                *(u32 *)(item + 0x18) = 1;
            }
            if (action[0] == 0) {
                obj = *(u8 **)(item + 0x24);
                if (obj != NULL) {
                    (*(void (**)(void *))(*(u8 **)(obj + 0x8C) + 4))(obj);
                    *(u32 *)(item + 0x18) = *(u32 *)(item + 0x14);
                    *(u8 **)(item + 0x24) = NULL;
                }
            }
        }
    }
    return 1;
}

void RenderQuadrant(u8 *quadrant) {
    typedef struct { void *pad0; void *pad4; void *geom; } StaticGeom;
    typedef struct { u8 *geom; u8 noDraw; u8 pad5; u8 hasDist; u8 visible; f32 dist; u8 padC[0xC]; u32 slaves[2]; } Model;
    typedef struct { Vector4 pos; Vector4 quat; u8 pad20[4]; Model *model; f32 dist; u8 pad2C[8]; u8 useDist; u8 pad35[7]; f32 radius; } Instance;
    typedef struct { u32 pad0; u32 count; Instance *instances; } InstanceList;
    typedef struct { u8 pad0[0x10]; f32 mtx[3][4]; Vector4 pos; Vector4 quat; u8 pad60[0x14]; u32 unk74; u8 pad78[0x78]; } DrawThing;
    void Gfx_DrawGeometry(void *geom, void *mtx);
    f32 Gfx_GetMinCameraDistSquared(void *pos);
    s32 Gfx_FrustrumCullSphereTestAllWindows(void *, f32);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void Thing_DrawSharedSlaves(void *thing, void *slaves);
    u32 i;
    u32 j;
    Instance *inst;
    Model *model;
    DrawThing *t;
    f32 distSq;
    DrawThing thing;

    if (quadrant == NULL || *(s8 *)(quadrant + 0x34) == 0 || *(StaticGeom **)(quadrant + 0x10) == NULL) {
        return;
    }
    for (i = 0; i < *(u32 *)(quadrant + 0xC); i++) {
        Gfx_DrawGeometry((*(StaticGeom **)(quadrant + 0x10))[i].geom, NULL);
    }
    for (j = 0; j < (*(InstanceList **)(quadrant + 0x30))->count; j++) {
        inst = &(*(InstanceList **)(quadrant + 0x30))->instances[j];
        model = inst->model;
        if (model->visible == 0) {
            continue;
        }
        t = &thing;
        distSq = Gfx_GetMinCameraDistSquared(inst);
        if ((u32)ViewManager_GetViewFormat() > 1) {
            distSq *= 4.0f;
        }
        if (inst->useDist) {
            if (distSq > inst->dist * inst->dist) {
                continue;
            }
        } else if (inst->model->hasDist && distSq > inst->model->dist * inst->model->dist) {
            continue;
        }
        if (Gfx_FrustrumCullSphereTestAllWindows(inst, inst->radius)) {
            t->unk74 = 0;
            t->quat = inst->quat;
            QuaternionToSubMatrix(&t->quat, t->mtx);
            inst->pos.w = 1.0f;
            t->pos = inst->pos;
            Thing_DrawSharedSlaves(t, model->slaves);
            if (model->noDraw == 0) {
                Gfx_DrawGeometry(model->geom, t->mtx);
            }
        }
    }
}

s32 TriggerVariableValueCheck(u32 *data) {
    f32 GetCodeVariable(s32 var, u8 *thing);
    u32 i;
    f32 value;
    s32 op;
    u8 **var;

    switch (data[data[0] + 1]) {
    case 0:
        op = data[data[0] + 3];
        value = *(f32 *)&data[data[0] + 2];
        break;
    case 1:
        op = data[data[0] + 3];
        value = *(f32 *)(*(u8 **)data[data[0] + 2] + 4);
        break;
    case 2:
        value = GetCodeVariable(data[data[0] + 2], (u8 *)data[data[0] + 3]);
        if (data[data[0] + 2] < 20) {
            op = data[data[0] + 4];
        } else {
            op = data[data[0] + 3];
        }
        break;
    }
    for (i = 0; i < data[0]; i++) {
        var = (u8 **)data[i + 1];
        switch (op) {
        case 20:
            if (*(f32 *)(*var + 4) < value) {
                return 1;
            }
            break;
        case 21:
            if (*(f32 *)(*var + 4) > value) {
                return 1;
            }
            break;
        case 22:
            if (*(f32 *)(*var + 4) == value) {
                return 1;
            }
            break;
        case 47:
            if (*(f32 *)(*var + 4) >= value) {
                return 1;
            }
            break;
        case 48:
            if (*(f32 *)(*var + 4) <= value) {
                return 1;
            }
            break;
        }
    }
    return 0;
}

s32 ActionVariableChangeValue(u32 *action) {
    u32 i;
    f32 value;
    u8 **var;

    switch (action[action[1] + 2]) {
    case 0:
        value = *(f32 *)&action[action[1] + 3];
        break;
    case 1:
        value = *(f32 *)(*(u8 **)action[action[1] + 3] + 4);
        break;
    case 2:
        value = GetCodeVariable(action[action[1] + 3], (u8 *)action[action[1] + 4]);
        break;
    }
    for (i = 0; i < action[1]; i++) {
        var = (u8 **)action[i + 2];
        switch (action[0]) {
        case 0xF:
            *(f32 *)(*var + 4) += value;
            break;
        case 0x10:
            *(f32 *)(*var + 4) -= value;
            break;
        case 0x2D:
            *(f32 *)(*var + 4) *= value;
            break;
        case 0x2E:
            if (value) {
                *(f32 *)(*var + 4) /= value;
            }
            break;
        case 0x11:
            *(f32 *)(*var + 4) = value;
            break;
        }
    }
    return 1;
}

s32 ActionVariableChangeMode(u32 *action) {
    u32 i;
    u8 *var;

    for (i = 0; i < action[1]; i++) {
        var = M2C_FIELD(*(void **)action[i + 2], u8 **, 0xC);
        if (var != NULL) {
            *var = action[0];
        }
    }
    return 1;
}

s32 ActionTimerChangeValue(u32 *action) {
    u32 i;
    f32 value;
    u8 **timer;

    switch (action[action[1] + 2]) {
    case 0:
        value = *(f32 *)&action[action[1] + 3];
        break;
    case 1:
        value = *(f32 *)(*(u8 **)action[action[1] + 3] + 4);
        break;
    case 2:
        value = GetCodeVariable(action[action[1] + 3], (u8 *)action[action[1] + 4]);
        break;
    }
    for (i = 0; i < action[1]; i++) {
        timer = (u8 **)action[i + 2];
        switch (action[0]) {
        case 0xF:
            *(f32 *)(*timer + 8) += value;
            break;
        case 0x10:
            *(f32 *)(*timer + 8) -= value;
            break;
        case 0x2D:
            *(f32 *)(*timer + 8) *= value;
            break;
        case 0x2E:
            if (value) {
                *(f32 *)(*timer + 8) /= value;
            }
            break;
        case 0x11:
            *(f32 *)(*timer + 8) = value;
            break;
        }
    }
    return 1;
}

s32 ActionTimerChangeMode(u32 *action) {
    u8 **item;
    u32 i;

    for (i = 0; i < action[2]; i++) {
        item = (u8 **)action[i + 3];
        M2C_FIELD(M2C_FIELD(*item, u8 **, 0xC), u8 *, 0) = action[0];
        M2C_FIELD(M2C_FIELD(*item, u8 **, 0xC), u8 *, 1) = action[1];
    }
    return 1;
}

s32 TriggerTimerValueCheck(u32 *data) {
    f32 GetCodeVariable(s32 var, u8 *thing);
    u32 i;
    f32 value;
    s32 op;
    u8 **timer;

    switch (data[data[0] + 1]) {
    case 0:
        op = data[data[0] + 3];
        value = *(f32 *)&data[data[0] + 2];
        break;
    case 1:
        op = data[data[0] + 3];
        value = *(f32 *)(*(u8 **)data[data[0] + 2] + 4);
        break;
    case 2:
        value = GetCodeVariable(data[data[0] + 2], (u8 *)data[data[0] + 3]);
        if (data[data[0] + 2] < 20) {
            op = data[data[0] + 4];
        } else {
            op = data[data[0] + 3];
        }
        break;
    }
    for (i = 0; i < data[0]; i++) {
        timer = (u8 **)data[i + 1];
        switch (op) {
        case 20:
            if (*(f32 *)(*timer + 8) < value) {
                return 1;
            }
            break;
        case 21:
            if (*(f32 *)(*timer + 8) > value) {
                return 1;
            }
            break;
        case 22:
            if (*(f32 *)(*timer + 8) == value) {
                return 1;
            }
            break;
        case 47:
            if (*(f32 *)(*timer + 8) >= value) {
                return 1;
            }
            break;
        case 48:
            if (*(f32 *)(*timer + 8) <= value) {
                return 1;
            }
            break;
        }
    }
    return 0;
}

