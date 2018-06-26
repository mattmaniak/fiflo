#include "errors.c"
#include "hardware.c"
#include "logic.c"

void programRound(char* filename)
{
	char pressed_key;
	for(;;)
	{
		pressed_key = unixGetch(); // TODO: FLUSHING.
		keyHandling(pressed_key, filename);
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
	filenameLenCheck(argv[1]);
	setBaseFilename(argv[1]);

	keyHandling('>', argv[1]);
	window('>');
	programRound(argv[1]);
	return 0;
}

