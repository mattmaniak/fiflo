#ifdef __linux__

#include "errors.c"
#include "hardware.c"
#include "logic.c"

void init(void)
{
	// Chars_c cannot be 0 because out of index error so will be reset.
	keyHandling(' ');
	chars_c--;
	text[0][0] = TERMINATOR;
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
	argcCheck(argc);
	if(argv[1] == NULL)
	{
		argv[1] = "noname.asdf\0"; // TODO: IS CORRECT?
	}
	else if(strcmp(argv[1], "-h\0") == 0 || strcmp(argv[1], "--help\0") == 0)
	{
		usageInfo();
	}
	setFilename(argv[1]);

	readFromFile();
	puts(" < DEBUG");

	init();
	window(TERMINATOR);
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

