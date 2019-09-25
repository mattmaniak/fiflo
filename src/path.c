#include "path.h"

bool path__extract_pathname_from_arg(V_file_t* const V_file)
{
    size_t ch_i           = 0;
    size_t slash_last_pos = 0;

    while(V_file->fname[ch_i] != '\0')
    {
        if(V_file->fname[ch_i] == '/')
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
    strncpy(V_file->pathname, V_file->fname, slash_last_pos);
    V_file->pathname[slash_last_pos] = '\0';

    if(chdir(V_file->pathname) == -1)
    {
        fprintf(stderr,
                "Can't change the direcory to get the parent file path.\n");
        return false;
    }

    V_file->pathname = getcwd(V_file->pathname, PATH_MAX);
    if(V_file->pathname == NULL)
    {
        fprintf(stderr, "Can't get the current directory. Too long.\n");
        return false;
    }
    return true;
}

void path__extract_basename_from_arg(V_file_t* const V_file)
{
    size_t ch_i             = strlen(V_file->fname);
    size_t slash_last_pos = 0;

    while(ch_i > 0)
    {
        if(V_file->fname[ch_i] == '/')
        {
            slash_last_pos = ch_i + SIZE__SLASH;
            break;
        }
        ch_i--;
    }
    strcpy(V_file->basename, &V_file->fname[slash_last_pos]);
}

void path__merge_pathname_and_basename(V_file_t* const V_file)
{
    strcpy(V_file->fname, V_file->pathname);

    V_file->fname[strlen(V_file->fname)] = '/'; // Append the slash.
    V_file->fname[strlen(V_file->fname)] = '\0';

    strcat(V_file->fname, V_file->basename);
}
