#include "fiflo.h"

void fiflo__run(int argc, char** const argv)
{
    const int fname_arg_sz = 1;

    char   pressed_key     = '\0'; // For an initialization purposes only.
    size_t actual_file_idx = 0;
    char** additional_argv = &argv[1];
    idx_t  file_buffers;
    int    recognized_extension;

    Buff_t*  Buffer;
    Conf_t   Config;
    Mod_t    Modes;
    Syntax_t Syntax;

    Syntax.kwrds_idx = 0;

    modes__init(&Modes);
    if(!args__parse(&Modes, &argc, argv))
    {
        return;
    }
    argc -= fname_arg_sz;
    file_buffers = (argc == 0) ? 1 : argc;

    printf("%u\n", file_buffers);

    if((Buffer = malloc(argc * sizeof(Buff_t))) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for file buffers.\n");
        goto free;
    }
    Buffer->extension = EXTENSION__NONE;

    for(idx_t buff_idx = 0; buff_idx < file_buffers; buff_idx++)
    {
        if(additional_argv[buff_idx][0] == '-')
        {
            additional_argv[buff_idx][0] = '\0';
        }
        if(!buffer__init(&Buffer[buff_idx])
           || !config__load(&Config)
           || !file__set_name(&Buffer[buff_idx], additional_argv[buff_idx])
           || !file__load(&Buffer[buff_idx], &Config, &Modes))
        {
            goto free;
        }
        strcpy(Buffer[buff_idx].fname_copy, Buffer[buff_idx].fname);

        if(!file__get_git_branch(&Buffer[buff_idx]))
        {
            break;
        }
    }

    for(;;) // The main program loop.
    {
        recognized_extension = extension__recognize(Buffer->basename);
        if(Buffer->extension != recognized_extension)
        {
            syntax__load(&Syntax, recognized_extension);
            Buffer->extension = recognized_extension;
        }
        if(!file__get_git_branch(&Buffer[actual_file_idx])
           || !input__parse_key(&Buffer[actual_file_idx], &Config, &Modes,
                                &actual_file_idx, pressed_key))
        {
            break;
        }
        // An user has selected too big id for a file, select the last one.
        if(actual_file_idx > (argc - SIZE__IDX))
        {
            actual_file_idx = argc - SIZE__IDX;
        }

        // Flushes and renders always after the keypress.
        if(!window__render(Buffer, &Config, &Modes, &Syntax,
                           (const idx_t) argc - SIZE__IDX,
                           (const idx_t) actual_file_idx)
           || ((pressed_key = input__getch()) == -1))
        {
            break;
        }
        window__flush();
    }

free:
    for(idx_t buff_idx = 0; buff_idx < file_buffers; buff_idx++)
    {
        buffer__free(&Buffer[buff_idx]);
    }
    free(Buffer);
}

int main(int argc, char** argv)
{
    fiflo__run(argc, argv);
    fflush(NULL); // Clean both output buffers.

    return 0;
}
