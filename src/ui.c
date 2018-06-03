#include "ui.h"

int16_t i;

void cursor(void) {
	printf("%s", BOLD);
	printf("%s", BLINK);
	printf("%c", '_');
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
	char charsText[8] = "chars: \0";
	char linesText[11] = " | lines: \0";
	char stdinPlace[10] = " | stdin>\0";
	uint16_t whitespace = strlen(programName) + strlen(filename)
	+ decimalIntLen(chars) + strlen(charsText)
	+ decimalIntLen(lines) + strlen(linesText)
	+ strlen(stdinPlace) + 1; // 1 - stdin buffer.

	printf("%s", BOLD);
	printf("%s", programName);
	printf("%s", filename);
	for(i = 0; i < windowSize('x') - whitespace; i++) {
		printf("%c", ' ');
	}
	printf("%s", charsText);
	printf("%d", chars);
	printf("%s", linesText);
	printf("%d", lines);
	printf("%s", stdinPlace);
	printf("%s", RESET);
}

void help(void) {
	puts("Usage: fiflo [one parameter].\n");
	puts("Parameter   | Description");
	puts("NULL        | Show help.");
	puts("[filename]  | Create/open given file. WIP");
}

