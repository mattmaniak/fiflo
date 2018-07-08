#ifdef __linux__

#include "fiflo.h"

#include "hardware.c"
#include "logic.c"

void sigHandler(int nothing)
{
	if(nothing == 0) {}
}

void usageInfo(void)
{
	puts("Usage: fiflo [option].");
	exit(0);
}


void programRound(void)
{
	char pressed_key;
	struct Params buff = readFile(buff);
	window(TERMINATOR, buff);

	for(;;)
	{
		pressed_key = unixGetch(); // TODO: FLUSHING.
		cleanFrame();
		buff = window(pressed_key, buff);
	}
}

void argcCheck(int arg_count)
{
	if(arg_count > 2)
	{
		fputs("Fiflo can handle max. one parameter.\n", stderr);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	const char *basename;

	signal(SIGTSTP, sigHandler); // CTRL_X
	getSize(0); // Returns Y but doesn't matter for that case. Just a check.
	argcCheck(argc);

	if(argv[1] == NULL)
	{
		basename = "noname.asdf\0";
	}
	else if(strcmp(argv[1], "-h\0") == 0 || strcmp(argv[1], "--help\0") == 0)
	{
		usageInfo();
	}
	else
	{
		basename = argv[1];
	}
	setFilename(basename);

	programRound();

	return 0;
}

#else

#include <stdio.h>

int main(void)
{
	fputs("Only Linux-based systems are supported, exited.\n", stderr);
	return 0;
}

#endif

