

#include "msl_file.h"

typedef unsigned long size_t;
size_t __fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file);
s32 fwide(FILE *file, s32 mode);
void *__memrchr(const void *s, s32 c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
s32 fseek(FILE *file, long offset, s32 file_mode);
void __stdio_atexit(void);
s32 __flush_buffer(FILE *file, size_t *bytes_flushed);
void __prep_buffer(FILE *file);
#pragma use_lmw_stmw on
size_t __fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file) {
    s32 always_buffer;
    u8 *write_ptr;
    size_t bytes_to_go;
    size_t bytes_written;
    size_t num_bytes;
    s32 ioresult;

    if (fwide(file, 0) == 0) {
        fwide(file, -1);
    }

    bytes_to_go = memb_size * num_memb;
    if (!bytes_to_go || file->state.error || file->mode.file_kind == 0) {
        return 0;
    }

    if (file->mode.file_kind == 2) {
        __stdio_atexit();
    }

    always_buffer = 1;
    if (!((!file->mode.binary_io || file->mode.buffer_mode == 2) ? 1 : 0) && file->mode.buffer_mode != 1) {
        always_buffer = 0;
    }

    if (file->state.io_state == 0 && (file->mode.io_mode & 2)) {
        if (file->mode.io_mode & 4) {
            if (fseek(file, 0, 2)) {
                return 0;
            }
        }
        file->state.io_state = 1;
        __prep_buffer(file);
    }

    if (file->state.io_state != 1) {
        file->state.error = 1;
        file->buffer_len = 0;
        return 0;
    }

    write_ptr = (u8 *)ptr;
    bytes_written = 0;

    if (bytes_to_go && (file->buffer_ptr != file->buffer || always_buffer)) {
        file->buffer_len = file->buffer_size - (file->buffer_ptr - file->buffer);
        do {
            u8 *newline = NULL;
            num_bytes = file->buffer_len;
            if (num_bytes > bytes_to_go) {
                num_bytes = bytes_to_go;
            }
            if (file->mode.buffer_mode == 1 && num_bytes) {
                if ((newline = (u8 *)__memrchr(write_ptr, '\n', num_bytes)) != NULL) {
                    num_bytes = newline + 1 - write_ptr;
                }
            }
            if (num_bytes) {
                memcpy(file->buffer_ptr, write_ptr, num_bytes);
                write_ptr += num_bytes;
                bytes_written += num_bytes;
                bytes_to_go -= num_bytes;
                file->buffer_ptr += num_bytes;
                file->buffer_len -= num_bytes;
            }
            if (!file->buffer_len || newline != NULL || file->mode.buffer_mode == 0) {
                ioresult = __flush_buffer(file, NULL);
                if (ioresult) {
                    file->state.error = 1;
                    file->buffer_len = 0;
                    bytes_to_go = 0;
                    break;
                }
            }
        } while (bytes_to_go && always_buffer);
    }

    if (bytes_to_go && !always_buffer) {
        u8 *save_buffer = file->buffer;
        size_t save_size = file->buffer_size;
        file->buffer = write_ptr;
        file->buffer_size = bytes_to_go;
        file->buffer_ptr = write_ptr + bytes_to_go;
        if (__flush_buffer(file, &num_bytes) != 0) {
            file->state.error = 1;
            file->buffer_len = 0;
        }
        bytes_written += num_bytes;
        file->buffer = save_buffer;
        file->buffer_size = save_size;
        __prep_buffer(file);
        file->buffer_len = 0;
    }

    if (file->mode.buffer_mode != 2) {
        file->buffer_len = 0;
    }

    return (bytes_written + memb_size - 1) / memb_size;
}
#pragma use_lmw_stmw reset
size_t fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file)
{
    return __fwrite(ptr, memb_size, num_memb, file);
}
