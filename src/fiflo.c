#ifdef __linux__

#include "hardware.c"
#include "logic.c"

void usageInfo(void)
{
	puts("Usage: fiflo [filename or nothing].");
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

void programRound(void)
{
	char pressed_key;
	struct Params buff = {0, 1, 0, {'\0'}}; // Value initializer.
	for(;;)
	{
		pressed_key = unixGetch(); // TODO: FLUSHING.
		cleanFrame();
		buff = window(pressed_key, buff);
	}
}

int main(int argc, char *argv[])
{
	getSize(0); // Returns Y but doesn't matter for that case. Just a check.
	argcCheck(argc);
	const char *basename;

	if(argv[1] == NULL)
	{
		basename = "noname.asdf\0";
	}
	else if(strcmp(argv[1], "-h\0") == 0 || strcmp(argv[1], "--help\0") == 0)
	{
		usageInfo();
	}
	setFilename(basename);

//	window('\0', {0, 1, 0, {'\0'}});
	programRound();

	return 0;
}

#else

#include <stdio.h>

int main(void)
{
	fputs("Only Linux-based systems are supported, exit.\n", stderr);
	return 0;
}

#endif

