#include "extension.h"

int extension__recognize(const char* const basename)
{
    const size_t dot_sz = 1;
    size_t       ch_i   = 0;
    const char*  extension;
    // size_t       bname_len;

    if(basename != NULL)
    {
        while((basename[ch_i] != '.') && (basename[ch_i] != '\0'))
        {
            ch_i++;
        }
        // bname_len = strlen(basename);
        if((strlen(basename) > 0) && (ch_i != strlen(basename))) // Not at the end of an extension.
        {
            extension = &basename[ch_i + dot_sz];

            if(!strcmp(extension, "c") || !strcmp(extension, "h"))
            {
                return EXTENSION__C;
            }
            else if(!strcmp(extension, "py"))
            {
                return EXTENSION__PYTHON;
            }
            else if(!strcmp(extension, "html") || !strcmp(extension, "htm"))
            {
                return EXTENSION__HTML;
            }
        }
    }
    return EXTENSION__NONE;
}
