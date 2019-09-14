#include "args.h"

bool args__parse(const int argc, char** const argv)
{
    // Notice: argv[0] is the program name.
    const int fname_arg_sz = 1;
    const int max_files    = 4;
    const int argc_max     = fname_arg_sz + max_files;

    if(argc > argc_max)
    {
        fprintf(stderr, "Maximum four additional args can be passed.\n");
        return false;
    }
    for(int arg_idx = fname_arg_sz; arg_idx < argc; arg_idx++)
    {
        /* Can't use the "ARG_MAX", because clang 6.0.0 with "-Weverything"
           doesn't recognize it. */
        if(strlen(argv[arg_idx]) < (PATH_MAX + NAME_MAX))
        {
            if(!options__parse_and_print(argv[arg_idx]))
            {
                return false; // Print Eg. help end exit.
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
