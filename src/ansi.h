#ifndef ANSI_H
#define ANSI_H

#include <stdio.h>

enum
{
    ANSI__RESET = 0,
    ANSI__INVERT = 7,
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_BLACK = 90,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE
};

#define ANSI__INVERT() printf("\033[7m");

#endif
