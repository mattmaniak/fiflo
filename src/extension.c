#include "extension.h"

int extension__recognize(const char* const basename)
{
    const idx_t dot_sz = 1;
    idx_t       ch_idx = 0;

    if(basename != NULL)
    {
        while((basename[ch_idx] != '.') && (basename[ch_idx] != '\0'))
        {
            ch_idx++;
        }
        if(ch_idx != strlen(basename)) // Not at the end of the extension.
        {
            const char* const extension = &basename[ch_idx + dot_sz];

            if(!strcmp(extension, "c") || !strcmp(extension, "h"))
            {
                return EXTENSION__C;
            }
            else if(!strcmp(extension, "py"))
            {
                return EXTENSION__PYTHON;
            }
        }
    }
    return EXTENSION__NONE;
}
