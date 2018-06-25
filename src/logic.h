#ifndef RENDER_H
#define RENDER_H

#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>


// Some special ASCII decimal codes.
#define ENTER 10 // Linefeed
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 600
#define MAX_HEIGHT 400

typedef int16_t BUFF_T; // Text buffer type.
#define BUFF_SZ SHRT_MAX - 1 // Always [TYPE]_MAX - 1.

extern char text[BUFF_SZ][BUFF_SZ];
extern char base_filename[510];
extern BUFF_T lines_c;
extern BUFF_T chars_c;

void setBaseFilename(char *filename);
void saveToFile(BUFF_T lines, BUFF_T chars, char *filename);
void keyHandling(char key, char *filename);
uint16_t windowSize(char axis);
void renderText(BUFF_T lines, BUFF_T chars);
void window(BUFF_T lines, BUFF_T chars, char key);
void cleanFrame(void);

#endif

