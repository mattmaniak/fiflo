#include "render.h"
#include "ui.c"

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 500
#define MAX_HEIGHT 300

int16_t i;
char singleLine[80];

int16_t windowSize(int8_t axis) {
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
	return 0; // Protection from the -Wreturn-type warning.
}

void cleanFrame(void) { // To provide rendering in a one frame.
	int16_t winHeight = windowSize('y');
	for(i = 0; i < winHeight; i++) {
		printf("%s", "\033[F\033[K");
	}
}

void lineLenCheck(int8_t chars) { // TODO: Doesn't works!.
	if(chars >= 80) {
		chars = 80;
	}
}

void window(char key, char filename[32], int8_t chars, int8_t lines) {
	int16_t lineLen = strlen(singleLine);
	lineLenCheck(chars);
	singleLine[80] = '\0';
	singleLine[chars] = key;

	for(i = 0; i < chars; i++) {
		printf("%c", singleLine[i]);
	}
	for(i = 1; i < windowSize('y'); i++) {
		printf("%c", '\n');
	}
	infoBar(filename, chars, lines);
}

