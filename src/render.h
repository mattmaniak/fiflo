#include <stdio.h>
#include <string.h>
#include "graphics.h"

unsigned short windowSize(char axis) {
	#define MIN_WIDTH 40
	#define MIN_HEIGHT 20

	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

//	if(win.ws_col < MIN_WIDTH) {
//		 }

	if(axis == 'x') {
		return win.ws_col;
	}
	else if(axis == 'y') {
		return win.ws_row;
	}
}

void window(void) {

	unsigned short i;
	unsigned short winWidth = windowSize('x');
	unsigned short winHeight = windowSize('y');

	char programName[16] = " Fiflo v.0.1.0 \0";
	unsigned short programNameLen = strlen(programName);
	unsigned short programNameCenter = (winWidth - programNameLen) / 2;

	for(i = 0; i < programNameCenter; i++) {
		printWhiteblock();
	}
	printf("%s", programName);
	for(i = 0; i < programNameCenter; i++) {
		printWhiteblock();
	}

	for(i = 2; i < winHeight; i++) {
		printf("%c", '\n');
	}
	for(i = 0; i < winWidth; i++) {
		printWhiteblock();
	}
}
