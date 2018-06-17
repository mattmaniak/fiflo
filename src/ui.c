// All rendered elements except chars you type.

#include "ui.h"
#include "hardware.h"

void cursor(void)
{
	printf("%s%s%c%s", BOLD, BLINK, '|', RESET);
}

int8_t decimalIntLen(int8_t number)
{
	// Return a length of decimal integer. Eg. 2 from number = 12.
	int8_t len = 1;

	while(number > 9)
	{
		len++;
		number /= 10;
	}
	return len;
}

void dynamicLen(char filename[])
{
	// If a filename is longer than 16. Only first 16 chars will be printed
	// and 3 dots at the end will be added.
	uint8_t char_pos;

	if(strlen(filename) > 16) // Max rendered first 32 chars.
	{
		for(char_pos = 0; char_pos < 16; char_pos++)
		{
			printf("%c", filename[char_pos]);
		}
		printf("%s", "..."); // strlen is 3.
	}
	else
	{
		printf("%s", filename);
		for(char_pos = 0; char_pos < 19 - strlen(filename); char_pos++)
		{
			printf("%c", ' ');
		}
	}
}

void upperBar(void)
{
	uint16_t width;
	printf("%s", INVERT);
	for(width = 0; width < windowSize('x'); width++)
	{
		printf("%c", ' ');
	}
	printf("%s", RESET);
}

// Lower border with a text.
void lowerBar(int8_t lines, int8_t chars, char key, char filename[])
{
	uint16_t width;

	char program_name[10] = " Fiflo | \0";
	char chars_text[11] = " | chars: \0";
	char lines_text[11] = " | lines: \0";
	char ascii_code_text[19] = "| Last char code: \0";

	uint16_t whitespace = strlen(program_name) + strlen(ascii_code_text)
	+ decimalIntLen(key) + strlen(chars_text) + decimalIntLen(chars)
	+ strlen(lines_text) + decimalIntLen(lines) + 19;

	char* bar_buffer = malloc(windowSize('x'));
	pointerCheck(bar_buffer);

	printf("%s%s", INVERT, program_name);

	dynamicLen(filename);

	for(width = 0; width < windowSize('x') - whitespace; width++)
	{
		printf("%c", ' ');
	}

	printf("%s%i%s%i%s%i%s", ascii_code_text, key, chars_text, chars,
	lines_text, lines, RESET);

	free(bar_buffer);
}

