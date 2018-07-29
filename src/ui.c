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

// Cuts a string when is too long. TODO
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

void bar(struct Data buff, char key) // TODO: SIMPLIFY!
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

