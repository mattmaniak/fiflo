#include "file_io.h"

bool file_io__load(V_file* const v_file, const Config* const config,
                   const Modes* const modes)
{
    char  ch = '\0';
    FILE* textfile;

    if(v_file->fname[v_file->fname_len - SIZE__NUL] == '/')
    {
        V_FILE__SET_STATUS("current directory set");
        return true;
    }
    textfile = fopen(v_file->fname, "r");
    if(textfile == NULL)
    {
        V_FILE__SET_STATUS("the file will be created");
        return true;
    }
    while(ch != EOF)
    {
        ch = (char) getc(textfile);
        switch(ch)
        {
        default:
            if(!input__printable_char(v_file, ch))
            {
                return false;
            }
            break;

        case '\t':
            if(!file_io__convert_tab_from_file(v_file, config, modes, ch))
            {
                return false;
            }
        }
    }
    if(fclose(textfile) == EOF)
    {
        fprintf(stderr, "Can't close a txtfile after load.\n");
        return false;
    }
    V_FILE__SET_STATUS("read a file");

    return true;
}

bool file_io__convert_tab_from_file(V_file* const v_file,
                                    const Config* const config,
                                    const Modes* const modes, const char ch)
{
    /* Converts in-file '\t' in to a sequence of e.g. "\t\t\t\t" if the Tab
       width is set to 4. */
    const size_t tab_sz = (size_t) config->tab_sz.value;
    const char   tab_char = (modes->tabs_to_spaces) ? ' ' : '\t';

    if(ch == '\t')
    {
        for(size_t ch_i = 0; ch_i < tab_sz; ch_i++)
        {
            if(!input__printable_char(v_file, tab_char))
            {
                return false;
            }
        }
    }
    return true;
}

void file_io__convert_tab_to_file(const V_file* const v_file,
                                  const Config* const config,
                                  const size_t ln_i, size_t* const ch_i)
{
    const size_t tab_sz = (size_t) config->tab_sz.value;

    // Convert editor-friendly Tab, e.g. "\t\t\t\t" into a file-friendly '\t'.
    for(size_t tab_i = 0; tab_i < tab_sz; tab_i++)
    {
        if(v_file->lines[ln_i].txt[*ch_i + tab_i] != '\t')
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

bool file_io__save(V_file* const v_file, const Config* const config)
{
    FILE* textfile = fopen(v_file->fname, "w");

    if(textfile == NULL)
    {
        V_FILE__SET_STATUS("can't write to the file");
        return true;
    }
    for(size_t ln_i = 0; ln_i <= v_file->lines_amount; ln_i++)
    {
        /* Using fputs or fprintf causes an use-of-uninitialized-value using
           MSan because of there is a more memory allocated than is needed. */
        for(size_t ch_i = 0; ch_i < v_file->lines[ln_i].len; ch_i++)
        {
            file_io__convert_tab_to_file(v_file, config, ln_i, &ch_i);
            putc(v_file->lines[ln_i].txt[ch_i], textfile);
        }
    }
    if(fclose(textfile) == EOF)
    {
        fprintf(stderr, "Can't close the textfile after save.\n");
        return false;
    }
    V_FILE__SET_STATUS("saved");

    return true;
}

bool file_io__get_git_branch(V_file* const v_file)
{
    char  git_head_file_pathname[PATH_MAX + NAME_MAX];
    FILE* Git_head_file;

    strcpy(git_head_file_pathname, v_file->pathname);
    strcat(git_head_file_pathname, "/.git/HEAD");

    if(access(git_head_file_pathname, R_OK) == -1)
    {
        goto empty_branch;
    }
    Git_head_file = fopen(git_head_file_pathname, "r");
    if(Git_head_file == NULL)
    {
    empty_branch:
        strcpy(v_file->git_branch, "[none]");
        return true;
    }

    // Ignore a passed string in a file to get a branch after the slash.
    if(fseek(Git_head_file, (long) strlen("ref: refs/heads/"), 0) == -1)
    {
        strcpy(v_file->git_branch, "[none]");
        return true;
    }

    // Read a contents of the file.
    while(fgets(v_file->git_branch, NAME_MAX, Git_head_file) != NULL)

    // Delete the linefeed from the name.
    if(v_file->git_branch[strlen(v_file->git_branch) - SIZE__NUL] == '\n')
    {
        v_file->git_branch[strlen(v_file->git_branch) - SIZE__NUL] = '\0';
    }

    if(fclose(Git_head_file) == EOF)
    {
        fprintf(stderr,
                "Can't close the git \".git/HEAD\" file to get a branch.\n");
        return false;
    }
    return true;
}
