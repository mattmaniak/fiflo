#include "ui.h"

uint16_t i;

int8_t decimalIntLen(int8_t chars) { // Return len of decimal charchars.
	int8_t len = 1;
	while(chars > 9) {
		len++;
		chars /= 10;
	}
	return len;
}

void lowerBorder(int8_t chars) {
	char programName[10] = " Fiflo | \0";
	char charAmount[10] = " chars | \0";
	char stdinPlace[9] = "| stdin:\0";
	uint16_t whitespace = strlen(programName) + strlen(charAmount)
	+ decimalIntLen(chars) + strlen(stdinPlace) + 1; // 1 - stdin buffer.

	printf("%s", BOLD);
	printf("%s", programName);
	printf("%d", chars);
	printf("%s", charAmount);
	for(i = 0; i < windowSize('x') - whitespace; i++) {
		printf("%c", ' ');
	}
	printf("%s", stdinPlace);
	printf("%s", RESTORE);
}

