#include "args.h"

bool args__parse(Mod_t* const Modes, int* argc, char** const argv)
{
    // Notice: argv[0] is the program name.
    const int fname_arg_sz = 1;
    const int max_files    = 4;
    const int options_max  = 8;
    const int argc_max     = fname_arg_sz + max_files + options_max;
    const int orig_argc    = *argc;

    if(*argc > argc_max)
    {
        fprintf(stderr, "Max. %d additional args can be passed.\n",
                argc_max - fname_arg_sz);
        return false;
    }
    for(int arg_i = fname_arg_sz; arg_i < orig_argc; arg_i++)
    {
        if(strlen(argv[arg_i]) < ARG_MAX)
        {
            if(!options__parse_and_print(Modes, argv[arg_i]))
            {
                return false;
            }
            if((argv[arg_i] != NULL) && (argv[arg_i][0] == '-'))
            {
                if(*argc >= 2)
                {
                    *argc -= 1;
                }
                for(int argc_i = arg_i; argc_i < *argc; argc_i++)
                {
                    argv[argc_i] = argv[argc_i + SIZE__NEXT];
                }
            }
        }
        else
        {
            fprintf(stderr, "A maximum argument's length is %u.\n",
                    PATH_MAX + NAME_MAX);
            return false;
        }
    }
    return true;
}
