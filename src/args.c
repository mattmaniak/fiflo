#include "args.h"

bool args__parse(int argc, char** argv)
{
    // Notice: argv[0] is the program name.

    const int max_files = 4;
    const int argc_max  = 1 + max_files;

    if(argc > argc_max)
    {
        fprintf(stderr, "Max. four additional args can be passed.\n");
        return false;
    }
    for(int arg_idx = 1; arg_idx < argc; arg_idx++)
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
            fprintf(stderr, "Max. argument length: %u.\n", PATH_MAX + NAME_MAX);
            return false;
        }
    }
    return true;
}
