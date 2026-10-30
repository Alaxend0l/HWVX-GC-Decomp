#include "common.h"


void UpdatePointers(u32 *offsets, u32 count, u8 *base) {
    u32 i;

    for (i = 0; i < count; i++) {
        *(u32 *)(base + offsets[i]) += (u32)base;
    }
}

void *LoadDataChunkFile(const char *name) {
    s32 File_Open(s32 dev, const char *name, s32 mode, s32 flags);
    u32 File_Read(u32 id, s32 size, void *data);
    void File_Close(s32 handle);
    void *Allocate(u32 size, u32 flags, const char *name);
    void Deallocate(void *p);
    struct {
        u32 count;
        u32 size;
    } header;
    u32 *relocs;
    u8 *data;
    s32 handle;

    handle = File_Open(0, name, 0, 0);
    if (handle == -1) {
        return NULL;
    }
    header.size = 0;
    header.count = 0;
    File_Read(handle, 8, &header);
    data = Allocate(header.size, 0, "");
    relocs = Allocate(header.count * 4, 0, "");
    if (File_Read(handle, header.count * 4, relocs) == 0) {
        return NULL;
    }
    if (File_Read(handle, header.size, data) == 0) {
        return NULL;
    }
    File_Close(handle);
    UpdatePointers(relocs, header.count, data);
    Deallocate(relocs);
    return data;
}

