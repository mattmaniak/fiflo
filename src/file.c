#include "file.h"

bool file__set_name(Buff_t* const Buffer, const char* const arg)
{
    size_t cw_dir_length;

    if(arg == NULL) // Name not passed by an user.
    {
        if((Buffer->pathname = getcwd(Buffer->pathname, PATH_MAX)) == NULL)
        {
            fprintf(stderr, "Can't get the current directory. Too long.\n");
            return false;
        }
        cw_dir_length = strlen(Buffer->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_length >= (PATH_MAX - SLASH_SZ))
        {
            fprintf(stderr,
                    "Can't insert the slash. The Current dir is too long.\n");
            return false;
        }
        strcpy(Buffer->fname, Buffer->pathname); // Copy pathname.

        Buffer->fname[cw_dir_length]            = '/'; // Add the slash.
        Buffer->fname[cw_dir_length + SLASH_SZ] = '\0';

        Buffer->fname_len = strlen(Buffer->fname);

        return true;
    }

    // A current or a parent directory.
    if(!strncmp(arg, "./", 2) || !strncmp(arg, "../", 3))
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "The passed filename is too long.\n");
            return false;
        }
        strncpy(Buffer->fname, arg, PATH_MAX + NAME_MAX);

        if(!path__extract_pathname_from_arg(Buffer))
        {
            return false;
        }
        path__extract_basename_from_arg(Buffer);
        path__merge_pathname_and_basename(Buffer);
    }
    else if(arg[0] == '/') // An absolute dir.
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "The passed filename is too long.\n");
            return false;
        }
        strncpy(Buffer->fname, arg, PATH_MAX + NAME_MAX);

        if(!path__extract_pathname_from_arg(Buffer))
        {
            return false;
        }
    }
    else // A relative pathname or a basename.
    {
        if(strlen(arg) >= NAME_MAX)
        {
            fprintf(stderr, "The passed filename is too long.\n");
            return false;
        }

        if((Buffer->pathname = getcwd(Buffer->pathname, PATH_MAX)) == NULL)
        {
            fprintf(stderr, "Can't get a current directory. Too long.\n");
            return false;
        }
        cw_dir_length = strlen(Buffer->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_length >= (PATH_MAX - SLASH_SZ))
        {
            fprintf(stderr,
                    "Can't insert the slash. The current dir is too long.\n");
            return false;
        }
        strncpy(Buffer->fname, Buffer->pathname, PATH_MAX); // Copy pathname.

        Buffer->fname[cw_dir_length]            = '/'; // Add the slash.
        Buffer->fname[cw_dir_length + SLASH_SZ] = '\0';

        // Append a basename.
        strncpy(&Buffer->fname[cw_dir_length + SLASH_SZ], arg, NAME_MAX);
    }
    Buffer->fname_len = strlen(Buffer->fname);

    return true;
}

bool file__convert_tab_from_file(Buff_t* const Buffer,
                                 const Conf_t* const Config, const char ch)
{
    /* Converts in-file '\t' in to a sequence of e.g. "\t\t\t\t" if the Tab
       width is set to 4. */
    switch(ch)
    {
    case '\t':
        for(idx_t ch_idx = 0; ch_idx < (const idx_t)
            (Config->Tab_sz.value - FILE__AT_LEAST_ONE_TAB); ch_idx++)
        {
            if(!chars__printable_char(Buffer, ch))
            {
                return false;
            }
        }
    }
    return true;
}

bool file__load(Buff_t* const Buffer, const Conf_t* const Config)
{
    FILE* Textfile;
    char  ch;

    if(Buffer->fname[Buffer->fname_len - NUL_SZ] == '/')
    {
        BUFFER__SET_STATUS("current directory set");
        return true;
    }

    if((Textfile = fopen(Buffer->fname, "r")) == NULL)
    {
        BUFFER__SET_STATUS("the file will be created");
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
        fprintf(stderr, "Can't close a textfile after load.\n");
        return false;
    }
    BUFFER__SET_STATUS("read a file");

    return true;
}

void file__convert_tab_to_file(const Buff_t* const Buffer,
                               const Conf_t* const Config, const idx_t ln_idx,
                               idx_t* const ch_idx)
{
    // Convert editor-friendly Tab, e.g. "\t\t\t\t" into a file-friendly '\t'.
    for(idx_t tab_idx = 0;
        tab_idx < (const idx_t) Config->Tab_sz.value; tab_idx++)
    {
        if(Buffer->Lines[ln_idx].txt[*ch_idx + tab_idx] != '\t')
        {
            break; // No Tab, so don't convert anything.
        }
        else if(tab_idx == (const idx_t) Config->Tab_sz.value - IDX)
        {
            // Some in-memory Tabs converted
            *ch_idx += (const idx_t) Config->Tab_sz.value
                       - FILE__AT_LEAST_ONE_TAB;
        }
    }
}

bool file__save(Buff_t* const Buffer, const Conf_t* const Config)
{
    FILE* Textfile = fopen(Buffer->fname, "w");

    if(Textfile == NULL)
    {
        BUFFER__SET_STATUS("can't write to the file");
        return true;
    }
    for(idx_t ln_idx = 0; ln_idx <= Buffer->lines_amount; ln_idx++)
    {
        /* Using fputs or fprintf causes an use-of-uninitialized-value using
           MSan because of there is a more memory allocated than is needed. */
        for(idx_t ch_idx = 0; ch_idx < Buffer->Lines[ln_idx].len; ch_idx++)
        {
            file__convert_tab_to_file(Buffer, Config, ln_idx, &ch_idx);
            putc(Buffer->Lines[ln_idx].txt[ch_idx], Textfile);
        }
    }
    if(fclose(Textfile) == EOF)
    {
        fprintf(stderr, "Can't close the textfile after save.\n");
        return false;
    }
    BUFFER__SET_STATUS("saved");

    return true;
}

bool file__get_git_branch(Buff_t* const Buffer)
{
    char  git_head_file_pathname[PATH_MAX + NAME_MAX];
    FILE* Git_head_file;

    strcpy(git_head_file_pathname, Buffer->pathname);
    strcat(git_head_file_pathname, "/.git/HEAD");

    if(access(git_head_file_pathname, F_OK) == ERROR)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    if((Git_head_file = fopen(git_head_file_pathname, "r")) == NULL)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    // Ignore a passed string in a file to get a branch after the slash.
    if(fseek(Git_head_file, (const long int) strlen("ref: refs/heads/"), 0)
       == ERROR)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    // Read a contents of the file.
    while(fgets(Buffer->git_branch, NAME_MAX, Git_head_file) != NULL)

    // Delete the linefeed from the name.
    if(Buffer->git_branch[strlen(Buffer->git_branch) - NUL_SZ] == '\n')
    {
        Buffer->git_branch[strlen(Buffer->git_branch) - NUL_SZ] = '\0';
    }

    if(fclose(Git_head_file) == EOF)
    {
        fprintf(stderr,
                "Can't close the git \".git/HEAD\" file to get a branch.\n");
        return false;
    }
    return true;
}
