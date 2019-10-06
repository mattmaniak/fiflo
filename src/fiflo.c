#include "fiflo.h"

void fiflo__run(int argc, char** const argv)
{
    const size_t fname_arg_sz = 1;

    char   pressed_key       = '\0'; // For initialization purposes only.
    size_t actual_file_i     = 0;
    size_t additional_argc_i = (size_t) argc - SIZE__I;
    char*  extension;

    V_file* v_files;
    Config  config;
    Modes   modes;
    Syntax  syntax;

    if(argc > 1)
    {
        additional_argc_i -= fname_arg_sz;
    }
    modes__init(&modes);
    if(!args__parse(&modes, &argc, argv))
    {
        return;
    }

    v_files = malloc((size_t) argc * sizeof(V_file));
    if(v_files == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for file buffers.\n");
        goto free;
    }
    for(size_t file_i = 0; file_i <= additional_argc_i; file_i++)
    {
        if(!v_file__init(&v_files[file_i]) || !config__load(&config)
           || !file_io__set_name(&v_files[file_i], argv[fname_arg_sz + file_i])
           || !file_io__load(&v_files[file_i], &config, &modes))
        {
            goto free;
        }
        strcpy(v_files[file_i].fname_copy, v_files[file_i].fname);
    }
    syntax.keywords_amount = 0;

    for(;;) // The main program loop.
    {
        if(!file_io__get_git_branch(&v_files[actual_file_i])
           || !input__parse_key(&v_files[actual_file_i], &config, &modes,
                                &actual_file_i, pressed_key))
        {
            break;
        }
        // An user has selected too big id for the file, select the last one.
        if(actual_file_i > additional_argc_i)
        {
            actual_file_i = additional_argc_i;
        }
        if(v_files[actual_file_i].basename[0] != '\0')
        {
            extension = extension__recognize(v_files[actual_file_i].basename);
            if((extension != NULL)
               && strcmp(v_files[actual_file_i].extension, extension))
            {
                syntax.keywords_amount = 0;
                syntax__load(&syntax, extension);
                strcpy(v_files[actual_file_i].extension, extension);
            }
        }

        // Flushes and renders always after the keypress.
        if(!window__render(v_files, &config, &modes, &syntax,
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
        v_file__delete(&v_files[file_i]);
    }
    if(v_files != NULL)
    {
        free(v_files);
    }
}

int main(int argc, char** argv)
{
    fiflo__run(argc, argv);
    fflush(NULL); // Clean both output buffers.

    return 0;
}
