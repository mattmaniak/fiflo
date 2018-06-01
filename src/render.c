#include "render.h"
#include "ui.c"

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 500
#define MAX_HEIGHT 300

uint16_t i;

uint16_t windowSize(int8_t axis) {
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT) {
		puts("Minimal terminal size is 40x20.");
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT) {
		puts("Maximum terminal size is 500x300.");
		exit(1);
	}

	switch(axis) {
		case 'x':
			return win.ws_col;
		case 'y':
			return win.ws_row;
	}
	return 0; // Protection from the [-Wreturn-type] warning.
}

void clearFrame(void) { // To provide rendering in a one frame.
	uint16_t winHeight = windowSize('y');
	for(i = 0; i < winHeight; i++) {
		printf("%s", "\033[F\033[K");
	}
}

void window(char pressedKey, int8_t chars) {
	printf("%c", pressedKey); // Printing all chars will be here.
	for(i = 1; i < windowSize('y'); i++) {
		printf("%c", '\n');
	}
	lowerBorder(chars);
}

