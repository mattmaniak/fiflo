#include "extension.h"

int extension__recognize(const char* const basename)
{
    const idx_t dot_sz = 1;
    idx_t       char_idx = 0;

    if(basename != NULL)
    {
        while((basename[char_idx] != '.') && (basename[char_idx] != '\0'))
        {
            char_idx++;
        }
        if(char_idx != strlen(basename))
        {
            const char* const extension = &basename[char_idx + dot_sz];

            if(!strcmp(extension, "c") || !strcmp(extension, "h"))
            {
                return C;
            }
        }
    }
    return NONE;
}
