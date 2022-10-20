#include "print_utils.h"

/** Prints in binary following big endian order */
void    print_binary_be(const void *const param, const size_t size)
{
    uint8_t *mask = (uint8_t*)param;

    for (size_t i = 0; i < size ; ++i)
    {
        for (int j = 7; j >= 0; --j)
        {
            printf("%d", (*mask >> j) & 0x1);
        }
        ++mask;
    }
}

/** Prints in binary following small endian order */
void    print_binary_se(const void *const param, const size_t size)
{
    uint8_t *mask = (uint8_t*)param + size - 1;

    for (size_t i = 0; i < size; ++i)
    {
        for (int j = 7; j >= 0; --j)
        {
            printf("%d", (*mask >> j) & 0x1);
        }
        printf (" ");
        --mask;
    }
}



int     print_failed(const char *const caller, const int32_t err)
{
    fprintf (stderr, "%s failed: %d: %s\n", caller, err, strerror(err));
    return (err);
}
