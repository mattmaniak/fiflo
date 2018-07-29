#ifdef __linux__

#include "fiflo.h"

#include "handling.c"
#include "render.c"

void run(char *name)
{
	struct Data buff = readFile(buff, name);
	char pressed_key = TERMINATOR;

	windowFill(-1); // This frame will be instantly cleaned.

	for(;;)
	{
		cleanFrame();
		buff = window(buff, pressed_key);
		pressed_key = unixGetch();
		if(buff.lines < termSize(Y) - BAR_SZ)
		{
			windowFill(buff.lines);
		}
	}
}

void ignoreSig(int nothing) // Arg for "‘__sighandler_t {aka void (*)(int)}".
{
	if(nothing == 0) {}
}

void argcCheck(int arg_count)
{
	if(arg_count > 2)
	{
		fputs("Fiflo can handle max. one additional arg.\n", stderr);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGTSTP, ignoreSig); // CTRL_Z
	signal(SIGINT, ignoreSig); // CTRL_C

	argcCheck(argc);
	if(argv[1] == NULL)
	{
		run("noname.asdf\0");
	}
	else if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		showHelp();
		exit(0);
	}
	else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
	{
		showVersion();
		exit(0);
	}
	else
	{
		run(argv[1]);
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

