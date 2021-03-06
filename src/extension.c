#include "extension.h"

char* extension__recognize(char* const basename)
{
    const size_t dot_sz    = 1;
    const size_t bname_len = strlen(basename);
    size_t       ch_i      = 0;

    if(basename == NULL)
    {
        return NULL;
    }
    while((basename[ch_i] != '.') && (basename[ch_i] != '\0'))
    {
        ch_i++;
    }
    // Not at the end of an extension.
    if((bname_len > 0) && (ch_i != bname_len))
    {
        return &basename[ch_i + dot_sz];
    }
    return NULL;
}
