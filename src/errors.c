#include "errors.h"

// Most of separated wrong case possibilities: just errors et al.

void usageInfo(void)
{
	fputs("Usage: fiflo [base filename-only-for-WIP].\n", stderr);
	exit(1);
}

void baseFilenameLenCheck(char base_filename[])
{
	if(strlen(base_filename) > 256) // Max. base filename len for *nix is 255.
	{
		fputs("Maximum base filename length: 255 chars.\n", stderr);
		exit(1);
	}
}

void argcCheck(int arg_count)
{
	if(arg_count > 2)
	{
		usageInfo();
		exit(1);
	}
}

