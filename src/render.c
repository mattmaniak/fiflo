#include "render.h"
#include "ui.c"

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 500
#define MAX_HEIGHT 300

int16_t i;
char singleLine[79];

uint16_t windowSize(char axis) { // Check term size and return width or height.
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT) {
		fputs("Minimal terminal size is 40x20.\n", stderr);
		exit(1);
	}
	else if(win.ws_col > MAX_WIDTH || win.ws_row > MAX_HEIGHT) {
		fputs("Maximum terminal size is 500x300.\n", stderr);
		exit(1);
	}
	switch(axis) {
		case 'x':
			return win.ws_col;
		case 'y':
			return win.ws_row;
	}
	return 0; // Protection from the -Wreturn-type warning.
}

void cleanFrame(void) { // To provide rendering in a one frame.
	uint16_t height = windowSize('y');
	for(i = 0; i < height; i++) {
		printf("%s", "\033[F\033[K");
	}
}

int8_t lineLenCheck(int8_t chars) { // TODO: NULL terminator!
	if(chars >= 80) {
		return 80;
	}
}

void window(int8_t chars, int8_t lines, char key) { // Wrapper.
	int16_t *lineBuffer = malloc((chars / lines * sizeof(char)) + 1);

	chars = lineLenCheck(chars);
	if(key == 127) {
		singleLine[chars - 1] = 'B';
	}
	else {
		singleLine[chars - 1] = key;
	}

	for(i = 0; i < chars; i++) {
		printf("%c", singleLine[i]);
	}
	cursor();

	for(i = 1; i < windowSize('y'); i++) {
		printf("%c", '\n');
	}
	infoBar(chars, lines);
	free(lineBuffer);
}

void windowEmpty(int8_t chars, int8_t lines) { // Showed at the beginning.
	cursor();
	for(i = 1; i < windowSize('y'); i++) {
		printf("%c", '\n');
	}
	infoBar(chars, lines);
}

