#include "options.h"

bool options__parse_and_print(const char* arg)
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
		"fiflo v3.1.0 (WIP)",
		"(C) 2018-2019 mattmaniak under the MIT License",
		"https://gitlab.com/mattmaniak/fiflo.git");
		return false;
	}
	return true;
}
