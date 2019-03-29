#include "buffer.h"
#include "config.h"
#include "modes.h"
#include "keys.h"
#include "file.h"
#include "path.h"

bool file__set_name(Buff_t* Buffer, const char* const arg)
{
    size_t cw_dir_length_idx;

    if((arg[0]  == '/') || (arg[0]  == '.')) // Absolute or parent dir.
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "Passed filename is too long.\n");
            return false;
        }
        strncpy(Buffer->fname, arg, PATH_MAX + NAME_MAX);

        if(arg[0]  == '.') // Get the pathname instead of "./" or "../" dirs.
        {
            if(!path__extract_pathname_from_arg(Buffer))
            {
                return false;
            }
            path__extract_basename_from_arg(Buffer);
            path__merge_pathname_and_basename(Buffer);
        }
    }
    else // Relative pathname or the basename.
    {
        if(strlen(arg) >= NAME_MAX)
        {
            fprintf(stderr, "Passed filename is too long.\n");
            return false;
        }

        if(getcwd(Buffer->pathname, PATH_MAX) != NULL)
        {
            Buffer->pathname = getcwd(Buffer->pathname, PATH_MAX);
        }
        else
        {
            fprintf(stderr, "Can't get the current directory. Too long.\n");
            return false;
        }
        cw_dir_length_idx = strlen(Buffer->pathname);

        // Getcwd() returns the pathname without the slash, which is required here.
        if(cw_dir_length_idx >= (PATH_MAX - SLASH_SZ))
        {
            fprintf(stderr,
                    "Can't insert the slash. Current direcory is too long.\n");

            return false;
        }
        strncpy(Buffer->fname, Buffer->pathname, PATH_MAX); // Copy the pathname.

        Buffer->fname[cw_dir_length_idx]            = '/'; // Add the slash.
        Buffer->fname[cw_dir_length_idx + SLASH_SZ] = '\0';

        // Append the basename.
        strncpy(&Buffer->fname[cw_dir_length_idx + SLASH_SZ], arg, NAME_MAX);
    }
    Buffer->fname_length = strlen(Buffer->fname);

    return true;
}

bool file__convert_tab_from_file(Buff_t* Buffer, Conf_t* Config, const char ch)
{
    /* Converts in-file '\t' in to sequence of e.g. "\t\t\t\t" if the tab width
    is set to 4. */
    if(ch == '\t')
    {
        for(idx_t char_idx = 0;
            char_idx < (idx_t) (Config->Tab_width.value - AT_LEAST_ONE_TAB);
            char_idx++)
        {
            if(!chars__printable_char(Buffer, ch))
            {
                return false;
            }
        }
    }
    return true;
}

bool file__load(Buff_t* Buffer, Conf_t* Config)
{
    FILE* Textfile;
    char  ch;

    if(Buffer->fname[Buffer->fname_length - NUL_SZ] == '/')
    {
        SET_STATUS("current directory set");
        return true;
    }
    Textfile = fopen(Buffer->fname, "r");

    if(Textfile == NULL)
    {
        SET_STATUS("the file will be created");
        return true;
    }
    while((ch = (char) getc(Textfile)) != EOF)
    {
        if(!file__convert_tab_from_file(Buffer, Config, ch))
        {
            return false;
        }
        if(!chars__printable_char(Buffer, ch))
        {
            return false;
        }
    }
    if(fclose(Textfile) == EOF)
    {
        fprintf(stderr, "Can't close the textfile after load.\n");
        return false;
    }
    SET_STATUS("read the file");

    return true;
}

void file__convert_tab_to_file(Buff_t* Buffer, Conf_t* Config,
                               const idx_t line_idx, idx_t* char_idx)
{
    // Converts editor-friendly e.g. "\t\t\t\t" into the file-friendly '\t'.
    for(idx_t tab_idx = 0; tab_idx < (idx_t) Config->Tab_width.value; tab_idx++)
    {
        if(Buffer->text[line_idx][*char_idx + tab_idx] != '\t')
        {
            break; // No tab, so don't convert anything.
        }
        else if(tab_idx == (idx_t) Config->Tab_width.value - IDX)
        {
            // Some in-memory tabs converted
            *char_idx += (idx_t) Config->Tab_width.value - AT_LEAST_ONE_TAB;
        }
    }
}

bool file__save(Buff_t* Buffer, Conf_t* Config)
{
    FILE* Textfile = fopen(Buffer->fname, "w");

    if(Textfile == NULL)
    {
        SET_STATUS("can't write to the file");
        return true;
    }
    for(idx_t line_idx = 0; line_idx <= Buffer->lines_idx; line_idx++)
    {
        /* Using fputs or fprintf causes use-of-uninitialized-value using
        MSan because of there is more memory allocated than is needed. */
        for(idx_t char_idx = 0;
            char_idx < Buffer->lines_length_idx[line_idx]; char_idx++)
        {
            file__convert_tab_to_file(Buffer, Config, line_idx, &char_idx);

            putc(Buffer->text[line_idx][char_idx], Textfile);
        }
    }
    if(fclose(Textfile) == EOF)
    {
        fprintf(stderr, "Can't close the textfile after save.\n");
        return false;
    }
    SET_STATUS("saved");

    return true;
}

bool file__get_git_branch(Buff_t* Buffer)
{
    char  git_head_file_pathname[PATH_MAX + NAME_MAX];
    FILE* Git_head_file;

    strncpy(git_head_file_pathname, Buffer->pathname, PATH_MAX);
    strcat(git_head_file_pathname, "/.git/HEAD");

    if(access(git_head_file_pathname, F_OK) == ERROR)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    Git_head_file = fopen(git_head_file_pathname, "r");
    if(Git_head_file == NULL)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    // Ignore the passed string in the file, to get the branch after the slash.
    if(fseek(Git_head_file, (long) strlen("ref: refs/heads/"), 0) == ERROR)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    // Read the content of the file.
    while(fgets(Buffer->git_branch, NAME_MAX, Git_head_file) != NULL)

    // Delete the linefeed from the name.
    if(Buffer->git_branch[strlen(Buffer->git_branch) - NUL_SZ] == '\n')
    {
        Buffer->git_branch[strlen(Buffer->git_branch) - NUL_SZ] = '\0';
    }

    if(fclose(Git_head_file) == EOF)
    {
        fprintf(stderr,
                "Can't close the git \".git/HEAD\" file to get the branch.\n");
        return false;
    }
    return true;
}
