#include "render.h"
#include "ui.c"

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 500
#define MAX_HEIGHT 300

uint16_t i;
char singleLine[80];

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

void memError(void) {
	fputs("Cannot allocate memory!\n", stderr);
	exit(1);
}

void window(int8_t chars, int8_t lines, char key) { // Wrapper.
	uint16_t charPos;
	uint16_t height = windowSize('y');
	char* lineBuffer = malloc(chars * lines * sizeof(char) + 1);
	if(lineBuffer == NULL) {
		memError();
	}

	if(key == 127) {
		singleLine[chars - 1] = singleLine[chars];
	}
	else {
		singleLine[chars - 1] = key;
	}
	/*
	Integration of variables: "chars" and "charPos" is highly required TODO.
	They means the same so there is no need to split.
	*/
	for(charPos = 0; charPos < chars; charPos++) {
		printf("%c", singleLine[charPos]);
	}
	cursor();
	for(i = 1; i < height; i++) {
		printf("%c", '\n');
	}
	infoBar(chars, lines);

	free(lineBuffer);
}

void windowEmpty(int8_t chars, int8_t lines) { // Showed at the beginning.
	uint16_t height = windowSize('y');

	cursor();
	for(i = 1; i < height; i++) {
		printf("%c", '\n');
	}
	infoBar(chars, lines);
}

