#include "path.h"

bool path__extract_pathname_from_arg(Buff_t* Buffer)
{
    size_t char_idx           = 0;
    size_t last_slash_pos_idx = 0;

    while(Buffer->fname[char_idx] != '\0')
    {
        if(Buffer->fname[char_idx] == '/')
        {
            if(char_idx == 0) // Cares about the absolute path.
            {
                last_slash_pos_idx = 1;
                break;
            }
            last_slash_pos_idx = char_idx;
        }
        char_idx++;
    }
    strncpy(Buffer->pathname, Buffer->fname, last_slash_pos_idx);
    Buffer->pathname[last_slash_pos_idx] = '\0';

    if(chdir(Buffer->pathname) == ERROR)
    {
        fprintf(stderr,
                "Can't change the direcory to get the parent file path.\n");
        return false;
    }

    if((Buffer->pathname = getcwd(Buffer->pathname, PATH_MAX)) == NULL)
    {
        fprintf(stderr, "Can't get the current directory. Too long.\n");
        return false;
    }
    return true;
}

void path__extract_basename_from_arg(Buff_t* Buffer)
{
    size_t char_idx           = strlen(Buffer->fname);
    size_t last_slash_pos_idx = 0;

    while(char_idx > 0)
    {
        if(Buffer->fname[char_idx] == '/')
        {
            last_slash_pos_idx = char_idx + SLASH_SZ;
            break;
        }
        char_idx--;
    }
    strcpy(Buffer->basename, &Buffer->fname[last_slash_pos_idx]);
}

void path__merge_pathname_and_basename(Buff_t* Buffer)
{
    strcpy(Buffer->fname, Buffer->pathname);

    Buffer->fname[strlen(Buffer->fname)] = '/';
    Buffer->fname[strlen(Buffer->fname)] = '\0';

    strcat(Buffer->fname, Buffer->basename);
}
