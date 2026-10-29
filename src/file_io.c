#include "msl_file.h"


int fflush(FILE *stream);
void free(void *ptr);
int fflush(FILE *file)
{
    int __flush_all(void);
    int __flush_buffer(FILE *file, u32 *bytes_flushed);
    long ftell(FILE *file);
    long pos;

    if (file == NULL) {
        return __flush_all();
    }
    if (file->state.error || file->mode.file_kind == 0) {
        return -1;
    }
    if (file->mode.io_mode == 1) {
        return 0;
    }
    if (file->state.io_state >= 3) {
        file->state.io_state = 2;
    }
    if (file->state.io_state == 2) {
        file->buffer_len = 0;
    }
    if (file->state.io_state != 1) {
        file->state.io_state = 0;
        return 0;
    }
    if (file->mode.file_kind != 1) {
        pos = 0;
    } else {
        pos = ftell(file);
    }
    if (__flush_buffer(file, NULL)) {
        file->state.error = 1;
        file->buffer_len = 0;
        return -1;
    }
    file->state.io_state = 0;
    file->position = pos;
    file->buffer_len = 0;
    return 0;
}

int fclose(FILE *file) {
    int flush_result, close_result;

    if (file == NULL) {
        return -1;
    }
    if (file->mode.file_kind == 0) {
        return 0;
    }

    flush_result = fflush(file);
    close_result = (*file->close_fn)(file->handle);
    file->mode.file_kind = 0;
    file->handle = 0;

    if (file->state.free_buffer) {
        free(file->buffer);
    }

    return ((flush_result || close_result) ? -1 : 0);
}
