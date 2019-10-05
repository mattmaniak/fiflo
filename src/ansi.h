#ifndef ANSI_H
#define ANSI_H

// ANSI escape codes for a terminal control and some colors..

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

#define ANSI__BG_COLOR_OFFSET 10

#define ANSI__CLEAN_WHOLE_LINE()   printf("\033[2K")
#define ANSI__SAVE_CURSOR_POS()    printf("\033[s")
#define ANSI__RESTORE_CURSOR_POS() printf("\033[u")
#define ANSI__CURSOR_UP(offset)    printf("\033[%uA", offset)
#define ANSI__CURSOR_DOWN(offset)  printf("\033[%uB", offset)
#define ANSI__CURSOR_RIGHT(offset) printf("\033[%uC", offset)
#define ANSI__CURSOR_LEFT(offset)  printf("\033[%uD", offset)
#define ANSI__INVERT()             printf("\033[7m");

#endif
