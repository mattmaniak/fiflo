#include "path.h"

bool path__extract_pathname_from_arg(V_file* const v_file)
{
    size_t char_i           = 0;
    size_t slash_last_pos = 0;

    while(v_file->fname[char_i] != '\0')
    {
        if(v_file->fname[char_i] == '/')
        {
            if(char_i == 0) // Cares about the absolute path.
            {
                slash_last_pos = 1;
                break;
            }
            slash_last_pos = char_i;
        }
        char_i++;
    }
    strncpy(v_file->pathname, v_file->fname, slash_last_pos);
    v_file->pathname[slash_last_pos] = '\0';

    if(chdir(v_file->pathname) == -1)
    {
        fprintf(stderr,
                "Can't change the direcory to get the parent file path.\n");
        return false;
    }

    v_file->pathname = getcwd(v_file->pathname, PATH_MAX);
    if(v_file->pathname == NULL)
    {
        fprintf(stderr, "Can't get the current directory. Too long.\n");
        return false;
    }
    return true;
}

void path__extract_basename_from_arg(V_file* const v_file)
{
    size_t char_i         = strlen(v_file->fname);
    size_t slash_last_pos = 0;

    while(char_i > 0)
    {
        if(v_file->fname[char_i] == '/')
        {
            slash_last_pos = char_i + SIZE__SLASH;
            break;
        }
        char_i--;
    }
    strcpy(v_file->basename, &v_file->fname[slash_last_pos]);
}

void path__merge_pathname_and_basename(V_file* const v_file)
{
    strcpy(v_file->fname, v_file->pathname);

    v_file->fname[strlen(v_file->fname)] = '/'; // Append the slash.
    v_file->fname[strlen(v_file->fname)] = '\0';

    strcat(v_file->fname, v_file->basename);
}
