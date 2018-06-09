#include "ui.h"
#include "hardware.h"

// All rendered elements except chars you type.
static uint16_t i;

void cursor(void) // Blinking floor.
{
	printf("%s", BLINK);
	printf("%c", '_');
	printf("%s", RESET);
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

 // Lower border with a text.
void infoBar(int8_t chars, int8_t lines, char baseFilename[])
{
	char programName[10] = " Fiflo | \0";
	char charsText[8] = "chars: \0";
	char linesText[11] = " | lines: \0";
	char stdinText[10] = " | stdin<\0";

	uint16_t whitespace
	= strlen(programName)
	+ strlen(baseFilename)
	+ decimalIntLen(chars)
	+ strlen(charsText)
	+ strlen(linesText)
	+ strlen(stdinText) + 2;

	char* barBuffer = malloc(windowSize('x'));
	if(barBuffer == NULL)
	{
		memError();
	}

	printf("%s%s%s", BOLD, programName, baseFilename);

	for(i = 0; i < windowSize('x') - whitespace; i++)
	{
		printf("%c", ' ');
	}

	printf("%s%d%s%d%s%s", charsText, chars, linesText, lines, stdinText,
	RESET);

	free(barBuffer);
}

