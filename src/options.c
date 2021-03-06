#include "options.h"

bool options__parse_and_print(Modes* const modes, const char* const arg)
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
               RELEASE__VERSION,
               RELEASE__COPYRIGHT,
               RELEASE__REPO);

        return false;
    }
    else if(!strcmp(arg, "-s") || !strcmp(arg, "--spaces"))
    {
        modes->tabs_to_spaces = true;
        return true;
    }
    else if(!strcmp(arg, "-t") || !strcmp(arg, "--tabs"))
    {
        modes->tabs_to_spaces = false;
        return true;
    }
    return true;
}
