#ifndef KEYS_H
#define KEYS_H

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

#define BUFFER_SIZE 126 // Always [TYPE]_MAX - 1.

extern char text[19][BUFFER_SIZE];

void keyHandling(int8_t lines, int8_t chars, char key);

#endif

