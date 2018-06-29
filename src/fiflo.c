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

void initRound(void)
{
	// Chars_c cannot be 0 because out of index error so will be reset.
	keyHandling(' ');
	chars_c--;
	text[0][0] = TERMINATOR;
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
	const char *base_filename;

	if(argv[1] == NULL)
	{
		base_filename = "noname.asdf\0";
	}
	else if(strcmp(argv[1], "-h\0") == 0 || strcmp(argv[1], "--help\0") == 0)
	{
		usageInfo();
	}
	setFilename(base_filename);

	readFromFile(); // DEBUG
	puts(" < DEBUG");

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

