#ifndef KEYS_H
#define KEYS_H

#include <limits.h>

// Some special ASCII decimal codes.
#define CTRL_B 2
#define ENTER 10 // Linefeed
#define CTRL_N 14
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

typedef int8_t BUFF_T;  // Text buffer.
#define BUFF_SZ SCHAR_MAX - 1 // Always [TYPE]_MAX - 1.

extern char text[19][BUFF_SZ];

void keyHandling(BUFF_T lines, BUFF_T chars, char key);

#endif

