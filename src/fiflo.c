#include "fiflo.h"

void fiflo__run(const char* const arg)
{
    char pressed_key = '\0'; // This assignment as the initializer only.

    Buff_t Buffer;
    Conf_t Config;
    Mod_t  Modes;

    modes__init(&Modes);

    if(!buffer__init(&Buffer))
    {
        goto free;
    }
    if(!config__load(&Config))
    {
        goto free;
    }
    if(!file__set_name(&Buffer, arg))
    {
        goto free;
    }
    if(!file__load(&Buffer, &Config))
    {
        goto free;
    }
    strncpy(Buffer.fname_copy, Buffer.fname, PATH_MAX);

    for(;;) // The main program loop.
    {
        if(!file__get_git_branch(&Buffer))
        {
            break;
        }
        if(!input__parse_key(&Buffer, &Config, &Modes, pressed_key))
        {
            break;
        }
        // Flushes and renders always after the keypress.
        if(!window__render(&Buffer, &Config, &Modes))
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
    buffer__free(&Buffer);
}

int main(int argc, char** argv)
{
    const int argc_max = 2;

    if(argc > argc_max)
    {
        fprintf(stderr, "Only one additional arg can be passed.\n");
        goto exit;
    }

    if(argv[1] == NULL) // Sets the default basename and starts.
    {
        fiflo__run("");
        goto exit;
    }
    /* Can't use the "ARG_MAX", because clang 6.0.0 with -Weverything doesn't
    recognize it. */
    else if(strlen(argv[1]) < (PATH_MAX + NAME_MAX))
    {
        if(!options__parse_and_print(argv[1]))
        {
            goto exit;
        }
        fiflo__run(argv[1]);
        goto exit;
    }
    else
    {
        fprintf(stderr, "Maximum parameter length is %u\n",
                PATH_MAX + NAME_MAX);
        goto exit;
    }
    exit:
    fflush(NULL); // Clean both output buffers.

    return 0;
}
