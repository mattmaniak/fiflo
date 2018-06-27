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
		cleanFrame(); // Provide one-frame rendering in a terminal window.
		window(pressed_key);
	}
}

int main(int argc, char *argv[])
{
	argcCheck(argc);
	if(argv[1] == NULL)
	{
		argv[1] = "noname.asdf\0";
	}
/*	else if(argv[1] == "-h" || argv[1] == "--help")
	{
		usageInfo(); // TODO: AND EXIT.
	}
*/	
	filenameLenCheck(argv[1]);
	setBaseFilename(argv[1]);

	readFromFile();
	puts(" < DEBUG");

	init();
	window(TERMINATOR);
	programRound();
	return 0;
}

