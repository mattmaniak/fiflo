#include "ui.h"

uint16_t i;

int8_t charcharsString(int8_t chars) { // Return len of decimal charchars.
	if(chars < 10) return 1;
	if(chars >= 10 && chars < 100) return 2;
	if(chars >= 100) return 3;
}

void lowerBorder(int8_t chars) {
	char programName[10] = " Fiflo | \0";
	char charAmount[10] = " chars | \0";
	char stdinPlace[9] = "| stdin:\0";
	uint16_t whitespace = strlen(programName) + strlen(charAmount)
	+ charcharsString(chars) + strlen(stdinPlace) + 1;

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

