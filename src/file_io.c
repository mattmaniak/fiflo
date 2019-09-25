#include "file_io.h"

bool file_io__set_name(V_file_t* const V_file, const char* const arg)
{
    size_t cw_dir_len;

    if(arg == NULL) // Name not passed by an user.
    {
        if((V_file->pathname = getcwd(V_file->pathname, PATH_MAX)) == NULL)
        {
            fprintf(stderr, "Can't get the current directory. Too long.\n");
            return false;
        }
        cw_dir_len = strlen(V_file->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_len >= (PATH_MAX - SIZE__SLASH))
        {
            fprintf(stderr,
                    "Can't insert the slash. The Current dir is too long.\n");
            return false;
        }
        strcpy(V_file->fname, V_file->pathname); // Copy pathname.

        V_file->fname[cw_dir_len]            = '/'; // Add the slash.
        V_file->fname[cw_dir_len + SIZE__SLASH] = '\0';

        V_file->fname_len = strlen(V_file->fname);

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
        strncpy(V_file->fname, arg, PATH_MAX + NAME_MAX);

        if(!path__extract_pathname_from_arg(V_file))
        {
            return false;
        }
        path__extract_basename_from_arg(V_file);
        path__merge_pathname_and_basename(V_file);
    }
    else if(arg[0] == '/') // An absolute dir.
    {
        if(strlen(arg) >= (PATH_MAX + NAME_MAX))
        {
            fprintf(stderr, "The passed filename is too long.\n");
            return false;
        }
        strncpy(V_file->fname, arg, PATH_MAX + NAME_MAX);

        if(!path__extract_pathname_from_arg(V_file))
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

        if((V_file->pathname = getcwd(V_file->pathname, PATH_MAX)) == NULL)
        {
            fprintf(stderr, "Can't get a current directory. Too long.\n");
            return false;
        }
        cw_dir_len = strlen(V_file->pathname);

        // Getcwd() returns a pathname without the slash, which is required.
        if(cw_dir_len >= (PATH_MAX - SIZE__SLASH))
        {
            fprintf(stderr,
                    "Can't insert the slash. The current dir is too long.\n");
            return false;
        }
        strncpy(V_file->fname, V_file->pathname, PATH_MAX); // Copy pathname.

        V_file->fname[cw_dir_len]            = '/'; // Add the slash.
        V_file->fname[cw_dir_len + SIZE__SLASH] = '\0';

        // Append a basename.
        strncpy(&V_file->fname[cw_dir_len + SIZE__SLASH], arg, NAME_MAX);
    }
    V_file->fname_len = strlen(V_file->fname);

    return true;
}

bool file_io__load(V_file_t* const V_file, const Conf_t* const Config,
                const Mod_t* const Modes)
{
    FILE* Textfile;
    char  ch;

    if(V_file->fname[V_file->fname_len - SIZE__NUL] == '/')
    {
        V_FILE__SET_STATUS("current directory set");
        return true;
    }

    if((Textfile = fopen(V_file->fname, "r")) == NULL)
    {
        V_FILE__SET_STATUS("the file will be created");
        return true;
    }
    while((ch = (const char) getc(Textfile)) != EOF)
    {
        switch(ch)
        {
        default:
            if(!chars__printable_char(V_file, ch))
            {
                return false;
            }
            break;

        case '\t':
            if(!file_io__convert_tab_from_file(V_file, Config, Modes, ch))
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
    V_FILE__SET_STATUS("read a file");

    return true;
}

bool file_io__convert_tab_from_file(V_file_t* const V_file,
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
            if(!chars__printable_char(V_file, tab_ch))
            {
                return false;
            }
        }
    }
    return true;
}

void file_io__convert_tab_to_file(const V_file_t* const V_file,
                               const Conf_t* const Config, const size_t ln_i,
                               size_t* const ch_i)
{
    const size_t tab_sz = (const size_t) Config->Tab_sz.value;

    // Convert editor-friendly Tab, e.g. "\t\t\t\t" into a file-friendly '\t'.
    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        if(V_file->Lines[ln_i].txt[*ch_i + tab_i] != '\t')
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

bool file_io__save(V_file_t* const V_file, const Conf_t* const Config)
{
    FILE* Textfile = fopen(V_file->fname, "w");

    if(Textfile == NULL)
    {
        V_FILE__SET_STATUS("can't write to the file");
        return true;
    }
    for(size_t ln_i = 0; ln_i <= V_file->ln_amount; ln_i++)
    {
        /* Using fputs or fprintf causes an use-of-uninitialized-value using
           MSan because of there is a more memory allocated than is needed. */
        for(size_t ch_i = 0; ch_i < V_file->Lines[ln_i].len; ch_i++)
        {
            file_io__convert_tab_to_file(V_file, Config, ln_i, &ch_i);
            putc(V_file->Lines[ln_i].txt[ch_i], Textfile);
        }
    }
    if(fclose(Textfile) == EOF)
    {
        fprintf(stderr, "Can't close the textfile after save.\n");
        return false;
    }
    V_FILE__SET_STATUS("saved");

    return true;
}

bool file_io__get_git_branch(V_file_t* const V_file)
{
    char  git_head_file_pathname[PATH_MAX + NAME_MAX];
    FILE* Git_head_file;

    strcpy(git_head_file_pathname, V_file->pathname);
    strcat(git_head_file_pathname, "/.git/HEAD");

    if((access(git_head_file_pathname, F_OK) == -1)
       || ((Git_head_file = fopen(git_head_file_pathname, "r")) == NULL))
    {
        strcpy(V_file->git_branch, "[none]");
        return true;
    }

    // Ignore a passed string in a file to get a branch after the slash.
    if(fseek(Git_head_file, (const long int) strlen("ref: refs/heads/"), 0)
       == -1)
    {
        strcpy(V_file->git_branch, "[none]");
        return true;
    }

    // Read a contents of the file.
    while(fgets(V_file->git_branch, NAME_MAX, Git_head_file) != NULL)

    // Delete the linefeed from the name.
    if(V_file->git_branch[strlen(V_file->git_branch) - SIZE__NUL] == '\n')
    {
        V_file->git_branch[strlen(V_file->git_branch) - SIZE__NUL] = '\0';
    }

    if(fclose(Git_head_file) == EOF)
    {
        fprintf(stderr,
                "Can't close the git \".git/HEAD\" file to get a branch.\n");
        return false;
    }
    return true;
}
