#include "path.h"

bool path__extract_pathname_from_arg(Buff_t* const Buffer)
{
    size_t ch_i           = 0;
    size_t slash_last_pos = 0;

    while(Buffer->fname[ch_i] != '\0')
    {
        if(Buffer->fname[ch_i] == '/')
        {
            if(ch_i == 0) // Cares about the absolute path.
            {
                slash_last_pos = 1;
                break;
            }
            slash_last_pos = ch_i;
        }
        ch_i++;
    }
    strncpy(Buffer->pathname, Buffer->fname, slash_last_pos);
    Buffer->pathname[slash_last_pos] = '\0';

    if(chdir(Buffer->pathname) == -1)
    {
        fprintf(stderr,
                "Can't change the direcory to get the parent file path.\n");
        return false;
    }

    Buffer->pathname = getcwd(Buffer->pathname, PATH_MAX);
    if(Buffer->pathname == NULL)
    {
        fprintf(stderr, "Can't get the current directory. Too long.\n");
        return false;
    }
    return true;
}

void path__extract_basename_from_arg(Buff_t* const Buffer)
{
    size_t ch_i             = strlen(Buffer->fname);
    size_t slash_last_pos = 0;

    while(ch_i > 0)
    {
        if(Buffer->fname[ch_i] == '/')
        {
            slash_last_pos = ch_i + SIZE__SLASH;
            break;
        }
        ch_i--;
    }
    strcpy(Buffer->basename, &Buffer->fname[slash_last_pos]);
}

void path__merge_pathname_and_basename(Buff_t* const Buffer)
{
    strcpy(Buffer->fname, Buffer->pathname);

    Buffer->fname[strlen(Buffer->fname)] = '/'; // Append the slash.
    Buffer->fname[strlen(Buffer->fname)] = '\0';

    strcat(Buffer->fname, Buffer->basename);
}
