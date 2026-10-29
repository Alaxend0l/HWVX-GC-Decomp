#include "msl_file.h"


long ftell(FILE *file)
{
    int charsInUndoBuffer;
    u32 position;
    u32 kind = file->mode.file_kind;

    if (!(kind == 1 || kind == 2) || file->state.error) {
        errno = 0x28;
        return -1;
    }
    if (file->state.io_state == 0) {
        return file->position;
    }
    position = file->buffer_pos + (file->buffer_ptr - file->buffer);
    if (file->state.io_state >= 3) {
        charsInUndoBuffer = file->state.io_state - 2;
        position -= charsInUndoBuffer;
    }
    return position;
}

int fseek(FILE *file, long offset, int file_mode)
{
    return _fseek(file, offset, file_mode);
}

