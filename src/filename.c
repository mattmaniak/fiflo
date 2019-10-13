#include "filename.h"

bool filename__set_name(V_file* const v_file, const char* const arg)
{
    size_t cw_dir_len;

    if(arg == NULL) // Name not passed by an user.
    {
        v_file->pathname = getcwd(v_file->pathname, PATH_MAX);
        if(v_file->pathname == NULL)
        {
            fprintf(stderr, "Can't get the current directory. Too long.\n");
            return false;
        }
        cw_dir_len = strlen(v_file->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_len >= (PATH_MAX - SIZE__SLASH))
        {
            fprintf(stderr,
                    "Can't insert the slash. The current dir is too long.\n");
            return false;
        }
        strcpy(v_file->fname, v_file->pathname); // Copy pathname.

        v_file->fname[cw_dir_len]               = '/'; // Add the slash.
        v_file->fname[cw_dir_len + SIZE__SLASH] = '\0';
        v_file->fname_len                       = strlen(v_file->fname);

        return true;
    }

    // A current or parent directory.
    if(!strncmp(arg, "./", 2) || !strncmp(arg, "../", 3))
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "Passed filename is too long.\n");
            return false;
        }
        strncpy(v_file->fname, arg, PATH_MAX + NAME_MAX);

        if(!filename__extract_pathname_from_arg(v_file))
        {
            return false;
        }
        filename__extract_basename_from_arg(v_file);
        filename__merge_pathname_and_basename(v_file);
    }
    else if(arg[0] == '/') // An absolute dir.
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "Passed filename is too long.\n");
            return false;
        }
        strncpy(v_file->fname, arg, PATH_MAX + NAME_MAX);

        if(!filename__extract_pathname_from_arg(v_file))
        {
            return false;
        }
    }
    else // A relative pathname or a basename.
    {
        if(strlen(arg) >= NAME_MAX)
        {
            fprintf(stderr, "Passed basename is too long.\n");
            return false;
        }
        if((v_file->pathname = getcwd(v_file->pathname, PATH_MAX)) == NULL)
        {
            fprintf(stderr, "Can't get a current directory. Too long.\n");
            return false;
        }
        cw_dir_len = strlen(v_file->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_len >= (PATH_MAX - SIZE__SLASH))
        {
            fprintf(stderr,
                    "Can't insert the slash. The current dir is too long.\n");
            return false;
        }
        strncpy(v_file->fname, v_file->pathname, PATH_MAX); // Copy pathname.

        v_file->fname[cw_dir_len]               = '/'; // Add the slash.
        v_file->fname[cw_dir_len + SIZE__SLASH] = '\0';

        // Append a basename.
        strncpy(&v_file->fname[cw_dir_len + SIZE__SLASH], arg, NAME_MAX);
        filename__extract_basename_from_arg(v_file);
    }
    v_file->fname_len = strlen(v_file->fname);

    return true;
}

bool filename__extract_pathname_from_arg(V_file* const v_file)
{
    size_t ch_i           = 0;
    size_t slash_last_pos = 0;

    while(v_file->fname[ch_i] != '\0')
    {
        if(v_file->fname[ch_i] == '/')
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

void filename__extract_basename_from_arg(V_file* const v_file)
{
    size_t ch_i         = strlen(v_file->fname);
    size_t slash_last_pos = 0;

    while(ch_i > 0)
    {
        if(v_file->fname[ch_i] == '/')
        {
            slash_last_pos = ch_i + SIZE__SLASH;
            break;
        }
        ch_i--;
    }
    strcpy(v_file->basename, &v_file->fname[slash_last_pos]);
}

void filename__merge_pathname_and_basename(V_file* const v_file)
{
    strcpy(v_file->fname, v_file->pathname);

    v_file->fname[strlen(v_file->fname)] = '/'; // Append the slash.
    v_file->fname[strlen(v_file->fname)] = '\0';

    strcat(v_file->fname, v_file->basename);
}
