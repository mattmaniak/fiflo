// All rendered elements except chars you type.

#include "ui.h"

int8_t decimalIntLen(int8_t number)
{
	// Return a length of decimal integer. Eg. 2 from number = 12.
	BUFF_T len = 1;

	while(number > 9)
	{
		len++;
		number /= 10;
	}
	return len;
}

void dynamicLen(char *filename)
{
	// If a filename is longer than 16. Only first 16 chars will be printed
	// and 3 dots at the end will be added.
	uint8_t chr_num;

	if(strlen(filename) > 16) // Max rendered first 32 chars.
	{
		for(chr_num = 0; chr_num < 16; chr_num++)
		{
			printf("%c", filename[chr_num]);
		}
		printf("%s", "..."); // strlen is 3.
	}
	else
	{
		printf("%s", filename);
		for(chr_num = 0; chr_num < 16 + 3 - strlen(filename); chr_num++)
		{
			printf("%c", ' ');
		}
	}
}

void upperBar(char *base_fn) // TODO: PATH + FN IN THE UPPER BAR.
{
	const char *program = " Fiflo | file: \0";

	uint16_t width;
	uint16_t whitespace = strlen(program) + strlen(base_fn);

	printf("%s%s%s", INVERT, program, base_fn);

	for(width = 0; width < windowSize('x') - whitespace; width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);
}

// Lower border with a text.
void lowerBar(BUFF_T lines, BUFF_T chars, char key)
{
	const char *lines_text = " lines: \0";
	const char *chars_text = " | chars: \0";
	const char *ascii_code_text = " | last char code: \0";

	uint16_t width;
	uint16_t whitespace =
	strlen(lines_text) + decimalIntLen(lines)
	+ strlen(chars_text) + decimalIntLen(chars) 
	+ strlen(ascii_code_text) + decimalIntLen(key);

	printf("%s%s%i%s%i%s%i", INVERT, lines_text, lines, chars_text, chars,
	ascii_code_text, key);

	char *bar_buff = malloc(windowSize('x')); // TODO: PLACE MALLOCS.
	pointerCheck(bar_buff);

	for(width = 0; width < windowSize('x') - whitespace; width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);

	free(bar_buff);
}

