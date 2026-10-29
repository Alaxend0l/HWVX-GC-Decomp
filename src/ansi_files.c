#include "msl_file.h"


int fflush(FILE *stream);
int fclose(FILE *stream);
void free(void *ptr);
s32 __read_console(s32 handle, u8 *buffer, u32 *count, void *idle);
int __write_console(s32 handle, u8 *buffer, u32 *count, void *idle_proc);
s32 __close_console(s32 handle);

static u8 stdin_buff[0x100];
static u8 stdout_buff[0x100];
static u8 stderr_buff[0x100];

FILE __files[4] = {
    { 0, { 0, 1, 1, 2, 0, 0 }, { 0, 0, 0, 0 }, 0, { 0 }, 0, stdin_buff, sizeof(stdin_buff), stdin_buff, 0, 0, 0, 0,
      NULL, (void *)__read_console, (void *)__write_console, (void *)__close_console, NULL, &__files[1] },
    { 1, { 0, 2, 1, 2, 0, 0 }, { 0, 0, 0, 0 }, 0, { 0 }, 0, stdout_buff, sizeof(stdout_buff), stdout_buff, 0, 0, 0, 0,
      NULL, (void *)__read_console, (void *)__write_console, (void *)__close_console, NULL, &__files[2] },
    { 2, { 0, 2, 0, 2, 0, 0 }, { 0, 0, 0, 0 }, 0, { 0 }, 0, stderr_buff, sizeof(stderr_buff), stderr_buff, 0, 0, 0, 0,
      NULL, (void *)__read_console, (void *)__write_console, (void *)__close_console, NULL, &__files[3] },
};
s32 __flush_all(void) {
    s32 retval = 0;
    FILE *stream = &__files[0];

    while (stream) {
        if (stream->mode.file_kind && fflush(stream)) {
            retval = -1;
        }
        stream = stream->next_file_struct;
    }
    return retval;
}

void __close_all(void) {
    FILE *p = &__files[0];
    FILE *plast;

    while (p) {
        if (p->mode.file_kind != 0) {
            fclose(p);
        }

        plast = p;
        p = p->next_file_struct;

        if (plast->is_dynamically_allocated) {
            free(plast);
        } else {
            plast->mode.file_kind = 3;
            if (p && p->is_dynamically_allocated) {
                plast->next_file_struct = NULL;
            }
        }
    }
}

