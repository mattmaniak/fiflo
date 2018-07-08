#ifndef RENDER_H
#define RENDER_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_WIDTH 80
#define MIN_HEIGHT 10
#define MAX_WIDTH 65534
#define MAX_HEIGHT 65534

#define MAX_CHARS (getSize(X) * (getSize(Y) - 3)) - 1

#define TERMINATOR 0
#define TAB 9
#define LINEFEED 10
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

#define KEYMAP \
key == TERMINATOR || \
key == BACKSPACE || \
key == LINEFEED || \
key == CTRL_X || \
key != ARROW_UP || \
key != ARROW_DOWN || \
key != ARROW_RIGHT || \
key != ARROW_LEFT


typedef int16_t BUFF_T; // Text Params type.
typedef uint16_t TERM_SIZE; // Unsigned short as in the "sys/ioctl.h".

void setFilename(const char *basename);
struct Params readFile(struct Params buff);
void saveFile(struct Params buff);
struct Params keyHandling(char key, struct Params buff);
void renderText(struct Params buff);
struct Params window(char key, struct Params buff);

#endif

