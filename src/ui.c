#include "ui.h"

uint16_t i;

void lowerBorder(void) {
	char text[7] = "Fiflo \0";
	printf("%s ", INVERT_COLORS);
	printf("%s", text);

	for(i = 0; i < windowSize('x') - strlen(text) - 1; i++) {
		printf("%c", ' ');
	}
	printf("%s", RESTORE_COLORS);
}

