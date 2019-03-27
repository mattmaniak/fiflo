#include "buffer.h"
#include "config.h"
#include "shortcuts.h"
#include "modes.h"
#include "file.h"
#include "path.h"

bool file__set_name(Buff_t* Buffer, const char* arg)
{
    size_t cw_dir_length;

    if((arg[0]  == '/') || (arg[0]  == '.')) // TODO: PATH INSTEAD OF "../".
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "Passed filename is too long.\n");
            return false;
        }
        strncpy(Buffer->fname, arg, PATH_MAX + NAME_MAX);

        if(!path__extract_path_from_arg(Buffer))
        {
            return false;
        }
        path__extract_basename_from_arg(Buffer);
        path__merge_path_and_basename(Buffer);
    }
    else // Relative path or the basename.
    {
        if(strlen(arg) >= NAME_MAX)
        {
            fprintf(stderr, "Passed filename is too long.\n");
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
        cw_dir_length = strlen(Buffer->path);

        // Getcwd() returns the path without the slash, which is required here.
        if(cw_dir_length >= (PATH_MAX - SLASH_SZ))
        {
            fprintf(stderr,
                    "Can't insert the slash. Current direcory is too long.\n");

            return false;
        }

        strncpy(Buffer->fname, Buffer->path, PATH_MAX); // Copy the path.

        Buffer->fname[cw_dir_length] = '/'; // Add the slash between.
        Buffer->fname[cw_dir_length + SLASH_SZ] = '\0';

        // Append the basename.
        strncpy(&Buffer->fname[cw_dir_length + SLASH_SZ], arg, NAME_MAX);
    }
    Buffer->fname_length = strlen(Buffer->fname);

    return true;
}

void file__live_edit_name(Buff_t* Buffer, Conf_t* Config, Mod_t* Modes,
                          char key)
{
    const char enter  = '\n';
    const char escape = '\033';

    if((key >= 32) && (key != BACKSPACE)
    && ((Buffer->fname_length + IDX) < PATH_MAX))
    {
        Buffer->fname[Buffer->fname_length] = key;
        Buffer->fname_length++;
        Buffer->fname[Buffer->fname_length] = '\0';
    }
    else if((key == BACKSPACE) && (Buffer->fname_length > 0))
    {
        Buffer->fname_length--;
        Buffer->fname[Buffer->fname_length] = '\0';
    }
    else if(key == enter)
    {
        Modes->live_fname_edit = false;

        SET_STATUS("filename edited and saved as");
        if(!strcmp(Buffer->fname, Buffer->orig_fname))
        {
            SET_STATUS("saved as");
        }

        file__save(Buffer, Config);
        strncpy(Buffer->orig_fname, Buffer->fname, PATH_MAX);
    }
    else if(key == escape)
    {
        strncpy(Buffer->fname, Buffer->orig_fname, PATH_MAX);
        Buffer->fname_length = strlen(Buffer->fname);

        Modes->live_fname_edit = false;
        SET_STATUS("filename unchanged");
    }
}

bool file__convert_tab_from_file(Buff_t* Buffer, Conf_t* Config, char ch)
{
    /* Converts in-file '\t' in to sequence of e.g. "\t\t\t\t" if the tab width
    is set to 4. */
    if(ch == '\t')
    {
        for(idx_t char_idx = 0;
            char_idx < (idx_t) (Config->Tab_width.value - AT_LEAST_ONE_TAB);
            char_idx++)
        {
            if(!keys__printable_char(Buffer, ch))
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

    if(Textfile != NULL)
    {
        while((ch = (char) getc(Textfile)) != EOF)
        {
            if(!file__convert_tab_from_file(Buffer, Config, ch))
            {
                return false;
            }
            if(!keys__printable_char(Buffer, ch))
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
    }
    else
    {
        SET_STATUS("the file will be created");
    }
    return true;
}

void file__convert_tab_to_file(Buff_t* Buffer, Conf_t* Config, idx_t line_idx,
                               idx_t* char_idx)
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

    if(Textfile != NULL)
    {
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
    }
    else
    {
        SET_STATUS("can't write to the file");
    }
    return true;
}

bool file__load_config(Conf_t* Config)
{
    char conf_fname[] = "/etc/fiflorc";

    if(access(conf_fname, F_OK) == ERROR)
    {
        config__set_default(Config);
    }

    Config->File = fopen(conf_fname, "r");
    if(Config->File != NULL)
    {
        config__load_custom(Config);

        if(fclose(Config->File) == EOF)
        {
            fprintf(stderr, "Can't close the config file.\n");
            return false;
        }
    }
    return true;
}

bool file__get_git_branch(Buff_t* Buffer) // TODO: ERROR MESSAGES.
{
    char  git_head_file_path[PATH_MAX + NAME_MAX];
    FILE* Git_head_file;

    strncpy(git_head_file_path, Buffer->path, PATH_MAX);
    strcat(git_head_file_path, "/.git/HEAD");

    if(access(git_head_file_path, F_OK) == ERROR)
    {
        return true;
    }

    Git_head_file = fopen(git_head_file_path, "r");
    if(Git_head_file == NULL)
    {
        return true;
    }

    // Ignore the passed string in the file, to get the branch after the slash.
    if(fseek(Git_head_file, (long) strlen("ref: refs/heads/"), 0) == ERROR)
    {
        return true;
    }
    while(fgets(Buffer->git_branch, NAME_MAX, Git_head_file) != NULL)

    if(Buffer->git_branch[strlen(Buffer->git_branch) - NUL_SZ] == '\n')
    {
        Buffer->git_branch[strlen(Buffer->git_branch) - NUL_SZ] = '\0';
    }

    if(fclose(Git_head_file) == EOF)
    {
        fprintf(stderr, "Can't close the git \"HEAD\" file.\n");
        return false;
    }
    return true;
}
