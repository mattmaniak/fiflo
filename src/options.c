#include "options.h"

bool options__parse_and_print(const char* const arg)
{
    if(!strcmp(arg, "-h") || !strcmp(arg, "--help"))
    {
        printf("%s\n\n%s\n%s\n%s\n",
        "Usage: fiflo [basename/filename/option].",

        "Options:      Description:",
        "-h, --help    Show the help.",
        "-v, --version Display info about this version.");

        return false;
    }
    else if(!strcmp(arg, "-v") || !strcmp(arg, "--version"))
    {
        printf("%s\n%s\n%s\n%s\n%s\n",
        "|``",
        "|``",
        VERSION,
        COPYRIGHT,
        REPO);

        return false;
    }
    return true;
}
