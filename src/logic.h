#ifndef RENDER_H
#define RENDER_H

#include <limits.h>
#include <signal.h>
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
key == (LINEFEED || CTRL_X || BACKSPACE) \
|| (key >= 32 && key < ARROW_UP) \
|| (key > ARROW_LEFT)

typedef int16_t buff_t; // Text Data type.
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

void setFilename(struct Data buff, char *name);
struct Data readFile(struct Data buff, char *name);
void saveFile(struct Data buff);
struct Data allocText(struct Data buff, char key);
void renderText(struct Data buff);
struct Data window(char key, struct Data buff);

#endif

