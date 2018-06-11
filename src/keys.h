#ifndef KEYS_H
#define KEYS_H

// Some special ASCII decimal codes.
#define ENTER 10
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

void keyHandling(int8_t lines, int8_t chars, char key);

#endif

