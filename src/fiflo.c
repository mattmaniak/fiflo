#include "fiflo.h"

void fiflo__run(int argc, char** const argv)
{
    const size_t fname_arg_sz = 1;

    char   pressed_key       = '\0'; // For an initialization purposes only.
    size_t actual_file_i     = 0;
    size_t additional_argc_i = (size_t) argc - SIZE__I;
    char*  extension;

    V_file_t* V_file;
    Conf_t    Config;
    Mod_t     Modes;
    Syntax_t  Syntax;

    if(argc > 1)
    {
        additional_argc_i -= fname_arg_sz;
    }
    modes__init(&Modes);
    if(!args__parse(&Modes, &argc, argv))
    {
        return;
    }

    V_file = malloc((size_t) argc * sizeof(V_file_t));
    if(V_file == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for file buffers.\n");
        goto free;
    }
    for(size_t file_i = 0; file_i <= additional_argc_i; file_i++)
    {
        if(!buffer__init(&V_file[file_i]) || !config__load(&Config)
           || !file_io__set_name(&V_file[file_i], argv[fname_arg_sz + file_i])
           || !file_io__load(&V_file[file_i], &Config, &Modes))
        {
            goto free;
        }
        strcpy(V_file[file_i].fname_copy, V_file[file_i].fname);
    }
    Syntax.kwrds_amount = 0;

    for(;;) // The main program loop.
    {
        if(V_file[actual_file_i].basename[0] != '\0')
        {
            extension = extension__recognize(V_file[actual_file_i].basename);
            if((extension != NULL)
               && strcmp(V_file[actual_file_i].extension, extension))
            {
                Syntax.kwrds_amount = 0;
                syntax__load(&Syntax, extension);
                strcpy(V_file[actual_file_i].extension, extension);
            }
        }
        if(!file_io__get_git_branch(&V_file[actual_file_i])
           || !input__parse_key(&V_file[actual_file_i], &Config, &Modes,
                                &actual_file_i, pressed_key))
        {
            break;
        }
        // An user has selected too big id for a file, select the last one.
        if(actual_file_i > additional_argc_i)
        {
            actual_file_i = additional_argc_i;
        }

        // Flushes and renders always after the keypress.
        if(!window__render(V_file, &Config, &Modes, &Syntax,
                           (size_t) argc - SIZE__I, (size_t) actual_file_i))
        {
            break;
        }
        pressed_key = input__getch();
        if(pressed_key == -1)
        {
            break;
        }
        window__flush();
    }

free:
    for(size_t file_i = 0; file_i <= additional_argc_i; file_i++)
    {
        buffer__free(&V_file[file_i]);
    }
    if(V_file != NULL)
    {
        free(V_file);
    }
}

int main(int argc, char** argv)
{
    fiflo__run(argc, argv);
    fflush(NULL); // Clean both output buffers.

    return 0;
}
