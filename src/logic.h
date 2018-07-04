#ifndef RENDER_H
#define RENDER_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 600
#define MAX_HEIGHT 400

#define TERMINATOR 0
#define TAB 9
#define LINEFEED 10
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

#define BUFF_SZ SHRT_MAX - 1 // Always [TYPE]_MAX - 1.
typedef int16_t BUFF_T; // Text Params type.
typedef uint16_t TERM_SIZE; // Unsigned short in the "sys/ioctl.h".

struct Params
{
	BUFF_T chars;
	BUFF_T lines;
	BUFF_T cursor_pos;
};

extern char text[BUFF_SZ];
//extern char *filename;

void setFilename(const char *basename);
void readFromFile(struct Params buff);
void saveToFile(struct Params buff);
struct Params keyHandling(char key, struct Params buff);
void renderText(struct Params buff);
void window(char key);

#endif

