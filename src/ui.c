#include "ui.h"

int16_t i;

void cursor(void) {
	printf("%s", BLINK);
	printf("%c", '|');
	printf("%s", RESET);
}

int8_t decimalIntLen(int8_t chars) { // Return len of decimal charchars.
	int8_t len = 1;
	while(chars > 9) {
		len++;
		chars /= 10;
	}
	return len;
}

void infoBar(char filename[32], int8_t chars, int8_t lines) {
	char programName[10] = " Fiflo | \0";
	char singleBorder[4] = " | \0";
	char charsText[10] = " chars | \0";
	char linesText[10] = " lines | \0";
	char stdinPlace[7] = "stdin:\0";

	uint16_t whitespace = strlen(programName) + strlen(filename)
	+ decimalIntLen(chars) + strlen(charsText)
	+ decimalIntLen(lines) + strlen(linesText)
	+ strlen(stdinPlace) + (2 * strlen(singleBorder)) + 1; // 1 - stdin buffer.

	printf("%s", BOLD);
	printf("%s", programName);
	printf("%s", filename);
	printf("%s", singleBorder);
	for(i = 0; i < windowSize('x') - whitespace; i++) {
		printf("%c", ' ');
	}
	printf("%s", singleBorder);
	printf("%d", chars);
	printf("%s", charsText);
	printf("%d", lines);
	printf("%s", linesText);
	printf("%s", stdinPlace);
	printf("%s", RESET);
}

