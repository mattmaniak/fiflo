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

void initRound(void) // Chars_c can't be 0 because out of index. Will be reset.
{
	readFromFile(); // DEBUG
	puts(" < DEBUG");
	window(TERMINATOR);
}

void programRound(void)
{
	char pressed_key;
	for(;;)
	{
		pressed_key = unixGetch(); // TODO: FLUSHING.
		keyHandling(pressed_key);
		cleanFrame();
		window(pressed_key);
	}
}

int main(int argc, char *argv[])
{
	termSize(0);
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

	initRound();
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

