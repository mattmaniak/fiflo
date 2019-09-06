#include "fformats.h"

const int FFORMATS_EXTENSION;

void fformats__recognize_extension(const char* const basename)
{
    idx_t char_idx = 0;

    if(basename != NULL)
    {
        while((basename[char_idx] != '.') && (basename[char_idx] != '\0'))
        {
            char_idx++;
        }
        if(char_idx != strlen(basename))
        {
            const idx_t       dot_sz = 1;
            const char* const extension = &basename[char_idx + dot_sz];

            puts(extension);
        }
    }
}
