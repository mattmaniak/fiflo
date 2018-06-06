#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

// Some special ASCII decimal codes.
#define ENTER 10
#define CTRL_X 24
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68
#define BACKSPACE 127

void memError(void);
void fileError(void);
char unixGetch(void);

#endif
