#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI escape codes.
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BLINK "\033[5m"
#define INVERT "\033[7m"

typedef int16_t BUFF_T;

int8_t decimalIntLen(int8_t chars);
void upperBar(const char *base_fn);
void dynamicLen(const char *base_fn);
void lowerBar(BUFF_T lines, BUFF_T chars, char key);
void help(void);

#endif

