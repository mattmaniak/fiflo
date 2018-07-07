// All rendered elements except chars you type.

#include "ui.h"

void cursor(void)
{
	printf("%s%c%s", INVERT, ' ', RESET);
}

BUFF_T decIntLen(BUFF_T number)
{
	// Return a length of decimal integer. Eg. 2 from number = 12.
	int8_t len = 1;
	// Works properly only for >= 0 numbers.
	while(number > 9)
	{
		len++;
		number /= 10;
	}
	return len;
}

TERM_SIZE getSize(bool axis) // Check terminal size.
{
	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		fprintf(stderr, "%s%i%c%i%s",
		"Min. term size is ", MIN_WIDTH, X, MIN_HEIGHT, ".\n");
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT)
	{
		fprintf(stderr, "%s%i%c%i%s",
		"Max. term size is ", MAX_WIDTH, X, MAX_HEIGHT, ", exited.\n");
		exit(1);
	}

	switch(axis)
	{
		case 1: // X
			return win.ws_col;
		case 0: // Y
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

void upperBar(const char *filename)
{
	const char *program = " Fiflo | file: \0";
	const char *shortcuts = " Exit: CTRL+C, save: CTRL+X.\0";

	TERM_SIZE width;
	TERM_SIZE whitespace = strlen(program) + strlen(filename);

	printf("%s%s%s", INVERT, program, filename); // TODO: RENDERING LIMIT
	for(width = 0; width < getSize(X) - whitespace; width++)
	{
		printf("%c", ' ');
	}

	printf("%s", shortcuts);
	for(width = 0; width < getSize(X) - strlen(shortcuts); width++)
	{
		printf("%c", ' ');
	}

	printf("%s", RESET);
}

TERM_SIZE autoFill(TERM_SIZE fill, char key, struct Params buff)
{
	switch(buff.chars)
	{
		case 0:
			fill = BARS_AMOUNT;
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
void lowerBar(char key, struct Params buff)
{
	const char *lines_text = " lines: \0";
	const char *chars_text = " | chars: \0";
	const char *ascii_code_text = " | last char code: \0";

	TERM_SIZE width;
	TERM_SIZE whitespace
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
	TERM_SIZE y;
	for(y = 0; y < getSize(Y); y++)
	{
		printf("%s", "\033[F\033[K");
	}
	fflush(stdout);
}

