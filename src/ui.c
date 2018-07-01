// All rendered elements except chars you type.

#include "ui.h"

int8_t decIntLen(int8_t number)
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

WIN_DIMENSION termSize(bool axis) // Check terminal size.
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
		"Max. term size is ", MAX_WIDTH, X, MAX_HEIGHT, ".\n");
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

	WIN_DIMENSION width;
	WIN_DIMENSION whitespace = strlen(program) + strlen(filename);

	printf("%s%s%s", INVERT, program, filename); // TODO: RENDERING LIMIT
	for(width = 0; width < termSize(X) - whitespace; width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);
}

WIN_DIMENSION autoFill(WIN_DIMENSION fill, char key, struct Params buff)
{
	if(buff.chars == 0 || text[0][0] == LINEFEED) // No visible char.
	{
		fill = 1;
	}
	else if(buff.chars > 1)
	{
		if(key != BACKSPACE &&  buff.chars % termSize(X) == 1)
		{
			buff.lines++;
		}
		else if(key == BACKSPACE && buff.chars % termSize(X) == 0)
		{
			buff.lines--;
		}
	}
	return fill;
}

// Lower border with a text.
void lowerBar(char key, struct Params buff)
{
	const char *lines_text = " lines: \0";
	const char *chars_text = " | chars: \0";
	const char *ascii_code_text = " | last char code: \0";

	WIN_DIMENSION width;
	WIN_DIMENSION whitespace
	= strlen(lines_text) + decIntLen(buff.lines)
	+ strlen(chars_text) + decIntLen(buff.chars) 
	+ strlen(ascii_code_text) + decIntLen(key);

	printf("%s%s%i%s%i%s%i", INVERT, lines_text, buff.lines, chars_text,
	buff.chars, ascii_code_text, key);

	char *bar_buff = malloc(termSize(X)); // TODO: PLACE MALLOCS.
	pointerCheck(bar_buff, "Cannot allocate memory for lower bar, exit.\0");

	for(width = 0; width < termSize(X) - whitespace; width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);

	free(bar_buff);
}

void cleanFrame(void) // To provide rendering in a one frame.
{
	WIN_DIMENSION lines;
	for(lines = 0; lines < termSize(Y); lines++)
	{
		printf("%s", "\033[F\033[K");
	}
	fflush(stdout);
}

