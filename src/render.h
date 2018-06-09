#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 500
#define MAX_HEIGHT 300

uint16_t windowSize(char axis);
void cleanFrame(void);
void writeToFile(char charToWrite);
void allocateChars(int8_t chars, int8_t lines, char key);
void window(int8_t chars, int8_t lines, char key, char baseFilename[]);

#endif

