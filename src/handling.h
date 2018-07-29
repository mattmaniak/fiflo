#ifndef HANDLING_H
#define HANDLING_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TERMINATOR 0
#define CTRL_D 4
#define LINEFEED 10
#define CTRL_X 24
#define BACKSPACE 127

#define MAX_CHARS 0x7fffffff - 1 // TODO
#define MAX_CHARS_PER_LINE 80

typedef int32_t buff_t;
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

void charsLimit(buff_t chars);
void setFilename(struct Data buff, char *name);
struct Data punchedCard(struct Data buff, term_t limit, bool mode, char key);
struct Data readFile(struct Data buff, char *name);
void saveFile(struct Data buff);
struct Data allocText(struct Data buff, char key);

#endif

