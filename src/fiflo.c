// File with the main logic.

#include "errors.c"
#include "hardware.c"
#include "render.c"

void programRound(char base_filename[])
{
	while(1)
	{
		char pressed_key = unixGetch();
		keyCheck(pressed_key); // Is it ENTER, BACKSPACE...? More specific.
		cleanFrame(); // Provide "one-window" rendering in a terminal.
		window(lines_amount, chars_amount, pressed_key, base_filename);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char* argv[])
{
	if(argv[1] == NULL)
	{
		argv[1] = "null.asdf";
	}
	baseFilenameLenCheck(argv[1]);
	argcCheck(argc);

	window(lines_amount, chars_amount, '>', argv[1]);
	programRound(argv[1]);
	return 0;
}

