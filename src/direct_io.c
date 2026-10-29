

#include "msl_file.h"

typedef unsigned long size_t;
size_t __fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file);
size_t fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file)
{
    return __fwrite(ptr, memb_size, num_memb, file);
}
