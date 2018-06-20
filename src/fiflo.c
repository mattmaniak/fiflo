// File with the main logic.

#include "errors.c"
#include "hardware.c"
#include "logic.c"

void programRound(char filename[])
{
	for(;;)
	{
		char pressed_key = unixGetch();
		keyHandling(lines_c, chars_c, pressed_key, filename);
		cleanFrame(); // Provide "one-window" rendering in a terminal.
		window(lines_c, chars_c, pressed_key, filename);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char* argv[])
{
	if(argv[1] == NULL)
	{
		argv[1] = "noname.asdf";
	}
	filenameLenCheck(argv[1]);
	argcCheck(argc);

	initWindow(lines_c, chars_c, argv[1]);
	programRound(argv[1]);
	return 0;
}

