#include "buffer.h"
#include "path.h"

bool path__extract_path_from_arg(Buff_t* Buffer)
{
    size_t char_idx           = 0;
    size_t last_slash_pos_idx = 0;

    while(Buffer->fname[char_idx] != '\0')
    {
        if(Buffer->fname[char_idx] == '/')
        {
            last_slash_pos_idx = char_idx;
        }
        char_idx++;
    }
    strncpy(Buffer->path, Buffer->fname, last_slash_pos_idx);

    Buffer->path[last_slash_pos_idx] = '\0';

    if(chdir(Buffer->path) == ERROR)
    {
        fprintf(stderr, "Can't change the current direcory.\n");
        return false;
    }

    if(getcwd(Buffer->path, PATH_MAX) != NULL)
    {
        Buffer->path = getcwd(Buffer->path, PATH_MAX);
    }
    else
    {
        fprintf(stderr, "Can't get the current directory. Too long.\n");
        return false;
    }
    puts(Buffer->path);

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

    puts(Buffer->basename);
}

void path__merge_path_and_basename(Buff_t* Buffer)
{
    strcpy(Buffer->fname, Buffer->path);
    Buffer->fname[strlen(Buffer->fname)] = '/';

    strcat(Buffer->fname, Buffer->basename);
}
