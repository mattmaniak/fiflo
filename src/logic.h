#ifndef RENDER_H
#define RENDER_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS (getSize(X) * (getSize(Y) - 3)) - 1

#define READ true
#define WRITE false

#define TERMINATOR 0
#define CTRL_D 4
#define LINEFEED 10
#define CTRL_X 24
#define BACKSPACE 127

#define MAX_CHARS_PER_LINE 80

typedef int16_t buff_t;
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

void setFilename(struct Data buff, char *name);
struct Data punchedCard(struct Data buff, term_t limit, bool mode, char key);
struct Data readFile(struct Data buff, char *name);
void saveFile(struct Data buff);
struct Data allocText(struct Data buff, char key);
void renderText(struct Data buff);
void windowFill(buff_t lines);
struct Data window(struct Data buff, char key);

#endif

