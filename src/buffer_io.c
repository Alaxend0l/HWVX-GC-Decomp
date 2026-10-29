#include "msl_file.h"


void __prep_buffer(FILE *file) {
    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len -= file->position & file->buffer_alignment;
    file->buffer_pos = file->position;
}
