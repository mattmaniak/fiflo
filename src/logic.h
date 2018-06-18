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

typedef int8_t BUFF_T; // Text buffer.

extern BUFF_T lines_c;
extern BUFF_T chars_c;

void saveToFile(BUFF_T lines, BUFF_T chars, char filename[]);
void keyCheck(BUFF_T lines, BUFF_T chars, char key, char filename[]);
uint16_t windowSize(char axis);

void cleanFrame(void);
void allocChars(BUFF_T lines, BUFF_T chars, char key);
void initWindow(BUFF_T lines, BUFF_T chars, char filename[]);

void window(BUFF_T lines, BUFF_T chars, char key, char filename[]);

#endif

