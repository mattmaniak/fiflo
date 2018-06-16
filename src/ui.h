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

void cursor(void);
static int8_t decimalIntLen(int8_t chars);
void upperBar(void);
static void dynamicLen(char base_filename[]);
void lowerBar(int8_t lines, int8_t chars, char key, char base_filename[]);
void help(void);

#endif

