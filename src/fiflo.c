#include "fiflo.h"

void fiflo__run(int argc, char** argv)
{
    const size_t no_filename_as_arg = 1;

    char pressed_key        = '\0'; // For an initialization purpose only.
    size_t current_file_idx = 0;

    Buff_t*  Buffer;
    Conf_t   Config;
    Mod_t    Modes;
    Syntax_t Syntax;

    Syntax.kwrds_idx = 0;

    size_t additional_argc_idx = (argc > 1)
                                 ? (size_t) argc - IDX - no_filename_as_arg
                                 : (size_t )argc - IDX;

    char** additional_argv     = &argv[1];

    modes__init(&Modes);

    if((Buffer = malloc((additional_argc_idx + IDX) * sizeof(Buff_t))) == NULL)
    {
        fprintf(stderr, "Can't alloc a memory for file buffers.\n");
        goto free;
    }
    Buffer->extension = NONE;

    for(idx_t buff_idx = 0; buff_idx <= additional_argc_idx; buff_idx++)
    {
        if(!buffer__init(&Buffer[buff_idx]))
        {
            goto free;
        }
        if(!config__load(&Config))
        {
            goto free;
        }
        if(!file__set_name(&Buffer[buff_idx], additional_argv[buff_idx]))
        {
            goto free;
        }
        if(!file__load(&Buffer[buff_idx], &Config))
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
        int recognized_extension = extension__recognize(Buffer->basename);

        if(Buffer->extension != recognized_extension)
        {
            syntax__load(&Syntax, recognized_extension);
            Buffer->extension = recognized_extension;
        }
        if(!file__get_git_branch(&Buffer[current_file_idx]))
        {
            break;
        }
        if(!input__parse_key(&Buffer[current_file_idx], &Config, &Modes,
                             &current_file_idx, pressed_key))
        {
            break;
        }
        if(current_file_idx > additional_argc_idx)
        {
            current_file_idx = additional_argc_idx;
        }
        // Flushes and renders always after the keypress.
        if(!window__render(Buffer, &Config, &Modes, &Syntax,
                           (const idx_t) additional_argc_idx,
                           (const idx_t) current_file_idx))
        {
            break;
        }
        if((pressed_key = input__getch()) == ERROR)
        {
            break;
        }
        window__flush();
    }

    free:
        for(idx_t buff_idx = 0; buff_idx <= additional_argc_idx; buff_idx++)
        {
            buffer__free(&Buffer[buff_idx]);
        }
        free(Buffer);
}

int main(int argc, char** argv)
{
    if(!args__parse(argc, argv))
    {
        goto exit;
    }
    fiflo__run(argc, argv);

    exit:
        fflush(NULL); // Clean both output buffers.
        return 0;
}
