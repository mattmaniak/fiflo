// Most of separated wrong case possibilities: just errors et al.

#include "errors.h"

void usageInfo(void)
{
	fputs("Usage: fiflo [filename or nothing].\n", stderr);
	exit(1);
}

void argcCheck(int arg_count)
{
	if(arg_count > 2)
	{
		usageInfo();
		exit(1);
	}
}

