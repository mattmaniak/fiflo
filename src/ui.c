#include "ui.h"
#include "hardware.h"

// All rendered elements except chars you type.
void cursor(void) // Blinking floor.
{
	printf("%s%c%s", INVERT, ' ', RESET);
}

static int8_t decimalIntLen(int8_t chars) // Return len of decimal charchars.
{
	int8_t len = 1;
	while(chars > 9)
	{
		len++;
		chars /= 10;
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

 // Lower border with a text.
void lowerBar(int8_t lines, int8_t chars, char base_filename[])
{
	uint8_t char_pos;
	uint16_t width;
	char program_name[10] = " Fiflo | \0";
	char chars_text[10] = "| chars: \0";
	char lines_text[11] = " | lines: \0";

	uint16_t whitespace // Between base filename and chars.
	= strlen(program_name)
	+ decimalIntLen(chars)
	+ strlen(chars_text)
	+ strlen(lines_text)
	+ decimalIntLen(lines) + 35;

	char* bar_buffer = malloc(windowSize('x'));
	pointerCheck(bar_buffer);

	printf("%s%s", INVERT, program_name);

	if(strlen(base_filename) >= 32) // Max rendered first 32 chars.
	{
		for(char_pos = 0; char_pos < 32; char_pos++)
		{
			printf("%c", base_filename[char_pos]);
		}
		printf("%s", "..."); // strlen is 3.
	}
	else
	{
		printf("%s", base_filename);
		for(char_pos = 0; char_pos < 35 - strlen(base_filename); char_pos++)
		{
			printf("%c", ' ');
		}
	}

	for(width = 0; width < windowSize('x') - whitespace; width++)
	{
		printf("%c", ' ');
	}

	printf("%s%d%s%d%s", chars_text, chars, lines_text, lines, RESET);

	free(bar_buffer);
}

