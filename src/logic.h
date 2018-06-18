#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 600
#define MAX_HEIGHT 400

#define LINES_CHARS_KEY_FILENAME int8_t lines, int8_t chars, char key, \
char filename[]

extern int8_t lines_amount, chars_amount;

void saveToFile(LINES_CHARS_KEY_FILENAME);

void keyCheck(LINES_CHARS_KEY_FILENAME);
uint16_t windowSize(char axis);
void cleanFrame(void);

void allocChars(LINES_CHARS_KEY_FILENAME);
void initWindow(int8_t lines, int8_t chars, char filename[]);
void window(LINES_CHARS_KEY_FILENAME);

#endif

