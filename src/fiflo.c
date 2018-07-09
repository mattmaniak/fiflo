#ifdef __linux__

#include "fiflo.h"

#include "hardware.c"
#include "logic.c"

void sigHandler(int nothing)
{
	if(nothing == 0) {}
}

void showHelp(void)
{
	puts("Usage: fiflo [option].");
	putchar('\n');
	puts("Options:    Description:");
	puts("<NULL>      Create and open the default file - 'noname.asdf'.");
	puts("<file>      Open the textfile named 'file'.");
	puts("-h, --help  Show program help.");
}

void programRound(const char *bname)
{
	struct Params buff = readFile(buff, bname);
	window(TERMINATOR, buff);

	for(;;)
	{
		char pressed_key = unixGetch(); // TODO: FLUSHING.
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
		showHelp();
		exit(0);
	}
	else
	{
		basename = argv[1];
	}
	programRound(basename);

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

