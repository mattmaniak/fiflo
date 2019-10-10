#include "extension.h"

char* extension__recognize(char* const basename)
{
    const size_t dot_sz    = 1;
    const size_t bname_len = strlen(basename);
    size_t       char_i    = 0;

    if(basename == NULL)
    {
        return NULL;
    }
    while((basename[char_i] != '.') && (basename[char_i] != '\0'))
    {
        char_i++;
    }
    if(bname_len && (char_i != bname_len)) // Not at the end of an extension.
    {
        return &basename[char_i + dot_sz];
    }
    return NULL;
}
