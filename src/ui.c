#include "ui.h"

uint16_t i;

void upperBorder(void) {
	uint16_t winWidth = windowSize('x');
	char programName[8] = " Fiflo \0";
	uint16_t programNameCenter = (winWidth - 7) / 2;
	for(i = 0; i < programNameCenter; i++) {
		printf("%s", WHITEBLOCK);
	}
	printf("%s", programName);

	if(winWidth % 2 == 0) { // Without it there could be non-colored block.
		for(i = 0; i <= programNameCenter; i++)
		{
			printf("%s", WHITEBLOCK);
		}
	}
	else {
		for(i = 1; i <= programNameCenter; i++) {
			printf("%s", WHITEBLOCK);
		}
	}
}

void lowerBorder(void) {
	uint16_t winWidth = windowSize('x');
	for(i = 0; i < winWidth; i++)
	{
		printf("%s", WHITEBLOCK);
	}
}

