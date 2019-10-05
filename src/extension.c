#include "extension.h"

char* extension__recognize(char* const basename)
{
    const size_t dot_sz = 1;
    size_t       char_i   = 0;

    if(basename == NULL)
    {
        return NULL;
    }
    while((basename[char_i] != '.') && (basename[char_i] != '\0'))
    {
        char_i++;
    }
    // bname_len = strlen(basename);
    if((strlen(basename) > 0) && (char_i != strlen(basename))) // Not at the end of an extension.
    {
        return &basename[char_i + dot_sz];
    }
    return NULL;
}
