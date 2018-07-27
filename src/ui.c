// All rendered elements.

#include "ui.h"

// Return a length of decimal integer. Eg. 2 from number = 12.
buff_t decIntLen(buff_t number)
{
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

term_t termSize(bool axis) // Check terminal size.
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

void charsLimit(buff_t chars)
{
	if(chars > MAX_CHARS)
	{
		fprintf(stderr, "%s%i%s%i%s",
		"Max. lines amount: ", MAX_CHARS, ", got: ", chars,
		". Stretch your terminal or sth.\n");
		exit(1);
	}
}

// Cuts a string when is too long.
void printDynamicFilename(const char *string, const char *prog, term_t max_len)
{
	term_t pos;
	term_t whitespace = termSize(X) - strlen(string) - strlen(prog);

	if(strlen(string) > max_len)
	{
		for(pos = 0; pos < max_len; pos++)
		{
			printf("%c", string[pos]);
		}
		printf("%s", "... ");
	}
	else
	{
		printf("%s", string);
		for(pos = 0; pos < whitespace; pos++)
		{
			printf("%c", ' ');
		}
	}
}

void bar(struct Data buff, char key)
{
	const char *program = " fiflo | file: \0";
	const char *lines_text = " lines: \0";
	const char *chars_text = " | chars: \0";
	const char *ascii_code_text = " | last: \0";
	const char *shortcuts = " | save: CTRL+D | exit: CTRL+X \0";

	const uint8_t dots_and_space = 4;
	term_t x;

	printf("%s%s", INVERT, program);
	printDynamicFilename(buff.filename, program, termSize(X) - strlen(program)
	- dots_and_space);

	// Lower part of the bar.
	term_t whitespace = strlen(shortcuts)
	+ strlen(lines_text) + decIntLen(buff.lines)
	+ strlen(chars_text) + decIntLen(buff.chars)
	+ strlen(ascii_code_text) + decIntLen(key);

	printf("%s%i%s%i%s%i%s", lines_text, buff.lines, chars_text,
	buff.chars, ascii_code_text, key, shortcuts);

	for(x = 0; x < termSize(X) - whitespace; x++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);

}

void cleanFrame(void) // To provide rendering in a one frame.
{
	term_t y;
	for(y = 0; y < termSize(Y); y++)
	{
		printf("%s", "\033[F\033[K");
	}
	fflush(stdout);
}

