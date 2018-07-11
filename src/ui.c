// All rendered elements.

#include "ui.h"

void cursor(void)
{
	printf("%s%c%s\n", INVERT, ' ', RESET);
}

buff_t decIntLen(buff_t number)
{
	// Return a length of decimal integer. Eg. 2 from number = 12.
	int8_t len = 1;
	if(number >= 0) // Prevent from weird < 0 values.
	{
		while(number > 9)
		{
			len++;
			number /= 10;
		}
	}
	return len;
}

term_t getSize(bool axis) // Check terminal size.
{
	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		fprintf(stderr, "%s%i%c%i%s",
		"Min. term size is ", MIN_WIDTH, 'x', MIN_HEIGHT, ".\n");
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT)
	{
		fprintf(stderr, "%s%i%c%i%s",
		"Max. term size is ", MAX_WIDTH, 'x', MAX_HEIGHT, ", exited.\n");
		exit(1);
	}

	switch(axis)
	{
		case X:
			return win.ws_col;
		case Y:
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

// Cuts a string when is too long.
void printDynamicString(const char *string, term_t max_len)
{
	#define PROGRAM_LEN 15
	term_t x;
	term_t whitespace = getSize(X) - strlen(string) - PROGRAM_LEN;

	if(strlen(string) > max_len)
	{
		for(x = 0; x < max_len; x++)
		{
			printf("%c", string[x]);
		}
		printf("%s", "... ");
	}
	else
	{
		printf("%s", string);
		for(x = 0; x < whitespace; x++)
		{
			printf("%c", ' ');
		}
	}
}

void upperBar(const char *fname)
{
	const char *program = " Fiflo | file: \0";
	const char *shortcuts = " Exit: CTRL+C, save: CTRL+X.\0";
	term_t width;
	#define DOTS_AND_SPACE 4

	printf("%s%s", INVERT, program);
	printDynamicString(fname, getSize(X) - strlen(program) - DOTS_AND_SPACE);

	printf("%s", shortcuts);

	// Lower part of the bar.
	for(width = 0; width < getSize(X) - strlen(shortcuts); width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);
}

term_t autoFill(struct Params buff, char key, term_t fill)
{
	switch(buff.chars)
	{
		case 0:
			fill = BARS_SZ;
		break;

		default:
			if(key != BACKSPACE && buff.chars % getSize(X) == 0)
			{
				fill++;
			}
			else if(key == BACKSPACE && buff.chars % getSize(X) == getSize(X) - 1)
			{
				fill--;
			}
		break;
	}
	return fill;
}

// Lower border with a text.
void lowerBar(struct Params buff, char key)
{
	const char *lines_text = " lines: \0";
	const char *chars_text = " | chars: \0";
	const char *ascii_code_text = " | last char code: \0";

	term_t width;
	term_t whitespace
	= strlen(lines_text) + decIntLen(buff.lines)
	+ strlen(chars_text) + decIntLen(buff.chars) 
	+ strlen(ascii_code_text) + decIntLen(key);

	printf("%s%s%i%s%i%s%i", INVERT, lines_text, buff.lines, chars_text,
	buff.chars, ascii_code_text, key);

	for(width = 0; width < getSize(X) - whitespace; width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	term_t y;
	for(y = 0; y < getSize(Y); y++)
	{
		printf("%s", "\033[F\033[K");
	}
	fflush(stdout);
}

