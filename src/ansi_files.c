#include "msl_file.h"


extern FILE __files[];
int fflush(FILE *stream);
int fclose(FILE *stream);
void free(void *ptr);
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

