#ifdef __linux__

#include "fiflo.h"

#include "hardware.c"
#include "logic.c"

void showHelp(void)
{
	puts("Usage: fiflo [option].\n");
	puts("Options:    Description:");
	puts("<NULL>      Create and open the default file - 'noname.asdf'.");
	puts("<file>      Open the textfile named 'file'.");
	puts("-h, --help  Show program help.");
}

void programRound(char *name)
{
	struct Params buff = readFile(buff, name);
	window(TERMINATOR, buff);

	for(;;)
	{
		char pressed_key = unixGetch(); // TODO: FLUSHING.
		cleanFrame();
		buff = window(pressed_key, buff);
	}
}

void freeExit(int nothing) // TODO: FREEING MEMORY.
{
	system("stty echo");
	cleanFrame();
	exit(0);
	if(nothing == 0) {}
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
	signal(SIGTSTP, freeExit); // CTRL_X
	signal(SIGINT, freeExit); // CTRL_C

	getSize(true); // Returns X but doesn't matter for that case. Just a check.
	argcCheck(argc);

	if(argv[1] == NULL)
	{
		programRound("noname.asdf\0");
	}
	else if(strcmp(argv[1], "-h\0") == 0 || strcmp(argv[1], "--help\0") == 0)
	{
		showHelp();
		exit(0);
	}
	else
	{
		programRound(argv[1]);
	}
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

