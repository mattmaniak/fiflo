#include "args.h"

bool args__parse(Mod_t* const Modes, int* argc, char** const argv)
{
    // Notice: argv[0] is the program name.
    const int fname_arg_sz = 1;
    const int max_files    = 4;
    const int argc_max     = fname_arg_sz + max_files;
    const int orig_argc    = *argc;

    if(*argc > argc_max)
    {
        fprintf(stderr, "Maximum four additional args can be passed.\n");
        return false;
    }
    for(int arg_i = fname_arg_sz; arg_i < orig_argc; arg_i++)
    {
        /* Can't use the "ARG_MAX", because clang 6.0.0 with "-Weverything"
           doesn't recognize it. */
        if(strlen(argv[arg_i]) < (PATH_MAX + NAME_MAX))
        {
            if(!options__parse_and_print(Modes, argc, argv[arg_i]))
            {
                return false;
            }
            // printf("%d\n", *argc);
        }
        else
        {
            fprintf(stderr, "A maximum argument's length is %u.\n", PATH_MAX
                    + NAME_MAX);
            return false;
        }
    }
    return true;
}
