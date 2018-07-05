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

typedef int16_t BUFF_T; // Text Params type.
typedef uint16_t TERM_SIZE; // Unsigned short as in the "sys/ioctl.h".

void setFilename(const char *basename);
struct Params readFromFile(struct Params buff);
void saveToFile(struct Params buff);
struct Params keyHandling(char key, struct Params buff);
void renderText(struct Params buff);
struct Params window(char key, struct Params buff);

#endif

