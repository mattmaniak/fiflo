#include "src/hardware.c"
#include "src/render.c"

// File with the main logic.

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t lines_amount = 1, chars_amount = 1; // text = lines + chars

void keyCheck(char key) // TODO: simplify these ifs!
{
	if(key == CTRL_X) // Check if exit key is pressed.
	{
		cleanFrame();
		exit(0);
	}

	if(key == ENTER) // Check if newline should be inserted.
	{
		lines_amount++;
		if(lines_amount >= 9) // 9 only for testing.
		{
			lines_amount = 9;
		}
	}
	if(key == BACKSPACE) // Check if user want to remove a last char.
	{
		chars_amount--;
		if(chars_amount <= 0)
		{
			chars_amount = 0;
		}
	}
	else
	{
		chars_amount++;
		if(chars_amount >= 80)
		{
			chars_amount = 80; // TODO: the last char is overwritten.
		}
	}
}

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

void usage(void)
{
	fputs("Usage: fiflo [base filename-only-for-WIP]\n", stderr);
	exit(1);
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char* argv[])
{
	if(argc > 2 || argv[1] == NULL)
	{
		usage();
	}

	// TODO: null terminator for a base filename.
	if(strlen(argv[1]) > 256) // Max. base filename len for *nix is 255.
	{
		fputs("Maximum base filename length: 255 chars.\n", stderr);
		exit(1);
	}
	window(lines_amount, chars_amount, '>', argv[1]);
	programRound(argv[1]);
	return 0;
}

