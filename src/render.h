#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "input.h"

#define MIN_WIDTH 40
#define MIN_HEIGHT 20

int8_t whiteblock[9] = "\033[7m \033[0m"; // Mostly white rectangle.

uint16_t windowSize(int8_t axis) {
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT) {
		puts("To small terminal for Fiflo!");
		exit(1); }

	if(axis == 'x') {
		return win.ws_col; }

	else if(axis == 'y') {
		return win.ws_row; }}

void window(void) {
	uint16_t i;
	uint16_t winWidth = windowSize('x');
	uint16_t winHeight = windowSize('y');

	int8_t programName[7] = " Fiflo ";
	uint16_t programNameLen = strlen(programName);
	uint16_t programNameCenter = (winWidth - programNameLen) / 2;

	for(i = 0; i < programNameCenter; i++) {
		printf("%s", whiteblock); }

	printf("%s", programName);

	if(winWidth % 2 == 0) {
		for(i = 0; i <= programNameCenter; i++) {
			printf("%s", whiteblock); }}
	else {
		for(i = 0; i < programNameCenter; i++) {
			printf("%s", whiteblock); }}

	for(i = 1; i < winHeight; i++) {
		printf("%c", '\n'); }

	for(i = 0; i < winWidth; i++) {
		printf("%s", whiteblock); }}

