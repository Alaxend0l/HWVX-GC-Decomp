#include "msl_file.h"


int __flush_buffer(FILE *file, u32 *bytes_flushed) {
    u32 buffer_len;
    int ioresult;

    buffer_len = file->buffer_ptr - file->buffer;
    if (buffer_len) {
        file->buffer_len = buffer_len;
        ioresult = file->write_fn(file->handle, file->buffer, &file->buffer_len, file->ref_con);
        if (bytes_flushed) {
            *bytes_flushed = file->buffer_len;
        }
        if (ioresult) {
            return ioresult;
        }
        file->position += file->buffer_len;
    }
    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len -= file->position & file->buffer_alignment;
    file->buffer_pos = file->position;
    return 0;
}

void __prep_buffer(FILE *file) {
    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len -= file->position & file->buffer_alignment;
    file->buffer_pos = file->position;
}
