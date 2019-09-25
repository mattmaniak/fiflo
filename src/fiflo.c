#include "fiflo.h"

void fiflo__run(int argc, char** const argv)
{
    const int fname_arg_sz = 1;

    char   pressed_key     = '\0'; // For an initialization purposes only.
    size_t actual_file_i   = 0;
    char** additional_argv = &argv[1];
    size_t file_buffers;
    int    recognized_extension;

    Buff_t*  Buffer;
    Conf_t   Config;
    Mod_t    Modes;
    Syntax_t Syntax;

    modes__init(&Modes);
    if(!args__parse(&Modes, &argc, argv))
    {
        return;
    }
    argc         -= fname_arg_sz;
    file_buffers = ((const size_t) argc == 0) ? 1 : (const size_t) argc;

    // printf("%lu\n", file_buffers);

    Buffer = malloc((const size_t) argc * sizeof(Buff_t));
    if(Buffer == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for file buffers.\n");
        goto free;
    }
    Buffer->extension = EXTENSION__NONE;

    for(size_t buff_i = 0; buff_i < file_buffers; buff_i++)
    {
        if(additional_argv[buff_i][0] == '-')
        {
            additional_argv[buff_i][0] = '\0';
        }
        if(!buffer__init(&Buffer[buff_i]) || !config__load(&Config)
           || !file_io__set_name(&Buffer[buff_i], additional_argv[buff_i])
           || !file_io__load(&Buffer[buff_i], &Config, &Modes))
        {
            goto free;
        }
        strcpy(Buffer[buff_i].fname_copy, Buffer[buff_i].fname);

        if(!file_io__get_git_branch(&Buffer[buff_i]))
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
        if(!file_io__get_git_branch(&Buffer[actual_file_i])
           || !input__parse_key(&Buffer[actual_file_i], &Config, &Modes,
                                &actual_file_i, pressed_key))
        {
            break;
        }
        // An user has selected too big id for a file, select the last one.
        if(actual_file_i > ((const size_t) argc - SIZE__I))
        {
            actual_file_i = (const size_t) argc - SIZE__I;
        }

        // Flushes and renders always after the keypress.
        if(!window__render(Buffer, &Config, &Modes, &Syntax,
                           (const size_t) argc - SIZE__I,
                           (const size_t) actual_file_i)
           || ((pressed_key = input__getch()) == -1))
        {
            break;
        }
        window__flush();
    }

free:
    for(size_t buff_i = 0; buff_i < file_buffers; buff_i++)
    {
        buffer__free(&Buffer[buff_i]);
    }
    free(Buffer);
}

// void fiflo__main_loop(void)
// {
//     for(;;) // The main program loop.
//     {
//         recognized_extension = extension__recognize(Buffer->basename);
//         if(Buffer->extension != recognized_extension)
//         {
//             syntax__load(&Syntax, recognized_extension);
//             Buffer->extension = recognized_extension;
//         }
//         if(!file_io__get_git_branch(&Buffer[actual_file_i])
//            || !input__parse_key(&Buffer[actual_file_i], &Config, &Modes,
//                                 &actual_file_i, pressed_key))
//         {
//             break;
//         }
//         // An user has selected too big id for a file, select the last one.
//         if(actual_file_i > (argc - SIZE__I))
//         {
//             actual_file_i = argc - SIZE__I;
//         }
//
//         // Flushes and renders always after the keypress.
//         if(!window__render(Buffer, &Config, &Modes, &Syntax,
//                            (const size_t) argc - SIZE__I,
//                            (const size_t) actual_file_i)
//            || ((pressed_key = input__getch()) == -1))
//         {
//             break;
//         }
//         window__flush();
//     }
// }

int main(int argc, char** argv)
{
    fiflo__run(argc, argv);
    fflush(NULL); // Clean both output buffers.

    return 0;
}
