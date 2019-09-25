#include "file_io.h"

bool file_io__set_name(Buff_t* const Buffer, const char* const arg)
{
    size_t cw_dir_len;

    if(arg == NULL) // Name not passed by an user.
    {
        if((Buffer->pathname = getcwd(Buffer->pathname, PATH_MAX)) == NULL)
        {
            fprintf(stderr, "Can't get the current directory. Too long.\n");
            return false;
        }
        cw_dir_len = strlen(Buffer->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_len >= (PATH_MAX - SIZE__SLASH))
        {
            fprintf(stderr,
                    "Can't insert the slash. The Current dir is too long.\n");
            return false;
        }
        strcpy(Buffer->fname, Buffer->pathname); // Copy pathname.

        Buffer->fname[cw_dir_len]            = '/'; // Add the slash.
        Buffer->fname[cw_dir_len + SIZE__SLASH] = '\0';

        Buffer->fname_len = strlen(Buffer->fname);

        return true;
    }

    // A current or parent directory.
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
        cw_dir_len = strlen(Buffer->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_len >= (PATH_MAX - SIZE__SLASH))
        {
            fprintf(stderr,
                    "Can't insert the slash. The current dir is too long.\n");
            return false;
        }
        strncpy(Buffer->fname, Buffer->pathname, PATH_MAX); // Copy pathname.

        Buffer->fname[cw_dir_len]            = '/'; // Add the slash.
        Buffer->fname[cw_dir_len + SIZE__SLASH] = '\0';

        // Append a basename.
        strncpy(&Buffer->fname[cw_dir_len + SIZE__SLASH], arg, NAME_MAX);
    }
    Buffer->fname_len = strlen(Buffer->fname);

    return true;
}

bool file_io__load(Buff_t* const Buffer, const Conf_t* const Config,
                const Mod_t* const Modes)
{
    FILE* Textfile;
    char  ch;

    if(Buffer->fname[Buffer->fname_len - SIZE__NUL] == '/')
    {
        BUFFER__SET_STATUS("current directory set");
        return true;
    }

    if((Textfile = fopen(Buffer->fname, "r")) == NULL)
    {
        BUFFER__SET_STATUS("the file will be created");
        return true;
    }
    while((ch = (const char) getc(Textfile)) != EOF)
    {
        switch(ch)
        {
        default:
            if(!chars__printable_char(Buffer, ch))
            {
                return false;
            }
            break;

        case '\t':
            if(!file_io__convert_tab_from_file(Buffer, Config, Modes, ch))
            {
                return false;
            }
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

bool file_io__convert_tab_from_file(Buff_t* const Buffer,
                                 const Conf_t* const Config,
                                 const Mod_t* const Modes, const char ch)
{
    /* Converts in-file '\t' in to a sequence of e.g. "\t\t\t\t" if the Tab
       width is set to 4. */
    const size_t tab_sz = (const size_t) Config->Tab_sz.value;
    const char  tab_ch = (Modes->tabs_to_spaces) ? ' ' : '\t';

    if(ch == '\t')
    {
        for(size_t ch_i = 0; ch_i < tab_sz; ch_i++)
        {
            if(!chars__printable_char(Buffer, tab_ch))
            {
                return false;
            }
        }
    }
    return true;
}

void file_io__convert_tab_to_file(const Buff_t* const Buffer,
                               const Conf_t* const Config, const size_t ln_i,
                               size_t* const ch_i)
{
    const size_t tab_sz = (const size_t) Config->Tab_sz.value;

    // Convert editor-friendly Tab, e.g. "\t\t\t\t" into a file-friendly '\t'.
    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        if(Buffer->Lines[ln_i].txt[*ch_i + tab_i] != '\t')
        {
            break; // No Tab, so don't convert anything.
        }
        else if(tab_i == (tab_sz - SIZE__I))
        {
            // Some in-memory Tabs converted
            *ch_i += tab_sz - FILE__AT_LEAST_ONE_TAB;
        }
    }
}

bool file_io__save(Buff_t* const Buffer, const Conf_t* const Config)
{
    FILE* Textfile = fopen(Buffer->fname, "w");

    if(Textfile == NULL)
    {
        BUFFER__SET_STATUS("can't write to the file");
        return true;
    }
    for(size_t ln_i = 0; ln_i <= Buffer->ln_amount; ln_i++)
    {
        /* Using fputs or fprintf causes an use-of-uninitialized-value using
           MSan because of there is a more memory allocated than is needed. */
        for(size_t ch_i = 0; ch_i < Buffer->Lines[ln_i].len; ch_i++)
        {
            file_io__convert_tab_to_file(Buffer, Config, ln_i, &ch_i);
            putc(Buffer->Lines[ln_i].txt[ch_i], Textfile);
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

bool file_io__get_git_branch(Buff_t* const Buffer)
{
    char  git_head_file_pathname[PATH_MAX + NAME_MAX];
    FILE* Git_head_file;

    strcpy(git_head_file_pathname, Buffer->pathname);
    strcat(git_head_file_pathname, "/.git/HEAD");

    if((access(git_head_file_pathname, F_OK) == -1)
       || ((Git_head_file = fopen(git_head_file_pathname, "r")) == NULL))
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    // Ignore a passed string in a file to get a branch after the slash.
    if(fseek(Git_head_file, (const long int) strlen("ref: refs/heads/"), 0)
       == -1)
    {
        strcpy(Buffer->git_branch, "[none]");
        return true;
    }

    // Read a contents of the file.
    while(fgets(Buffer->git_branch, NAME_MAX, Git_head_file) != NULL)

    // Delete the linefeed from the name.
    if(Buffer->git_branch[strlen(Buffer->git_branch) - SIZE__NUL] == '\n')
    {
        Buffer->git_branch[strlen(Buffer->git_branch) - SIZE__NUL] = '\0';
    }

    if(fclose(Git_head_file) == EOF)
    {
        fprintf(stderr,
                "Can't close the git \".git/HEAD\" file to get a branch.\n");
        return false;
    }
    return true;
}
