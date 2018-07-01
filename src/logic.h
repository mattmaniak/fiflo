#ifndef RENDER_H
#define RENDER_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_WIDTH 600
#define MAX_HEIGHT 400

#define UPPER_LINE lines - 2
#define CURRENT_LINE lines - 1

// Some special ASCII decimal codes.
#define TERMINATOR 0
#define LINEFEED 10 // Linefeed (LF)
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

typedef int16_t BUFF_T; // Text buffer type.
typedef uint16_t WIN_DIMENSION; // Unsigned short in the "sys/ioctl.h".
#define BUFF_SZ SHRT_MAX - 1 // Always [TYPE]_MAX - 1.

struct Buffer
{
	BUFF_T chars;
	BUFF_T lines;
	BUFF_T cursor_pos;
};

extern BUFF_T lines;
extern BUFF_T chars;
extern BUFF_T cursor_pos;

extern char text[BUFF_SZ][MAX_WIDTH + 1]; // + 1 for null or linefeed.
//extern char *filename;

void setFilename(const char *basename);
void readFromFile(void);
void saveToFile(void);
struct Buffer keyHandling(char key, struct Buffer buff);
void renderText(char key);
void window(char key);

#endif

