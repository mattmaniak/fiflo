#include "errors.c"
#include "hardware.c"
#include "logic.c"

void programRound()
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

	keyHandling('>');
	window('>');
	programRound();
	return 0;
}

