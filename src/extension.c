#include "extension.h"

char* extension__recognize(char* const basename)
{
    const size_t dot_sz = 1;
    size_t       ch_i   = 0;

    if(basename == NULL)
    {
        return NULL;
    }
    while((basename[ch_i] != '.') && (basename[ch_i] != '\0'))
    {
        ch_i++;
    }
    // bname_len = strlen(basename);
    if((strlen(basename) > 0) && (ch_i != strlen(basename))) // Not at the end of an extension.
    {
        return &basename[ch_i + dot_sz];
    }
    return NULL;
}
