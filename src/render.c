#include "render.h"
#include "ui.c"

// Drawing funcions.
uint16_t i;
char text[1][81];

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
	for(i = 0; i < windowSize('y'); i++) {
		printf("%s", "\033[F\033[K");
	}
}

void writeToFile(char charToWrite) {
	FILE* selectedFile = fopen("textfile.asdf", "r+");
	if(selectedFile == NULL) {
		selectedFile = fopen("textfile.asdf", "wb");

		if(selectedFile == NULL) {
			fileError();
		}
	}
	fprintf(selectedFile, "%c\n", charToWrite);
	fclose(selectedFile);
}

void window(int8_t chars, int8_t lines, char key) { // Wrapper.
	uint16_t charPos;
	char* lineBuffer = malloc(chars * lines * sizeof(char) + 1);

	if(lineBuffer == NULL) {
		memError();
	}
	if(key != BACKSPACE) {
		text[lines - 1][chars - 2] = key; // TODO: allocates only 79 char.
		text[lines - 1][chars - 1] = '\0';
	}
	else {
		text[lines - 1][chars - 1] = '\0';
	}
	/*
	Integration of variables: "chars" and "charPos" is highly required TODO.
	They means the same so there is no need to split.
	*/
	for(charPos = 0; charPos < chars; charPos++) {
		char currentChar = text[lines - 1][charPos - 1];
		printf("%c", text[lines - 1][charPos - 1]);
		writeToFile(currentChar);
	}
	cursor();
	for(i = 1; i < windowSize('y'); i++) {
		printf("%c", '\n');
	}
	infoBar(chars, lines);
	free(lineBuffer);
}

