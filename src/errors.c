// Most of separated wrong case possibilities: just errors et al.

#include "errors.h"

void usageInfo(void)
{
	fputs("Usage: fiflo [base filename-only-for-WIP].\n", stderr);
	exit(1);
}

void filenameLenCheck(char filename[])
{
	if(strlen(filename) > 255) // Max. base filename len for ext4 is 255.
	{
		fputs("Max filename length is 255 chars.\n", stderr);
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

