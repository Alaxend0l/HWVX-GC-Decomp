#include "msl_file.h"


int fflush(FILE *stream);
void free(void *ptr);
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
