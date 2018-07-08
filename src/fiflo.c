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
	struct Params buff = readFile(buff);
	window(buff.text[buff.chars], buff);

	for(;;)
	{
		pressed_key = unixGetch(); // TODO: FLUSHING.
		cleanFrame();
		buff = window(pressed_key, buff);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGTSTP, sigHandler); // CTRL_X

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

