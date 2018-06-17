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

extern int8_t lines_amount, chars_amount;

void keyCheck(char key);
static uint16_t windowSize(char axis);
void cleanFrame(void);
static void allocateChars(int8_t lines, int8_t chars, char key, char base_filename[]);
void window(int8_t lines, int8_t chars, char key, char base_filename[]);

#endif

