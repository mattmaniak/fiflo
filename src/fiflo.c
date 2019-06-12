#include "fiflo.h"

void fiflo__run(int argc, char** argv)
{
    size_t current_file_idx = 0;
    char pressed_key = '\0'; // This assignment as the initializer only.

    Buff_t* Buffer;
    Conf_t  Config;
    Mod_t   Modes;

    modes__init(&Modes);

    if((Buffer = malloc((argc - IDX) * sizeof(Buff_t))) == NULL)
    {
        fprintf(stderr, "Can't alloc the memory for the file buffers.\n");
        goto free;
    }


    for(int fname_idx = 1; fname_idx < argc; fname_idx++)
    {
        if(!buffer__init(&Buffer[fname_idx - IDX]))
        {
            goto free;
        }
        if(!config__load(&Config))
        {
            goto free;
        }
        if(!file__set_name(&Buffer[fname_idx - IDX], argv[fname_idx]))
        {
            goto free;
        }
        if(!file__load(&Buffer[fname_idx - IDX], &Config))
        {
            goto free;
        }
        strcpy(Buffer[fname_idx - IDX].fname_copy,
               Buffer[fname_idx - IDX].fname);

        puts(Buffer[fname_idx - IDX].pathname);

        if(!file__get_git_branch(&Buffer[fname_idx - IDX]))
        {
            break;
        }
    }

    for(;;) // The main program loop.
    {
        if(!file__get_git_branch(&Buffer[current_file_idx]))
        {
            break;
        }
        if(!input__parse_key(&Buffer[current_file_idx], &Config, &Modes,
                             &current_file_idx, pressed_key))
        {
            break;
        }
        if(current_file_idx >= (argc - 1))
        {
            current_file_idx = argc - 2;
        }
        // Flushes and renders always after the keypress.
        if(!window__render(&Buffer[current_file_idx], &Config, &Modes))
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
    for(int buff_idx = 1; buff_idx < argc; buff_idx++)
    {
        buffer__free(&Buffer[buff_idx - IDX]);
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
