#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI escape codes.
#define RESET "\033[0m\0"
#define BOLD "\033[1m\0"
#define BLINK "\033[5m\0"
#define INVERT "\033[7m\0"

typedef int16_t BUFF_T;

int8_t decimalIntLen(int8_t chars);
void upperBar(char *base_fn);
void dynamicLen(char *filename);
void lowerBar(BUFF_T lines, BUFF_T chars, char key);
void help(void);

#endif

