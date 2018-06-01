#include "ui.h"

uint16_t i;

void lowerBorder(void) {
	char leftText[8] = " Fiflo \0";
	char rightText[7] = "stdin:\0";
	uint16_t whitespace = strlen(leftText) + strlen(leftText);

	printf("%s", BOLD);
	printf("%s", leftText);
	for(i = 0; i < windowSize('x') - whitespace; i++) {
		printf("%c", ' ');
	}
	printf("%s", rightText);
	printf("%s", RESTORE);
}

