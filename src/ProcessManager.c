
#include "common.h"

typedef struct Process {
    void (*init)(void);
    void (*update)(void);
    s32 arg;
    u32 flags;
    s32 unk10;
    s32 unk14;
    char name[16];
} Process;

typedef struct ProcessManager {
    u32 count;
    s32 unk4;
    s32 unk8;
    Process processes[27];
} ProcessManager;

ProcessManager g_processManager;

char *strcpy(char *dst, const char *src);

#pragma push
#pragma use_lmw_stmw on
#pragma auto_inline off

void ProcessManager_Create(void) {
}

void ProcessManager_Init(void) {
    g_processManager.count = 0;
}

void ProcessManager_Add(char *name, void (*init)(void), void (*update)(void), s32 arg, u32 flags) {
    u32 index = g_processManager.count++;

    if (name != NULL) {
        strcpy(g_processManager.processes[index].name, name);
    }
    else {
        strcpy(g_processManager.processes[index].name, "");
    }
    if (init != NULL) {
        init();
    }
    g_processManager.processes[index].init = init;
    g_processManager.processes[index].update = update;
    g_processManager.processes[index].arg = arg;
    g_processManager.processes[index].flags = flags;
    g_processManager.processes[index].unk10 = 0;
    g_processManager.processes[index].unk14 = 0;
}

