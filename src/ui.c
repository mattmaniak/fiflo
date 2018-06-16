// All rendered elements except chars you type.

#include "ui.h"
#include "hardware.h"

void cursor(void) // Blinking floor.
{
	printf("%s%s%c%s", BOLD, BLINK, '|', RESET);
}

static int8_t decimalIntLen(int8_t number) // Return len of decimal charchars.
{
	int8_t len = 1;
	while(number > 9)
	{
		len++;
		number /= 10;
	}
	return len;
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

static void dynamicLen(char base_filename[])
{
	uint8_t char_pos;

	if(strlen(base_filename) >= 16) // Max rendered first 32 chars.
	{
		for(char_pos = 0; char_pos < 16; char_pos++)
		{
			printf("%c", base_filename[char_pos]);
		}
		printf("%s", "..."); // strlen is 3.
	}
	else
	{
		printf("%s", base_filename);
		for(char_pos = 0; char_pos < 19 - strlen(base_filename); char_pos++)
		{
			printf("%c", ' ');
		}
	}
}

// Lower border with a text.
void lowerBar(int8_t lines, int8_t chars, char key, char base_filename[])
{
	uint16_t width;

	char program_name[10] = " Fiflo | \0";
	char chars_text[11] = " | chars: \0";
	char lines_text[11] = " | lines: \0";
	char ascii_code_text[22] = "| Last char (ASCII): \0";

	uint16_t whitespace = strlen(program_name) + strlen(ascii_code_text)
	+ decimalIntLen(key) + strlen(chars_text) + decimalIntLen(chars)
	+ strlen(lines_text) + decimalIntLen(lines) + 19;

	char* bar_buffer = malloc(windowSize('x'));
	pointerCheck(bar_buffer);

	printf("%s%s", INVERT, program_name);

	dynamicLen(base_filename);

	for(width = 0; width < windowSize('x') - whitespace; width++)
	{
		printf("%c", ' ');
	}

	printf("%s%i%s%i%s%i%s", ascii_code_text, key, chars_text, chars,
	lines_text, lines, RESET);

	free(bar_buffer);
}

