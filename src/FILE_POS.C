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
int __flush_buffer(FILE *file, u32 *bytes_flushed);

int _fseek(FILE *file, u32 offset, int file_mode)
{
    int (*position_proc)(u32, u32 *, int, void *);

    if (file->mode.file_kind != 1 || file->state.error) {
        errno = 0x28;
        return -1;
    }
    if (file->state.io_state == 1) {
        if (__flush_buffer(file, NULL)) {
            file->state.error = 1;
            file->buffer_len = 0;
            errno = 0x28;
            return -1;
        }
    }
    if (file_mode == 1) {
        file_mode = 0;
        offset += ftell(file);
    }
    if (file_mode != 2 && file->mode.io_mode != 3 &&
        (file->state.io_state == 2 || file->state.io_state == 3)) {
        if (offset >= file->position || offset < file->buffer_pos) {
            file->state.io_state = 0;
        } else {
            file->buffer_ptr = file->buffer + (offset - file->buffer_pos);
            file->buffer_len = file->position - offset;
            file->state.io_state = 2;
        }
    } else {
        file->state.io_state = 0;
    }
    if (file->state.io_state == 0) {
        if ((position_proc = (int (*)(u32, u32 *, int, void *))file->position_fn) != NULL &&
            position_proc(file->handle, &offset, file_mode, file->ref_con)) {
            file->state.error = 1;
            file->buffer_len = 0;
            errno = 0x28;
            return -1;
        }
        file->state.eof = 0;
        file->position = offset;
        file->buffer_len = 0;
    }
    return 0;
}
